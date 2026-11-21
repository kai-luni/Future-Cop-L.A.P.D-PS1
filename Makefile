BIN := Future\ Cop\ -\ L.A.P.D.\ (USA)/Future\ Cop\ -\ L.A.P.D.\ (USA).bin
ISO := build/disc/future-cop-usa.iso
FILES := build/disc/files
PYTHON ?= python3
VENV := .venv
VENV_PYTHON := $(VENV)/bin/python
VENV_STAMP := $(VENV)/.installed
EXE := baseroms/us/SLUS_007.39

.PHONY: all setup toolchain verify split function-inventory verify-matching verify-noncode sweep-candidates report check extract inventory inventory-noncode offline-decomp-status clean host-env host-naming host-naming-wave host-naming-round3 host-naming-round4 host-naming-round5 function-map

all: split

$(VENV_STAMP): requirements.txt
	$(PYTHON) -m venv $(VENV)
	$(VENV_PYTHON) -m pip install --upgrade pip
	$(VENV_PYTHON) -m pip install -r requirements.txt
	@touch "$@"

$(EXE): $(FILES)/.extracted
	@mkdir -p "$(dir $(EXE))"
	@chmod u+w "$@" 2>/dev/null || true
	install -m 0644 "$(FILES)/SLUS_007.39" "$@"

setup: $(VENV_STAMP) $(EXE)

toolchain:
	tools/bootstrap_toolchain.sh

verify: $(EXE)
	$(PYTHON) tools/verify_exe.py "$(EXE)"

split: setup verify
	$(PYTHON) tools/split_staged.py $(VENV_PYTHON)

function-inventory: split
	$(PYTHON) tools/inventory_functions.py asm/us/main.s build/us/function_candidates.json

verify-matching: verify toolchain
	PATH="$(CURDIR)/.toolchain/mipsel-binutils/root/usr/bin:$$PATH" \
	LD_LIBRARY_PATH="$(CURDIR)/.toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
	$(PYTHON) tools/verify_matching_units.py

verify-noncode: verify extract
	$(PYTHON) tools/verify_noncode.py

sweep-candidates: verify toolchain
	$(PYTHON) tools/sweep_candidate_compilers.py

report:
	$(PYTHON) tools/generate_decomp_report.py

check: verify function-inventory verify-matching verify-noncode report function-map host-env host-naming host-naming-wave host-naming-round3 host-naming-round4 host-naming-round5
	$(PYTHON) -m unittest discover -s tests -t . -v

# Run the naming-round behavioral tests against the original C entry points.
host-naming:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round_test.sh; \
	else \
	  echo "host-naming: qemu or C toolchain missing, skipped"; \
	fi

# Behavioral checks for the units renamed in the 100/100 naming wave.
host-naming-wave:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_wave_test.sh; \
	else \
	  echo "host-naming-wave: qemu or C toolchain missing, skipped"; \
	fi

# Behavioral checks for the units renamed in naming round 3.
host-naming-round3:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round3_test.sh; \
	else \
	  echo "host-naming-round3: qemu or C toolchain missing, skipped"; \
	fi

# Behavioral checks for the units renamed in naming round 4.
host-naming-round4:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round4_test.sh; \
	else \
	  echo "host-naming-round4: qemu or C toolchain missing, skipped"; \
	fi

# Behavioral checks for the units renamed in naming round 5.
host-naming-round5:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round5_test.sh; \
	else \
	  echo "host-naming-round5: qemu or C toolchain missing, skipped"; \
	fi

# Run the environment/matrix render cores under qemu. Skipped (not failed) when
# the local qemu-user build or the retail executable is missing, so a fresh
# checkout can still run the rest of `make check`.
host-env:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -f baseroms/us/SLUS_007.39 ]; then \
	  bash tests/host/env_test.sh; \
	else \
	  echo "host-env: qemu or baseroms/us/SLUS_007.39 missing, skipped"; \
	fi

function-map:
	$(PYTHON) tools/build_function_map.py --check

extract: $(FILES)/.extracted

$(ISO): tools/extract_mode2.py | $(BIN)
	@mkdir -p build/disc
	python3 tools/extract_mode2.py "$<" "$@"

$(FILES)/.extracted: $(ISO)
	@mkdir -p "$(FILES)"
	@chmod -R u+w "$(FILES)" 2>/dev/null || true
	xorriso -osirrox on -indev "$(ISO)" -extract / "$(abspath $(FILES))"
	@touch "$@"

inventory: extract
	@mkdir -p build/inventory
	find "$(FILES)" -type f ! -name .extracted -printf '%P\t%s\n' | sort > build/inventory/files.tsv
	find "$(FILES)" -type f ! -name .extracted -print0 | sort -z | xargs -0 sha256sum > build/inventory/sha256.txt
	python3 tools/inspect_psx_exe.py "$(FILES)/SLUS_007.39" > build/inventory/psx-exe.txt

inventory-noncode: extract
	$(PYTHON) tools/inventory_noncode.py

offline-decomp-status:
	$(PYTHON) tools/offline_decomp.py status

clean:
	rm -rf build

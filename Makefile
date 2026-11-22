BIN := Future\ Cop\ -\ L.A.P.D.\ (USA)/Future\ Cop\ -\ L.A.P.D.\ (USA).bin
ISO := build/disc/future-cop-usa.iso
FILES := build/disc/files
PYTHON ?= python3
VENV := .venv
VENV_PYTHON := $(VENV)/bin/python
VENV_STAMP := $(VENV)/.installed
EXE := baseroms/us/SLUS_007.39

.PHONY: all setup toolchain verify split function-inventory verify-matching verify-noncode sweep-candidates report check extract inventory inventory-noncode offline-decomp-status clean host-env host-naming host-naming-wave host-naming-round3 host-naming-round4 host-naming-round5 host-naming-round6 host-naming-round7 host-naming-round8 host-weapon-anim host-weapon-input host-terrain-surface host-iso9660-path host-mocks-selftest host-leaf-a1 host-leaf-a2 host-leaf-extra host-leaf-more host-leaf-final portable-check function-map coverage reach

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

check: verify function-inventory verify-matching verify-noncode report function-map host-env host-naming host-naming-wave host-naming-round3 host-naming-round4 host-naming-round5 host-naming-round6 host-naming-round7 host-naming-round8 host-weapon-anim host-weapon-input host-terrain-surface host-iso9660-path host-mocks-selftest host-leaf-a1 host-leaf-a2 host-leaf-extra host-leaf-more host-leaf-final host-leaf-hundred-a host-leaf-hundred-b host-leaf-hundred-c host-leaf-follow-a host-leaf-follow-b host-leaf-thirty
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

# Behavioral checks for the units renamed in naming round 6.
host-naming-round6:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round6_test.sh; \
	else \
	  echo "host-naming-round6: qemu or C toolchain missing, skipped"; \
	fi

# Behavioral checks for the units renamed in naming round 7.
host-naming-round7:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round7_test.sh; \
	else \
	  echo "host-naming-round7: qemu or C toolchain missing, skipped"; \
	fi

host-naming-round8:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/naming_round8_test.sh; \
	else \
	  echo "host-naming-round8: qemu or C toolchain missing, skipped"; \
	fi

host-weapon-anim:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/weapon_anim_test.sh; \
	else \
	  echo "host-weapon-anim: qemu or C toolchain missing, skipped"; \
	fi

host-weapon-input:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/weapon_input_test.sh; \
	else \
	  echo "host-weapon-input: qemu or C toolchain missing, skipped"; \
	fi

# Terrain surface sampling and ground query (func_80067480, func_80067684).
# Both units are hardware-free real C, so this runs a genuine two-deep call
# chain with no mocked game logic.
host-terrain-surface:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/terrain_surface_test.sh; \
	else \
	  echo "host-terrain-surface: qemu or C toolchain missing, skipped"; \
	fi

# ISO-9660 path formatting (func_8001DC48) driven through the shared BIOS mock.
# The unit does all its string work via BIOS vectors, so this is the proof that
# the mock in tests/host/mocks/ answers with real semantics and that an
# unimplemented vector aborts instead of returning a plausible 0.
host-iso9660-path:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/iso9660_path_test.sh; \
	else \
	  echo "host-iso9660-path: qemu or C toolchain missing, skipped"; \
	fi

# The shared mock library's own contract: unconfigured returns are 0, records are
# the caller's own data, and an unsupported BIOS call aborts. Links no game unit,
# so it fails for a mock's reasons only.
host-mocks-selftest:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  bash tests/host/mocks_selftest_test.sh; \
	else \
	  echo "host-mocks-selftest: qemu or C toolchain missing, skipped"; \
	fi

# Leaf suites exercise single-object functions using mixed evidence: specs,
# math, libc, inferred semantics and source-derived behavior. Synthetic globals
# in A2 are fixtures, not proof of retail initial state.
host-leaf-a1:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  python3 tools/host_leaf_suite.py report leaf_a1; \
	else \
	  echo "host-leaf-a1: qemu or C toolchain missing, skipped"; \
	fi

# Leaf suite A2: like A1, but the units read/write a few globals. The harness
# defines every D_ symbol itself and sets it before each check.
host-leaf-a2:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  python3 tools/host_leaf_suite.py report leaf_a2; \
	else \
	  echo "host-leaf-a2: qemu or C toolchain missing, skipped"; \
	fi

# Additional leaf assertions, generated from unchanged matching C units.
host-leaf-extra:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_extra; \
	else \
	  echo "host-leaf-extra: qemu or C toolchain missing, skipped"; \
	fi

# Additional leaf assertions with explicitly initialized synthetic globals.
host-leaf-more:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_more; \
	else \
	  echo "host-leaf-more: qemu or C toolchain missing, skipped"; \
	fi

# Additional original leaf functions with synthetic state.
host-leaf-final:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_final; \
	else \
	  echo "host-leaf-final: qemu or C toolchain missing, skipped"; \
	fi

.PHONY: host-leaf-hundred-a host-leaf-hundred-b host-leaf-hundred-c host-leaf-follow-a host-leaf-follow-b host-leaf-thirty

# More original leaf functions with synthetic fixtures.
host-leaf-thirty:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_thirty; \
	else \
	  echo "host-leaf-thirty: qemu or C toolchain missing, skipped"; \
	fi

# Follow-up suites use synthetic data and unchanged original functions.
host-leaf-follow-a host-leaf-follow-b:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report $(subst host-leaf-follow-,leaf_follow_,$@); \
	else \
	  echo "$@: qemu or C toolchain missing, skipped"; \
	fi

host-leaf-hundred-c:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_hundred_c; \
	else \
	  echo "host-leaf-hundred-c: qemu or C toolchain missing, skipped"; \
	fi

host-leaf-hundred-a:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_hundred_a; \
	else \
	  echo "host-leaf-hundred-a: qemu or C toolchain missing, skipped"; \
	fi

# Additional original leaf functions with synthetic fixtures.
host-leaf-hundred-b:
	@QEMU=$${QEMU:-$$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}; \
	if [ -x "$$QEMU" ] && [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  $(PYTHON) tools/host_leaf_suite.py report leaf_hundred_b; \
	else \
	  echo "host-leaf-hundred-b: qemu or C toolchain missing, skipped"; \
	fi

# Replay portable vectors independently under QEMU; failure is not a skip.
portable-check:
	$(PYTHON) tools/portable_tests.py validate tests/portable/a1-a2.v1.json --sources
	$(PYTHON) -m unittest discover -s tests/portable -p 'test*.py' -v
	$(PYTHON) tools/portable_tests.py replay tests/portable/a1-a2.v1.json

# Line coverage of the host tests. gcov is unavailable (gcc 2.7.2), so this maps
# executed blocks from qemu -d in_asm onto the .loc line info of a -g compile.
coverage:
	@if [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  python3 tools/host_coverage.py $(if $(LCOV),--lcov $(LCOV),); \
	else \
	  echo "coverage: C toolchain missing, skipped"; \
	fi

# How many decompiled functions the host tests reach at all - including the units
# they link indirectly, which the coverage target above does not know about.
reach:
	@if [ -x .toolchain/gcc-2.7.2-cdk/gcc ]; then \
	  python3 tools/host_test_reach.py $(if $(LIST),--list,) \
	    $(if $(JSON),--json $(JSON),); \
	else \
	  echo "reach: C toolchain missing, skipped"; \
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

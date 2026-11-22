"""Fresh no-retail MIPS builds and QEMU observations for portable fixtures."""
from collections import Counter
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

if __package__:
    from .portable_tests import DATA, OBSERVATIONS, ROOT, WIDTHS, digest, require, save, signature
else:
    from portable_tests import DATA, OBSERVATIONS, ROOT, WIDTHS, digest, require, save, signature

HOST = ROOT / "tests/host"
PRELUDE = """#include "fc_testkit.h"

/* The two anonymous struct typedefs used by the source declarations. */
typedef struct { u8 value; u8 pad[5]; } Entry;
typedef struct { Entry *entries; int unused; u8 count; } List;
typedef struct { u8 pad_00[0x46]; u8 device; u8 pad_47[0x9F]; u16 ready; } Unk8008C858;

static void write_byte(u32 byte)
{
    static const char hex[] = "0123456789abcdef";
    char out[2];
    out[0] = hex[(byte >> 4) & 15];
    out[1] = hex[byte & 15];
    fck_sys3(4004, 1, (long)out, 2);
}

static void write_word(u32 value)
{
    write_byte(value >> 24);
    write_byte(value >> 16);
    write_byte(value >> 8);
    write_byte(value);
    fck_write("\\n");
}

static void write_span(const volatile u8 *bytes, u32 size)
{
    u32 i;
    for (i = 0; i < size; ++i) write_byte(bytes[i]);
    fck_write("\\n");
}

static void put_scalar(volatile u8 *bytes, u32 value, u32 size)
{
    u32 i;
    for (i = 0; i < size; ++i) {
        bytes[i] = (u8)value;
        value >>= 8;
    }
}

static u32 get_scalar(const volatile u8 *bytes, u32 size)
{
    u32 i, value = 0;
    for (i = 0; i < size; ++i) value |= ((u32)bytes[i]) << (i * 8);
    return value;
}
"""


def span(kind):
    return 4 if kind == "ptr" else WIDTHS[kind]


def harness(case, signatures):
    regions = {region["id"]: f"region_{i}" for i, region in enumerate(case["regions"])}
    labels = {region["id"]: region["symbol"] or f"portable_region_{i}"
              for i, region in enumerate(case["regions"])}

    def address(cell):
        return f"((u8 *){regions[cell['region']]} + {cell['offset']})"

    def expression(typed):
        value = typed["value"]
        if typed["type"] == "ptr":
            return address(value) if value is not None else "0"
        return f"0x{int(value) & 0xffffffff:08x}u"

    lines = [PRELUDE]
    types = " ".join(info[1] for info in signatures.values())
    lines += [f"struct {tag};" for tag in sorted(set(re.findall(r"\bstruct (\w+)", types)))]
    lines += [info[1] for info in signatures.values()]
    for region in case["regions"]:
        name = region["id"]
        # Word alignment, with padding excluded from the portable region.
        lines.append(f'u32 {regions[name]}[{(region["size"] + 3) // 4 + 1}] '
                     f'__asm__("{labels[name]}") = {{0}};')
    lines += ["int main(void)", "{", "    u32 i, result;"]
    for region in case["regions"]:
        lines.append(f"    for (i = 0; i < {region['size']}; ++i) "
                     f"((volatile u8 *){regions[region['id']]})[i] = {region['fill']['value']};")
    for step in case["steps"]:
        for cell in step["arrange"]:
            value = cell["value"]
            kind = value["type"]
            if kind == "bytes":
                data = "".join(f"\\x{byte:02x}" for byte in bytes.fromhex(value["value"]))
                lines.append(f'    memcpy({address(cell)}, "{data}", {len(data) // 4});')
            else:
                lines.append(f"    put_scalar({address(cell)}, (u32){expression(value)}, {span(kind)});")
        _, _, name, arg_types, _, result_type = signatures[step["function"]]
        args = ", ".join(f"({ctype}){expression(value)}" for ctype, value in zip(arg_types, step["args"]))
        call = f"{name}({args});"
        if result_type != "void":
            call = "(result = (u32)" + call[:-1] + ");"
        if step["repeat"] != 1:
            call = f"for (i = 0; i < {step['repeat']}; ++i) " + call
        lines.append("    " + call)
        lines.append("    fck_write(\"step\\n\");" if result_type == "void" else "    write_word(result);")
        for cell in step["memory"]:
            value = cell["value"]
            kind = value["type"]
            if kind == "bytes":
                lines.append(f"    write_span({address(cell)}, {len(value['value']) // 2});")
            else:
                lines.append(f"    write_word(get_scalar({address(cell)}, {span(kind)}));")
    lines += ["    return 0;", "}", ""]
    return "\n".join(lines), labels


NM_ROW = re.compile(r"^([0-9a-fA-F]+)\s+(?:([0-9a-fA-F]+)\s+)?([A-Za-z])\s+(\S+)$")
MAP_TEXT = re.compile(r"^ \.text\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(.+\.o)$", re.M)
TRACE = re.compile(r"^Trace \d+: \S+ \[[0-9a-fA-F]+/([0-9a-fA-F]+)/")


def nm_rows(text):
    rows = {}
    for line in text.splitlines():
        match = NM_ROW.fullmatch(line.strip())
        if match:
            address, length, kind, symbol = match.groups()
            if kind.isupper():
                require(symbol not in rows, f"duplicate global symbol {symbol}")
                rows[symbol] = int(address, 16), int(length or "0", 16), kind
    return rows


def verify_region_storage(linked, object_symbols, spans, label, size):
    require(label in linked and label in object_symbols, f"missing storage symbol {label}")
    offset, _, kind = object_symbols[label]
    linked_address, _, linked_kind = linked[label]
    require(kind == linked_kind and kind in ('D', 'B'), f"{label}: invalid storage symbol type")
    section = '.data' if kind == 'D' else '.bss'
    require(section in spans, f"{label}: missing storage section")
    start, length = spans[section]
    boundaries = [value[0] for name, value in object_symbols.items()
                  if name != label and value[2] == kind and value[0] > offset]
    end = min([start + length, *boundaries])
    require(start <= offset and offset + size <= end, f"{label}: linked storage too small")
    require(linked_address != 0, f"{label}: invalid linked address")


def section_spans(text):
    spans = {}
    for line in text.splitlines():
        match = re.match(r'^\s*\d+\s+(\.\S+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)', line)
        if match:
            name, size, start = match.groups()
            spans[name] = (int(start, 16), int(size, 16))
    return spans


def binding(symbol, obj_name, object_nm, linked_nm, map_text):
    offset, object_size, object_kind = object_nm.get(symbol, (0, 0, ""))
    address, size, kind = linked_nm.get(symbol, (0, 0, ""))
    require(object_kind == kind == "T" and size == object_size and size > 0,
            f"{symbol}: missing/mismatched strong text definition")
    ranges = [(int(start, 16), int(length, 16), obj) for start, length, obj in MAP_TEXT.findall(map_text)]
    owners = [(start, obj) for start, length, obj in ranges if start <= address < start + length]
    require(owners == [(address - offset, obj_name)] and
            any(obj == obj_name and start <= address and address + size <= start + length
                for start, length, obj in ranges),
            f"{symbol}: linked text is not from {obj_name}: {owners}")
    require(re.search(rf"^\s*0x0*{address:x}\s+{symbol}\s*$", map_text, re.M) is not None,
            f"{symbol}: missing linked symbol in map")
    return address, size


def decode(case, stdout, bases):
    lines = iter(stdout.splitlines())

    def value(kind, raw, byte_length=None):
        if kind == "void":
            require(raw == "step", "missing void step marker")
            return {"type": kind, "value": None}
        if kind == "bytes":
            require(re.fullmatch(r"[0-9a-f]+", raw) and len(raw) == 2 * byte_length, "bad byte capture")
            return {"type": kind, "value": raw}
        require(re.fullmatch(r"[0-9a-f]{8}", raw), f"bad scalar capture: {raw!r}")
        number = int(raw, 16)
        if kind == "ptr":
            payload = None
            if number:
                intervals = [(region["id"], bases[region["id"]], region["size"]) for region in case["regions"]]
                matches = [(name, number - base) for name, base, size in intervals
                           if base <= number < base + size]
                require(len(matches) == 1, f"pointer {number:#x} is outside named regions")
                name, offset = matches[0]
                payload = {"region": name, "offset": offset}
        else:
            bits = 8 * span(kind)
            number &= (1 << bits) - 1
            if kind.startswith("s") and number >= 1 << (bits - 1):
                number -= 1 << bits
            payload = str(number)
        return {"type": kind, "value": payload}

    steps = []
    for step in case["steps"]:
        result = value(step["return"]["type"], next(lines, ""))
        memory = [value(cell["value"]["type"], next(lines, ""),
                        len(cell["value"]["value"]) // 2 if cell["value"]["type"] == "bytes" else None)
                  for cell in step["memory"]]
        steps.append({"return": result, "memory": memory})
    require(next(lines, None) is None, "unexpected trailing QEMU output")
    return {"id": case["id"], "steps": steps}


class Build:
    def __init__(self):
        self.toolchain = Path(os.environ.get("TOOLCHAIN", ROOT / ".toolchain")).resolve()
        self.gcc = self.toolchain / "gcc-2.7.2-cdk/gcc"
        self.i386 = os.environ.get("PORTABLE_I386")
        if self.i386:
            self.i386 = Path(self.i386).resolve()
        maspsx = sorted(self.toolchain.glob("maspsx-*/maspsx.py"))
        require(len(maspsx) == 1, f"expected one maspsx under {self.toolchain}")
        self.maspsx = maspsx[0]
        self.qemu = os.environ.get("QEMU") or shutil.which("qemu-mipsel") or (
            Path.home() / ".local/share/qemu-user/extracted/usr/bin/qemu-mipsel")
        require(os.access(self.gcc, os.X_OK) and os.access(self.qemu, os.X_OK),
                "missing GCC or QEMU executable (set TOOLCHAIN / QEMU)")
        self.qemu = Path(self.qemu).resolve()
        self.binaries = self.toolchain / "mipsel-binutils/root/usr/bin"
        libs = self.binaries.parent / "lib/x86_64-linux-gnu"
        self.env = os.environ.copy()
        self.env["LD_LIBRARY_PATH"] = os.pathsep.join(filter(None, (str(libs), self.env.get("LD_LIBRARY_PATH"))))
        self.env["LC_ALL"] = "C"

    def run(self, args, cwd, data=None):
        args = list(map(str, args))
        try:
            result = subprocess.run(args, cwd=cwd, env=self.env, capture_output=True, input=data,
                                    text=True, timeout=30)
        except subprocess.TimeoutExpired as error:
            raise RuntimeError(f"{cwd.name}: timeout running {args[0]}") from error
        if result.returncode:
            raise RuntimeError(f"{cwd.name}: {args} exited {result.returncode}: {result.stderr[-3000:]}")
        return result.stdout

    def binutils(self, command, cwd, *args):
        return self.run([self.binaries / f"mipsel-linux-gnu-{command}", *args], cwd)

    def compile(self, source, name, cwd):
        include = [f"-I{ROOT / 'include'}", f"-I{HOST}"]
        if self.i386:
            # On hosts without native i386 execution: split the gcc -v -S
            # invocation into its original cpp and cc1 subprocesses.
            defines = ("__GNUC__=2", "__GNUC_MINOR__=7", "mips", "MIPSEL", "__mips__",
                       "__MIPSEL__", "__mips", "__MIPSEL", "__CHAR_UNSIGNED__", "__OPTIMIZE__",
                       "__EXTENSIONS__", "_MIPSEL", "_LANGUAGE_C", "LANGUAGE_C",
                       "__SIZE_TYPE__=unsigned int", "__PTRDIFF_TYPE__=int")
            self.run([self.i386, self.gcc.with_name("cpp"), "-lang-c", *include,
                      "-isystem", self.gcc.parent / "include", "-undef",
                      *(f"-D{define}" for define in defines), source, f"{name}.i"], cwd)
            self.run([self.i386, self.gcc.with_name("cc1"), f"{name}.i", "-quiet",
                      "-dumpbase", source.name, "-O1", "-G0", "-o", f"{name}.s"], cwd)
        else:
            self.run([self.gcc, f"-B{self.gcc.parent}/", *include,
                      "-O1", "-G0", "-S", source, "-o", f"{name}.s"], cwd)
        assembly = self.run([sys.executable, self.maspsx, f"{name}.s"], cwd)
        aligned = self.run([sys.executable, HOST / "fix_align.py"], cwd, assembly)
        (cwd / f"{name}.mas.s").write_text(aligned)
        self.binutils("as", cwd, "-o", f"{name}.o", f"{name}.mas.s")

    def execute(self, suite, root, suite_digest):
        signatures = {symbol: signature(symbol) for symbol in suite["functions"]}
        objects = {}
        for symbol, (source, *_) in signatures.items():
            name = symbol[5:]
            source_hash = suite["functions"][symbol]["sha256"]
            require(digest(source) == source_hash, f"{symbol}: source changed before compile")
            self.compile(source, name, root)
            require(digest(source) == source_hash, f"{symbol}: source changed during compile")
            obj = f"{name}.o"
            undefined = {line.split()[-1] for line in self.binutils("nm", root, "-u", obj).splitlines()}
            require(not any(s.startswith("func_") for s in undefined), f"{symbol}: not a standalone leaf")
            objects[symbol] = (nm_rows(self.binutils("nm", root, "-gS", obj)), undefined)
        self.binutils("as", root, "-o", "crt0.o", HOST / "crt0.s")

        observations = {"schema": OBSERVATIONS, "version": 1, "cases": []}
        proof = {"kind": "qemu-unchanged-source", "suite_sha256": suite_digest,
                 "compiler": str(self.gcc), "compiler_host": "qemu-i386" if self.i386 else "native",
                 "compiler_sha256": {name: digest(self.gcc.with_name(name)) for name in ("gcc", "cpp", "cc1")},
                 "maspsx_sha256": digest(self.maspsx),
                 "qemu_version": self.run([self.qemu, "--version"], root).splitlines()[0],
                 "cases": []}
        for case in suite["cases"]:
            try:
                actual, evidence = self.execute_case(case, signatures, objects, root)
            except (ValueError, OSError, RuntimeError) as error:
                raise RuntimeError(f"{case['id']}: {error}") from error
            observations["cases"].append(actual)
            proof["cases"].append(evidence)
        save(root / "proof.json", proof)
        return observations

    def execute_case(self, case, signatures, objects, root):
        cwd = root / case["id"]
        cwd.mkdir()
        calls = Counter()
        for step in case["steps"]:
            calls[step["function"]] += step["repeat"]
        declarations = {symbol: signatures[symbol] for symbol in calls}
        source, labels = harness(case, declarations)
        required = {s for symbol in calls for s in objects[symbol][1] if re.fullmatch(DATA, s)}
        supplied = {region["symbol"] for region in case["regions"] if region["symbol"]}
        require(required == supplied, f"fixture data symbols: required {sorted(required)}, supplied {sorted(supplied)}")
        (cwd / "driver.c").write_text(source)
        self.compile(cwd / "driver.c", "driver", cwd)
        self.binutils("ld", cwd, "-static", "-e", "__start", "-Map=link.map", "-o", "case.elf",
                      "../crt0.o", "driver.o", *(f"../{s[5:]}.o" for s in calls))
        map_text = (cwd / "link.map").read_text()
        linked = nm_rows(self.binutils("nm", cwd, "-gS", "case.elf"))
        linked_symbols = {s for s in linked if s.startswith("func_")}
        require(linked_symbols == calls.keys(), f"unexpected game definitions: {linked_symbols}")
        addresses = {s: binding(s, f"../{s[5:]}.o", objects[s][0], linked, map_text) for s in calls}
        driver_symbols = nm_rows(self.binutils("nm", cwd, "-gS", "driver.o"))
        spans = section_spans(self.binutils("objdump", cwd, "-h", "driver.o"))
        for region in case["regions"]:
            verify_region_storage(linked, driver_symbols, spans, labels[region["id"]], region["size"])
        bases = {name: linked[label][0] for name, label in labels.items()}
        stdout = self.run([self.qemu, "-d", "exec,nochain", "-D", "trace.log", "./case.elf"], cwd)
        (cwd / "stdout.txt").write_text(stdout)
        hits = Counter()
        with (cwd / "trace.log").open() as trace:
            for line in trace:
                if match := TRACE.match(line):
                    hits[int(match[1], 16)] += 1
        rows = {}
        for symbol, repeat in calls.items():
            address, size = addresses[symbol]
            require(hits[address] == repeat,
                    f"{symbol}: expected {repeat} QEMU entry dispatches at {address:#x}, saw {hits[address]}")
            obj = f"{symbol[5:]}.o"
            rows[symbol] = {"object": obj, "object_sha256": digest(root / obj),
                            "source_sha256": digest(signatures[symbol][0]), "text_address": f"0x{address:08x}",
                            "text_size": size, "qemu_entry_dispatches": hits[address]}
        return decode(case, stdout, bases), {
            "id": case["id"], "elf_sha256": digest(cwd / "case.elf"), "link_map_sha256": digest(cwd / "link.map"),
            "trace_sha256": digest(cwd / "trace.log"), "qemu_exit": 0, "bindings": rows,
        }


def replay(suite, artifacts, suite_digest):
    build = Build()
    if artifacts:
        artifacts.mkdir()
        return build.execute(suite, artifacts.resolve(), suite_digest)
    with tempfile.TemporaryDirectory(prefix="fc-portable-") as directory:
        return build.execute(suite, Path(directory), suite_digest)

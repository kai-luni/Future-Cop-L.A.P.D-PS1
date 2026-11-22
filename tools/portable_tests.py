#!/usr/bin/env python3
"""Validate, replay, and compare version 1 portable leaf fixtures."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SCHEMA = "fc-portable-tests"
OBSERVATIONS = "fc-portable-observations"
WIDTHS = {f"{sign}{width}": width // 8 for sign in "su" for width in (8, 16, 32)}
TYPES = {*WIDTHS, "ptr", "void"}
FUNC = r"func_([0-9A-F]{8})"
NAME = r"[A-Za-z][A-Za-z0-9_-]{0,63}"
DATA = r"D_[0-9A-F]{8}"
CTYPE = {
    "s8": "s8", "signed char": "s8",
    "u8": "u8", "unsigned char": "u8",
    "s16": "s16", "short": "s16", "signed short": "s16",
    "u16": "u16", "unsigned short": "u16",
    "s32": "s32", "int": "s32", "signed int": "s32",
    "u32": "u32", "unsigned": "u32", "unsigned int": "u32",
    "void": "void",
}


class Invalid(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise Invalid(message)


def fields(obj, names):
    require(isinstance(obj, dict) and obj.keys() == set(names.split()),
            f"expected fields: {names}")


def array(value, limit, minimum=0):
    require(isinstance(value, list) and minimum <= len(value) <= limit,
            f"expected array with {minimum}..{limit} entries")
    return value


def integer(value, low, high):
    require(type(value) is int and low <= value <= high,
            f"expected integer in [{low}, {high}]: {value!r}")
    return value


def match(value, pattern):
    return isinstance(value, str) and re.fullmatch(pattern, value) is not None


def load(path):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            require(key not in result, f"duplicate JSON key: {key}")
            result[key] = value
        return result

    def reject_constant(value):
        raise Invalid(f"not a JSON number: {value}")

    return json.loads(path.read_text(encoding="utf-8"), object_pairs_hook=unique,
                      parse_constant=reject_constant)


def save(path, value):
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signature(symbol):
    """Read the unit's own asm alias, not prose or the leaf harness declaration."""
    require(match(symbol, FUNC), f"invalid function: {symbol!r}")
    path = ROOT / "src/game" / (symbol[5:] + ".c")
    source = path.read_text()
    declarations = list(re.finditer(
        rf'^\s*(?:extern\s+)?([\w \t*]+?)[ \t]*\b(\w+)\(([^()]*)\)'
        rf'\s*__asm__\("{symbol}"\)\s*;', source, re.M))
    require(len(declarations) == 1, f"{symbol}: missing or ambiguous source declaration")
    declaration = declarations[0]
    result, name, args = declaration.groups()
    arg_types = []
    if args.strip() != "void":
        for arg in args.split(","):
            parsed = re.fullmatch(r'\s*([\w\s*]+?)\b\w+\s*', arg)
            require(parsed is not None, f"{symbol}: unsupported parameter {arg!r}")
            arg_types.append(parsed[1].strip())

    def abi(ctype):
        if ctype.endswith("*"):
            return "ptr"
        ctype = " ".join(ctype.split())
        require(ctype in CTYPE, f"{symbol}: unsupported C type {ctype!r}")
        return CTYPE[ctype]

    return path, declaration[0].strip(), name, arg_types, [abi(t) for t in arg_types], abi(result.strip())


def location(region, offset, sizes, length=1):
    require(isinstance(region, str) and region in sizes, f"unknown region: {region!r}")
    integer(offset, 0, sizes[region] - length)


def scalar(value, sizes, kind):
    fields(value, "type value")
    require(value["type"] == kind, f"expected {kind}, got {value['type']!r}")
    payload = value["value"]
    if kind in WIDTHS:
        require(match(payload, r"0|-?[1-9][0-9]{0,9}"), f"noncanonical {kind}: {payload!r}")
        bits = WIDTHS[kind] * 8
        lower, upper = (-(1 << (bits - 1)), (1 << (bits - 1)) - 1) if kind[0] == "s" else (0, (1 << bits) - 1)
        integer(int(payload), lower, upper)
        return WIDTHS[kind]
    if kind == "ptr":
        if payload is not None:
            fields(payload, "region offset")
            location(payload["region"], payload["offset"], sizes)
        return 4
    if kind == "bytes":
        require(match(payload, r"(?:[0-9a-f]{2}){1,65536}"), "invalid lowercase hex byte span")
        return len(payload) // 2
    require(kind == "void" and payload is None, "expected void with null value")
    return 0


def cells(entries, sizes):
    spans = {name: [] for name in sizes}
    for entry in array(entries, 2048):
        fields(entry, "region offset value")
        value = entry["value"]
        require(isinstance(value, dict), "cell requires a typed value")
        kind = value.get("type")
        require(isinstance(kind, str) and kind in {*WIDTHS, "ptr", "bytes"}, "invalid memory type")
        size = scalar(value, sizes, kind)
        region, offset = entry["region"], entry["offset"]
        location(region, offset, sizes, size)
        spans[region].append((offset, offset + size))
    for region, intervals in spans.items():
        intervals.sort()
        require(all(left[1] <= right[0] for left, right in zip(intervals, intervals[1:])),
                f"overlapping memory spans: {region}")


def validate(suite, check_sources=False):
    fields(suite, "schema version byte_order functions cases")
    require(suite["schema"] == SCHEMA and suite["byte_order"] == "little", "unknown schema or byte order")
    integer(suite["version"], 1, 1)
    functions = suite["functions"]
    require(isinstance(functions, dict) and 1 <= len(functions) <= 1024, "invalid functions catalogue")
    for symbol, function in functions.items():
        require(match(symbol, FUNC), f"invalid function symbol: {symbol!r}")
        fields(function, "source sha256 args return origin")
        require(function["source"] == f"src/game/{symbol[5:]}.c", f"{symbol}: wrong source path")
        require(match(function["sha256"], r"[0-9a-f]{64}"), f"{symbol}: invalid source digest")
        require(all(isinstance(t, str) and t in TYPES - {"void"}
                    for t in array(function["args"], 32)), f"{symbol}: invalid argument type")
        require(isinstance(function["return"], str) and function["return"] in TYPES,
                f"{symbol}: invalid return type")
        origin = function["origin"]
        fields(origin, "suite evidence note")
        require(origin["suite"] in ("a1", "a2"), f"{symbol}: unknown source suite")
        require(origin["evidence"] in ("spec", "math", "libc", "semantic", "source"),
                f"{symbol}: unknown evidence class")
        require(isinstance(origin["note"], str) and origin["note"].strip(), f"{symbol}: empty origin note")
        if check_sources:
            path, _, _, _, args, result = signature(symbol)
            require(digest(path) == function["sha256"], f"{symbol}: source digest changed")
            require((args, result) == (function["args"], function["return"]),
                    f"{symbol}: fixture ABI differs from the source declaration")

    ids, used = set(), set()
    for case in array(suite["cases"], 4096, 1):
        fields(case, "id regions steps")
        identifier = case["id"]
        require(match(identifier, NAME) and identifier not in ids, f"invalid/duplicate case id: {identifier!r}")
        ids.add(identifier)
        sizes, symbols = {}, set()
        for region in array(case["regions"], 128):
            fields(region, "id size fill symbol")
            name = region["id"]
            require(match(name, NAME) and name not in sizes, f"{identifier}: invalid/duplicate region {name!r}")
            sizes[name] = integer(region["size"], 1, 65536)
            scalar(region["fill"], {}, "u8")
            label = region["symbol"]
            if label is not None:
                require(match(label, DATA) and label not in symbols, f"{identifier}: invalid/duplicate data symbol")
                symbols.add(label)
        for step in array(case["steps"], 10000, 1):
            try:
                fields(step, "label function args repeat arrange return memory")
                require(isinstance(step["label"], str) and step["label"].strip(), "missing step label")
                symbol = step["function"]
                require(isinstance(symbol, str) and symbol in functions, f"unknown function: {symbol!r}")
                used.add(symbol)
                function = functions[symbol]
                args = array(step["args"], len(function["args"]), len(function["args"]))
                for value, kind in zip(args, function["args"]):
                    scalar(value, sizes, kind)
                integer(step["repeat"], 1, 10000)
                cells(step["arrange"], sizes)
                cells(step["memory"], sizes)
                scalar(step["return"], sizes, function["return"])
                require(function["return"] != "void" or step["memory"], "void call has no observation")
            except Invalid as error:
                raise Invalid(f"{identifier}, step {step.get('label') if isinstance(step, dict) else '?'}: {error}") from error
    require(used == functions.keys(), f"unused functions: {sorted(functions.keys() - used)}")


def expected(suite):
    return {
        "schema": OBSERVATIONS, "version": 1,
        "cases": [
            {"id": case["id"], "steps": [
                {"return": step["return"], "memory": [cell["value"] for cell in step["memory"]]}
                for step in case["steps"]]}
            for case in suite["cases"]],
    }


def compare(suite, observations):
    fields(observations, "schema version cases")
    require(observations["schema"] == OBSERVATIONS, "unknown observation schema")
    integer(observations["version"], 1, 1)
    rows = array(observations["cases"], len(suite["cases"]), len(suite["cases"]))
    for case, row in zip(suite["cases"], rows):
        fields(row, "id steps")
        require(row["id"] == case["id"], f"expected observation for {case['id']}")
        sizes = {region["id"]: region["size"] for region in case["regions"]}
        steps = array(row["steps"], len(case["steps"]), len(case["steps"]))
        for index, (step, result) in enumerate(zip(case["steps"], steps)):
            context = f"{case['id']} step {index + 1} ({step['label']}, {step['function']})"
            fields(result, "return memory")
            scalar(result["return"], sizes, step["return"]["type"])
            require(result["return"] == step["return"],
                    f"{context}: return expected {step['return']}, got {result['return']}")
            values = array(result["memory"], len(step["memory"]), len(step["memory"]))
            for cell, actual in zip(step["memory"], values):
                scalar(actual, sizes, cell["value"]["type"])
                require(actual == cell["value"],
                        f"{context}: {cell['region']}+{cell['offset']} expected {cell['value']}, got {actual}")


def counts(suite):
    steps = [step for case in suite["cases"] for step in case["steps"]]
    return (f"{len(suite['functions'])} functions, {len(suite['cases'])} cases, {len(steps)} steps, "
            f"{sum(step['repeat'] for step in steps)} calls, "
            f"{sum((step['return']['type'] != 'void') + len(step['memory']) for step in steps)} checks")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="action", required=True)
    for action in ("validate", "replay", "compare"):
        command = sub.add_parser(action)
        command.add_argument("suite", type=Path)
        if action == "validate":
            command.add_argument("--sources", action="store_true")
        if action == "replay":
            command.add_argument("--artifacts", type=Path, help="new directory to retain build and proof files")
            command.add_argument("--observed", type=Path, help="write independent runtime observations")
        if action == "compare":
            command.add_argument("observed", type=Path)
    args = parser.parse_args()
    try:
        suite = load(args.suite)
        validate(suite, check_sources=args.action == "replay" or getattr(args, "sources", False))
        if args.action == "compare":
            compare(suite, load(args.observed))
        if args.action == "replay":
            from portable_qemu import replay
            observations = replay(suite, args.artifacts, digest(args.suite))
            if args.observed:
                save(args.observed, observations)
            compare(suite, observations)
        print(f"{args.action}: {counts(suite)}")
        return 0
    except (ValueError, OSError, RuntimeError) as error:
        print(f"portable: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())

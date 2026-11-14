#!/usr/bin/env python3
"""Split Splat function owners into configured, retail-checked windows."""

import collections
import re

import offline_decomp as worker
import inventory_functions as inventory

WORD = re.compile(
    r"^\s*/\*\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]{8})\s+([0-9a-fA-F]{8})\s*\*/\s*(.*?)\s*$"
)
LABEL = re.compile(
    r"^\s*(?:(?:glabel|dlabel|enddlabel|alabel|ehlabel|jlabel)\s+[\w.$]+|[\w.$]+:)\s*$"
)
HEADER = re.compile(
    r"^nonmatching\s+(func_[0-9A-Fa-f]{8}|entrypoint),\s*(0x[0-9a-fA-F]+)\s*$"
)


def is_control_transfer(word):
    op = word >> 26
    rs = (word >> 21) & 31
    fn = word & 63
    return (
        op in (1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23)
        or (op == 0 and fn in (8, 9))
        or (16 <= op <= 19 and rs == 8)
    )


def split(text, rows, retail, include_text, link_text=""):
    lines = text.splitlines(keepends=True)
    targets = worker.parse_targets(text)
    if len(targets) != len({t.name for t in targets}):
        raise ValueError("duplicate source target names")
    desired = []
    for r in rows:
        n = r["name"]
        a = int(r["address"], 0)
        z = int(r["size"], 0)
        if not (
            n == "entrypoint"
            and a == 0x80096520
            or re.fullmatch(r"func_[0-9A-F]{8}", n)
            and int(n[5:], 16) == a
        ):
            raise ValueError("bad target name/address")
        if z <= 0 or a % 4 or z % 4:
            raise ValueError("invalid size/alignment")
        desired.append((a, a + z, n))
    desired.sort()
    if len({n for a, b, n in desired}) != len(desired) or any(
        x[1] > y[0] for x, y in zip(desired, desired[1:])
    ):
        raise ValueError("duplicate or overlapping windows")
    groups = collections.defaultdict(list)
    for a, b, n in desired:
        owners = [t for t in targets if t.address <= a < b <= t.address + t.size]
        if len(owners) != 1:
            raise ValueError(f"{n}: missing or ambiguous owner")
        groups[owners[0].name].append((a, b, n))
    byname = {t.name: t for t in targets}
    hpos = {m[1]: i for i, l in enumerate(lines) if (m := HEADER.match(l.strip()))}
    reasons = {}
    jobs = []
    done = set()
    alias_warnings = []
    cut_aliases = []
    declarations = re.compile(
        r"(?m)^\s*(?:(dlabel|glabel|alabel|ehlabel|jlabel)\s+(D_[\w.$]+)|(D_[\w.$]+):)\s*$"
    )
    all_aliases = collections.defaultdict(list)
    for m in declarations.finditer(text + "\n" + include_text):
        all_aliases[m[2] or m[3]].append(m[1] or ":")
    # Explicit equates/common allocations are definitions too, not harmless references.
    definitions = re.compile(
        r"(?m)^\s*(?:\.(?:set|equ|equiv|comm|lcomm)\s+(D_[\w.$]+)\s*,|(D_[\w.$]+)\s*=)"
    )
    for m in definitions.finditer(text + "\n" + include_text):
        all_aliases[m[1] or m[2]].append("other definition")
    for owner, rs in groups.items():
        o = byname[owner]
        if len(rs) == 1 and (rs[0][0], rs[0][1], rs[0][2]) == (
            o.address,
            o.address + o.size,
            o.name,
        ):
            words = [
                (int(m[2], 16), m[3].upper())
                for line in o.assembly.splitlines()
                if (m := WORD.match(line))
            ]
            if [a for a, w in words] != list(range(o.address, o.address + o.size, 4)):
                raise ValueError(f"{owner}: invalid instruction boundaries")
            if any(
                retail[0x800 + a - 0x80010000 : 0x804 + a - 0x80010000].hex().upper()
                != w
                for a, w in words
            ):
                raise ValueError(f"{owner}: retail annotation mismatch")
            continue
        errors = []
        if rs[0][0] != o.address or rs[0][2] != owner:
            errors.append("owner label cannot be preserved as first function")
        gaps = []
        cursor = o.address
        for a, b, n in rs:
            if cursor < a:
                gaps.append((cursor, a))
            cursor = b
        if cursor < o.address + o.size:
            gaps.append((cursor, o.address + o.size))
        start = hpos[owner]
        end = next(
            i
            for i in range(start + 1, len(lines))
            if lines[i].strip() == f"endlabel {owner}"
        )
        body = lines[start + 1 : end]
        ws = []
        for l in body:
            m = WORD.match(l)
            if m:
                a = int(m[2], 16)
                w = m[3].upper()
                instr = m[4]
                ws.append((a, w, instr, l))
                if instr.startswith((".word", ".byte", ".short", ".ascii", ".incbin")):
                    errors.append("data directive in owner")
                off = 0x800 + a - 0x80010000
                if retail[off : off + 4].hex().upper() != w:
                    errors.append("retail annotation mismatch")
            elif l.strip() and not LABEL.fullmatch(l.strip()):
                errors.append("unrecognized owner directive")
        if [x[0] for x in ws] != list(range(o.address, o.address + o.size, 4)):
            errors.append("missing/duplicate/noncontiguous instructions")
        if sum(l.strip() == f"glabel {owner}" for l in body) != 1:
            errors.append("owner glabel ambiguity")
        wmap = {a: int.from_bytes(bytes.fromhex(w), "little") for a, w, ins, l in ws}
        # A D_ definition can coexist with a new function at a cut only when
        # its unique, understood zero-width definition is physically at that cut.
        # Never replace it, promote it to a function, or infer location from its name.
        at = collections.defaultdict(list)
        cursor = o.address
        for line in body:
            if m := declarations.fullmatch(line.strip()):
                at[cursor].append((m[2] or m[3], m[1] or ":"))
            if WORD.match(line):
                cursor += 4
        boundaries = sorted(
            {x for a, b, n in rs for x in (a, b) if o.address < x < o.address + o.size}
        )
        for a in boundaries:
            if a - 4 not in wmap or is_control_transfer(wmap[a - 4]):
                errors.append(f"cut in branch delay slot at 0x{a:08X}")
        for a, b, n in rs[1:]:
            expected = f"D_{a:08X}"
            here = {name for name, kind in at[a]}
            if expected in all_aliases and expected not in here:
                errors.append("D_ alias name defined away from cut")
            for name, kind in at[a]:
                if kind not in ("dlabel", "alabel", ":") or all_aliases[name] != [kind]:
                    errors.append("ambiguous or unsupported D_ alias at cut: " + name)
            if re.search(
                r"(?<![\w.$])" + re.escape(n) + r"(?:\.NON_MATCHING)?(?![\w.$])",
                text + "\n" + include_text + "\n" + link_text,
            ):
                errors.append("new symbol already referenced/defined: " + n)
        errors = sorted(set(errors))
        if errors:
            reasons[owner] = {
                "errors": errors,
                "targets": [n for a, b, n in rs],
                "gaps": [[f"0x{a:08X}", f"0x{b:08X}"] for a, b in gaps],
            }
            continue
        events = collections.defaultdict(list)
        for i, (a, b, n) in enumerate(rs):
            if i:
                events[a].append(f"nonmatching {n}, 0x{b-a:X}\nglabel {n}\n")
            if b < o.address + o.size:
                events[b].insert(0, f"endlabel {n}\n\n")
        indices = {}
        cursor = o.address
        for i in range(start + 1, end):
            if cursor in events and cursor not in indices:
                indices[cursor] = i
            if m := WORD.match(lines[i]):
                if int(m[2], 16) != cursor:
                    raise ValueError("cursor drift")
                cursor += 4
        if set(indices) != set(events):
            raise ValueError("missing cut")
        cut_aliases.extend(
            {"owner": owner, "new_owner": n, "alias": name, "kind": kind}
            for a, b, n in rs[1:]
            for name, kind in at[a]
        )
        alias_warnings.extend(
            {
                "owner": owner,
                "new_owner": n,
                "alias": name,
                "warning": ".aent ownership changes",
            }
            for a, b, n in rs[1:]
            for name, kind in at[a]
            if kind == "alabel"
        )
        jobs.append((start, end, owner, rs, events, indices, gaps))
        done.update(n for a, b, n in rs)
    out = list(lines)
    for start, end, owner, rs, events, indices, gaps in sorted(jobs, reverse=True):
        out[start] = f"nonmatching {owner}, 0x{rs[0][1]-rs[0][0]:X}\n"
        if out[end].strip() != f"endlabel {owner}":
            raise ValueError("owner endlabel changed")
        out[end] = (
            f"endlabel {rs[-1][2]}\n"
            if rs[-1][1] == byname[owner].address + byname[owner].size
            else ""
        )
        for addr, idx in sorted(indices.items(), key=lambda x: x[1], reverse=True):
            out[idx:idx] = ["".join(events[addr])]
    result = "".join(out)
    old_words = [l for l in lines if WORD.match(l)]
    new_words = [l for l in out if WORD.match(l)]
    if old_words != new_words:
        raise ValueError("original annotation lines changed")
    existing = [l for l in lines if LABEL.fullmatch(l.strip())]
    got = [l for l in out if LABEL.fullmatch(l.strip())]
    new_names = {n for _, _, _, rs, _, _, _ in jobs for _, _, n in rs[1:]}
    got = [
        l
        for l in got
        if not (l.strip().startswith("glabel ") and l.strip().split()[1] in new_names)
    ]
    if existing != got:
        raise ValueError("original label order/content changed")
    parsed = worker.parse_targets(result)
    before = {t.name: (t.address, t.size) for t in targets}
    after = {t.name: (t.address, t.size) for t in parsed}
    want = {n: (a, b - a) for a, b, n in desired}
    if len(parsed) != len(after):
        raise ValueError("duplicate parser names")
    missing = sorted(n for n in want if n not in after)
    wrong = sorted(n for n in want if n in after and want[n] != after[n])
    extras = sorted(set(after) - set(want))
    for n in done:
        if after.get(n) != want[n]:
            raise ValueError("rewritten target mismatch " + n)
    for n in set(before) - done:
        if before[n] != after.get(n):
            raise ValueError("untouched target changed " + n)
    try:
        inv = {"count": len(inventory.scan(result)), "error": None}
    except ValueError as e:
        inv = {"count": None, "error": str(e)}
    return result, {
        "input_count": len(before),
        "output_count": len(after),
        "wanted_count": len(want),
        "exact_count": sum(after.get(n) == v for n, v in want.items()),
        "missing": missing,
        "wrong_size_or_address": wrong,
        "extra": extras,
        "groups_rewritten": len(jobs),
        "rewritten_groups": {
            owner: {
                "owner_range": [
                    f"0x{byname[owner].address:08X}",
                    f"0x{byname[owner].address+byname[owner].size:08X}",
                ],
                "targets": [n for a, b, n in rs],
                "gaps": [[f"0x{a:08X}", f"0x{b:08X}"] for a, b in gaps],
            }
            for _, _, owner, rs, _, _, gaps in jobs
        },
        "rewritten_targets": len(done),
        "new_targets": len(new_names),
        "blocked_groups": reasons,
        "preserved_cut_aliases": cut_aliases,
        "alias_metadata_warnings": alias_warnings,
        "inventory": inv,
        "unchanged_annotated_lines": len(old_words),
        "preserved_original_label_lines": len(existing),
    }

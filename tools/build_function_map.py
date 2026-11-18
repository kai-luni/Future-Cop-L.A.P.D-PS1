#!/usr/bin/env python3
"""Generate docs/FUNCTION_MAP.md: a markdown map of the documented functions.

Scope is whatever is documented in the tree itself: every src/game/<addr>.c whose
first line is a `// Categories: ...` line and whose address is registered in
config/us/matching_units.json. Edges come from analysis/edges.json, direct calls
only, and solely between two documented functions.

Usage: python3 tools/build_function_map.py [--check]
       --check  regenerate in memory and fail if docs/FUNCTION_MAP.md is stale.
"""
import argparse
import collections
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
OUT = ROOT / 'docs' / 'FUNCTION_MAP.md'
CATEGORY_RE = re.compile(r'^// Categories: (.+)$')


def readable_name(text, retail):
    """Return the C name bound to *retail* by an asm-labelled prototype.

    Parameters may contain function pointers, so the argument list is
    balanced by hand instead of matched with a regular expression.
    """
    marker = f'__asm__("{retail}")'
    index = 0
    while True:
        index = text.find(marker, index)
        if index < 0:
            return None
        close = index
        while close > 0 and text[close - 1] in " \t\n":
            close -= 1
        if close > 0 and text[close - 1] == ')':
            depth = 0
            open_at = close - 1
            while open_at >= 0:
                if text[open_at] == ')':
                    depth += 1
                elif text[open_at] == '(':
                    depth -= 1
                    if depth == 0:
                        break
                open_at -= 1
            name = re.search(r'([A-Za-z_]\w*)\s*$', text[:open_at])
            if name:
                return name.group(1)
        index += len(marker)


def load():
    manifest = json.load(open(ROOT / 'config/us/matching_units.json'))
    man = {u['name']: u for u in manifest['units']}
    cats = {}
    cnames = {}
    for path in sorted((ROOT / 'src/game').glob('*.c')):
        name = 'func_' + path.stem.upper()
        if name not in man:
            continue
        text = path.read_text(errors='ignore')
        first = text.split('\n', 1)[0]
        m = CATEGORY_RE.match(first)
        if not m:
            continue
        cats[name] = [c.strip() for c in m.group(1).split('|') if c.strip()]
        alias = readable_name(text, name)
        if alias:
            cnames[name] = alias
    edges = [e for e in json.load(open(ROOT / 'analysis/edges.json'))['edges']
             if e.get('kind') == 'direct' and e.get('from') and e.get('to')]
    return man, cats, cnames, edges


def group_of(primary):
    toks = [t.strip() for t in primary.split('>')]
    area, subs = toks[0], set(toks[1:])
    if area in {'Game Engine', 'Input'}:
        return 'Boot & Front-End'
    if area == 'Utilities':
        return 'Utilities'
    if area in {'Platform', 'BIOS'}:
        return 'Platform'
    if area == 'Resources':
        return 'Video, MDEC & Movies' if 'Movie Playback' in subs else 'Memory & Resources'
    if area in {'Gameplay', 'AI', 'Physics'}:
        return 'Gameplay, AI & Physics'
    if area == 'Graphics':
        if 'GTE' in subs:
            return 'GTE Pipeline'
        if subs & {'Model Renderer', 'Renderer', 'Scene Graph', 'Lighting', 'Effects'}:
            return 'Renderer & Scene'
        return 'GPU, 2D & HUD'
    return 'Other'


ORDER = ['Boot & Front-End', 'Memory & Resources', 'Video, MDEC & Movies', 'GTE Pipeline',
         'Renderer & Scene', 'GPU, 2D & HUD', 'Gameplay, AI & Physics', 'Platform', 'Utilities',
         'Other']

SUMMARIES = {
 'Boot & Front-End':
   'Start, main loop and front-end. `func_80076A2C` is the **logical** entry point: the PS-X-EXE '
   'startup (PC0 `0x80096520`) clears `.bss`, computes the heap from `D_8009EA04`/`D_8009EA08`, '
   'sets `$gp` and calls it. Memory, file and image paths run from here, then the screen state '
   'machine on `D_8009FA00`.',
 'Memory & Resources':
   'Two allocators (block allocator at the bottom, mode allocator at the top), three pools, the four '
   'file slots (`D_800A6D78`) and the load/unpack paths. The rest of the program takes its buffers '
   'here; the CD-ROM and movie code feeds this layer.',
 'Video, MDEC & Movies':
   'MDEC tables, decode callbacks and movie/image output. Few internal edges because these functions '
   'are driven from the (not yet documented) playback path.',
 'GTE Pipeline':
   'Initialisation (`H`, `DQA`, `DQB`, `ZSF`), matrix loaders, batch transform, projection '
   '(RTPS/RTPT) and lighting (NCCS). The renderer requests this chain, the game code does not, which '
   'is why the edges point inwards.',
 'Renderer & Scene':
   'Dispatcher `func_80073700` (nodes with geometry go to `func_80071FA4`, empty ones to '
   '`func_80072F7C`), six layer lists in `func_8003F4A0` (order 2, 3, 0, 1, 4, 5), polygon pass '
   '`func_80075A84`, model entries `func_8006CB58`/`func_8006DC24`.',
 'GPU, 2D & HUD':
   'Packet and VRAM transfers, rectangles, sprites, fades, text and display state. The 2D layer '
   'works in half resolution; primitives come from `D_8009FA90`, 3D packets from `D_8009F9F0` '
   '(the last slot is reused, never allocated).',
 'Platform':
   'CD-ROM, interrupts, BIOS wrappers and memory card - constraints, not graphics code.',
 'Utilities':
   'Mathematics (vector length, fixed point, reciprocal tables) and the lagged-Fibonacci RNG over '
   '`D_800A0480`, used by the renderer and the effects.',
 'Gameplay, AI & Physics':
   'Entities, their message and script paths, motion, collision and targeting. These functions '
   'barely draw themselves; they move models, step animation frames and trigger effects, while '
   'the drawing runs through the renderer areas.',
 'Other': 'No unambiguous area match in the category line.',
}


def render():
    man, cats, cnames, edges = load()
    size_of = {n: int(man[n]['size'], 0) for n in cats}
    addr_of = {n: int(man[n]['address'], 16) for n in cats}
    name_of = {a: n for n, a in addr_of.items()}
    primary = {n: cats[n][0] for n in cats}
    extra = {n: [c.split('>')[-1].strip() for c in cats[n][1:]] for n in cats}

    callers = collections.defaultdict(set)
    callees = collections.defaultdict(set)
    for e in edges:
        f, t = e['from'], e['to']
        if f in name_of and t in name_of and f != t:
            callees[name_of[f]].add(name_of[t])
            callers[name_of[t]].add(name_of[f])
    for n in cats:
        callers[n], callees[n]

    groups = collections.defaultdict(list)
    for n in cats:
        groups[group_of(primary[n])].append(n)

    def nid(n):
        return 'f' + n[5:].lower()

    def node(n):
        return f'{nid(n)}["{n[5:].upper()}"]'

    def edge_block(pairs):
        out = ['```mermaid', 'graph LR']
        for a, b in sorted(pairs):
            out.append(f'    {nid(a)} --> {nid(b)}')
        for a in sorted({x for p in pairs for x in p}):
            out.append('    ' + node(a))
        out.append('```')
        return out

    def diagram(members):
        pairs = {(a, b) for a in members for b in callees[a] if b in members}
        if not pairs:
            return ['_No calls between documented functions of this area._']
        out = edge_block(pairs)
        isolated = [a for a in members if not any(a in p for p in pairs)]
        if isolated:
            out += ['', f'{len(isolated)} further functions of this area without a documented '
                        'connection (see the table below): ' +
                    ', '.join(f'`{a}`' for a in isolated) + '.']
        return out

    def bfs(start, depth=2, cap=30):
        seen, frontier = {start}, [start]
        for _ in range(depth):
            nxt = []
            for a in frontier:
                for b in sorted(callees[a]):
                    if b not in seen and len(seen) < cap:
                        seen.add(b)
                        nxt.append(b)
            frontier = nxt
        return seen

    total = len(cats)
    lines = [f'# Function map - documented functions ({total} units)\n',
             'Markdown map of every function whose source carries a `// Categories:` line: the '
             'category is the first line of each `src/game/<addr>.c`, sizes come from '
             '`config/us/matching_units.json`, edges from `analysis/edges.json`.\n',
             '**Reading the map.** Each node is a documented function; a renamed function shows its '
             'C name first and the retail address in parentheses. Edges are '
             '**direct calls** and only between two documented functions, so a missing edge means '
             '"the caller lies outside this set" or "not in the graph", never "no call". `jalr` '
             'calls, jump tables and `address_taken` edges are not drawn. The category column shows '
             'the full primary path, further tags of line 1 in short form after `·`.\n',
             'Regenerate with `python3 tools/build_function_map.py` (add `--check` in CI to detect a '
             'stale map); do not edit this file by hand.\n']

    gid = {g: f'g{i}' for i, g in enumerate(g for g in ORDER if groups[g])}
    gedges = collections.Counter()
    for a in cats:
        for b in callees[a]:
            g1, g2 = group_of(primary[a]), group_of(primary[b])
            if g1 != g2 and g1 in gid and g2 in gid:
                gedges[(g1, g2)] += 1
    lines += ['## Areas\n',
              'One box per area; the number on an edge is the count of direct calls between '
              'documented functions of those areas.\n', '```mermaid', 'graph LR']
    for g in gid:
        lines.append(f'    {gid[g]}["{g}<br/>{len(groups[g])} functions"]')
    for (g1, g2), cnt in sorted(gedges.items(), key=lambda kv: (-kv[1], kv[0])):
        lines.append(f'    {gid[g1]} -->|{cnt}| {gid[g2]}')
    lines += ['```', '']

    lines += ['## Areas in detail\n']
    for g in ORDER:
        members = groups.get(g)
        if not members:
            continue
        inner = sum(1 for a in members for b in callees[a] if b in members)
        used = {x for a in members for b in callees[a] if b in members for x in (a, b)}
        lines += [f'### {g} - {len(members)} functions, {inner} calls inside the area\n',
                  SUMMARIES[g] + '\n']
        if len(used) <= 30:
            lines += diagram(members) + ['']
        else:
            lines += ['The area is large, so only the connected functions are drawn; the table below '
                      'lists every member.\n'] + diagram(members) + ['']
        lines += ['| Function | Bytes | Categories | calls (documented) | called by (documented) |',
                  '|---|---|---|---|---|']
        for a in sorted(members, key=lambda x: (size_of[x], x)):
            cs = ', '.join(f'`{b}`' for b in sorted(callees[a])) or '-'
            rs = ', '.join(f'`{b}`' for b in sorted(callers[a])) or '-'
            ex = f' · {", ".join(extra[a])}' if extra[a] else ''
            label = f'`{cnames[a]}` (`{a}`)' if a in cnames else f'`{a}`'
            lines.append(f'| {label} | {size_of[a]} | {primary[a]}{ex} | {cs} | {rs} |')
        lines += ['']

    lines += ['## Rendering core chain\n',
              'Breadth-first walk from `func_8003F4A0` (render pass driver) **and** `func_80073700` '
              '(object dispatcher), two levels deep over documented calls: the path a frame takes.\n']
    chain = bfs('func_8003F4A0') | bfs('func_80073700') if 'func_80073700' in cats else bfs('func_8003F4A0')
    lines += edge_block({(a, b) for a in chain for b in callees[a] if b in chain})
    lines += ['', 'Stage order (measured): matrix load (`func_80010750` / `func_80010820`) - '
                  'batch transform (`func_8001006C` / `func_80010234`) - projection '
                  '(`func_800102A0` / `func_8001047C`) - lighting (`func_80015F14`) - packet from '
                  '`D_8009F9F0`.', '']

    roots = sorted(a for a in cats if not callers[a])
    leaves = sorted(a for a in cats if not callees[a])
    lines += ['## Limits of the map\n']

    def shortlst(items, limit=16):
        s = ', '.join(f'`{a}`' for a in items[:limit])
        return s + (f' ... (+{len(items) - limit})' if len(items) > limit else '')

    lines += [f'- **No documented caller ({len(roots)}):** ' + shortlst(roots) + '',
              f'- **No documented call ({len(leaves)}):** ' + shortlst(leaves) + '',
              'These are entry points and leaves of the documented part; their counterparts lie '
              'outside this set or are reached indirectly.\n',
              '- **Most called (hubs):**\n',
              '| Function | calls (documented) | callers (documented) | area |',
              '|---|---|---|---|']
    for a in sorted(cats, key=lambda x: (-len(callees[x]), x))[:12]:
        lines.append(f'| `{a}` | {len(callees[a])} | {len(callers[a])} | {group_of(primary[a])} |')
    lines.append('')
    return '\n'.join(lines) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true')
    args = ap.parse_args()
    text = render()
    if args.check:
        current = OUT.read_text() if OUT.exists() else ''
        if current != text:
            print('docs/FUNCTION_MAP.md is stale; run tools/build_function_map.py', file=sys.stderr)
            return 1
        print('docs/FUNCTION_MAP.md is up to date')
        return 0
    OUT.write_text(text)
    print(f'wrote {OUT.relative_to(ROOT)}: {len(text)} chars')
    return 0


if __name__ == '__main__':
    sys.exit(main())

# Future Cop: L.A.P.D. (USA) — matching decompilation

[![Code](https://img.shields.io/badge/code-555576%2F555576%20(100%25)-brightgreen)](#current-state)
[![Functions](https://img.shields.io/badge/functions-1771%2F1771-brightgreen)](#current-state)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

A **complete, byte-exact C reconstruction** of the North American PlayStation
release of *Future Cop: L.A.P.D.* (`SLUS-00739`). Every one of the 1,771 functions
in the boot executable now recompiles to retail-identical bytes — **100 % of the
code, 555,576 / 555,576 bytes**.

This is a fork of [TheGh0stShip/Future-Cop-L.A.P.D-PS1](https://github.com/TheGh0stShip/Future-Cop-L.A.P.D-PS1),
which contributed the original scaffold and 522 matched functions (51,512 bytes).
The fork carried the reconstruction from that base to a full match, largely through
an automated, compiler-guided LLM campaign (see [the campaign](#the-llm-reconstruction-campaign)).

The target is a user-supplied `Future Cop - L.A.P.D. (USA).cue` / `.bin` (Redump
image). **No retail game data belongs in this repository** — retail inputs,
extracted files, assembly, toolchains and build output are all git-ignored.

## Current state

| Measure | Value |
| --- | --- |
| Functions matched | **1,771 / 1,771 (100 %)** |
| Code bytes matched | **555,576 / 555,576 (100 %)** |
| Reconstructed C source | 103,043 lines across 1,771 units |
| Retail instruction words | 138,894 |
| Executable data | 32,224 bytes declared (404 bytes of padding recipes verified) |

The measurement scope is **code-only** (`config/project.json`: `assets.measure =
false`). Disc assets — movies, missions, level scripts, audio, configuration —
carry retail game data and are deliberately **not** reconstructed; they are excluded
from the progress denominator. The executable's non-code sections (`.rodata`,
`.data`, jump tables, constants) are declared as retail data ranges in
`config/us/executable_data_ranges.json` and checked by `verify_noncode.py`; they are
not yet rebuilt as C.

The function catalogue was refined from the upstream 1,855 ranges to **1,771** through
boundary corrections (merging analyzer-split prologue stubs, removing a handful of
non-function data remnants). The code denominator moved from 555,552 to 555,576 bytes
when a few leading instructions were reassigned to their true functions.

```sh
make setup            # venv + toolchain + retail executable check
make verify-matching  # compile every unit, compare to retail bytes
make verify-noncode   # check the declared data ranges
make report           # write build/report.json (objdiff v2)
make check            # the full pipeline above, in order
```

The Makefile expects the disc image in `Future Cop - L.A.P.D. (USA)/`. Matching
source lives under `src/game/`; each unit's address, size, compiler profile and
symbols are declared in `config/us/matching_units.json`. Only units that pass the
retail-byte verifier count as matched — provisional assembly, static-recompiler
output and boundary guesses never award progress.

## The LLM reconstruction campaign

The 1,249 functions added on top of the upstream base were reconstructed through a
mix of automated LLM campaigns, hand work and free compiler sweeps. The automated
queue (`fcq`) alone attributes 685 of them to specific models: each model proposed C,
the acceptance gate compiled it with the pinned PSX toolchain and compared the bytes
against retail, and only exact matches were promoted into the manifest.

The table below is computed from the queue's attempt ledger
(`build/us/fcq/fcq.sqlite`). **Tokens** are all recorded runs of a channel
(including failed attempts); **functions solved** are units that channel closed
byte-exact and that passed the gate into the manifest (attributed to the promoting
run, deduplicated per function). **Avg words** is the retail MIPS instruction count
(bytes / 4); **avg lines** is the size of the reconstructed C source.

| LLM | Harness channels | Functions solved | Avg words | Avg lines | Tokens | Runs |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| GPT-6.1 Terra | terra-cc · terra-az · codex-terra | 299 | 97.4 | 63.2 | 1,354,383,801 | 790 |
| DeepSeek V4.1 Flash | deepseek-cc | 68 | 84.0 | 64.7 | 1,023,165,698 | 249 |
| MiniMax M3 | kilo-minimax · go-minimax | 30 | 79.1 | 71.3 | 507,865,822 | 80 |
| Mistral Large / Vibe | vibe-pro · mistral-cc · … | 10 | 120.9 | 83.5 | 429,308,035 | 64 |
| Claude Sonnet | sonnet-high · sonnet-medium | 65 | 159.9 | 109.2 | 331,385,364 | 286 |
| GPT-6.1 Sol | sol · codex-sol | 62 | 262.1 | 154.9 | 266,735,380 | 116 |
| Claude Opus | opus-high | 24 | 300.8 | 185.2 | 263,680,982 | 98 |
| Qwen | qwen-flash · qwen4-max · … | 6 | 102.2 | 77.8 | 193,338,514 | 44 |
| Tencent HY4 | hy4-cc · go-hy4 | 1 | 129.0 | 90.0 | 76,450,091 | 20 |
| Gemini 3.8 Flash | gemini-flash | 88 | 91.6 | 80.1 | 67,979,068 | 139 |
| Gemini 4 Argon | gemini-argon · go-argon | 20 | 182.6 | 159.4 | 50,486,439 | 33 |
| Gemini 4 Hydrogen | gemini-hydrogen | 2 | 190.0 | 127.5 | 5,187,411 | 9 |
| Kimi | go-kimi | 1 | 17.0 | 35.0 | 0 | 0 |

**Total: ≈4.57 billion tokens across 1,928 recorded runs; 676 functions attributed to
a named model above** (685 including a few hand-promoted runs with no model tag).

What the numbers show:

- **GPT-6.1 Terra was the workhorse** — 299 functions, more than any other model,
  spread across three harnesses (AI/ML API, Azure, and the Codex subscription).
- **Gemini 3.8 Flash was the most token-efficient**: 88 functions for ~68 M tokens
  (~0.77 M per solve), almost all small targets.
- **Claude Opus and GPT-6.1 Sol closed the hardest functions** — average ~300 and
  ~262 words respectively. Large near-misses that cheaper models plateaued on were
  escalated to them, often closed in a single run.
- **DeepSeek V4.1 Flash cost the most per solve** (~15 M tokens/function) through the
  Claude Code harness, whose tool surface adds overhead.

### Scope and honesty notes

- The ledger covers the **automated queue from 2026-11-01 onward**. The remaining
  ~1,086 manifest functions are **not** per-model token-attributed there: the 522
  upstream base functions, hand reconstruction, Hermes subagents, foreign-model
  briefs (e.g. ChatGPT web), and free compiler-matrix sweeps. They are real matched
  code, just outside this table's attribution.
- "Solved" means **gate-promoted byte-exact**, not a high word score. A model that
  delivered a 99 %-match which never passed the gate is not counted.
- Token counts are the queue's recorded usage; subscription channels report 0 USD,
  so tokens (not cost) are the comparison unit.

## How matching works

A unit is matched only when its C, compiled with the **exact** pinned toolchain,
produces byte-identical output to the retail executable at that function's address.
The pipeline (Splat + MASPSX + pinned psy-q GCC) is unforgiving about details that
ordinary decompilation ignores:

- **Five pinned compilers** appear in one retail executable, chosen per unit:
  `gcc-2.7.2-cdk` (1,056 units), `gcc-2.7.2-psx` (218), `gcc-2.8.1-psx` (200),
  `gcc-2.6.3-psx` (131), `gcc-2.8.0-psx` (47); 119 hardware units are hand-written
  inline assembly. The compiler family alone decides epilogue order, register
  allocation tie-breaks and branch polarity.
- **MASPSX** (pinned commit) translates GCC's GNU-as output into the psy-q assembler
  dialect; per-unit switches such as `--expand-div` reproduce the original
  checked-division guard, and `aspsx_*` keys patch assembler scheduling quirks.
- **`$gp` / small-data** layout (`-G` thresholds, declaration order) must match for
  gp-relative loads to land on the right offsets.
- **The link path** resolves internal `j`/`jal` targets and `%hi/%lo` relocations;
  many "one word off" candidates become exact with `link: true` and the right
  symbol set, unchanged C.

The acceptance gate (`tools/offline_decomp.py preview` / `promote`,
`tools/verify_matching_units.py`) compiles each candidate and compares bytes,
accepting only the deterministic 16-byte `.text` zero-fill as padding. Jump-table
units are checked against `.rodata` too. A free **compiler matrix** sweep (every
pinned GCC × `-O1/-O2/-O3` × `-G` × link × maspsx) closes near-misses without any
model call and is always run before spending tokens.

The 12 hardware functions that no C can reach (GTE `cop2` operations and BIOS `$t2`
dispatch stubs) are transcribed as inline assembly / `.word` data — the GTE operation
words are emitted verbatim because the generic assembler does not know those
mnemonics.

See [`docs/OFFLINE_DECOMP.md`](docs/OFFLINE_DECOMP.md) for the resumable, parallel
candidate-search worker, and [`docs/RESEARCH.md`](docs/RESEARCH.md) for the research
log. [`docs/FUNCTION_MAP.md`](docs/FUNCTION_MAP.md) is the generated architecture map
of the functions whose source carries a category line (`tools/build_function_map.py`).
[`docs/video-loading-path.md`](docs/video-loading-path.md) traces the movie path from
the file open through the CD stream, the MDEC slice decode and the audio clock pacing.

## Repository layout

```
src/game/                 reconstructed C / inline-asm units (one file per function)
config/us/
  matching_units.json     the manifest: address, size, compiler profile, symbols
  executable_data_ranges.json   declared non-code data ranges
  function_boundary_overrides.json   audited catalogue corrections
  splat.yaml              Splat split configuration + generated linker script
asm/us/main.s             Splat split of the retail executable (git-ignored output)
tools/                    verifier, report generator, acceptance gate, sweeps
build/us/fcq/             the job queue (SQLite ledger, work dirs, briefs)
docs/                     worker documentation and research log
```

## Building and verifying

1. Provide the Redump disc image as `Future Cop - L.A.P.D. (USA)/Future Cop - L.A.P.D. (USA).{cue,bin}`.
2. `make setup` builds the venv, bootstraps the pinned toolchain (five psy-q GCCs,
   MASPSX, mipsel-binutils) and verifies the executable against its pinned SHA-1/SHA-256.
3. `make check` runs split → inventory → verify-matching → verify-noncode → report.

Retail inputs, extracted files, assembly, toolchains and build output are ignored;
a fresh clone needs the disc image and a toolchain bootstrap before the byte checks
can run.

## Credits and prior art

- **[TheGh0stShip/Future-Cop-L.A.P.D-PS1](https://github.com/TheGh0stShip/Future-Cop-L.A.P.D-PS1)** —
  the upstream scaffold and the first 522 matched functions this fork builds on.
- Prior art surveyed at the start: `alexbeavs-ps1-ports/future-cop-lapd-recomp` (a
  European SLES-01449 static recompilation, useful structurally but not byte-identical
  to this USA target) and `Ghoster738/Future-Cop-MIT` (an incomplete engine
  reimplementation). Neither contributes matching bytes here.

## License

[MIT](LICENSE). The reconstruction is independent C source; no retail game data is
distributed in this repository.
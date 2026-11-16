# GPU port pointer globals

Two globals in Future Cop: L.A.P.D. (US) hold the addresses of the PlayStation
GPU ports. They are ordinary writable pointer variables in the retail image, not
hardware registers themselves, and the game only stores them once at reset.

| retail symbol | C name | slot address | initial value | hardware target |
| --- | --- | --- | --- | --- |
| `D_8009DEC4` | `gGpuGp0PortPtr` | `0x8009DEC4` | `0x1F801810` | GP0 command port / GPUREAD |
| `D_8009DEC8` | `gGpuGp1PortPtr` | `0x8009DEC8` | `0x1F801814` | GP1 command port / GPUSTAT |

Per-variable detail, evidence and function lists:
[`D_8009DEC4.md`](D_8009DEC4.md), [`D_8009DEC8.md`](D_8009DEC8.md).

## Two different accesses

* `gGpuGp1PortPtr` — reading the **variable** loads four bytes from RAM at
  `0x8009DEC8` and yields `0x1F801814`. It has no hardware side effect. In the
  retail listing this is always a `lui`/`lw` pair on the symbol.
* `*gGpuGp1PortPtr` — dereferencing **accesses the register** at `0x1F801814`:
  reading returns GPUSTAT, writing issues a GP1 command.

No code in the repository writes to either slot; `analysis/refs.json` records
only 4-byte loads of `0x8009DEC4` (14 in 5 functions) and `0x8009DEC8` (34 in
18 functions). All GPU traffic goes through the dereferenced pointer.

## How the names are bound

`include/game/gpu_globals.h` defines only the assembler labels:

```c
#define GPU_GP0_PORT_ASM "D_8009DEC4"
#define GPU_GP1_PORT_ASM "D_8009DEC8"
```

Each using translation unit declares the pointer with its own qualification and
binds it to the retail symbol:

```c
extern volatile u32 *gGpuGp0PortPtr __asm__("D_8009DEC4");
```

The GCC 2.8.1-psx stage emits the retail symbol in every `%hi`/`%lo` reference,
so the object code is unchanged.

**Do not define an object for either name.** The four retail bytes live in
`asm/us/main.s` (`dlabel`/`.word`), the whole payload is a single `.text` blob,
and the generated linker script discards everything except `main.s.o`. A C
definition would silently land at a different address (measured: directly after
`.text`), and inside the same translation unit it would override the absolute
symbol and shift the load to the wrong address. Moving the data into C needs a
segment split, an absolute placement rule in the linker script, removal of the
`.set` alias and a byte comparison — none of that is in place today.

## Why the declarations are not all in the header

The units need different qualification: a volatile pointee in most, a volatile
pointer in `func_8009446C`, a plain pointer in the rest. A single shared type
changes code generation. Measured on the 18 affected units: one shared
`volatile` declaration kept 14/18 byte-exact, one shared non-volatile
declaration 11/18. With the historical per-unit types all 18 stay identical,
therefore the header carries the binding and each unit keeps its own type.

## Inline assembly

Hand-written assembly keeps the retail names (`%hi(D_8009DEC8)`,
`%lo(D_8009DEC8)`), because GCC does not rewrite symbol names inside asm
strings: `src/game/80094CEC.c` and `src/game/8009520C.c`. Renaming there would
break the reference; the labels above only exist for C declarations.

## Verifying a change

The pipeline compiles each unit and compares `.text` bytes with the retail
range; `python3 tools/verify_matching_units.py` runs the whole catalogue. For a
quick check of only the units that touch these globals:

```
export PATH=$PWD/.toolchain/mipsel-binutils/root/usr/bin:$PATH
export LD_LIBRARY_PATH=$PWD/.toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu
python3 tools/verify_matching_units.py
```

Any change to a declaration's `volatile` or pointer qualification must be
re-measured: it can add or remove loads of the slot even though the symbol
address is unchanged.

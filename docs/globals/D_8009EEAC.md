# D_8009EEAC — DMA channel 4 block control pointer

## Identity

| item | value |
| --- | --- |
| retail symbol | `D_8009EEAC` |
| C name | `gDma4BlockCtrlPtr` |
| slot address | `0x8009EEAC` (4 bytes, writable, inside the payload's `.text`) |
| initial value | `0x1F8010C4` |
| points at | DMA channel 4 block control register (BCR4) |
| assembler label | `DMA4_BLOCK_CTRL_ASM` in `include/game/dma_globals.h` |

## Retail evidence

`asm/us/main.s`:

```
dlabel D_8009EEAC
    /* 8F6AC 8009EEAC C410801F */ .word 0x1F8010C4
```

File offset `0x8F6AC` of `SLUS_007.39` holds `C4 10 80 1F`, i.e. `0x1F8010C4`.

## Hardware meaning

`0x1F8010C4` is `DMA4_BCR`, the block control register of the SPU channel. The
unit writes `arg2 | 0x10`: the caller supplies the block count in the high half
and the transfer length in the low bits, with `0x10` added. The exact split
between size and count is **not** provable from this unit alone; only the
written expression is measured.

## Correct C type

| unit | declaration |
| --- | --- |
| `func_800973B4` | `u32 *` |
| `func_8009745C` | `s32 *` |

## Users

* `func_800973B4` — programs the block control word before starting the SPU
  transfer.
* `func_8009745C` — register-write helper that mentions the pointer.

`analysis/refs.json` records the slot loads; the slot itself is never written.

## Accessing the variable vs. the register

```c
*gDma4BlockCtrlPtr = arg2 | 0x10;  /* programs BCR4 */
u32 addr = (u32)gDma4BlockCtrlPtr; /* 0x1F8010C4    */
```

## C integration status

Renamed from `D_8009EEAC` to `gDma4BlockCtrlPtr` in its two C users; the retail
symbol is kept with `__asm__("D_8009EEAC")`. No object is defined, the retail
word stays in `asm/us/main.s`, and both units still compile to the exact retail
bytes (SHA-256 compared before and after the rename).
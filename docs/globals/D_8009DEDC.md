# D_8009DEDC — DMA channel 6 block control pointer

## Identity

| item | value |
| --- | --- |
| retail symbol | `D_8009DEDC` |
| C name | `gDma6BlockCtrlPtr` |
| slot address | `0x8009DEDC` (4 bytes, writable, inside the payload's `.text`) |
| initial value | `0x1F8010E4` |
| points at | DMA channel 6 block control register (BCR6) |
| assembler label | `DMA6_BLOCK_CTRL_ASM` in `include/game/dma_globals.h` |

## Retail evidence

`asm/us/main.s`:

```
dlabel D_8009DEDC
    /* 8E6DC 8009DEDC E410801F */ .word 0x1F8010E4
```

File offset `0x8E6DC` of `SLUS_007.39` holds `E4 10 80 1F`, i.e. `0x1F8010E4`.

## Hardware meaning

`0x1F8010E4` is `DMA6_BCR`, the block control register of the ordering-table
clear channel. This unit writes the raw element count (`arg1`) — the channel is
used in a mode where the value is not split into size and count halves, which
matches the backwards walk programmed in the address register.

## Correct C type

`u32 *`.

## Users

* `func_8009415C` — writes the element count between the address and the
  channel control word.

`analysis/refs.json` records the slot loads; the slot itself is never written.

## Accessing the variable vs. the register

```c
*gDma6BlockCtrlPtr = count;        /* programs BCR6 */
u32 addr = (u32)gDma6BlockCtrlPtr; /* 0x1F8010E4    */
```

## C integration status

Renamed from `D_8009DEDC` to `gDma6BlockCtrlPtr` in its single user; the retail
symbol is kept with `__asm__("D_8009DEDC")`. No object is defined, the retail
word stays in `asm/us/main.s`, and the unit still compiles to the exact retail
bytes (SHA-256 compared before and after the rename).
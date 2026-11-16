# D_8009CBAC — DMA channel 3 block control pointer

## Identity

| item | value |
| --- | --- |
| retail symbol | `D_8009CBAC` |
| C name | `gDma3BlockCtrlPtr` |
| slot address | `0x8009CBAC` (4 bytes, writable, inside the payload's `.text`) |
| initial value | `0x1F8010B4` |
| points at | DMA channel 3 block control register (BCR3) |
| assembler label | `DMA3_BLOCK_CTRL_ASM` in `include/game/dma_globals.h` |

## Retail evidence

`asm/us/main.s`:

```
dlabel D_8009CBAC
    /* 8D3AC 8009CBAC B410801F */ .word 0x1F8010B4
```

File offset `0x8D3AC` of `SLUS_007.39` holds `B4 10 80 1F`, i.e. `0x1F8010B4`.

## Hardware meaning

`0x1F8010B4` is `DMA3_BCR`, the block control register of the CD-ROM channel:
low half = block size in words, high half = block count. The sector path writes
`size | 0x10000`, i.e. one block whose low half carries the size.

## Correct C type

`volatile u32 *` in both users.

## Users

* `func_8008FC30` — `*ptr = size | 0x10000`.
* `func_8008FD30` — same programming in the variant path.

`analysis/refs.json` records the slot loads; the slot itself is never written.

## Accessing the variable vs. the register

```c
*gDma3BlockCtrlPtr = size | 0x10000;  /* programs BCR3 */
u32 addr = (u32)gDma3BlockCtrlPtr;    /* 0x1F8010B4    */
```

## C integration status

Renamed from `D_8009CBAC` to `gDma3BlockCtrlPtr` in its two C users; the retail
symbol is kept with `__asm__("D_8009CBAC")`. No object is defined, the retail
word stays in `asm/us/main.s`, and both units still compile to the exact retail
bytes (SHA-256 compared before and after the rename).
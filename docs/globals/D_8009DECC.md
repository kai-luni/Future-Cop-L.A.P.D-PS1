# D_8009DECC — DMA channel 2 memory address pointer

## Identity

| item | value |
| --- | --- |
| retail symbol | `D_8009DECC` |
| C name | `gDma2MemAddrPtr` |
| slot address | `0x8009DECC` (4 bytes, writable, inside the payload's `.text`) |
| initial value | `0x1F8010A0` |
| points at | DMA channel 2 memory address register (MADR2) |
| assembler label | `DMA2_MEM_ADDR_ASM` in `include/game/dma_globals.h` |

## Retail evidence

`asm/us/main.s`:

```
nonmatching D_8009DECC

dlabel D_8009DECC
    /* 8E6CC 8009DECC A010801F */ .word 0x1F8010A0
```

File offset `0x8E6CC` of `SLUS_007.39` holds `A0 10 80 1F`, i.e. `0x1F8010A0`.
The slot precedes the block control (`0x1F8010A4`) and channel control
(`0x1F8010A8`) pointers of the same channel.

Store through the pointer, `func_800946A8` at `0x8009464C`:

```
    /* 84E4C 8009464C 0A80023C */  lui        $v0, %hi(D_8009DECC)
    /* 84E50 80094650 CCDE428C */  lw         $v0, %lo(D_8009DECC)($v0)
    /* 84E54 80094654 0001043C */  lui        $a0, (0x1000201 >> 16)
    /* 84E58 80094658 000052AC */  sw         $s2, 0x0($v0)
```

## Hardware meaning

`0x1F8010A0` is `DMA2_MADR`, the memory address register of DMA channel 2 (the
GPU channel). It is the RAM address a transfer reads from or writes to; the
direction is chosen by the channel control register. Channel numbering follows
public PlayStation documentation.

## Correct C type

`volatile u32 *` — pointer to a volatile 32-bit register that holds a RAM
address.

| unit | declaration |
| --- | --- |
| `func_8009446C`, `func_800946A8`, `func_8009520C` | `volatile u32 *` |
| `func_800949A0` | `u32 *` |

## Users

* `func_8009446C` — `*gDma2MemAddrPtr = (u32)data;` before starting a GPU
  upload.
* `func_800946A8` — `*gDma2MemAddrPtr = (u32)out;` before a GPU read-back.
* `func_800949A0` — writes the caller's address before a packet based transfer.
* `func_8009520C` — reads the register value and passes it to `func_8008E000`
  (a diagnostic dump), so this is a read of the hardware, not of a transfer
  setup.

`analysis/refs.json` lists 4 references to the slot, all 4-byte loads; the slot
itself is never written.

## Accessing the variable vs. the register

```c
*gDma2MemAddrPtr = (u32)buffer;   /* programs DMA2_MADR            */
u32 addr = (u32)gDma2MemAddrPtr;  /* 0x1F8010A0, no side effect    */
```

## C integration status

Renamed from `D_8009DECC` to `gDma2MemAddrPtr` in the four C users; the retail
symbol is kept with `__asm__("D_8009DECC")`. No object is defined, the retail
word stays in `asm/us/main.s`, and all four units still compile to the exact
retail bytes (SHA-256 compared before and after the rename).

# Named hardware pointer globals

Several globals in Future Cop: L.A.P.D. (US) hold the addresses of hardware
registers. They are ordinary writable pointer variables in the retail image, not
registers themselves: the game stores the address once at startup and writes and
reads through the pointer afterwards.

| retail symbol | C name | slot address | initial value | points at |
| --- | --- | --- | --- | --- |
| `D_8009DEC4` | `gGpuGp0PortPtr` | `0x8009DEC4` | `0x1F801810` | GP0 command port / GPUREAD |
| `D_8009DEC8` | `gGpuGp1PortPtr` | `0x8009DEC8` | `0x1F801814` | GP1 command port / GPUSTAT |
| `D_8009DECC` | `gDma2MemAddrPtr` | `0x8009DECC` | `0x1F8010A0` | DMA2 memory address (GPU) |
| `D_8009DED0` | `gDma2BlockCtrlPtr` | `0x8009DED0` | `0x1F8010A4` | DMA2 block control (GPU) |
| `D_8009DD60` | `gDmaInterruptCtrlPtr` | `0x8009DD60` | `0x1F8010F4` | DMA interrupt control (DICR) |
| `D_8009DD1C` | `gDmaControlPtr` | `0x8009DD1C` | `0x1F8010F0` | DMA control (DPCR) |
| `D_8009C788` | `gInterruptStatusPtr` | `0x8009C788` | `0x1F801070` | interrupt status (I_STAT) |
| `D_8009DD18` | `gInterruptMaskPtr` | `0x8009DD18` | `0x1F801074` | interrupt mask (I_MASK) |
| `D_8009C78C` | `gSio0RegBasePtr` | `0x8009C78C` | `0x1F801040` | SIO0 register block (pad/memory card) |
| `D_8009CBB0` | `gDma3ChannelCtrlPtr` | `0x8009CBB0` | `0x1F8010B8` | DMA3 channel control (CD-ROM) |
| `D_8009E9D4` | `gDma1ChannelCtrlPtr` | `0x8009E9D4` | `0x1F801098` | DMA1 channel control (MDEC input) |
| `D_8009DED4` | `gDma2ChannelCtrlPtr` | `0x8009DED4` | `0x1F8010A8` | DMA2 channel control (GPU) |
| `D_8009EEA4` | `gSpuRegBasePtr` | `0x8009EEA4` | `0x1F801C00` | SPU register block (voice 0 base) |
| `D_8009CB6C` | `gCdromIndexStatusPtr` | `0x8009CB6C` | `0x1F801800` | CD-ROM index/status port |
| `D_8009CB78` | `gCdromRequestStatusPtr` | `0x8009CB78` | `0x1F801803` | CD-ROM request / IRQ flag port |
| `D_8009CB70` | `gCdromCommandPtr` | `0x8009CB70` | `0x1F801801` | CD-ROM command / response port |
| `D_8009CB74` | `gCdromParameterPtr` | `0x8009CB74` | `0x1F801802` | CD-ROM parameter / IRQ enable port |
| `D_8009DD14` | `gInterruptStatusHalfPtr` | `0x8009DD14` | `0x1F801070` | interrupt status, halfword view |
| `D_8009DEE4` | `gDmaControlGpuPtr` | `0x8009DEE4` | `0x1F8010F0` | DMA control (DPCR), GPU path |
| `D_8009EEB4` | `gSpuDelayCtrlPtr` | `0x8009EEB4` | `0x1F8010F0` | DMA control (DPCR), SPU driver |
| `D_8009E9C8` | `gDma0ChannelCtrlPtr` | `0x8009E9C8` | `0x1F801088` | DMA0 channel control (MDEC output) |
| `D_8009EEB8` | `gSpuAccessDelayPtr` | `0x8009EEB8` | `0x1F801014` | SPU access delay |
| `D_8009CBA0` | `gCdromAccessDelayPtr` | `0x8009CBA0` | `0x1F801018` | CD-ROM access delay |
| `D_8009CB7C` | `gComAccessDelayPtr` | `0x8009CB7C` | `0x1F801020` | common access delay |
| `D_8009DD84` | `gDmaChannelRegBasePtr` | `0x8009DD84` | `0x1F801080` | DMA channel block base |
| `D_8009E9C0` | `gDma0MemAddrPtr` | `0x8009E9C0` | `0x1F801080` | DMA0 memory address |
| `D_8009E9C4` | `gDma0BlockCtrlPtr` | `0x8009E9C4` | `0x1F801084` | DMA0 block control |
| `D_8009E9CC` | `gDma1MemAddrPtr` | `0x8009E9CC` | `0x1F801090` | DMA1 memory address |
| `D_8009E9D0` | `gDma1BlockCtrlPtr` | `0x8009E9D0` | `0x1F801094` | DMA1 block control |
| `D_8009CBA8` | `gDma3MemAddrPtr` | `0x8009CBA8` | `0x1F8010B0` | DMA3 memory address (CD-ROM) |
| `D_8009CBAC` | `gDma3BlockCtrlPtr` | `0x8009CBAC` | `0x1F8010B4` | DMA3 block control (CD-ROM) |
| `D_8009EEA8` | `gDma4MemAddrPtr` | `0x8009EEA8` | `0x1F8010C0` | DMA4 memory address (SPU) |
| `D_8009EEAC` | `gDma4BlockCtrlPtr` | `0x8009EEAC` | `0x1F8010C4` | DMA4 block control (SPU) |
| `D_8009EEB0` | `gDma4ChannelCtrlPtr` | `0x8009EEB0` | `0x1F8010C8` | DMA4 channel control (SPU) |
| `D_8009DED8` | `gDma6MemAddrPtr` | `0x8009DED8` | `0x1F8010E0` | DMA6 memory address (OTC) |
| `D_8009DEDC` | `gDma6BlockCtrlPtr` | `0x8009DEDC` | `0x1F8010E4` | DMA6 block control (OTC) |
| `D_8009DEE0` | `gDma6ChannelCtrlPtr` | `0x8009DEE0` | `0x1F8010E8` | DMA6 channel control (OTC) |
| `D_8009CC1C` | `gTimer1CounterPtr` | `0x8009CC1C` | `0x1F801110` | timer 1 counter |
| `D_8009DD54` | `gTimer1ModePtr` | `0x8009DD54` | `0x1F801114` | timer 1 mode |
| `D_8009E9F4` | `gMdecControlStatusPtr` | `0x8009E9F4` | `0x1F801824` | MDEC control/status word |

Per-variable detail, evidence and function lists:
[`D_8009DEC4.md`](D_8009DEC4.md), [`D_8009DEC8.md`](D_8009DEC8.md),
[`D_8009DECC.md`](D_8009DECC.md), [`D_8009DED0.md`](D_8009DED0.md),
[`D_8009DED4.md`](D_8009DED4.md), [`D_8009DD60.md`](D_8009DD60.md),
[`D_8009DD1C.md`](D_8009DD1C.md), [`D_8009C788.md`](D_8009C788.md),
[`D_8009DD18.md`](D_8009DD18.md), [`D_8009C78C.md`](D_8009C78C.md),
[`D_8009CBB0.md`](D_8009CBB0.md), [`D_8009E9D4.md`](D_8009E9D4.md),
[`D_8009EEA4.md`](D_8009EEA4.md), [`D_8009CB6C.md`](D_8009CB6C.md), [`D_8009CB78.md`](D_8009CB78.md), [`D_8009E9F4.md`](D_8009E9F4.md).

## Two different accesses

* reading the **variable** (for example `gGpuGp1PortPtr`) loads four bytes from
  RAM and yields the register address. No hardware side effect; the retail
  listing always shows a `lui`/`lw` pair on the symbol.
* dereferencing it (`*gGpuGp1PortPtr`) **accesses the register** at that
  address: a load reads the hardware status, a store issues a command or
  programs a transfer.

No code in the repository writes to these slots; `analysis/refs.json` records
only 4-byte loads. All device traffic goes through the dereferenced pointer.

## How the names are bound

The subsystem headers carry the assembler labels, not the declarations:

```c
/* include/game/gpu_globals.h */
#define GPU_GP0_PORT_ASM "D_8009DEC4"
#define GPU_GP1_PORT_ASM "D_8009DEC8"

/* include/game/dma_globals.h */
#define DMA2_BLOCK_CTRL_ASM "D_8009DED0"
```

Each using translation unit declares the pointer with its own qualification and
binds it to the retail symbol:

```c
extern volatile u32 *gDma2BlockCtrlPtr __asm__("D_8009DED0");
```

The GCC 2.8.1-psx stage emits the retail symbol in every `%hi`/`%lo` reference,
so the object code is unchanged.

**Do not define an object for these names.** The retail words live in
`asm/us/main.s` (`dlabel`/`.word`), the whole payload is a single `.text` blob,
and the generated linker script discards everything except `main.s.o`. A C
definition would silently land at a different address (measured: directly after
`.text`), and inside the same translation unit it would override the absolute
symbol and shift the load to the wrong address. Moving such a word into C needs
a segment split, an absolute placement rule in the linker script, removal of the
`.set` alias and a byte comparison — none of that is in place today.

## Why the declarations are not all in the header

The units need different qualification: a volatile pointee in most, a volatile
pointer in `func_8009446C`, a plain pointer in the rest. A single shared type
changes code generation. Measured on the 18 GPU units: one shared `volatile`
declaration kept 14/18 byte-exact, one shared non-volatile declaration 11/18.
With the historical per-unit types all units stay identical, therefore the
header carries the binding and each unit keeps its own type.

## Inline assembly

Hand-written assembly keeps the retail names (`%hi(D_8009DEC8)`,
`%lo(D_8009DEC8)`), because GCC does not rewrite symbol names inside asm
strings: `src/game/80094CEC.c` and `src/game/8009520C.c`. Renaming there would
break the reference; the labels above only exist for C declarations.

## Verifying a change

The pipeline compiles each unit and compares `.text` bytes with the retail
range; `python3 tools/verify_matching_units.py` runs the whole catalogue:

```
export PATH=$PWD/.toolchain/mipsel-binutils/root/usr/bin:$PATH
export LD_LIBRARY_PATH=$PWD/.toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu
python3 tools/verify_matching_units.py
```

Any change to a declaration's `volatile` or pointer qualification must be
re-measured: it can add or remove loads of the slot even though the symbol
address is unchanged.

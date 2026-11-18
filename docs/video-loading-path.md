# Video loading and playback (`.wve` movies)

Traced from the retail code: how a film file is opened, streamed from the CD, decoded and paced. All
statements name the unit and line they come from. The memory-card save path that sits next to it in
the boot sequence is **not** part of this — see the last section.

## Call chain

| step | function | unit |
| --- | --- | --- |
| 1 | `gDrawImageResource` — opens the layer on demand and draws the named resource | `80089D44.c` |
| 2 | `gOpenVideoLayer` — allocates the buffers and sets the display mode | `80087EA0.c` |
| 3 | `gPlayVideoFile` — open, stream, decode, convert | `80088120.c` |
| 4 | `gCloseVideoLayer` — releases everything step 2 allocated | `80087D48.c` |

`gDrawImageResource` is used for both still images (`Ea_logo.fsv`) and films (`Movies/Intro.wve`); the
flag it passes to `gOpenVideoLayer` decides the mode word and the buffer sizes.

## Layer bring-up (`gOpenVideoLayer`, `80087EA0.c`)

1. Both pad masks are primed: `gMoviePadMask0/1 = func_8001EF8C(port) & 0x40080000` (`:100-105`), i.e.
   "Cross or Start is held". This makes the first pass of the demux loop ignore a button that was
   pressed before the film started.
2. The mode word becomes `gMovieDisplayMode` (`:134`), geometry constants are written
   (`D_8009F960 = 0x400`, `D_8009F964 = 0x800`, `:107-111`) and `gLayerModeBits` becomes `0xC8`, or
   `0x27` when bit 0 of the argument is set (`:116-120`).
3. Buffers, in this order (`:122-199`): picture buffer `0x22D00` (or `0xF6C0`), `0x6000` and `0x600`
   for the VLC tables, `0x130` for the strip triple, the stream ring
   `func_800896D4(gLayerModeBits, 0x4400, 1)`, the `0x7B80`-byte audio block and two `0xC00`-byte SPU
   buffers (`func_80097B28`, the SPU allocator).
4. Any failed allocation jumps to `gCloseVideoLayer()`, so the layer is either complete or closed
   (`gLayerOpenResult` at `+0` is the flag the callers test).

Teardown (`80087D48.c:44-80`) runs in the reverse order and unhooks the fade layer and the display
only when mode bit `0x20` is clear.

## Stream reader (`gMovieStreamReaderInit`, `80089700.c`)

The caller passes a heap block (`gMovieStreamBuffer`), a callback, an array of 24-byte stream
descriptors and their count:

- context header `0x57C` bytes, then `gMovieDescriptorCount * 56` bytes of entries, then the ring
  `gMovieRingBytes` (0x4400, or 0x424C in the tolerant mode) and `0x800`-byte CD staging pages.
- each 24-byte descriptor is copied into the `+12` field of its 56-byte entry (`:106-108`).
- the DMA3 and CD callbacks are swapped out with `func_8008E5D8(0)` / `func_80091240(0)` and stored at
  `+0x574`/`+0x578`, the first entry is primed and `gCdStreamService(1)` starts streaming
  (CdSetmode `0xA0`, `800895E4.c`).
- `gMovieStreamReaderClose` (`80089874.c`) undoes the swap.

`gMovieStreamReserve` (`80089404.c`) hands out ring space — the caller stores the requested size in a
slot and gets the available size back — and `gMovieStreamAdvance` (`80089304.c`) releases it.

## Chunk format and demux (`gMovieStreamDemux`, `800879DC.c`)

Every pass:

1. **Skip check** — `func_8001EF8C(0/1)` is the per-port pad poll (`8001EF8C.c`); `0x40080000` is
   bit 30 | bit 19 of the complemented pad word, i.e. **Cross | Start**. A newly pressed button is
   ignored for the first four chunks; afterwards it ends playback, and Cross also sets
   `gMovieAbortFlag` (`:98-113`).
2. **Header** — 8 bytes: a big-endian FourCC tag and a size, both byte-swapped (`:118-135`).
3. **Sanity** — with `gMovieDisplayMode & 1` a size of `0x424C` or more skips the chunk by four bytes,
   with it clear `0x4400` is the limit.
4. **Dispatch** on the FourCC:

| tag | ASCII | handler | payload |
| --- | --- | --- | --- |
| `0x564C4330` | `VLC0` | `gMovieBuildVlcTables` | Huffman/VLC tables |
| `0x4164313x`, `0x616431/2x` | `Ad10`…`ad21` | `gMovieAudioChunkPush` | compressed audio into the SPU ring |
| `0x61753030/1` | `au00`, `au01` | `gMovieAudioChunkPushSplit` | the second audio format |
| `0x4D444332`, `0x4D444543` | `MDC2`, `MDEC` | `gMovieDecodeSlice` | one video frame |

5. **Frame** — chunk counter 1 initialises the triple buffer (`gMovieAudioStart`), then
   `gMovieAudioClockEnable`, the deadline from `gMovieFrameDeadline(counter)` and the slice decode;
   the body spins on `func_80091270(0)` until `gMovieAudioClockRead()` reaches the deadline and
   `gMoviePresentFrame` flips the buffers (`:170-200`).

## Slice decode (`gMovieDecodeSlice`, `80085228.c` / `80085630.c`)

Two feeders decode the same 16-byte slice header; `gMovieDecodeSlice` picks one:
`gMovieSliceFeedDma` uploads the whole slice with one DMA to VRAM, `gMovieSliceFeedRows` walks it row
by row through the GPU. Both:

1. store the destination and source rectangles in `gMovieSliceContext` (`:186-200`),
2. write the MDEC control word (`func_80095FE8`, `func_80095FF4`),
3. VLC-decode the payload with `func_80089BBC` when the slice carries the `MDEC` tag,
4. move the result to the framebuffer with `func_80096070` and wait on `func_800960CC`,
5. `func_80095E90(1)` resets the decoder between frames.

`gMoviePresentFrame` (`80085914.c`) then flips the two picture buffers and hands the back buffer to
the display.

## Audio and pacing

`gPlayVideoFile` brings the audio side up: `func_80087824(gMovieAudioBlock, 0x400000, gMovieSpuBufferA,
0x800000, gMovieSpuBufferB, D_8009F964, 1)` (`80088120.c:235`), setting the voice/interrupt callbacks
inside `gMovieAudioState`. `gMovieAudioChunkPush` (`80086FE4.c`) fills the two `0x3C00`-byte rings; the
same block carries the clock the video is paced against:

- `gMovieAudioClockEnable` / `gMovieAudioClockDisable` (`80086F1C.c`, `80086F30.c`) toggle `+0x7B20`,
- `gMovieAudioClockRead` (`80086F44.c`) returns the timestamp `+0x7B24`,
- `gMovieFrameDeadline` (`80086F58.c`) returns `index << 10`, or `index * 0x271000 / 0xBB5` when the
  mode word `+0x7B74` is 1.

After the demux returns, `gPlayVideoFile` waits for `gMovieAudioBusyCount()` to drop to zero, shuts the
audio side down (`func_80087750`), closes the stream reader and the file (`80088120.c:232-245`).

## Framebuffer conversion

When mode bits `0x24` are both set, `80088120.c:254-300` converts the 16-bit picture line by line
(`0x1E0` pixels) into `0x140`-wide 24-bit output using two scratch tables that are copied into the
scratchpad at `0x1F800000`/`0x1F800100`.

## Not video code

The boot path calls the profile save/load subsystem immediately before it starts `Movies/Intro.wve`,
which is how an earlier round mistook it for the movie player. `8007F8BC`…`8007FB04`, `80080B08`,
`8008204C` and `80020600`…`800206D0` are the **memory-card** state machine: they open
`BASLUS-00739FCOPLAPD`, read and write a 0x678-byte image and run the BIOS card events. Their unit
headers and categories now say so.
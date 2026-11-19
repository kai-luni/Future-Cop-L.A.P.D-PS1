// Categories: Resources > Asset Loader | Platform > Streaming Audio
#include "game/types.h"

typedef struct File File;
typedef struct Chunk Chunk;
typedef struct Sound Sound;
struct File { s32 a, b, pos, refs, e; };
struct Chunk { File *owner; s32 size; s32 a, b, subtype; u16 total, index; u8 data[1]; };
struct Sound { u8 *data; u8 pad[0x1c]; s32 size; };
struct SoundLoad { Sound *sound; s32 soundpos; };
struct State { u8 pad0[8]; s32 pos; u8 padc[8]; File *file; s32 pad18; s32 limit; u8 playing; u8 pad21[3]; struct SoundLoad load; u8 pad2c[0x12]; u16 count; u16 rate; u8 pad42[2]; s32 a, b, c; };
struct Other { u8 pad0[8]; Chunk *chunks[2]; u8 pad10; u8 count; u8 sum; u8 pad13; s32 size; };
extern struct State gStreamBufferList __asm__("D_800A0B70");
extern struct Other gReleaseQueues __asm__("D_800A0C30");
extern Chunk *gVagmRing[4] __asm__("D_800A0C50");
extern u8 gLatchFlag __asm__("D_8009F6C0");
extern u8 gBitFieldState __asm__("D_8009F6B8");
extern s32 gPortraitStreamDeadline __asm__("D_8009F6C4");
extern s32 gVagmPrimeBlockCount __asm__("D_8009F6E0");
extern s32 gCallGateFlag __asm__("D_8009F6DC");
extern volatile s32 gVagmWriteIndex __asm__("D_8009F6D4");
extern s32 gGameSuspended __asm__("D_8009FA24");
extern s32 gServiceModuleReadQueues(s32) __asm__("func_80028E58");
extern void gRetireParsedModuleBuffer(void) __asm__("func_80028D28");
extern s32 gCreateModuleAssetFromHeader(void *, void *) __asm__("func_80028BCC");
extern void gCopyWholeWords(void *, void *, u32) __asm__("func_80028BA0");
extern void gQueueCompletedModuleAsset(void *) __asm__("func_80028DC0");
extern s32 gInitializeVagmAudioStream(void) __asm__("func_800244A4");
extern void func_8002A544(void);
extern u8 gUploadCvkSpuDataAndWait(void *, s32, s32) __asm__("func_80023CA4");
extern void func_8002A16C(s32);

void gParseStreamedModuleChunks(void) __asm__("func_8002902C");

/**
 * @brief Parses completed module buffers and dispatches their asset, control, audio and
 * animation chunks.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Services the file rings, increments the parsed-buffer counter, then reads each tag and size
 * up to the parse limit. SHOC prepares SHDR and copies SDAT data, CTRL initializes the limit
 * metadata, VAGM queues retained audio blocks, VAGB uploads SPU data, CANM retains animation
 * blocks and FILL advances to the limit. Retires each parsed buffer before servicing the ring
 * again.
 *
 * @par Globals
 * Uses gStreamBufferList, gReleaseQueues, gVagmRing, gVagmWriteIndex, gVagmPrimeBlockCount,
 * gPortraitAudioByteCount, gPortraitStreamDeadline, gBitFieldState, gLatchFlag, gCallGateFlag
 * and gGameSuspended.
 *
 * @par Callers
 * Direct calls from gPopCompletedModuleAsset (func_800295E8).
 *
 * @par Role and path
 * Called by the asset pop path to deliver module assets and feed the synchronized audio and
 * portrait queues.
 *
 * @par Unknown
 * Unrecognized tags are advanced by their size and the full tag vocabulary is unresolved.
 */
void gParseStreamedModuleChunks(void)
{
    struct State *state;
    File *file;
    Chunk *chunk;
    s32 size;
    struct Other *other;
    Chunk **queue;
    s32 one;
    File copy;
    register s32 pos __asm__("$5");
    if (!gServiceModuleReadQueues(1)) return;
    state = &gStreamBufferList;
    file = state->file;
    if (!file) return;
    queue = gVagmRing;
    one = 1;
    do {
        state->count++;
        copy = *file;
        pos = file->pos;
        if (pos < state->limit) {
            other = &gReleaseQueues;
        chunk_loop:
            {
                s32 pivot = 0x53484F43;
                __asm__("" : : "r"(pivot));
                chunk = (Chunk *)((u8 *)file + (pos + 0x10));
                size = chunk->size;
                {
                    s32 tag = (s32)chunk->owner;
                    switch (tag) {
                    case 0x53484F43: goto choc;
                    case 0x4354524C: goto ctrl;
                    case 0x43414E4D: goto canm;
                    case 0x46494C4C: goto fill;
                    case 0x56414742: goto vagb;
                    case 0x5641474D: goto vagm;
                    case 0x54000000: goto dispatch_end;
                    default: goto dispatch_end;
                    }
                choc:
                    if (chunk->subtype == 0x53484452) {
                        if (gCreateModuleAssetFromHeader(&state->load, chunk)) return;
                    } else if (chunk->subtype == 0x53444154) {
                        register struct SoundLoad *load __asm__("$16");
                        register s32 newpos __asm__("$2");
                        newpos = pos + 0x14;
                        file->pos = newpos;
                        size -= 0x14;
                        load = &state->load;
                        if (load->sound) {
                            __asm__("" : "+r"(chunk));
                            gCopyWholeWords(load->sound->data + load->soundpos, (u8 *)chunk + 0x14, size);
                            load->soundpos += size;
                            if (load->soundpos == load->sound->size) {
                                gQueueCompletedModuleAsset(load->sound);
                                load->sound = 0;
                            }
                        }
                    }
                    goto dispatch_end;
                ctrl:
                    if (state->rate == 0xffff) {
                        state->rate = *(u32 *)((u8 *)chunk + 0x14) / 0x6000;
                        state->a = chunk->b;
                        state->b = chunk->subtype;
                        state->c = *(s32 *)((u8 *)chunk + 0x14);
                    }
                    goto dispatch_end;
                vagm:
                    if (gCallGateFlag != 0 && gCallGateFlag != 2 && queue[gVagmWriteIndex] == 0) {
                        register Chunk *p __asm__("$16");
                        p = chunk;
                        __asm__("" : "+r"(p));
                        queue[gVagmWriteIndex] = chunk;
                        gVagmWriteIndex++;
                        if (gVagmWriteIndex >= 4) gVagmWriteIndex = 0;
                        file->refs++;
                        __asm__("" : "+r"(chunk));
                        chunk->owner = file;
                        if (gCallGateFlag == 3) {
                            gVagmPrimeBlockCount++;
                            if ((u32)gVagmPrimeBlockCount >= 2) {
                                if (!gInitializeVagmAudioStream()) func_8002A544();
                                else gCallGateFlag = one;
                            }
                        } else gCallGateFlag = one;
                        if (gCallGateFlag && p->index + 1 == p->total && state->playing == one) {
                            state->playing = 0;
                            state->pos = state->c + state->b;
                        }
                    }
                    goto dispatch_end;
                vagb:
                    gBitFieldState |= 1;
                    if (gUploadCvkSpuDataAndWait((u8 *)chunk + 0x18, size - 0x18, chunk->index)) {
                        if (chunk->index == one) other->size = 0;
                        other->size += ((size + 0x27) >> 6) << 6;
                        if (chunk->index == chunk->total) {
                            gPortraitStreamDeadline = 0;
                            gBitFieldState = (gBitFieldState | 4) & 0xbf;
                            if (gLatchFlag) {
                                gBitFieldState |= 8;
                                gLatchFlag = 0;
                            }
                            if (!gGameSuspended) {
                                state->playing = one;
                                func_8002A16C(1);
                            }
                        }
                    }
                    goto dispatch_end;
                canm:
                    if (other->count < 2) {
                        file->refs++;
                        chunk->owner = file;
                        other->chunks[other->count++] = chunk;
                        { u8 c = other->count;
                        gBitFieldState |= 2;
                        if (c == one) other->sum = 0; }
                        other->sum += *(u8 *)((u8 *)chunk + 0x16);
                    }
                    goto dispatch_end;
                fill:
                    file->pos = state->limit;
                    goto next;
                }
            dispatch_end:
                file->pos += size;
            next:;
                pos = file->pos;
            }
            if (pos < state->limit) goto chunk_loop;
        }
        gRetireParsedModuleBuffer();
        gServiceModuleReadQueues(0);
        file = state->file;
    } while (file);
}

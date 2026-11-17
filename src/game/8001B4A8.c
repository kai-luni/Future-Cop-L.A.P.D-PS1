// Categories: Graphics > Frame Submission | Game Engine > Frame End | Platform > GPU
#include "game/types.h"


extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");
extern u8 gDisplayListHead[] __asm__("D_800A63E0");
extern u8 D_800A0520[];
extern u8 D_800A0528[];
extern u8 D_800A0660[];
extern u32 D_8009AA68[];
extern s32 gPlayerCount __asm__("D_8009FA0C");

extern u32 D_8009F5C8[];
extern u32 gRenderNodeTable[] __asm__("D_8009F5D0");
extern u32 D_8009F5D8[];

extern s32 D_8009F218;
extern s32 D_8009F21C;
extern s32 D_8009F220;
extern s32 D_8009F5C0;
extern u32 gPacketPoolBase __asm__("D_8009F5E0");
extern u32 gNodePoolBase __asm__("D_8009F5E4");
extern s32 D_8009F5E8;
extern s32 D_8009F5EC;
extern s32 D_8009F5F0;
extern s32 D_8009F5F4;
extern s32 D_8009F5F8;
extern s32 D_8009F5FC;
extern s32 D_8009F600;
extern u32 gPrimPoolNext __asm__("D_8009F9F0");
extern u32 gPacketCursor __asm__("D_8009FA90");
extern s32 gScreenOffsetValue __asm__("D_8009F9D4");
extern u32 gPacketLimit __asm__("D_8009F9C0");
extern u32 gPrimPoolEnd __asm__("D_8009FA28");

extern u8 func_8001C034(void);
extern s32 func_80095C14(s32 value);
extern void func_80029650(u32 arg0);
extern void func_80092F38(u32 arg0, u32 arg1);
extern void func_80092C8C(s32 arg0);
extern void func_80093178(u32 arg0, s32 arg1);
extern void func_80093280(u32 arg0);
extern void func_8009398C(u32 arg0, void *arg1);
extern void func_80093A0C(u32 arg0, void *arg1);
extern void func_800861E4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);

/**
 * @brief Ends a frame: buckets everything drawn this frame into the ordering
 *        table by depth, submits it, flushes pending uploads and swaps the
 *        double buffer.
 *
 * @param arg0 0 = sub-screen frame (0xA0 wide clip when D_8009F5C0 is set),
 *             non-zero = full-screen frame (0x140 x 0xF0 clip). A non-zero
 *             value also raises the frame barrier D_8009FA3C at the end.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. Spins while D_8009FA3C is set (the previous frame's DMA is still running).
 * 2. h = func_8001C034() ? 0x100 : 0; the x offset is mirrored into
 *    D_8009F9D4.
 * 3. Walks both packet pools backwards from the current cursor
 *    (D_8009F9F0 with 0x34 stride, D_8009FA90 with 0x1C stride) down to the
 *    bases D_8009F5E0/D_8009F5E4, collecting the 24-bit depth tag
 *    (word & 0xFFFFFF) into min/max. Because the pools are walked downwards,
 *    the ordering matches the order the primitives were emitted.
 * 4. Computes the bucket count: quot = (D_8009F5FC + max - 1 - min) /
 *    D_8009F5FC, shift = 0x20 - func_80095C14(quot) (a log2 helper), and
 *    count = ((max - min) >> shift) + 1 (0 when no primitive was emitted).
 * 5. Second pass links every primitive into the bucket list of the ordering
 *    table word at D_8009F5D8[D_8009F600]: the low 24 bits of the primitive's
 *    tag become the bucket index, the previous bucket head is chained into the
 *    primitive and the primitive becomes the new head.
 * 6. If D_8009FA0C >= 2 or arg0 != 0, a drawing area (rect = x, y, w, h) is
 *    installed with func_8009398C and func_80093A0C, and the ordering table
 *    itself is spliced into the display list at D_800A63E0.
 * 7. Deferred work queues are drained in ring-buffer order (0x14 slots):
 *    func_800A0520 entries release handles (func_80029650), D_800A0528
 *    entries submit transfers (func_80092F38) followed by func_80092C8C.
 *    A single-shot queue in D_8009AA68 is handled the same way.
 * 8. When arg0 != 0, D_8009F5C0 == 0 or D_8009FA0C == 1, the 4-slot image
 *    upload ring D_800A0660 is flushed: queued handles are freed and queued
 *    images are uploaded with func_800861E4(ptr, mode, width, offset).
 * 9. func_80092C8C(0) and func_80093280(table + (count + 1) * 4) hand the list
 *    to the GPU (DrawOTag style).
 * 10. Buffer swap: D_8009F600 toggles 0/1, the cursors D_8009F9F0/D_8009FA90
 *     and limits D_8009FA28/D_8009F9C0 are reset to the other buffer pair
 *     (D_8009F5C8/D_8009F5D0), and func_80093178() terminates the new table.
 *
 * @par Globals
 * D_8009FA3C (frame barrier), D_8009F9D4 (x offset), D_8009F9F0/D_8009FA90
 * (packet cursors), D_8009F5E0/D_8009F5E4 (pool bases), D_8009FA28/D_8009F9C0
 * (pool limits), D_8009F5C8/D_8009F5D0 (buffer bases), D_8009F5D8 (ordering
 * table), D_8009F600 (buffer index), D_8009F5FC (bucket size),
 * D_8009F5F4/D_8009F5F8 (pool entry counts), D_8009FA0C (screen state),
 * D_8009F5C0 (sub-screen flag), D_8009F218/D_8009F21C/D_8009F220 (ring heads),
 * D_8009A0520/D_800A0528/D_800A0660/D_8009AA68 (queues).
 *
 * @par Side effects
 * GPU DMA is started, queued images are uploaded to VRAM, handles are freed
 * and the render buffers are swapped. This is the only place the frame is
 * submitted.
 *
 * @par Unknown
 * The exact meaning of D_8009FA0C values (0/1/2) beyond selecting the clip
 * rectangle, and of D_8009F5C0.
 */
void func_8001B4A8(s32 arg0)
{
    register s32 max __asm__("$16");
    register u32 q __asm__("$17");
    register u32 p __asm__("$18");
    register s32 min __asm__("$19");
    register u32 limY __asm__("$20");
    register u32 limX __asm__("$21");
    register u32 h __asm__("$22");
    register u32 table __asm__("$23");
    s32 count;
    register s32 v __asm__("$2");
    register s32 xbase __asm__("$3");
    register s32 shift __asm__("$7");
    register u32 mask __asm__("$6");
    register s32 arg9 __asm__("$9");

    u32 pcur;
    u32 qcur;
    register u32 av __asm__("$5");
    register u32 bv __asm__("$4");
    register u32 val __asm__("$8");
    u32 ptr;
    s32 quot;
    s32 idx;
    s16 rect2[2];
    s16 rect[4];
    u32 e;
    u32 hnd;
    s32 dx;
    s32 dy;

    while (gFrameBarrier) {
    }

    __asm__("" : "+m"(arg0));
    h = func_8001C034();
    if (h != 0) {
        h = 0x100;
    }

    xbase = gPrimPoolNext;
    v = 0x100 - h;
    gScreenOffsetValue = v;
    v = gPacketCursor;
    limX = gPacketPoolBase;
    limY = gNodePoolBase;

    p = xbase - 0x34;
    pcur = p;
    q = v - 0x1C;
    qcur = q;

    if (p < limX && q < limY) {
        goto done;
    }

    max = 0xFFC00000;
    min = 0x400000;

    if (p >= limX) {
        do {
            v = *(s32 *)pcur;
            pcur -= 0x34;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (pcur >= limX);
    }

    if (qcur >= limY) {
        do {
            v = *(s32 *)qcur;
            qcur -= 0x1C;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (qcur >= limY);
    }

    v = max - 1;
    quot = (D_8009F5FC + v - min) / D_8009F5FC;
    shift = 1;
    if (quot != 0) {
        v = func_80095C14(quot);
        xbase = 0x20;
        shift = xbase - v;
    }

    if (max != 0xFFC00000) {
        v = max - min;
        v = v >> shift;
        count = v + 1;
    } else {
        count = 0;
    }

    mask = 0xFF0000;
    v = D_8009F600;
    __asm__("" : "+r"(mask) : "r"(v));
    mask |= 0xFFFF;
    xbase = (s32)D_8009F5D8;
    v = v << 2;
    v = v + xbase;
    table = *(u32 *)v;

    if (q >= limY) {
        do {
            v = *(u32 *)q;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)q = v;
            v = q & mask;
            q -= 0x1C;
            v = av | v;
            *(u32 *)xbase = v;
        } while (q >= limY);
    }

    if (p >= limX) {
        do {
            v = *(u32 *)p;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)p = v;
            v = p & mask;
            p -= 0x34;
            v = av | v;
            *(u32 *)xbase = v;
        } while (p >= limX);
    }

    if (gPlayerCount >= 2 || ({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        arg9 = arg0;
        limX = 0;
        if (arg9 == 0 && D_8009F5C0 != 0) {
            limX = 0xA0;
        }

        rect[0] = limX;
        rect[1] = h;
        rect[2] = (arg9 != 0) ? 0x140 : 0xA0;
        rect[3] = 0xF0;

        q = (u32)gDisplayListHead;
        func_8009398C(q, rect);

        p = 0xFFFFFF;
        mask = q & p;
        __asm__ volatile("" : "+r"(q) : "r"(count));
        q += 0x1C;
        __asm__("" : "+r"(q) : "r"(count));
        bv = q;
        __asm__("" : : "r"(bv));
        max = (count << 2) + table;
        min = 0xFF000000;
        xbase = *(u32 *)gDisplayListHead;
        v = *(u32 *)(max + 4);
        *(u32 *)gDisplayListHead = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | mask;

        rect2[0] = limX;
        rect2[1] = h;
        func_80093A0C(q, rect2);

        xbase = *(u32 *)q;
        v = *(u32 *)(max + 4);
        *(u32 *)q = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | (q & p);
    }

    if (D_8009F21C != D_8009F220) {
        do {
            v = (u32)D_800A0520 + (D_8009F21C << 4);
            hnd = *(u32 *)(v + 4);
            if (hnd != 0) {
                func_80029650(hnd);
            }
            D_8009F21C++;
            if (D_8009F21C >= 0x14) {
                D_8009F21C = 0;
            }
        } while (D_8009F21C != D_8009F220);
    }

    if (D_8009F220 != D_8009F218) {
        ptr = (u32)D_800A0528;
        q = ptr - 8;
        do {
            bv = D_8009F220 << 4;
            v = bv + q;
            av = *(u32 *)v;
            func_80092F38(bv + ptr, av);
            func_80092C8C(0);
            D_8009F220++;
            if (D_8009F220 >= 0x14) {
                D_8009F220 = 0;
            }
        } while (D_8009F220 != D_8009F218);
    }

    if (D_8009AA68[0] != 0) {
        func_80092F38((u32)D_8009AA68 + 8, D_8009AA68[0]);
        func_80092C8C(0);
        if (D_8009AA68[1] != 0) {
            func_80029650(D_8009AA68[1]);
        }
        D_8009AA68[0] = 0;
    }

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; }) || D_8009F5C0 == 0 || gPlayerCount == 1) {
        if (D_8009F5EC != D_8009F5F0) {
            do {
                v = (u32)D_800A0660 + D_8009F5EC * 20;
                hnd = *(u32 *)(v + 4);
                if (hnd != 0) {
                    func_80029650(hnd);
                }
                D_8009F5EC++;
                if (D_8009F5EC >= 4) {
                    D_8009F5EC = 0;
                }
            } while (D_8009F5EC != D_8009F5F0);
        }

        if (D_8009F5F0 != D_8009F5E8) {
            do {
                xbase = (u32)D_800A0660 + D_8009F5F0 * 20;
                func_800861E4(*(u32 *)xbase, *(u8 *)(xbase + 8),
                              *(u32 *)(xbase + 0xC), *(u32 *)(xbase + 0x10));
                func_80092C8C(0);
                D_8009F5F0++;
                if (D_8009F5F0 >= 4) {
                    D_8009F5F0 = 0;
                }
            } while (D_8009F5F0 != D_8009F5E8);
        }
    }

    max = count + 1;
    func_80092C8C(0);
    func_80093280(table + (max << 2));

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        gFrameBarrier = 1;
    }

    mask = 1;
    v = D_8009F600;
    __asm__ volatile("" : "+r"(mask) : "r"(v));
    xbase = (s32)gRenderNodeTable;
    mask -= v;
    av = mask << 2;
    v = (s32)D_8009F5D8 + av;
    bv = *(u32 *)v;
    __asm__ volatile("" : : "r"(v));
    v = (s32)D_8009F5C8 + av;
    av += xbase;
    shift = *(u32 *)v;
    val = *(u32 *)av;
    __asm__("" : : "r"(av));
    av = D_8009F5FC;
    xbase = D_8009F5F4;
    D_8009F600 = mask;
    v = xbase * 52;
    xbase = D_8009F5F8;
    __asm__("" : : "r"(xbase));
    v -= 0x34;
    gPacketPoolBase = shift;
    gPrimPoolNext = shift;
    shift += v;
    gNodePoolBase = val;
    gPacketCursor = val;
    gPrimPoolEnd = shift;
    v = xbase * 28;
    v -= 0x1C;
    val += v;
    gPacketLimit = val;
    func_80093178(bv, av + 2);
done:
    return;
}

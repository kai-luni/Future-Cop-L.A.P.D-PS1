#include "game/types.h"

struct Unk5AB8CParam {
    s32 unk0;
    u8 pad4[4];
    s32 unk8;
};

struct SpritePos {
    s16 x;
    s16 y;
};

struct Unk5AB8CSub {
    u8 pad00[0xA4];
    u32 unkA4;
    u8 colors[8];
    struct SpritePos pos[8];
};

struct Unk5AB8CObj {
    u8 pad00[0xC1];
    s8 unkC1;
    u8 padC2[0x300 - 0xC2];
    struct Unk5AB8CSub *unk300;
    u8 pad304[0x310 - 0x304];
    s8 unk310;
};

struct Unk5AB8C {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0xAC - 0x28];
    struct Unk5AB8CObj *unkAC;
};

struct DrawObj {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x50 - 0x28];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
};

struct PacketWord5 {
    u32 word[5];
};

extern s32 D_8009F7E8;
extern s32 D_8009F7EC;
extern s32 D_8009F7E0[];
extern s32 D_8009F7E4[];
extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;
extern struct DrawObj *D_8009FA58[];
extern struct DrawObj *D_8009F9EC;
extern struct DrawObj *D_8009FA94;
extern u32 D_8009AFC0[];
extern u32 D_800A6F60[];
extern u16 D_800A8EA6[];

extern s32 func_80018580(s32 arg0, s32 arg1);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern void func_800694C8(s32 *outX, s32 *outY, s32 arg2, s32 arg3);
extern void func_8001C214(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend);

void func_8005AB8C(struct Unk5AB8C *input, s32 *arg1, s32 *arg2)
{
    struct Unk5AB8C *arg0;
    s32 head;
    register s32 width __asm__("$7");
    register s32 height __asm__("$2");
    register u32 callColor __asm__("$4");
    register s32 callX __asm__("$5");
    register s32 callY __asm__("$6");
    register s32 sum __asm__("$2");
    register u32 addressHi __asm__("$2");
    s32 x;
    s32 y;
    register s32 offset __asm__("$20");
    struct Unk5AB8CObj *obj;
    register u8 *p __asm__("$6");
    s32 i;
    register struct Unk5AB8CSub *sub __asm__("$19");
    u32 bits;
    struct DrawObj *drawObj;
    register struct DrawObj **drawList __asm__("$17");
    register struct Unk5AB8CSub *pos __asm__("$17");

    D_8009F7EC = arg1[0];
    __asm__ volatile ("" : : : "memory");
    head = arg1[2];
    __asm__("addu %0,%1,$zero" : "=r"(arg0) : "r"(input), "r"(head));
    D_8009F7E8 = head;
    __asm__ volatile ("" : : : "memory");
    offset = 0;
    func_8001846C(-0x400 - (func_80018580(arg2[2], arg2[0]) << 4),
                  D_8009F7E0, D_8009F7E4);

    obj = arg0->unkAC;
    if (obj->unkC1 != 0) {
        offset = 0x50;
    }
    __asm__ volatile ("" : "=r" (offset) : "0" (offset));

    if (obj->unk310 == 0) {
        register u8 *p2 __asm__("$7");
        register u8 *next3 __asm__("$4");
        u32 clut;
        register s32 cmp3 __asm__("$2");
        register u8 *next __asm__("$3");
        register u8 *limit __asm__("$3");
        register u32 mask __asm__("$4");
        register s32 cmp __asm__("$3");
        register u8 *next2 __asm__("$5");
        register u32 tag __asm__("$2");
        register u32 pageCode __asm__("$3");

        next = D_8009FA90;
        if (next < D_8009F9C0) goto alloc1;
        __asm__("" : : "r"(next));
        p = next; goto ready1;
alloc1:
        p = next; D_8009FA90 = p + 0x1C;
ready1:
        pageCode = 0xE100069E;
        __asm__("" : : "r"(pageCode));
        p[3] = 1;
        tag = *(u32 *)p;
        __asm__("" : : "r"(tag) : "memory");
        next2 = D_8009FA90;
        mask = 0xFF000000;
        *(u32 *)(p + 4) = pageCode;
        limit = D_8009F9C0;
        tag &= mask;
        cmp = next2 < limit;
        *(u32 *)p = tag;
        if (cmp) goto alloc2;
        __asm__("" : : "r"(next2));
        p = next2; goto ready2;
alloc2:
        p = next2; D_8009FA90 = p + 0x1C;
ready2:
        p[7] = 0x66;
        p[4] = 0x80;
        p[5] = 0x80;
        p[6] = 0x80;
        *(s16 *)(p + 8) = offset + 0xC;
        *(s16 *)(p + 0xA) = 0x10;
        p[0xC] = 0xC7;
        p[0xD] = 0x94;
        *(s16 *)(p + 0x10) = 0x38;
        *(s16 *)(p + 0x12) = 0x38;
        *(u32 *)p &= 0xFF000000;
        p[3] = 4;
        __asm__("lui\t%0,0x800B" : "=r"(addressHi) : "m"(*(u32 *)p));
        clut = *(u16 *)(addressHi - 0x715A);
        __asm__("" : : "r"(clut) : "memory");
        next3 = D_8009FA90;
        cmp3 = next3 < D_8009F9C0;
        *(s16 *)(p + 0xE) = clut;
        if (cmp3) goto alloc3;
        __asm__("" : : "r"(next3));
        p2 = next3; goto ready3;
alloc3:
        p2 = next3; D_8009FA90 = p2 + 0x1C;
ready3:
        *(struct PacketWord5 *)p2 = *(struct PacketWord5 *)p;
    } else {
        u32 *src;
        register u32 r __asm__("$9");
        register u32 g __asm__("$8");
        register u32 b __asm__("$7");
        register s32 packetIndex __asm__("$4");
        register u8 *cursor __asm__("$3");
        register s32 cmp __asm__("$2");
        u32 tag;
        u32 first;
        register u32 coord __asm__("$2");
        u32 second;
        register u32 mask __asm__("$12");
        register u32 length __asm__("$11");
        register u32 opcode __asm__("$10");
        register s32 last __asm__("$13");

        r = 0x40;
        g = 0x28;
        b = 0;
        __asm__("" : "=r"(b) : "0"(b));
        addressHi = 0x800A0000;
        __asm__("" : "=r"(addressHi) : "0"(addressHi), "r"(b));
        src = (u32 *)(addressHi - 0x5040);
        packetIndex = b;
        last = 4;
        mask = 0xFF000000;
        length = 3;
        opcode = 0x62;
        do {
            if (packetIndex == last) {
                r = 0x10;
                g = 0x18;
                b = 0x28;
            }

            cursor = D_8009FA90;
            cmp = cursor < D_8009F9C0;
            p = cursor;
            if (cmp) {
                D_8009FA90 = p + 0x1C;
            }
            tag = *(u32 *)p;
            p[7] = opcode;
            p[4] = r;
            p[5] = g;
            p[6] = b;
            *(u32 *)p = tag & mask;
            p[3] = length;
            first = src[0];
            packetIndex++;
            *(u32 *)(p + 8) = first;
            second = src[1];
            __asm__("" : : "r"(second) : "$2");
            src += 2;
            coord = *(u16 *)(p + 8);
            coord += offset;
            *(u16 *)(p + 8) = coord;
            *(u32 *)(p + 0xC) = second;
        } while (packetIndex < 5);
    }

    if (arg0->unkAC->unk310 != 0) {
        drawList = D_8009FA58;
        for (i = 0; i < 2; i++, drawList++) {
            register struct DrawObj *d __asm__("$3");
            d = *drawList;
            if (d == 0) {
                break;
            }
            if ((d->unk24 & 0x20) == 0) {
                func_800694C8(&x, &y, d->unk58, d->unk50);
                callColor = 0xFFFF;
                width = 4;
                sum = x;
                callY = y;
                sum += offset;
                callX = (sum + 0xD) << 1;
                callY = (callY + 0x11) << 1;
                x = sum;
                __asm__("addu\t%0,%1,$zero" : "=r"(height) : "r"(width), "r"(callX), "r"(callY), "r"(callColor));
                func_8001C214(callColor, callX, callY, width, height, 0);
            }
            __asm__("" : : "r"(i));
        }

        drawObj = D_8009F9EC;
        if (drawObj != 0) {
            if (D_8009FA94->unk5C == 1) {
                func_800694C8(&x, &y, drawObj->unk58, drawObj->unk50);
                callColor = 0xFFFF;
                width = 4;
                sum = x;
                callY = y;
                sum += offset;
                callX = (sum + 0xD) << 1;
                callY = (callY + 0x11) << 1;
                x = sum;
                __asm__("addu\t%0,%1,$zero" : "=r"(height) : "r"(width), "r"(callX), "r"(callY), "r"(callColor));
                func_8001C214(callColor, callX, callY, width, height, 0);
            }
        }

        sub = arg0->unkAC->unk300;
        if (sub != 0) {
            bits = sub->unkA4;
            if ((bits & 0x492492) != 0) {
                register u32 *colorTable __asm__("$21");
                register u32 color __asm__("$5");
                register u8 *tileCursor __asm__("$4");
                register u8 *tileLimit __asm__("$3");
                register s32 tileCmp __asm__("$3");
                register u32 dims __asm__("$4");
                register u32 palIndex __asm__("$2");
                register u32 *colorAddress __asm__("$2");
                register u32 tileWord __asm__("$2");
                register u32 px __asm__("$2");
                u32 py;

                i = 0;
                addressHi = 0x800A0000;
                __asm__("" : "=r"(addressHi) : "0"(addressHi));
                colorTable = (u32 *)(addressHi + 0x6F60);
                pos = sub;
                for (; i < 8; i++, pos = (struct Unk5AB8CSub *)((u8 *)pos + 4)) {
                    if ((bits & 2) != 0) {
                        func_800694C8(&x, &y, pos->pos[0].x << 9, pos->pos[0].y << 9);
                        palIndex = sub->colors[i];
                        __asm__("" : : "r"(palIndex) : "memory");
                        tileCursor = D_8009FA90;
                        tileLimit = D_8009F9C0;
                        palIndex &= 0x1F;
                        palIndex <<= 2;
                        colorAddress = (u32 *)(palIndex + (u32)colorTable);
                        __asm__("" : "=r"(tileLimit) : "0"(tileLimit), "r"(colorAddress));
                        tileCmp = tileCursor < tileLimit;
                        color = *colorAddress;
                        p = tileCursor;
                        if (tileCmp) {
                            D_8009FA90 = p + 0x1C;
                        }
                        px = x;
                        __asm__("" : : "r"(px) : "$4");
                        dims = 0x00020002;
                        px += offset;
                        x = px;
                        px = 0x03000000;
                        *(u32 *)p = px;
                        tileWord = color | 0x60000000;
                        *(u32 *)(p + 4) = tileWord;
                        px = (u16)x;
                        py = (u16)y;
                        *(u32 *)(p + 0xC) = dims;
                        *(s16 *)(p + 8) = px + 0xD;
                        *(s16 *)(p + 0xA) = py + 0x11;
                    }
                    bits >>= 3;
                }
            }
        }
    } else {
        if ((arg0->unk24 & 0x10) != 0) {
            callColor = 0xFFFF;
            callX = 0x4E;
            callY = 0x56;
            width = 4;
            __asm__("addu\t%0,%1,$zero" : "=r"(height) : "r"(width), "r"(callX), "r"(callY), "r"(callColor));
            func_8001C214(callColor, callX, callY, width, height, 0);
        }
    }
}

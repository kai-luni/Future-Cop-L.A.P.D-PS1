// Categories: Graphics > HUD > Minimap | Graphics > Renderer > Map Cell Geometry
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct { int x, y, z; } Vec;
extern void func_800648E8(void *, u32, Vec *);
/**
 * @brief Walks a packed minimap cell list and emits the visible cells as
 *        geometry, clipping against two bounds.
 *
 * @param obj Map block: the position triple at +0, a u16 index table at
 *        +0x3CC, the packed word list at +0x5D0.
 * @param list Output geometry list; each emitted cell appends 8 bytes.
 * @param a, b The two clip bounds (their difference is the type-4 delta).
 * @param index Start index into the +0x3CC table (word offset >> 4).
 * @return Nothing.
 *
 * @par Confirmed
 * The list pointer is obj + 0x5D0 + (table[index] >> 4). Each word's type
 * comes from the scratchpad table at 0x1F800000[(word >> 15) & 0x7F] & 7.
 * A set bit 0 marks a counted cell: the index advances by 1, or by 13 on
 * every fourth marked cell; the walk stops after 16 marked cells. Type 0
 * folds bit 11 into bit 0 and always emits; types 1..4 compute a signed
 * distance d from the current index against a/b (type 1: (idx & 0xF0) - b;
 * type 2: ((idx << 4) & 0xF0) - a; type 3: the sum of both minus a and b;
 * type 4: (a - b) - ((idx << 4) & 0xF0) + (idx & 0xF0)). d >= 0 emits only
 * with word bit 0x800 and d > 0; otherwise word bit 0x1000 is required.
 * Emission calls func_800648E8(list + arg * 8, word, &pos) with
 * arg = idx + (idx >> 4), tripled.
 *
 * @par Likely role
 * The minimap cell renderer: the scratchpad type table classifies cell
 * shapes, the two bounds are the visible window, and the 16-cell cap limits
 * the per-frame geometry budget.
 *
 * @par Unknown
 * The exact layout of the scratchpad type table and why every fourth marked
 * cell skips 13 indices.
 *
 * @par Callers
 * func_8006531C.
 */
void func_80065134(void *obj, void *list, int a, int b, unsigned int index)
{
    Vec pos;
    u32 *p;
    int count = 0;
    int d;
    int delta = a - b;
    register unsigned int old __asm__("$7");
    u32 word;
    unsigned int type;
    register unsigned int t __asm__("$2");
    register unsigned int u __asm__("$3");
    register unsigned int arg __asm__("$4");
    p = (u32 *)((u8 *)obj + 0x5d0 + (((u16 *)((u8 *)obj + 0x3cc))[index] >> 4));
    pos = *(Vec *)obj;
    obj = &pos;
    do {
        word = *p++;
        type = ((u8 *)0x1f800000)[(word >> 15) & 0x7f] & 7;
        old = index;
        if (word & 1) {
            word &= ~1;
            count++;
            index = old + 1;
            if ((count & 3) == 0) index = old + 13;
        }
        if (type == 0) {
            word |= (word >> 11) & 1;
            goto call;
        }
        switch (type) {
        case 1:
            d = (old & 0xf0) - b;
            break;
        case 2:
            d = ((old << 4) & 0xf0) - a;
            break;
        case 3:
            u = (old << 4) & 0xf0;
            __asm__("" : "+r"(u));
            t = (old & 0xf0) + 16;
            __asm__("" : "+r"(t));
            u += t;
            __asm__("" : "+r"(u));
            u -= a;
            __asm__("" : "+r"(u));
            d = u - b;
            break;
        case 4:
            d = delta - ((old << 4) & 0xf0) + (old & 0xf0);
            break;
        }
        if (d >= 0) {
            if (!(word & 0x800)) continue;
            if (d > 0) goto call;
        }
        if (!(word & 0x1000)) continue;
call:
        arg = old >> 4;
        __asm__("" : "+r"(arg));
        arg = old + arg;
        __asm__("" : "+r"(arg));
        t = arg << 1;
        __asm__("" : "+r"(t));
        arg = arg + t;
        __asm__("" : "+r"(arg));
        func_800648E8((u8 *)list + arg * 8, word, obj);
    } while (count < 16);
}

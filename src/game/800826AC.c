// Categories: Graphics > Image Compositor | Resources > Asset Tables
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned int u32;
typedef struct { u32 addr:24; u32 len:8; u32 code; } Page;
typedef struct { u32 addr:24; u32 len:8; u8 r,g,b,code; short x,y; u8 u,v; u16 clut,w,h; } Sprt;
typedef struct { u16 n; s8 a; s8 b; u32 t[2]; } Ent;
extern Ent D_8009B32C[];
extern s8 D_8009F874;
extern u8 D_8009F876;
extern int **D_8009F3F4;
extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");
extern void func_800861E4(int, int, int, int);

/**
 * @brief Composes the two-frame image block of one screen and uploads the slices.
 *
 * @param idx Entry index into the descriptor table D_8009B32C.
 * @return Nothing; returns when the table is empty or the entry has no records.
 *
 * @par Confirmed
 * The current index is cached in D_8009F874 and the upload flag in D_8009F876;
 * the loops walk up to two records per entry (`for (; j < 2; ...)` with a
 * 4-byte stride over `Ent`) and, for each non-empty table slot
 * `D_8009F3F4[t]`, call func_800861E4 with (`*D_8009F3F4[t]`, D_8009F876,
 * `s[1] + 0x140`, `off + s[3]`) - i.e. four pictures per entry, two per record.
 * The 2D pool is checked before each of the four packet writes, and the
 * texpage words 0xE1000105 / 0xE1000115 select the two page halves.
 *
 * @par Likely role
 * The compositor of the front-end/pause screen: an entry is a small graphic (two
 * rows of two slices) that is uploaded into the 320x240 buffer.
 *
 * @par Unknown
 * The record layout of D_8009B32C beyond its two-entry structure.
 */
void func_800826AC(int idx)
{
    register int cur __asm__("$2");
    register u32 val __asm__("$3");
    short h[10];
    register u32 cnt __asm__("$23");
    u32 k, j, i;
    int *tbl;
    short *p;
    u8 *packet;
    register u8 *next2 __asm__("$3");
    register u8 *next3 __asm__("$4");
    register u32 cmp3 __asm__("$3");
    register u8 *next4 __asm__("$3");
    int col;

    if (cur == idx) goto tail;
    tbl = (int *)D_8009F3F4;
    D_8009F874 = val;
    if (tbl == 0) return;
    cur = val < 14;
    if (!cur) goto tail;
    h[1] = 0; h[3] = 0; h[2] = 0; h[4] = 0;
    if (D_8009B32C[idx].a) {
        cnt = 2;
        h[0] = 1; h[6] = 0x20; h[5] = idx; h[8] = 0x31; h[9] = 0x31;
    } else {
        h[0] = idx; cnt = 1;
    }
    i = 0;
    if (cnt == 0) goto tail;
    {
    Ent *tab = D_8009B32C;
    short *base = h;
    short *g;
    __asm__("" : : "r"(base));
    g = base;
    do {
        int n = 0;
        register u16 e __asm__("$19") = g[0];
        short *q;
        short *r;
        Ent *en;
        j = n;
        q = g; r = base;
        en = (Ent *)((int)(e * 12) + (int)tab);
        for (; j < 2; q++, r++, j++, en = (Ent *)((u8 *)en + 4)) {
            u32 t = en->t[0];
            if (t && ((int *)D_8009F3F4)[t]) {
                n++;
                { register u32 one __asm__("$2") = 1; if (i == one) {
                    if (j != 0) r[6] = 0; else { register int m32 __asm__("$2") = 0x20; h[6] = m32; }
                }
            } } else {
                { register int m1 __asm__("$2") = -1; q[1] = m1; }
            }
        }
        if (n == tab[e].n) {
            int off = 0;
            register short *s __asm__("$17");
            j = off;
            s = g;
            for (; j < 2; j++, s++) {
                { register int m2 __asm__("$2") = -1; if (s[1] != m2)
                    func_800861E4(*D_8009F3F4[tab[e].t[j]], D_8009F876, s[1] + 0x140, off + s[3]); }
                off += 0x100;
            }
        }
        i++; g += 5;
    } while (i < cnt);
    }
tail:
    col = D_8009B32C[D_8009F874].b ? 0x60 : 0x80;
    if (gPacketCursor < gPacketLimit) goto a1;
    packet = gPacketCursor; __asm__("" : : "r"(packet)); goto p1;
a1: packet = gPacketCursor; gPacketCursor = packet + 28;
p1:
    ((Page *)packet)->addr = 0xFFFF; ((Page *)packet)->len = 1;
    ((Page *)packet)->code = 0xe1000105;
    next2 = gPacketCursor;
    if (next2 < gPacketLimit) goto a2;
    __asm__("" : : "r"(next2)); packet = next2; goto p2;
a2: packet = next2; gPacketCursor = packet + 28;
p2:
    ((Sprt *)packet)->code = 0x64;
    ((Sprt *)packet)->w = 0xa0; ((Sprt *)packet)->h = 0xf0;
    ((Sprt *)packet)->r = col; ((Sprt *)packet)->g = col; ((Sprt *)packet)->b = col;
    ((Sprt *)packet)->u = 0; ((Sprt *)packet)->v = 0;
    ((Sprt *)packet)->x = 0; ((Sprt *)packet)->y = 0;
    ((Sprt *)packet)->addr = 0xFFFF; ((Sprt *)packet)->len = 4;
    next3 = gPacketCursor;
    cmp3 = next3 < gPacketLimit;
    if (cmp3) goto a3;
    __asm__("" : : "r"(next3)); packet = next3; goto p3;
a3: packet = next3; gPacketCursor = packet + 28;
p3:
    ((Page *)packet)->addr = 0xFFFF; ((Page *)packet)->len = 1;
    ((Page *)packet)->code = 0xe1000115;
    next4 = gPacketCursor;
    if (next4 < gPacketLimit) goto a4;
    __asm__("" : : "r"(next4)); packet = next4; goto p4;
a4: packet = next4; gPacketCursor = packet + 28;
p4:
    ((Sprt *)packet)->code = 0x64;
    ((Sprt *)packet)->w = 0xa0; ((Sprt *)packet)->h = 0xf0;
    ((Sprt *)packet)->x = 0xa0;
    ((Sprt *)packet)->r = col; ((Sprt *)packet)->g = col; ((Sprt *)packet)->b = col;
    ((Sprt *)packet)->u = 0; ((Sprt *)packet)->v = 0;
    ((Sprt *)packet)->y = 0;
    ((Sprt *)packet)->addr = 0xFFFF; ((Sprt *)packet)->len = 4;
}

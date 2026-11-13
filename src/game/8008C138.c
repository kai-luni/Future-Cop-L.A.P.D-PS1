#include "game/types.h"

struct Entry {
    u8 pad[2];
    u8 mode;
    u8 cost;
    u8 pad4;
};

struct Port {
    u8 pad0[4];
    struct Entry *entries;
    u8 pad8[0x20];
    u8 *flags;
    u8 pad2c[8];
    u8 count;
    u8 pad35[0x22];
    u8 enabled[6];
    u8 ids[6];
    u8 pad63[0x83];
    u16 fieldE6;
    u8 type;
    u8 nentries;
};
extern void func_8008CD00(u8 *, int);
extern int D_8009C770;

void func_8008C138(struct Port *p)
{
    int i, j, n;
    register int mask __asm__("$7");
    register int found __asm__("$6");
    register u8 *ids __asm__("$5");
    register u8 *flags __asm__("$4");
    register u8 *enabled __asm__("$4");
    func_8008CD00(p->enabled, 6);
    if (p->fieldE6 && p->flags) {
        n = 6;
        if (p->count < 7) {
            __asm__("" : "=r"(p) : "0"(p));
            n = p->count;
        }
        i = 0;
        if (p->nentries) do {
            __asm__("" : "=r"(p) : "0"(p));
            found = 0;
            mask = p->entries[i].mode ? 0xff : 1;
            ids = p->ids;
            flags = p->flags;
            for (j = 0; j < n; j++) {
                if (*ids == i && (*flags & mask)) {
                    found = 1;
                    break;
                }
                ids++;
                flags++;
            }
            if (found) {
                if (D_8009C770 + p->entries[i].cost < 61)
                    D_8009C770 += p->entries[i].cost;
                else
                    found = 0;
            }
            if (found) {
                ids = p->ids;
                enabled = p->enabled;
                for (j = 0; j < n; j++) {
                    if (*ids++ == i)
                        *enabled = 1;
                    enabled++;
                }
            }
        } while (++i < p->nentries);
    } else if (((unsigned)(p->type - 4) < 2 || p->type == 7) && !p->fieldE6 && p->count >= 2) {
        if ((p->flags[0] & 0xc0) == 0x40 && (p->flags[1] & 1) && D_8009C770 + 10 < 61) {
            p->enabled[1] = 1;
            p->enabled[0] = 1;
            D_8009C770 += 10;
        }
    } else if (*(volatile u8 *)&p->type == 3) {
        p->enabled[0] = 1;
    } else if (!p->fieldE6) {
        enabled = (u8 *)p + 5;
        for (j = 5; j >= 0; j--, enabled--)
            enabled[0x57] = 1;
    }
}

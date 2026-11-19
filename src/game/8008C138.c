// Categories: Input > Controller Configuration
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
extern int gPadActuatorCostSum __asm__("D_8009C770");
void gSelectPadActuators(struct Port *p) __asm__("func_8008C138");


/**
 *  @brief Selects enabled controller entries subject to a shared cost limit.
 *
 *  Clears the six enable flags, then uses entry modes, IDs and input flags
 *  to select entries, with fallback rules for particular port types.
 *  @param p Port configuration, entry table and enable flags.
 *  @par Globals
 *  D_8009C770 is read and increased when an accepted selection incurs cost.
 *  @par Control flow
 *  Scans entries and up to six slots when configuration and flags are present;
 *  otherwise branches by type, fieldE6 and count to apply fallback rules.
 *  @par Calls
 *  func_8008CD00 clears the six-byte enabled array.
 *  @par Side effects
 *  Writes enabled flags and may increase the shared cost counter.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Clears the six-byte p->enabled array via func_8008CD00, then enables
 * entries in p->enabled when p->fieldE6 and p->flags are nonzero by matching
 * p->ids and p->flags against p->entries while keeping gPadActuatorCostSum
 * below 61. When p->fieldE6 is zero, it applies fallback rules for port types
 * 4, 5, and 7 (cost 10), type 3 (enables slot 0), or other types (enables all
 * six slots).
 */
void gSelectPadActuators(struct Port *p)
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
                if (gPadActuatorCostSum + p->entries[i].cost < 61)
                    gPadActuatorCostSum += p->entries[i].cost;
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
        if ((p->flags[0] & 0xc0) == 0x40 && (p->flags[1] & 1) && gPadActuatorCostSum + 10 < 61) {
            p->enabled[1] = 1;
            p->enabled[0] = 1;
            gPadActuatorCostSum += 10;
        }
    } else if (*(volatile u8 *)&p->type == 3) {
        p->enabled[0] = 1;
    } else if (!p->fieldE6) {
        enabled = (u8 *)p + 5;
        for (j = 5; j >= 0; j--, enabled--)
            enabled[0x57] = 1;
    }
}

// Categories: Platform > Serial I/O
#include "game/sio_globals.h"

#include "game/intc_globals.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
struct State { u8 a[4]; volatile u16 status; u8 b[2]; volatile u16 x; volatile u16 flags; u8 c[2]; volatile u16 y; };
struct Obj { u8 a[12]; u8 *data; u8 b[0x2C]; u8 *p; u8 c[0x10]; u8 v50; u8 d[0x97]; u8 ve8; };
extern struct State *gSio0RegBasePtr __asm__("D_8009C78C");
extern s32 gPadSlotIndex __asm__("D_8009C768");
extern s32 D_8009C780[];
extern void (*D_8009C748)(void *);
extern void (*D_8009C74C)(void *);
extern s32 *gInterruptStatusPtr __asm__("D_8009C788");
extern void func_8008C890(s32);
extern void func_8008AE44(void);

/**
 *  @brief Configures the port and processes pending callbacks for an object.
 *
 *  The routine initializes hardware registers, drains the selected pending count, and checks port status.
 *  It clears the object's pointed-to byte only when neither early-return condition applies.
 *  @param obj Object supplying callback data, mode, and completion state.
 *  @return One when the pointed-to byte is cleared, or zero on an early return.
 *  @par Globals
 *  D_8009C78C and D_8009C788 provide hardware state; D_8009C768 selects D_8009C780 and the pending count.
 *  D_8009C748 and D_8009C74C are callbacks invoked while draining and completing pending work.
 *  @par Control flow
 *  Drains positive counts, dispatches completion callbacks at zero, and branches on status and object flags.
 *  @par Calls
 *  func_8008C890 configures a mode; func_8008AE44 handles the persistent status condition.
 *  @par Side effects
 *  Writes hardware registers and pending counts, invokes callbacks, and may clear the byte at obj->p.
 *  @par Callers
 *  Confirmed: func_8008A43C, func_8008BF78.
 */
s32 func_8008A6CC(struct Obj *obj)
{
    s32 result;
    s32 n;
    register s32 *base __asm__("$17");
    register s32 idx __asm__("$2");
    register struct State *state __asm__("$4");
    register s32 flags __asm__("$3");
    gSio0RegBasePtr->flags = 0x40;
    gSio0RegBasePtr->flags = 0;
    gSio0RegBasePtr->x = 13;
    gSio0RegBasePtr->y = 0x88;
    func_8008C890(obj->ve8 == 8 ? 0x50 : 0x91);
    idx = gPadSlotIndex;
    state = gSio0RegBasePtr;
    flags = 0x1003;
    if (idx) flags = 0x3003;
    idx <<= 2;
    __asm__("" : "+r"(idx));
    state->flags = flags;
    {
        register u8 *arr __asm__("$3");
        idx = *(s32 *)((s32)D_8009C780 + idx);
        __asm__("" : "+r"(idx));
        arr = (u8 *)0x800a0000;
        __asm__("" : "+r"(arr));
        arr -= 0x3880;
        if (idx >= 0) {
            if (idx > 0) {
                base = (s32 *)arr;
                __asm__("" : "+r"(base));
                do {
                    n = --base[gPadSlotIndex];
                    D_8009C748(obj->data + n * 240);
                } while (base[gPadSlotIndex] > 0);
                __asm__ volatile ("" ::: "memory");
            }
            {
                register s32 index __asm__("$2");
                register u8 *arr2 __asm__("$3");
                index = gPadSlotIndex;
                __asm__("" : "+r"(index));
                arr2 = (u8 *)0x800a0000;
                __asm__("" : "+r"(arr2));
                arr2 -= 0x3880;
                if (((s32 *)arr2)[index] == 0) {
                    ((s32 *)arr2)[index] = -1;
                    D_8009C748(obj);
                    D_8009C74C(obj);
                }
            }
        }
    }
    if (gSio0RegBasePtr->status & 0x200) {
        gSio0RegBasePtr->flags |= 0x10;
        if (gSio0RegBasePtr->status & 0x200) {
            *(volatile u8 *)gSio0RegBasePtr = 1;
            func_8008AE44();
            (void)*(volatile u8 *)gSio0RegBasePtr;
            return 0;
        }
        *gInterruptStatusPtr = -0x81;
    }
    if (obj->v50 && *((u8 *)obj + 0x36)) return 0;
    *obj->p = 0;
    return 1;
}

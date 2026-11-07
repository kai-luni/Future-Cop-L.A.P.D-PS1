#include "game/types.h"
typedef struct Obj { s32 value; u8 pad[24]; s32 id; } Obj;
typedef struct Slot { s8 id; u8 type; u8 active; u8 pad; Obj *a; Obj *b; s32 time; } Slot;
extern Obj *D_8009F270, *D_8009F274;
extern Slot D_800A0A00[];
extern s32 D_8009FA2C;
extern s32 func_80023528(s32, s32 *);
extern void func_800181D4(Obj *);
extern s32 func_80021CB0(s32, s32, s32, void *, s32);
extern void func_80023500(void);
void func_800236FC(void)
{
    u16 i = 0;
    u8 failed = 0;
    s32 result;
    s32 time;
    if (D_8009F270 && D_8009F274) {
        if (D_8009F270->id == D_8009F274->id && func_80023528(D_8009F270->id, 0)) {
            while (i < 8) {
                if (D_800A0A00[i].id == -1) break;
                i++;
            }
            if (i >= 8) failed = 1;
        } else failed = 1;
        if (failed) {
            func_800181D4(D_8009F270);
            func_800181D4(D_8009F274);
        } else {
            result = func_80021CB0(D_8009F270->value, D_8009F274->value, -1, func_80023500, i);
            if (result >= 0) {
                D_800A0A00[i].active = 1;
                time = D_8009FA2C + 1500;
                D_800A0A00[i].id = result;
                D_800A0A00[i].time = time;
                D_800A0A00[i].type = D_8009F270->id;
                D_800A0A00[i].a = D_8009F270;
                D_800A0A00[i].b = D_8009F274;
            }
        }
        D_8009F274 = 0;
        D_8009F270 = 0;
    }
}

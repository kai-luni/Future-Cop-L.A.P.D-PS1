// Categories: Gameplay > Progression
typedef signed char s8;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
struct Mode { char pad[12]; s32 players; char pad10[60]; s8 value; };
struct Entry { char pad[31]; u8 current; u8 maximum; char rest[3]; };
extern s32 gPendingScreenState[] __asm__("D_8009F9A8");
extern s32 gFrameTickCounter[] __asm__("D_8009FA2C");
extern s32 gIdleDeadline __asm__("D_8009F870");
extern s32 gStillRunningState __asm__("D_8009F464");
extern s8 D_8009F45A, D_8009F889, D_8009F880, D_8009F888, D_8009F882;
extern s8 D_8009F3F1;
extern s8 gRowClearFlag __asm__("D_8009FA10");
extern u8 D_8009F8D9;
extern u8 gUnlockFlag __asm__("D_8009F3C2");
extern u8 gRumbleEnabled __asm__("D_8009F8D8");
extern s8 *D_8009F884;
extern s8 D_8009F458;
extern struct Mode *gGameSession[] __asm__("D_8009FA94");
extern struct Entry gWideTextTable[] __asm__("D_8009B4E4");
extern char D_800A38F8[];
extern void func_80078210(void), func_8001EC04(s32, s32), func_80078650(void *, s32);

/**
 *  @brief Updates mode-dependent progression state and invokes the next handler.
 *
 *  Sets a state value and deadline before dispatching on the current mode.
 *  Some modes update progression entries or mark a change flag.
 *  @par Globals
 *  D_8009F464 and D_8009F870 receive the state and deadline from D_8009FA2C.
 *  D_8009F45A selects the mode; D_8009F9A8 and D_8009F458 affect progression.
 *  D_8009F889, D_8009F880, D_8009F888, and D_8009F882 control progression updates.
 *  D_8009F3F1 and D_8009FA10 participate in change detection.
 *  D_8009FA94, D_8009F884, D_8009B4E4, and D_8009F3C2 supply entry data.
 *  D_8009F8D8, D_8009F8D9, and D_800A38F8 supply handler arguments.
 *  @par Control flow
 *  Mode 1 updates progression; mode 2 updates and scans entries, possibly setting a flag.
 *  Mode 3 makes two calls before falling through to the default handler.
 *  @par Calls
 *  func_80078210 handles modes 1 and 2; func_8001EC04 handles mode 3 values.
 *  func_80078650 handles the default path and mode 3 fallthrough.
 *  @par Side effects
 *  Changes global state, progression values, and possibly the change flag.
 *  @par Callers
 *  Confirmed: func_80077AD4.
 */
void func_800782EC(void)
{
    register s32 v __asm__("$2");
    register s32 raw __asm__("$6");
    register s32 tmp __asm__("$5");
    s8 value;
    u32 i;
    gStillRunningState = 4;
    gIdleDeadline = gFrameTickCounter[0] + 9000;
    switch (D_8009F45A) {
    case 1:
        if (gPendingScreenState[0] == 0) {
            s8 *p;
            __asm__("lui %0,%%hi(D_8009F458)\n\taddiu %0,%0,%%lo(D_8009F458)" : "=r"(p));
            if (D_8009F880 == *(s8 *)(D_8009F889 + (s32)p) && D_8009F880 < D_8009F888) {
                (*(s8 *)(D_8009F889 + (s32)p))++;
                if (*(s8 *)(D_8009F889 + (s32)p) < D_8009F888) {
                    D_8009F880++;
                    D_8009F882++;
                }
            }
        }
        if (D_8009F3F1 != D_8009F458) {
        set_flag:
            gRowClearFlag = 1;
        }
        func_80078210();
        break;
    case 2:
        {
            struct Mode *m;
            __asm__("lui %0,%%hi(D_8009FA94)\n\tlw %0,%%lo(D_8009FA94)(%0)" : "=r"(m));
            if (m->players == 1) {
                v = m->value;
                raw = v;
                tmp = (v < 0) ? 0 : (((__extension__({ __asm__("" : "+r"(raw)); v; })) < 11) ? raw : (__extension__({ __asm__("" : : "r"(raw)); 10; })));
                __asm__("" : "+r"(tmp));
                value = tmp;
                if (value > gWideTextTable[D_8009F884[D_8009F880]].current)
                    gWideTextTable[D_8009F884[D_8009F880]].current = value;
            }
        }
        for (i = 0; i < gUnlockFlag + 4; i++) {
            if (gWideTextTable[D_8009F884[i]].current > gWideTextTable[D_8009F884[i]].maximum)
                goto set_flag;
        }
        func_80078210();
        break;
    case 3:
        func_8001EC04(0, gRumbleEnabled);
        func_8001EC04(1, D_8009F8D9);
    default:
        func_80078650(D_800A38F8, 0);
        break;
    }
}

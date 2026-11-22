/* Source-derived characterizations with synthetic state; not retail fixtures. */
#include "leaf_kit.h"
u8 gpu __asm__("D_8009DDF2");
u8 f_80092B84(void) __asm__("func_80092B84");
s32 music __asm__("D_8009F330");
s32 f_8004522C(void) __asm__("func_8004522C");
s32 mode __asm__("D_8009C800");
int f_8008D7CC(void) __asm__("func_8008D7CC");
u16 irq __asm__("D_8009CC8A");
u16 f_800915D4(void) __asm__("func_800915D4");
u8 layer __asm__("D_8009F224");
u8 f_8001C034(void) __asm__("func_8001C034");
u8 video __asm__("D_8009F504");
u8 f_80087D3C(void) __asm__("func_80087D3C");
s32 blocks __asm__("D_800A09DC");
s32 f_80022B74(void) __asm__("func_80022B74");
int f_80048E94(void) __asm__("func_80048E94");
int f_80085A1C(void) __asm__("func_80085A1C");
int f_80087814(void) __asm__("func_80087814");
int f_8008781C(void) __asm__("func_8008781C");
u8 sound __asm__("D_8009F340");
void f_80044FEC(void) __asm__("func_80044FEC");
void * zone __asm__("D_8009F6F8");
void f_80031984(void) __asm__("func_80031984");
s32 defs __asm__("D_8009F7F8");
void f_800609F4(void) __asm__("func_800609F4");
s32 descs __asm__("D_8009F848");
void f_8006A88C(void) __asm__("func_8006A88C");
s32 placements __asm__("D_8009F7B8");
void f_8005130C(void) __asm__("func_8005130C");
s32 freed __asm__("D_8009F7B0");
void f_8004EFD4(void) __asm__("func_8004EFD4");
u32 stream_flag __asm__("D_8009F6DC");
s32 boot_state __asm__("D_8009F464");
void *camera[1] __asm__("D_8009F9E4");
void *camera_list __asm__("D_8009F588");
s32 camera_id __asm__("D_8009F58C");
void *sprite_table __asm__("D_8009F77C");
s32 spatial_mode __asm__("D_8009F734");
s32 *movie_context __asm__("D_8009F4BC");
void *card_manager __asm__("D_8009AB08");
s32 cursor __asm__("D_8009F5AC");
s32 rng[20] __asm__("D_800A04D0");
s32 f_8002A58C(void) __asm__("func_8002A58C");
s32 f_8007F8CC(void) __asm__("func_8007F8CC");
void f_8007F8BC(void) __asm__("func_8007F8BC");
void f_8001774C(void) __asm__("func_8001774C");
void f_8004979C(void *) __asm__("func_8004979C");
void f_800451EC(s32) __asm__("func_800451EC");
void f_800858F4(s32) __asm__("func_800858F4");
int f_800207B4(void) __asm__("func_800207B4");
int f_8001F37C(void) __asm__("func_8001F37C");
s32 f_800196C4(void) __asm__("func_800196C4");

int main(void)
{
    int object, i;
    s32 ctx[0x50];
    struct CardState { u8 before[0x900]; s32 result; u8 between[0xa0]; s32 pending; } state;
    /* @leaf 80092B84 source returns the current gpu value */
    LK_FUNC("func_80092B84");
    gpu = 119; FCK_EQ(f_80092B84(), 119, "first value");
    gpu = 255; FCK_EQ(f_80092B84(), 255, "second value");
    /* @leaf 8004522C source returns the current music value */
    LK_FUNC("func_8004522C");
    music = 127; FCK_EQ(f_8004522C(), 127, "first value");
    music = -1; FCK_EQ(f_8004522C(), -1, "second value");
    /* @leaf 8008D7CC source returns the current mode value */
    LK_FUNC("func_8008D7CC");
    mode = 1; FCK_EQ(f_8008D7CC(), 1, "first value");
    mode = 0; FCK_EQ(f_8008D7CC(), 0, "second value");
    /* @leaf 800915D4 source returns the current irq value */
    LK_FUNC("func_800915D4");
    irq = 3; FCK_EQ(f_800915D4(), 3, "first value");
    irq = 65535; FCK_EQ(f_800915D4(), 65535, "second value");
    /* @leaf 8001C034 source returns the current layer value */
    LK_FUNC("func_8001C034");
    layer = 0; FCK_EQ(f_8001C034(), 0, "first value");
    layer = 241; FCK_EQ(f_8001C034(), 241, "second value");
    /* @leaf 80087D3C source returns the current video value */
    LK_FUNC("func_80087D3C");
    video = 0; FCK_EQ(f_80087D3C(), 0, "first value");
    video = 128; FCK_EQ(f_80087D3C(), 128, "second value");
    /* @leaf 80022B74 source returns the current blocks value */
    LK_FUNC("func_80022B74");
    blocks = 2; FCK_EQ(f_80022B74(), 2, "first value");
    blocks = -1; FCK_EQ(f_80022B74(), -1, "second value");
    /* @leaf 80048E94 source returns a constant */
    LK_FUNC("func_80048E94"); FCK_EQ(f_80048E94(), 0, "constant result");
    /* @leaf 80085A1C source returns a constant */
    LK_FUNC("func_80085A1C"); FCK_EQ(f_80085A1C(), 304, "constant result");
    /* @leaf 80087814 source returns a constant */
    LK_FUNC("func_80087814"); FCK_EQ(f_80087814(), 3072, "constant result");
    /* @leaf 8008781C source returns a constant */
    LK_FUNC("func_8008781C"); FCK_EQ(f_8008781C(), 31616, "constant result");
    /* @leaf 80044FEC source clears the global */
    LK_FUNC("func_80044FEC"); sound = 37; f_80044FEC(); FCK_EQ(sound, 0, "reset");
    /* @leaf 80031984 source clears the global */
    LK_FUNC("func_80031984"); zone = &object; f_80031984(); FCK_CHECK(zone == 0, "reset");
    /* @leaf 800609F4 source clears the global */
    LK_FUNC("func_800609F4"); defs = 37; f_800609F4(); FCK_EQ(defs, 0, "reset");
    /* @leaf 8006A88C source clears the global */
    LK_FUNC("func_8006A88C"); descs = 37; f_8006A88C(); FCK_EQ(descs, 0, "reset");
    /* @leaf 8005130C source clears the global */
    LK_FUNC("func_8005130C"); placements = 37; f_8005130C(); FCK_EQ(placements, 0, "reset");
    /* @leaf 8004EFD4 source clears the global */
    LK_FUNC("func_8004EFD4"); freed = 37; f_8004EFD4(); FCK_EQ(freed, 0, "reset");
    /* @leaf 8002A58C source normalizes the stream flag */
    LK_FUNC("func_8002A58C"); stream_flag = 0; FCK_EQ(f_8002A58C(), 0, "inactive");
    stream_flag = 0x80000000; FCK_EQ(f_8002A58C(), 1, "active");
    /* @leaf 8007F8CC source compares the boot state with two */
    LK_FUNC("func_8007F8CC"); boot_state = 2; FCK_EQ(f_8007F8CC(), 0, "running");
    boot_state = 3; FCK_EQ(f_8007F8CC(), 1, "not running");
    /* @leaf 8007F8BC source stores boot state two */
    LK_FUNC("func_8007F8BC"); boot_state = -1; f_8007F8BC(); FCK_EQ(boot_state, 2, "started");
    /* @leaf 8001774C source resets camera pointers and ID */
    LK_FUNC("func_8001774C"); camera[0] = &object; camera_list = &object; camera_id = 42;
    f_8001774C(); FCK_CHECK(camera[0] == 0 && camera_list == 0, "pointers reset");
    FCK_EQ(camera_id, 0, "ID reset");
    /* @leaf 8004979C source stores the table pointer */
    LK_FUNC("func_8004979C"); f_8004979C(&object); FCK_CHECK(sprite_table == &object, "pointer set");
    f_8004979C(0); FCK_CHECK(sprite_table == 0, "null set");
    /* @leaf 800451EC source stores the mode word */
    LK_FUNC("func_800451EC"); f_800451EC(2); FCK_EQ(spatial_mode, 2, "mode set");
    f_800451EC(-1); FCK_EQ(spatial_mode, -1, "negative mode");
    /* @leaf 800858F4 source writes the context word at offset 0x114 */
    LK_FUNC("func_800858F4"); lk_fill(ctx, 0xa5, sizeof(ctx)); movie_context = ctx;
    f_800858F4(-0x123456); FCK_EQ(ctx[0x114 / 4], -0x123456, "page set");
    FCK_CHECK(lk_all(ctx, 0, 0x114, 0xa5) && lk_all(ctx, 0x118, sizeof(ctx), 0xa5), "surrounding bytes");
    lk_fill(&state, 0xa5, sizeof(state)); state.result = -2; state.pending = 77; card_manager = &state;
    /* @leaf 800207B4 source reads card result at offset 0x900 */
    LK_FUNC("func_800207B4"); FCK_EQ(f_800207B4(), -2, "result");
    state.result = 42; FCK_EQ(f_800207B4(), 42, "changed result");
    /* @leaf 8001F37C source reads separate pending word at offset 0x9a4 */
    LK_FUNC("func_8001F37C"); FCK_EQ(f_8001F37C(), 77, "distinct pending");
    state.pending = -3; FCK_EQ(f_8001F37C(), -3, "negative pending");
    FCK_EQ(state.result, 42, "result unchanged");
    /* @leaf 800196C4 source lagged-additive sequence and wrap */
    LK_FUNC("func_800196C4"); for (i = 0; i < 20; i++) rng[i] = 0x1000 + i * 0x10;
    cursor = 0; FCK_EQ(f_800196C4(), 0x2150, "wrap sums entries 19 and 2");
    FCK_EQ(cursor, 19, "cursor wraps"); FCK_EQ(rng[19], 0x2150, "sum stored");
    FCK_EQ(f_800196C4(), 0x2130, "next sums entries 18 and 1");
    FCK_EQ(cursor, 18, "cursor decremented"); cursor = 5; rng[4] = 100; rng[7] = 23;
    FCK_EQ(f_800196C4(), 123, "middle pair"); FCK_EQ(rng[7], 23, "source unchanged");
    cursor = 1; rng[0] = 51; rng[3] = 7; rng[19] = 99;
    FCK_EQ(f_800196C4(), 58, "cursor one uses entry zero");
    FCK_EQ(cursor, 0, "cursor reaches zero"); FCK_EQ(rng[19], 99, "wrap slot unchanged");
    return lk_done("leaf_hundred_c");
}

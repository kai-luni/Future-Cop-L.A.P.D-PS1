// Categories: Platform > Memory Card | BIOS > Events
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef struct {
    u8 pad[0x8C8];
    s32 count;
    u8 pad2[8];
    s32 h0, h1, h2, h3, h4, h5, h6, h7;
} State;
extern State *gOperationState __asm__("D_8009AB08");
extern u8 D_8001FAB8[], D_8001FF44[], D_8001FE40[], D_8001FD28[];
extern u8 D_8001F394[], D_8001F968[], D_8001F818[], D_8001F784[];
extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
extern s32 func_8008C960(u32, u32, u32, void *);
extern void func_8008C980(s32);
void gAcquireMcEvents(void) __asm__("func_80020120");


/**
 * @brief Acquires the memory-card event subsystem, registering eight events.
 *
 * Reference-counted acquisition. When the counter at D_8009AB08+0x8C8 is
 * zero, a critical section is entered and four high-level card events (class
 * 0xF4000001) plus four low-level card I/O events (class 0xF0000011) are
 * opened, all in interrupt callback mode 0x1000, then enabled. The four
 * specs per class are completion (4), general/write error (0x8000), busy
 * error (0x100) and eject error (0x2000). Handles land at +0x8D4..+0x8F0 and
 * the counter is incremented on every call; func_8002036C is the release
 * counterpart that closes the same eight handles at count zero.
 *
 * @par Globals
 * D_8009AB08 - card subsystem block: counter +0x8C8, handles +0x8D4..+0x8F0.
 * D_8001FAB8, D_8001FF44, D_8001FE40, D_8001FD28 - high-level card callbacks.
 * D_8001F394, D_8001F968, D_8001F818, D_8001F784 - low-level card callbacks.
 *
 * @par Control flow
 * One branch on count == 0 guarding an unrolled open sequence, an unrolled
 * enable sequence and the critical-section pair; the increment always runs.
 *
 * @par Calls
 * func_8008C9C0 / func_8008C9E0 - critical section enter/exit via COP0 status.
 * func_8008C960 - BIOS OpenEvent through the B0 vector, function index 8.
 * func_8008C980 - BIOS EnableEvent through the B0 vector, function index 0Ch.
 *
 * @par Side effects
 * Registers and enables eight BIOS events; writes the handle words and the
 * reference counter in the card subsystem block.
 *
 * @par Callers
 * Confirmed: func_800206D0 (subsystem setup path).
 *
 * @par Role and path
 * Acquires the memory-card BIOS event subsystem by opening and enabling four
 * 0xF4000001 high-level events and four 0xF0000011 low-level I/O events via
 * func_8008C960 and func_8008C980 inside a critical section when
 * gOperationState->count is 0, then increments count. Reads and writes
 * D_8009AB08; called by func_800206D0.
 */
void gAcquireMcEvents(void)
{
    if (gOperationState->count == 0) {
        gDisableCpuInterrupts();
        gOperationState->h0 = func_8008C960(0xF4000001, 4, 0x1000, D_8001FAB8);
        gOperationState->h1 = func_8008C960(0xF4000001, 0x8000, 0x1000, D_8001FF44);
        gOperationState->h2 = func_8008C960(0xF4000001, 0x100, 0x1000, D_8001FE40);
        gOperationState->h3 = func_8008C960(0xF4000001, 0x2000, 0x1000, D_8001FD28);
        gOperationState->h4 = func_8008C960(0xF0000011, 4, 0x1000, D_8001F394);
        gOperationState->h5 = func_8008C960(0xF0000011, 0x8000, 0x1000, D_8001F968);
        gOperationState->h6 = func_8008C960(0xF0000011, 0x100, 0x1000, D_8001F818);
        gOperationState->h7 = func_8008C960(0xF0000011, 0x2000, 0x1000, D_8001F784);
        func_8008C980(gOperationState->h0);
        func_8008C980(gOperationState->h1);
        func_8008C980(gOperationState->h2);
        func_8008C980(gOperationState->h3);
        func_8008C980(gOperationState->h4);
        func_8008C980(gOperationState->h5);
        func_8008C980(gOperationState->h6);
        func_8008C980(gOperationState->h7);
        gEnableCpuInterrupts();
    }
    gOperationState->count++;
}

// Categories: Platform > CD-ROM | Resources > File Access
typedef unsigned char u8;
typedef struct { int state; volatile int remain; int pos; volatile int result; int idx; void (*cb)(int, int); int unk18; int unk1C; int buf; u8 flag; } St;
typedef struct { int flags, a, b, c, d, e; } Ent;
extern St func_80010000;
extern Ent gFileSlotTable[] __asm__("D_800A6D78");
extern u8 gLengthTable[] __asm__("D_800A85C0");
extern u8 gCdCurrentLoc[] __asm__("D_8009F61C");
extern volatile unsigned int *gTransferPending __asm__("D_8009F250");
extern volatile unsigned int *gTransferStatusPtr __asm__("D_8009F254");
extern int gTransferError __asm__("D_8009F244");
extern int gTransferRetryCount __asm__("D_8009F24C");
extern int gCdCallbackActive __asm__("D_8009F618");
extern int gCdAbortFlag __asm__("D_8009F228");
extern volatile u8 gEventBuffer[] __asm__("D_800A0798");
extern int gSetCdLogLevel(int) __asm__("func_8008E0D8");
extern int gCdRead(void *, int) __asm__("func_8008E598");
extern int gCdlLocToByteOffset(void *) __asm__("func_8001C588");
extern void gIncrementBcdLoc(void *) __asm__("func_8001C288");
extern void gStartCdDirRead(void) __asm__("func_8001C718");
extern void gResetCdReadState(void) __asm__("func_8001C680");
extern void gClearWord(void *) __asm__("func_8001C470");
extern int func_8008CD10(int, void *, int);
extern int gCdControl(int, int, void *) __asm__("func_8008E1BC");
extern void gSetIrqCallback(int, int) __asm__("func_800914B0");
extern void gStartCdInit(void) __asm__("func_8008DEA0");
extern int gSetCdReadyCallback(void *) __asm__("func_8008E1A8");
extern void gCdSectorReadyCallbackAddress(void) __asm__("D_8001C808");
void gCdSectorCallback(int arg) __asm__("func_8001C808");


/**
 * @brief Completion callback of the streaming file reader: verifies the sector
 *        address, copies the block into the slot buffer and reports completion to
 *        the requester.
 *
 * @param mode Callback mode byte (masked to 8 bits). 1 runs the loader state
 *        machine; any other value takes the abort path with result -0x1D.
 * @return Nothing.
 *
 * @par Confirmed
 * The body is bracketed by func_8008E0D8(0) and func_8008E0D8(saved), which save
 * and restore the driver flag word. The loader state block at 0x80010000 holds the
 * state byte +0x00, the remaining count +0x04, the written position +0x08, the
 * result +0x0C, the slot index +0x10, the completion callback +0x14, a driver word
 * +0x18, the destination pointer +0x20 and a flag byte +0x24; the result is cleared
 * on entry, and func_8001CE54 and func_8001D130 fill the same fields, which is how
 * their roles are known.
 * State 2 (transfer): when gTransferPending is non-zero the code first waits for bit
 * 0x1000000 of the word pointed at by gTransferStatusPtr, with a 0x10000 iteration cap.
 * A 3-unit header is read into a 0x10-byte stack buffer and func_8001C588 turns it
 * into a sector address; that address is compared with the slot word at +0x08
 * masked with -0x800. On a mismatch gTransferError is set to -1, two drain reads (0x200
 * and 0x46 units) are issued, and the directory read is restarted with
 * func_8001C718 unless the wanted address lies fewer than 0x10 sectors ahead, in
 * which case the sector is simply dropped.
 * Still in state 2: func_8001C288(D_8009F61C) advances the sector address by one,
 * n = slot +0x08 & 0x7FF is the offset inside the sector, and
 * length = min(remaining + n, 0x800) - n. Below 0x800 the block is read into
 * D_800A85C0 and func_8008CD10 copies length units from +n into the destination;
 * otherwise the block is read straight into the destination. A failed read sets the
 * result to -0xC, and a trailing 0x46-unit read always follows.
 * Counters advance only on success (destination, position and slot +0x08 by length,
 * remaining down by length). When the result is negative or nothing remains, the
 * state returns to 0, the slot flag word is set to 1 (plus 0x100 when the flag byte
 * is 1), gTransferRetryCount is re-armed to 0x8CA and the requester's callback is called with
 * (position, result), bracketed by D_8009F618 = 1 and 0.
 * State 1 does nothing, so a pending transfer waits for the next callback. State 0
 * advances the sector address the same way and runs the 0x8CA countdown in
 * gTransferRetryCount; at zero it calls func_8001C680 and reloads the countdown. Any other
 * state sets the result to -0x18.
 * Abort path (mode other than 1): result -0x1D, D_8009F228 = 1, then a retry loop
 * that polls func_8008E1BC(1, 0, D_800A0798), leaves when bit 0x10 of the event byte
 * is clear and otherwise runs func_800914B0(2, 0), func_8008DEA0,
 * func_8001C470(D_8009F61C) and reinstalls itself through
 * func_8008E1A8(func_8001C808); func_8001C718 then restarts the directory read and
 * D_8009F228 returns to 0.
 *
 * @par Likely role
 * The completion half of the slot reader that func_8001CE54 starts: the 3-unit
 * header compare against the sector-aligned payload address of the slot, the
 * sector-at-a-time copy and the 0x8CA (2250) re-arm all fit a streamed read of one
 * file slot, and the drive routine func_8001C680 it falls back to touches the
 * 0x1F801800/0x1F801803 interface registers. The two distinct read sizes (a header
 * and a trailer) suggest a Mode 2 sector layout with its sync block and its EDC/ECC
 * tail separated from the user data.
 *
 * @par Unknown
 * - What the 0x200 and 0x46 read sizes count. The buffers suggest 32-bit words: the
 *   0x46 read lands in a 288-byte stack buffer, which 70 words fill almost exactly,
 *   and 0x200 words are the 2048 bytes of a sector payload. The word reading would
 *   also make 0x46 the 280-byte EDC/ECC tail of such a sector, yet the 3-unit header
 *   read then covers the 12-byte sync block, while func_8001C588 parses the first
 *   three bytes as minute, second and sector.
 * - What gTransferPending and gTransferStatusPtr point at; only the 0x1000000 handshake bit is
 *   visible.
 * - The exact meaning of the -0xC, -0x18 and -0x1D result codes.
 *
 * @par Callers
 * No direct callers in the call graph. func_8001D130 installs the address with
 * func_8008E1A8 and the abort path reinstalls it, so the driver invokes it through
 * that saved function pointer.
 *
 * @par Role and path
 * CD-ROM driver sector callback bracketed by func_8008E0D8: on mode 1 it
 * dispatches on 0x80010000.state (0 increments gCdCurrentLoc and counts down
 * gTransferRetryCount to call func_8001C680; 2 reads 3-unit sector header via
 * func_8008E598, checks against gFileSlotTable, copies up to 0x800 bytes via
 * gLengthTable and func_8008CD10, and invokes 0x80010000.cb). Non-1 modes
 * enter the abort retry loop with gCdAbortFlag, func_800914B0, func_8008DEA0,
 * and func_8001C718; installed by func_8001D130 via func_8008E1A8.
 */
void gCdSectorCallback(int arg)
{
    int saved;
    int cur, want, n, len;
    register int i __asm__("$4");
    register volatile unsigned int *port __asm__("$5");
    register St *head __asm__("$5");
    register int one __asm__("$3");
    u8 buf[0x10];
    u8 buf2[0x120];
    St *s;
    St *q;
    Ent *entries;
    register u8 *loc __asm__("$4");

    saved = gSetCdLogLevel(0);
    head = &func_80010000;
    __asm__ volatile ("" : : "r" (saved), "r" (head));
    arg &= 0xFF;
    one = 1;
    head->result = 0;
    __asm__ volatile ("" : : "r" (head) : "memory");
    s = head;
    if (arg == one) {
        register int state __asm__("$3");
        state = func_80010000.state;
        switch (state) {
        case 1:
            break;
        case 2:
            if (gTransferPending != 0) {
                port = gTransferStatusPtr;
                i = 0;
                while ((*port & 0x1000000) && i++ != 0x10000) { }
                __asm__ volatile ("" : : "r" (i));
            }
            gCdRead(buf, 3);
            cur = gCdlLocToByteOffset(buf);
            entries = gFileSlotTable;
            q = &func_80010000;
            want = entries[q->idx].c & -0x800;
            if (cur != want) {
                gTransferError = -1;
                gCdRead(gLengthTable, 0x200);
                gCdRead(buf2, 0x46);
                if (cur < want && want < cur + 0x10)
                    return;
                gStartCdDirRead();
                return;
            }
            loc = gCdCurrentLoc;
            __asm__ volatile ("" : : "r" (loc));
            gIncrementBcdLoc(loc);
            n = gFileSlotTable[q->idx].c & 0x7FF;
            len = q->remain + n;
            if (len > 0x800) len = 0x800;
            len -= n;
            if (len < 0x800) {
                if (gCdRead(gLengthTable, 0x200) == 1) {
                    int idx;
                    register int cnt __asm__("$6");
                    idx = q->idx;
                    __asm__ volatile ("" : : "r" (idx));
                    cnt = len;
                    __asm__ volatile ("" : : "r" (idx), "r" (cnt) : "$5");
                    gTransferError = gFileSlotTable[idx].c & -0x800;
                    func_8008CD10(q->buf, gLengthTable + n, cnt);
                } else {
                    q->result = -0xC;
                }
            } else {
                if (!gCdRead((void *)q->buf, 0x200))
                    q->result = -0xC;
            }
            gCdRead(buf2, 0x46);
            if (func_80010000.result >= 0) {
                func_80010000.buf += len;
                func_80010000.pos += len;
                gFileSlotTable[func_80010000.idx].c += len;
                func_80010000.remain -= len;
            }
            if (func_80010000.result < 0 || func_80010000.remain <= 0) {
                func_80010000.state = 0;
                gFileSlotTable[func_80010000.idx].flags = 1;
                gTransferRetryCount = 0x8CA;
                if (func_80010000.flag == 1)
                    gFileSlotTable[func_80010000.idx].flags |= 0x100;
                gCdCallbackActive = 1;
                func_80010000.cb(func_80010000.pos, func_80010000.result);
                gCdCallbackActive = 0;
            }
            break;
        case 0:
            loc = gCdCurrentLoc;
            __asm__ volatile ("" : : "r" (loc));
            gIncrementBcdLoc(loc);
            if (gTransferRetryCount != 0) {
                gTransferRetryCount--;
                if (gTransferRetryCount == 0) {
                    gResetCdReadState();
                    gTransferRetryCount = 0x8CA;
                }
            }
            break;
        default:
            func_80010000.result = -0x18;
            break;
        }
    } else {
        s->result = -0x1D;
        gCdAbortFlag = one;
        do {
            gCdControl(1, 0, (void *)gEventBuffer);
            if (!(gEventBuffer[0] & 0x10)) break;
            gSetIrqCallback(2, 0);
            gStartCdInit();
            loc = gCdCurrentLoc;
            __asm__ volatile ("" : : "r" (loc));
            gClearWord(loc);
            s->unk18 = gSetCdReadyCallback(gCdSectorReadyCallbackAddress);
            gCdCallbackActive = 0;
        } while (gEventBuffer[0] & 0x10);
        gStartCdDirRead();
        gCdAbortFlag = 0;
    }
    gSetCdLogLevel(saved);
}

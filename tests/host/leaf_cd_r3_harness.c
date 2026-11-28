/* CD-ROM / DMA3 / kernel-patch platform band (round 3): five unchanged
 * src/game units that no other host suite reaches.
 *
 *  - CD driver DMA3 setup: gStartCdDmaAsync (fire-and-forget) and
 *    gReadCdDmaSync (blocking) program the same eight registers that the
 *    driver keeps as pointers in D_8009CB6C..D_8009CBB0, then poll the
 *    controller status bit 0x40 and (sync only) the DMA busy bit 0x01000000.
 *  - gSetCdReadCallback: the handwritten asm stub for libcd CdReadCallback.
 *  - gPatchKernelMaskHalfword: a halfword read-modify-write at $3 + 0xA.
 *  - gSetCdLogLevel: the libcd CdSetDebug equivalent (D_8009C8B0).
 *
 * Two hardware substitutions are made, neither of which touches the units:
 *
 *  1. CD-ROM / DMA3 registers. The units reach them only through the eight
 *     pointer words D_8009CB6C/78/A0/7C/A4/A8/AC/B0, so every one of those
 *     points at a harness-owned memory cell in a MAP_SHARED page. Those cells
 *     are memory, not a drive: the two handshake bits the controller raises
 *     (status bit 0x40 after a command write, and clearing DMA busy bit
 *     0x01000000 at transfer end) would make the unchanged polling loops spin
 *     forever. A forked helper process plays exactly those two transitions and
 *     records the ack and refusal counts plus the COM-delay snapshot taken
 *     while it clears the busy bit. It models the handshake only: no transfer, no timing, no
 *     interrupt, no CD command protocol is emulated. The one readiness rule it
 *     does enforce is that a command write is acknowledged only while the DMA
 *     channel still holds its idle sentinel (0xDEADBEEF), so a unit that does
 *     not actually wait for the ack gets none and fails the status/ack checks
 *     instead of passing silently.
 *
 *  2. gSetCdLogLevel's write target. The store address is a literal constant
 *     (0x800A0000 - 0xDD4 = 0x8009C8B0, kseg0), not one of the redirection
 *     pointers, and qemu-user cannot map kseg0 (its guest address space stops
 *     below 0x80000000: MAP_FIXED at 0x70000000 succeeds, at 0x7FFFE000 and
 *     0x8009C000 it returns ENOMEM and touching the range is a SIGSEGV). A
 *     SIGSEGV handler therefore decodes the faulting SW, checks its effective
 *     address, records the stored value and skips the instruction (it is the
 *     delay slot of the unit's `jr $ra`, so the handler resumes at ra). The unit's
 *     *read* side still comes from the linked D_8009C8B0 symbol, so the two
 *     halves are asserted separately - in retail they are the same address.
 *
 * Every other callee of these five units is a leaf (they call nothing), so no
 * stand-in is needed. Expected values are literals taken from the unit bodies,
 * not from a re-implementation.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* ========================================================================
 * Harness-owned storage
 * ======================================================================== */

/* The eight CD-ROM/DMA3 register pointers the CD units dereference. */
u8 *gCdIndexStatusReg_g __asm__("D_8009CB6C");
u8 *gCdRequestStatusReg_g __asm__("D_8009CB78");
u32 *gCdAccessDelayReg_g __asm__("D_8009CBA0");
u32 *gComAccessDelayReg_g __asm__("D_8009CB7C");
u32 *gDmaControlCdromReg_g __asm__("D_8009CBA4");
u32 *gDma3MemAddrReg_g __asm__("D_8009CBA8");
u32 *gDma3BlockCtrlReg_g __asm__("D_8009CBAC");
u32 *gDma3ChannelCtrlReg_g __asm__("D_8009CBB0");
/* CD read completion callback slot and the CD debug verbosity word. */
s32 gCdReadCallbackSlot_g __asm__("D_8009C810");
s32 gCdLogLevel_g __asm__("D_8009C8B0");

/* ========================================================================
 * Units under test (prototypes copied from the units' own declarations)
 * ======================================================================== */
s32 gStartCdDmaAsync(u32 destination, u32 size) __asm__("func_8008FD30");
s32 gReadCdDmaSync(u32 destination, u32 size) __asm__("func_8008FC30");
/* the unit declares this void with no arguments; the retail stub is the
 * libcd exchange (a0 = new callback, v0 = previous one). */
s32 gSetCdReadCallback(s32 callback) __asm__("func_80091240");
void gPatchKernelMaskHalfword(void) __asm__("func_8008DC50");
s32 gSetCdLogLevel(s32 value) __asm__("func_8008E0D8");

/* ========================================================================
 * Freestanding process/mapping plumbing (no libc): open("/dev/zero"), mmap,
 * fork, wait4, exit. qemu-mipsel passes o32 syscall args 5 and 6 in $8/$9.
 * ======================================================================== */
static long fcs_sys3(long n, long a, long b, long c)
{
    register long v0 __asm__("$2") = n;
    register long a0 __asm__("$4") = a;
    register long a1 __asm__("$5") = b;
    register long a2 __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
    return v0;
}

static long fcs_open_zero(void)
{
    register long v0 __asm__("$2") = 4005;          /* open */
    register long a0 __asm__("$4") = (long)"/dev/zero";
    register long a1 __asm__("$5") = 2;             /* O_RDWR */
    register long a2 __asm__("$6") = 0;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7");
    return v0;
}

/* mmap(addr, len, prot, flags, fd, offset) */
static long fcs_mmap(u32 addr, u32 len, u32 prot, u32 flags, long fd, long off)
{
    register long v0 __asm__("$2") = 4090;
    register long a0 __asm__("$4") = (long)addr;
    register long a1 __asm__("$5") = (long)len;
    register long a2 __asm__("$6") = (long)prot;
    register long a3 __asm__("$7") = (long)flags;
    register long a4 __asm__("$8") = fd;
    register long a5 __asm__("$9") = off;
    __asm__ volatile("syscall" : "+r"(v0)
                     : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5)
                     : "memory", "$10", "$11", "$12", "$13", "$14", "$15",
                       "$24", "$25");
    return v0;
}

static long fcs_fork(void)
{
    register long v0 __asm__("$2") = 4002;
    __asm__ volatile("syscall" : "+r"(v0) :: "memory", "$4", "$5", "$6", "$7",
                     "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
                       "$24", "$25");
    return v0;
}

static long fcs_waitpid(long pid)
{
    register long v0 __asm__("$2") = 4007;          /* wait4 */
    register long a0 __asm__("$4") = pid;
    register long a1 __asm__("$5") = 0;
    register long a2 __asm__("$6") = 0;
    register long a3 __asm__("$7") = 0;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                     : "memory", "$8", "$9");
    return v0;
}

static void fcs_exit(long code)
{
    register long v0 __asm__("$2") = 4001;
    register long a0 __asm__("$4") = code;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0) : "memory");
}

/* ========================================================================
 * CD-ROM/DMA3 register file: memory cells in a shared page plus the two
 * handshake transitions the controller performs.
 * ======================================================================== */
/* The channel register value that means "no transfer programmed". The helper
 * only acknowledges a command write while the channel is idle, which is what
 * makes "did the unit wait for the ack?" observable at all. */
#define FC_CHAN_IDLE 0xDEADBEEFu

struct FcCdRegs {
    u32 status;           /* D_8009CB6C target: byte command/status register */
    u32 request;          /* D_8009CB78 target: byte request register */
    u32 access_delay;     /* D_8009CBA0 target */
    u32 com_access;       /* D_8009CB7C target */
    u32 dma_control;      /* D_8009CBA4 target: read-modify-write |= 0x8000 */
    u32 dma3_mem;         /* D_8009CBA8 target */
    u32 dma3_block;       /* D_8009CBAC target */
    u32 dma3_chan;        /* D_8009CBB0 target */
    /* helper bookkeeping, not part of the register file */
    u32 stop;
    u32 status_zero_writes;
    u32 ack_refused;
    u32 busy_clears;
    u32 com_at_busy_clear;
};

static volatile struct FcCdRegs *fc_regs;
static long fc_device_pid;

/* Runs in the forked helper: play the two handshakes, nothing else. */
static void fc_device_loop(volatile struct FcCdRegs *r)
{
    volatile u32 k;
    for (;;) {
        if (r->stop) break;
        /* A real controller does not answer within a few nanoseconds. The
         * settle spin keeps the helper from observing a command write in the
         * handful of instructions between a unit's status check and its next
         * register write, which is the only window in which a unit that does
         * not wait could still see the acknowledgement. */
        for (k = 0; k < 256; k++) {}
        if (*(volatile u8 *)&r->status == 0) {
            if (r->dma3_chan == FC_CHAN_IDLE) {
                *(volatile u8 *)&r->status = 0x40;
                r->status_zero_writes++;
            } else {
                r->ack_refused++;      /* command written while a channel is armed */
            }
        }
        if (r->dma3_chan == 0x11000000u) {
            r->com_at_busy_clear = r->com_access;
            r->dma3_chan = 0x10000000u;
            r->busy_clears++;
        }
    }
}

static int fc_device_up;

static void fc_device_start(void)
{
    long fd, addr;

    fd = fcs_open_zero();
    if (fd < 0) return;
    addr = fcs_mmap(0, 0x2000, 3, 0x01, fd, 0);   /* RW, MAP_SHARED on /dev/zero */
    if (addr <= 0) return;
    fc_regs = (volatile struct FcCdRegs *)addr;
    fc_regs->status = 0x40;
    fc_regs->request = 0;
    fc_regs->access_delay = 0;
    fc_regs->com_access = 0;
    fc_regs->dma_control = 0;
    fc_regs->dma3_mem = 0;
    fc_regs->dma3_block = 0;
    fc_regs->dma3_chan = FC_CHAN_IDLE;
    fc_regs->stop = 0;
    fc_regs->status_zero_writes = 0;
    fc_regs->ack_refused = 0;
    fc_regs->busy_clears = 0;
    fc_regs->com_at_busy_clear = 0;

    fc_device_pid = fcs_fork();
    if (fc_device_pid == 0) {
        fc_device_loop(fc_regs);
        fcs_exit(0);
        for (;;) {}
    }
    if (fc_device_pid < 0) return;
    fc_device_up = 1;

    /* bind the driver's register pointers into the mapped cells */
    gCdIndexStatusReg_g = (u8 *)&fc_regs->status;
    gCdRequestStatusReg_g = (u8 *)&fc_regs->request;
    gCdAccessDelayReg_g = (u32 *)&fc_regs->access_delay;
    gComAccessDelayReg_g = (u32 *)&fc_regs->com_access;
    gDmaControlCdromReg_g = (u32 *)&fc_regs->dma_control;
    gDma3MemAddrReg_g = (u32 *)&fc_regs->dma3_mem;
    gDma3BlockCtrlReg_g = (u32 *)&fc_regs->dma3_block;
    gDma3ChannelCtrlReg_g = (u32 *)&fc_regs->dma3_chan;
}

static void fc_device_stop(void)
{
    if (!fc_device_up) return;
    fc_regs->stop = 1;
    fcs_waitpid(fc_device_pid);
    fc_device_up = 0;
}

/* Bounded liveness probe: one command write must be answered with status
 * 0x40. Without it a dead helper would hang the unit's poll forever. */
static int fc_device_ready(void)
{
    u32 i;
    if (!fc_device_up) return 0;
    fc_regs->dma3_chan = FC_CHAN_IDLE;
    fc_regs->status = 0;
    for (i = 0; i < 20000000u; i++) {
        if (*(volatile u8 *)&fc_regs->status == 0x40) {
            fc_regs->status_zero_writes = 0;
            return 1;
        }
    }
    return 0;
}

/* Reset the register file to a known non-zero state before one call. */
static void fc_regs_reset(u32 dma_control, u32 com_access, u32 chan)
{
    fc_regs->status = 0x40;
    fc_regs->request = 0;
    fc_regs->access_delay = 0;
    fc_regs->com_access = com_access;
    fc_regs->dma_control = dma_control;
    fc_regs->dma3_mem = 0;
    fc_regs->dma3_block = 0;
    fc_regs->dma3_chan = chan;
    fc_regs->status_zero_writes = 0;
    fc_regs->ack_refused = 0;
    fc_regs->busy_clears = 0;
    fc_regs->com_at_busy_clear = 0;
}

#define REG_STATUS  (*(volatile u8 *)&fc_regs->status)
#define REG_REQUEST (*(volatile u8 *)&fc_regs->request)

/* ========================================================================
 * kseg0 store trap for gSetCdLogLevel.
 * ======================================================================== */
static volatile s32 fc_trap_count;
static volatile s32 fc_trap_bad;
static volatile u32 fc_trap_addr;
static volatile u32 fc_trap_value;
static volatile u32 fc_trap_insn;

struct FcSigaction { u32 flags; u32 handler; u32 restorer; u32 mask[4]; };

void fc_sig_return(void) __asm__("fc_sig_return");
__asm__(".text\n.set noreorder\n.globl fc_sig_return\n"
        "fc_sig_return:\n\tli\t$2,4193\n\tsyscall\n\tnop\n.set reorder\n");

/* mips o32 sigcontext: sc_regmask, sc_status, sc_pc (u64), sc_regs[32] (u64
 * each), ... - verified by dumping the frame of a deliberate store fault. */
static void fc_kseg0_handler(int sig, void *info, void *frame)
{
    u32 *sc = (u32 *)frame;
    u32 pc;
    u32 insn, op, rs, rt;
    s32 imm;
    (void)sig; (void)info;

    fc_trap_count++;
    if (!sc) return;
    pc = sc[2];
    if (pc & 3) return;
    insn = *(volatile u32 *)pc;
    op = insn >> 26;
    if (op != 0x2B) {
        /* qemu reports the branch PC when the branch's delay slot faults, and
         * the retail store is exactly that delay slot: retarget to pc + 4. */
        u32 next = *(volatile u32 *)(pc + 4);
        if ((next >> 26) == 0x2B) {
            pc = pc + 4;
            insn = next;
            op = 0x2B;
        }
    }
    rs = (insn >> 21) & 31;
    rt = (insn >> 16) & 31;
    imm = (s32)(short)(insn & 0xFFFF);
    fc_trap_insn = insn;
    if (op == 0x2B) {                     /* sw rt, imm(rs) */
        fc_trap_addr = (rs == 0) ? (u32)imm : sc[4 + 2 * rs] + (u32)imm;
        fc_trap_value = sc[4 + 2 * rt];
    } else {
        fc_trap_bad++;
        fc_trap_addr = 0;
    }
    /* The retail store sits in the delay slot of `jr $ra` (the unit's last
     * instruction before the branch), so the guest must resume at the jump
     * target, not at pc + 4 - decoding the delay slot is the whole point of
     * skipping the instruction here. */
    {
        u32 prev = *(volatile u32 *)(pc - 4);
        if ((prev >> 26) == 0 && (prev & 0x3F) == 0x08) {   /* jr rs */
            sc[2] = sc[4 + 2 * ((prev >> 21) & 31)];
        } else {
            sc[2] = pc + 4;
        }
    }
}

static long fcs_sigaction(int sig, struct FcSigaction *act)
{
    register long v0 __asm__("$2") = 4194;        /* rt_sigaction */
    register long a0 __asm__("$4") = sig;
    register long a1 __asm__("$5") = (long)act;
    register long a2 __asm__("$6") = 0;
    register long a3 __asm__("$7") = 16;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                     : "memory", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
    return v0;
}

static s32 fc_trap_installed;

static void fc_trap_install(void)
{
    struct FcSigaction sa;
    sa.flags = 0x04000000u;                       /* SA_RESTORER (mips) */
    sa.handler = (u32)(unsigned long)&fc_kseg0_handler;
    sa.restorer = (u32)(unsigned long)&fc_sig_return;
    sa.mask[0] = sa.mask[1] = sa.mask[2] = sa.mask[3] = 0;
    fc_trap_installed = (fcs_sigaction(11, &sa) == 0);   /* SIGSEGV */
}

/* ========================================================================
 * Call trampoline for gPatchKernelMaskHalfword: the unit reads $3 (base) and
 * $2 (mask) as register variables that its caller has to supply.
 * ======================================================================== */
void fc_call_patch(u32 p, u32 mask) __asm__("fc_call_patch");
__asm__(".text\n.set noreorder\n.set noat\n.globl fc_call_patch\n"
        "fc_call_patch:\n\t"
        "addiu\t$29,$29,-32\n\t"
        "sw\t$31,16($29)\n\t"
        "move\t$3,$4\n\t"
        "move\t$2,$5\n\t"
        "jal\tfunc_8008DC50\n\t"
        "nop\n\t"
        "lw\t$31,16($29)\n\t"
        "addiu\t$29,$29,32\n\t"
        "jr\t$31\n\t"
        "nop\n"
        ".set at\n.set reorder\n");

/* ========================================================================
 * Tests
 * ======================================================================== */

static void test_start_cd_dma_async(void)
{
    /* @leaf 8008FD30 source src/game/platform/gStartCdDmaAsync.c: programs the eight CD-ROM/DMA3 registers through the D_8009CB6C..CBB0 pointers (status byte 0, request 0x80, delay 0x21020843, COM 0x1325, control |= 0x8000, mem = destination, block = size | 0x10000), waits for status bit 0x40, then writes and reads 0x11400100 and returns 0 */
    LK_FUNC("func_8008FD30");
    if (!fc_device_ready()) {
        FCK_CHECK(0, "CD register handshake helper did not answer");
        return;
    }

    fc_regs_reset(0x00001234, 0x7777, 0xDEADBEEF);
    FCK_EQ(gStartCdDmaAsync(0x00123400, 0x00000020), 0, "always returns 0");
    FCK_EQ(REG_STATUS, 0x40, "status byte was 0 and the controller raised 0x40");
    FCK_EQ(fc_regs->status_zero_writes, 1, "the unit wrote 0 to the status register exactly once");
    FCK_EQ(fc_regs->ack_refused, 0, "the command write happened with the channel idle (unit waited for 0x40)");
    FCK_EQ(REG_REQUEST, 0x80, "request register = 0x80");
    FCK_EQ(fc_regs->access_delay, 0x21020843, "access delay = 0x21020843");
    FCK_EQ(fc_regs->com_access, 0x1325, "COM access delay = 0x1325");
    FCK_EQ(fc_regs->dma_control, 0x00009234, "control register keeps 0x1234 and gains 0x8000");
    FCK_EQ(fc_regs->dma3_mem, 0x00123400, "DMA3 memory address = destination");
    FCK_EQ(fc_regs->dma3_block, 0x00010020, "block control = size | 0x10000");
    FCK_EQ(fc_regs->dma3_chan, 0x11400100, "channel control = 0x11400100");
    FCK_EQ(fc_regs->busy_clears, 0, "the async path never waits for the busy bit");

    fc_regs_reset(0x00000000, 0x00000000, 0xDEADBEEF);
    FCK_EQ(gStartCdDmaAsync(0xFFFFFFFF, 0x00000000), 0, "zero size still returns 0");
    FCK_EQ(fc_regs->dma_control, 0x00008000, "control register starts empty: only 0x8000");
    FCK_EQ(fc_regs->dma3_mem, 0xFFFFFFFF, "all-ones destination is stored verbatim");
    FCK_EQ(fc_regs->dma3_block, 0x00010000, "size 0 becomes the bare 0x10000 flag");
    FCK_EQ(fc_regs->status_zero_writes, 1, "second call zeroes the status register once");
    FCK_EQ(fc_regs->ack_refused, 0, "no refused command on the second call either");

    fc_regs_reset(0x0000FFFF, 0x00000000, 0xDEADBEEF);
    FCK_EQ(gStartCdDmaAsync(0x00000010, 0x00008000), 0, "bit 16 already clear in size");
    FCK_EQ(fc_regs->dma3_block, 0x00018000, "size 0x8000 gains bit 16");
    fc_regs_reset(0x0000FFFF, 0x00000000, 0xDEADBEEF);
    FCK_EQ(gStartCdDmaAsync(0x00000010, 0x00018000), 0, "bit 16 already set in size");
    FCK_EQ(fc_regs->dma3_block, 0x00018000, "0x18000 | 0x10000 is still 0x18000: OR, not add");
    fc_regs_reset(0x00000001, 0x00000000, 0xDEADBEEF);
    FCK_EQ(gStartCdDmaAsync(0x00000010, 0x0000FFFF), 0, "full low halfword size");
    FCK_EQ(fc_regs->dma3_block, 0x0001FFFF, "0xFFFF | 0x10000 = 0x1FFFF");
    FCK_EQ(fc_regs->dma_control, 0x00008001, "control keeps bit 0 and gains 0x8000");
}

static void test_read_cd_dma_sync(void)
{
    /* @leaf 8008FC30 source src/game/platform/gReadCdDmaSync.c: programs the same register file with delay 0x20943 / COM 0x1323, waits for status bit 0x40, starts DMA3 with 0x11000000, polls the busy bit 0x01000000 until it clears, restores COM to 0x1325 and returns 0 */
    LK_FUNC("func_8008FC30");
    if (!fc_device_ready()) {
        FCK_CHECK(0, "CD register handshake helper did not answer");
        return;
    }

    fc_regs_reset(0x00000010, 0x00007777, 0xDEADBEEF);
    FCK_EQ(gReadCdDmaSync(0x00080000, 0x00000040), 0, "always returns 0");
    FCK_EQ(REG_STATUS, 0x40, "status byte zeroed then acknowledged with 0x40");
    FCK_EQ(fc_regs->status_zero_writes, 1, "one status write per call");
    FCK_EQ(fc_regs->ack_refused, 0, "the command write happened with the channel idle (unit waited for 0x40)");
    FCK_EQ(REG_REQUEST, 0x80, "request register = 0x80");
    FCK_EQ(fc_regs->access_delay, 0x20943, "access delay = 0x20943");
    FCK_EQ(fc_regs->dma_control, 0x00008010, "control keeps 0x10 and gains 0x8000");
    FCK_EQ(fc_regs->dma3_mem, 0x00080000, "DMA3 memory address = destination");
    FCK_EQ(fc_regs->dma3_block, 0x00010040, "block control = size | 0x10000");
    FCK_EQ(fc_regs->dma3_chan, 0x10000000, "the unit's 0x11000000 arrived and the busy bit was cleared");
    FCK_EQ(fc_regs->busy_clears, 1, "exactly one DMA start per call");
    FCK_EQ(fc_regs->com_at_busy_clear, 0x1323, "COM still held 0x1323 while the transfer was running");
    FCK_EQ(fc_regs->com_access, 0x1325, "COM restored to 0x1325 after completion");

    fc_regs_reset(0x00008000, 0x00001234, FC_CHAN_IDLE);
    FCK_EQ(gReadCdDmaSync(0xFFFFFFFF, 0x00000000), 0, "zero size still returns 0");
    FCK_EQ(fc_regs->dma_control, 0x00008000, "already-set bit 0x8000 is idempotent");
    FCK_EQ(fc_regs->dma3_mem, 0xFFFFFFFF, "all-ones destination is stored verbatim");
    FCK_EQ(fc_regs->dma3_block, 0x00010000, "size 0 becomes the bare 0x10000 flag");
    FCK_EQ(fc_regs->busy_clears, 1, "one DMA start on the second call too");
    FCK_EQ(fc_regs->com_at_busy_clear, 0x1323, "COM was re-armed to 0x1323 before the wait");
    FCK_EQ(fc_regs->ack_refused, 0, "no refused command on the second sync call");
    FCK_EQ(fc_regs->com_access, 0x1325, "and restored again afterwards");
}

static void test_set_cd_read_callback(void)
{
    /* @leaf 80091240 source src/game/platform/gSetCdReadCallback.c: returns the word in D_8009C810 and stores $a0 there in the branch delay slot */
    LK_FUNC("func_80091240");
    gCdReadCallbackSlot_g = 0x11112222;
    FCK_EQ(gSetCdReadCallback(0x0000BEEF), 0x11112222, "previous callback returned");
    FCK_EQ(gCdReadCallbackSlot_g, 0x0000BEEF, "new callback installed in D_8009C810");
    FCK_EQ(gSetCdReadCallback(0x0000CAFE), 0x0000BEEF, "second exchange returns the first one");
    FCK_EQ(gCdReadCallbackSlot_g, 0x0000CAFE, "the slot tracks the last install");
    FCK_EQ(gSetCdReadCallback(0), 0x0000CAFE, "uninstall still reports the old pointer");
    FCK_EQ(gCdReadCallbackSlot_g, 0, "the slot can hold NULL");
    FCK_EQ(gSetCdReadCallback(0), 0, "NULL in, NULL out");
    gCdReadCallbackSlot_g = 0x12345678;
    FCK_EQ(gSetCdReadCallback(0x12345678), 0x12345678, "reinstalling the same value returns it");
    FCK_EQ(gCdReadCallbackSlot_g, 0x12345678, "slot unchanged by a duplicate install");
    gCdReadCallbackSlot_g = (s32)(unsigned long)&fc_kseg0_handler;
    FCK_EQ(gSetCdReadCallback((s32)(unsigned long)&fc_device_loop), (s32)(unsigned long)&fc_kseg0_handler,
           "real code pointers round-trip as the previous value");
    FCK_EQ(gCdReadCallbackSlot_g, (s32)(unsigned long)&fc_device_loop,
           "real code pointer lands in the slot");
}

static void test_patch_kernel_halfword(void)
{
    u16 cells[8];
    s32 i;

    /* @leaf 8008DC50 source src/game/platform/gPatchKernelMaskHalfword.c: reads the halfword at $3 + 0xA, stores it back ORed with $2 and 0x12 (halfword store, so the result is truncated to 16 bits) and then runs its 0x28-count delay loop */
    LK_FUNC("func_8008DC50");
    for (i = 0; i < 8; i++) cells[i] = 0xA5A5;
    cells[0] = 0x0000;
    fc_call_patch((u32)(unsigned long)&cells[0] - 0xA, 0x0000F000);
    FCK_EQ(cells[0], 0xF012, "zero | mask 0xF000 | 0x12");
    FCK_EQ(cells[1], (u16)0xA5A5, "neighbour above untouched");
    FCK_EQ(cells[7], (u16)0xA5A5, "far neighbour untouched");

    cells[2] = 0x0100;
    fc_call_patch((u32)(unsigned long)&cells[2] - 0xA, 0x00000000);
    FCK_EQ(cells[2], 0x0112, "mask 0 still forces bit 1 and bit 4 (0x12)");
    FCK_EQ(cells[1], (u16)0xA5A5, "the write did not drift one halfword down");

    cells[3] = 0xFFFF;
    fc_call_patch((u32)(unsigned long)&cells[3] - 0xA, 0x00000000);
    FCK_EQ(cells[3], 0xFFFF, "all-ones halfword is a no-op");

    cells[4] = 0x0000;
    fc_call_patch((u32)(unsigned long)&cells[4] - 0xA, 0xFFFF0000);
    FCK_EQ(cells[4], 0x0012, "the halfword store drops mask bits above bit 15");

    cells[5] = 0x8000;
    fc_call_patch((u32)(unsigned long)&cells[5] - 0xA, 0x00000001);
    FCK_EQ(cells[5], 0x8013, "bit 15 survives, mask bit 0 and 0x12 are added");
    FCK_EQ(cells[6], (u16)0xA5A5, "the next halfword is still untouched");
    FCK_EQ(cells[0], 0xF012, "earlier result not revisited");
}

static void test_set_cd_log_level(void)
{
    /* @leaf 8008E0D8 source src/game/platform/gSetCdLogLevel.c: returns the previous D_8009C8B0 word and stores the new value at the literal address 0x800A0000 - 0xDD4 = 0x8009C8B0 */
    LK_FUNC("func_8008E0D8");
    fc_trap_install();
    FCK_EQ(fc_trap_installed, 1, "the kseg0 store trap is installed");
    if (!fc_trap_installed) return;

    fc_trap_count = 0; fc_trap_addr = 0; fc_trap_value = 0; fc_trap_bad = 0;
    gCdLogLevel_g = 7;
    FCK_EQ(gSetCdLogLevel(3), 7, "previous level 7 returned");
    FCK_EQ(fc_trap_count, 1, "exactly one kseg0 store trapped");
    FCK_EQ(fc_trap_bad, 0, "the trapped instruction was a word store");
    FCK_EQ(fc_trap_addr, 0x8009C8B0, "store target is 0x800A0000 - 0xDD4");
    FCK_EQ(fc_trap_value, 3, "the new level 3 was the stored word");
    FCK_EQ(gCdLogLevel_g, 7, "the read side still reads the linked D_8009C8B0 symbol");

    gCdLogLevel_g = 0;
    FCK_EQ(gSetCdLogLevel(0), 0, "clearing the level returns the zero it read");
    FCK_EQ(fc_trap_count, 2, "second store trapped");
    FCK_EQ(fc_trap_value, 0, "level 0 stored");

    gCdLogLevel_g = -1;
    FCK_EQ(gSetCdLogLevel(0x00012345), -1, "a negative level is returned unchanged");
    FCK_EQ(fc_trap_addr, 0x8009C8B0, "same target for the second call");
    FCK_EQ(fc_trap_value, 0x00012345, "0x12345 stored verbatim");

    gCdLogLevel_g = 5;
    FCK_EQ(gSetCdLogLevel(5), 5, "same value in and out");
    FCK_EQ(fc_trap_count, 4, "one trapped store per call");
    FCK_EQ(fc_trap_bad, 0, "all four traps decoded as word stores");
    FCK_EQ(fc_trap_value, 5, "the last stored word is 5");
}

int main(void)
{
    fc_device_start();
    test_start_cd_dma_async();
    test_read_cd_dma_sync();
    fc_device_stop();
    test_set_cd_read_callback();
    test_patch_kernel_halfword();
    test_set_cd_log_level();
    return lk_done("leaf_cd_r3");
}

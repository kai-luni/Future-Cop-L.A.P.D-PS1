// Categories: Input > Port Initialization
typedef unsigned char u8;
typedef struct Obj Obj;
struct Obj {
    int a, b, c;
    int d;
    int e, f, g, h, i, j, k, l;
    u8 *out;
    int n, o;
    u8 *buf1, *buf2;
    u8 pad[0x19];
    u8 flags[6];
    u8 rest[0x8d];
};
extern int gPadExtEnableFlag __asm__("D_8009C774");
extern int gPadPresentFlag __asm__("D_8009C760");
extern void (*gPadStepErrorHook)() __asm__("D_8009C728");
extern void (*gPadResetPortHook)() __asm__("D_8009C72C");
extern void (*gPadTxByteHook)() __asm__("D_8009C730");
extern void (*gPadConfigHook)() __asm__("D_8009C734");
extern void (*gPadAddrToCodeHook)() __asm__("D_8009C738");
extern void (*gPadLookupPortFn)() __asm__("D_8009C73C");
extern void (*gPadCompleteHook)() __asm__("D_8009C74C");
extern Obj *gPadBasePort __asm__("D_8009C75C");
extern Obj gCompareAddress[] __asm__("D_800A3C20");
extern u8 gPadWorkBufA[] __asm__("D_800A3B90");
extern u8 gPadWorkBufB[] __asm__("D_800A3BD8");
extern void gAdvancePadSlot() __asm__("func_8008BF78");
extern void gResetPadPortState() __asm__("func_8008BF10");
extern void gReadPadTxByte() __asm__("func_8008C07C");
extern void gSelectPadActuators() __asm__("func_8008C138");
extern void gGetPadPortCode() __asm__("func_8008C3B4");
extern void gLookupPadPort() __asm__("func_8008C3EC");
extern void gConsumePadCmd() __asm__("func_8008C06C");
extern void gInstallPadProtoHooks() __asm__("func_8008C410");
extern void func_8008CD00();
extern void gInitPadCallbackTable() __asm__("func_8008A3A8");
void gInitPadPorts(u8 *a, u8 *b) __asm__("func_8008BDB0");


/**
 *  @brief Initializes two port objects and installs their dispatch functions.
 *
 *  The objects receive caller-provided output buffers and separate internal buffer regions.
 *  Initialization clears the object array, sets sentinel bytes and marks the subsystem ready.
 *  @param a Output buffer assigned to the first object.
 *  @param b Output buffer assigned to the second object.
 *  @par Globals
 *  D_8009C760 and D_8009C774 are reset; D_8009C75C points to D_800A3C20.
 *  D_8009C728, D_8009C72C, D_8009C730, D_8009C734, D_8009C738, D_8009C73C and D_8009C74C receive dispatch pointers.
 *  D_800A3B90 and D_800A3BD8 provide per-object buffer regions.
 *  @par Control flow
 *  A two-iteration loop initializes each object and its six flag bytes.
 *  @par Calls
 *  func_8008C410 performs preliminary setup; func_8008CD00 clears the array; func_8008A3A8 performs final setup.
 *  @par Side effects
 *  Writes global state, object storage and the first two bytes of each supplied output buffer.
 *  @par Callers
 *  Confirmed: func_8001E87C.
 *
 * @par Role and path
 * Resets gPadPresentFlag and D_8009C774, calls func_8008C410(), installs
 * seven pad dispatch function pointers (D_8009C728..D_8009C73C and
 * D_8009C74C), points gPadBasePort at gCompareAddress (D_800A3C20), and
 * zeroes 0x1E0 bytes via func_8008CD00. Initializes both 0xF0-byte port
 * structures with caller output buffers a and b, internal buffers D_800A3B90
 * and D_800A3BD8 at stride 0x23, and six 0xFF flag bytes, then calls
 * func_8008A3A8() and sets gPadPresentFlag to 1.
 */
void gInitPadPorts(u8 *a, u8 *b)
{
    register int i __asm__("$8");
    register int j __asm__("$3");
    register int ff __asm__("$9");
    register u8 *q __asm__("$4");
    register void (*value)() __asm__("$2");
    register u8 *flag __asm__("$5");
    register Obj *p __asm__("$16");
    register u8 *arg1 __asm__("$17") = a;
    register u8 *arg2 __asm__("$18") = b;
    register u8 *x __asm__("$6");
    register u8 *y __asm__("$7");
    gPadPresentFlag = 0;
    gPadExtEnableFlag = 0;
    gInstallPadProtoHooks();
    p = gCompareAddress;
    gPadStepErrorHook = gAdvancePadSlot;
    gPadResetPortHook = gResetPadPortState;
    gPadTxByteHook = gReadPadTxByte;
    gPadConfigHook = gSelectPadActuators;
    gPadAddrToCodeHook = gGetPadPortCode;
    gPadLookupPortFn = gLookupPadPort;
    __asm__("" ::: "memory");
    value = gConsumePadCmd;
    __asm__("" : "+r"(value) : : "memory");
    q = (u8 *)p;
    __asm__("" : "+r"(q) : : "memory");
    gPadBasePort = p;
    gPadCompleteHook = value;
    func_8008CD00(q, 0x1e0);
    i = 0;
    ff = 255;
    q = (u8 *)p + 0x40;
    y = gPadWorkBufB;
    x = gPadWorkBufA;
    p[0].out = arg1;
    p[1].out = arg2;
    do {
        flag = p->flags;
        *(int *)(q - 0x34) = 0;
        *(Obj **)(q - 0x30) = p;
        (*(u8 **)(q - 0x10))[0] = ff;
        (*(u8 **)(q - 0x10))[1] = 0;
        *(u8 **)(q - 4) = x;
        *(u8 **)q = y;
        j = 5;
        do { *flag++ = ff; } while (--j >= 0);
        y += 0x23;
        x += 0x23;
        i++;
        q += 0xf0;
        p++;
    } while (i < 2);
    gInitPadCallbackTable();
    gPadPresentFlag = 1;
    __asm__("" ::: "memory");
}

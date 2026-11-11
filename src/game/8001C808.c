typedef unsigned char u8;
typedef struct { int state; volatile int remain; int pos; volatile int result; int idx; void (*cb)(int, int); int unk18; int unk1C; int buf; u8 flag; } St;
typedef struct { int flags, a, b, c, d, e; } Ent;
extern St func_80010000;
extern Ent D_800A6D78[];
extern u8 D_800A85C0[];
extern u8 D_8009F61C[];
extern volatile unsigned int *D_8009F250;
extern volatile unsigned int *D_8009F254;
extern int D_8009F244, D_8009F24C, D_8009F228, D_8009F618;
extern volatile u8 D_800A0798[];
extern int func_8008E0D8(int);
extern int func_8008E598(void *, int);
extern int func_8001C588(void *);
extern void func_8001C288(void *);
extern void func_8001C718(void);
extern void func_8001C680(void);
extern void func_8001C470(void *);
extern int func_8008CD10(int, void *, int);
extern int func_8008E1BC(int, int, void *);
extern void func_800914B0(int, int);
extern void func_8008DEA0(void);
extern int func_8008E1A8(void *);
extern void D_8001C808(void);

void func_8001C808(int arg)
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

    saved = func_8008E0D8(0);
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
            if (D_8009F250 != 0) {
                port = D_8009F254;
                i = 0;
                while ((*port & 0x1000000) && i++ != 0x10000) { }
                __asm__ volatile ("" : : "r" (i));
            }
            func_8008E598(buf, 3);
            cur = func_8001C588(buf);
            entries = D_800A6D78;
            q = &func_80010000;
            want = entries[q->idx].c & -0x800;
            if (cur != want) {
                D_8009F244 = -1;
                func_8008E598(D_800A85C0, 0x200);
                func_8008E598(buf2, 0x46);
                if (cur < want && want < cur + 0x10)
                    return;
                func_8001C718();
                return;
            }
            loc = D_8009F61C;
            __asm__ volatile ("" : : "r" (loc));
            func_8001C288(loc);
            n = D_800A6D78[q->idx].c & 0x7FF;
            len = q->remain + n;
            if (len > 0x800) len = 0x800;
            len -= n;
            if (len < 0x800) {
                if (func_8008E598(D_800A85C0, 0x200) == 1) {
                    int idx;
                    register int cnt __asm__("$6");
                    idx = q->idx;
                    __asm__ volatile ("" : : "r" (idx));
                    cnt = len;
                    __asm__ volatile ("" : : "r" (idx), "r" (cnt) : "$5");
                    D_8009F244 = D_800A6D78[idx].c & -0x800;
                    func_8008CD10(q->buf, D_800A85C0 + n, cnt);
                } else {
                    q->result = -0xC;
                }
            } else {
                if (!func_8008E598((void *)q->buf, 0x200))
                    q->result = -0xC;
            }
            func_8008E598(buf2, 0x46);
            if (func_80010000.result >= 0) {
                func_80010000.buf += len;
                func_80010000.pos += len;
                D_800A6D78[func_80010000.idx].c += len;
                func_80010000.remain -= len;
            }
            if (func_80010000.result < 0 || func_80010000.remain <= 0) {
                func_80010000.state = 0;
                D_800A6D78[func_80010000.idx].flags = 1;
                D_8009F24C = 0x8CA;
                if (func_80010000.flag == 1)
                    D_800A6D78[func_80010000.idx].flags |= 0x100;
                D_8009F618 = 1;
                func_80010000.cb(func_80010000.pos, func_80010000.result);
                D_8009F618 = 0;
            }
            break;
        case 0:
            loc = D_8009F61C;
            __asm__ volatile ("" : : "r" (loc));
            func_8001C288(loc);
            if (D_8009F24C != 0) {
                D_8009F24C--;
                if (D_8009F24C == 0) {
                    func_8001C680();
                    D_8009F24C = 0x8CA;
                }
            }
            break;
        default:
            func_80010000.result = -0x18;
            break;
        }
    } else {
        s->result = -0x1D;
        D_8009F228 = one;
        do {
            func_8008E1BC(1, 0, (void *)D_800A0798);
            if (!(D_800A0798[0] & 0x10)) break;
            func_800914B0(2, 0);
            func_8008DEA0();
            loc = D_8009F61C;
            __asm__ volatile ("" : : "r" (loc));
            func_8001C470(loc);
            s->unk18 = func_8008E1A8(D_8001C808);
            D_8009F618 = 0;
        } while (D_800A0798[0] & 0x10);
        func_8001C718();
        D_8009F228 = 0;
    }
    func_8008E0D8(saved);
}

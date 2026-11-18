/* Exercises the unmodified func_800879DC with deterministic stream, pad and clock
 * stubs. Truncated reservations deliberately expose zero-padded backing storage:
 * this safely records the C control flow, not undefined reads from a real buffer. */
typedef unsigned int u32;
typedef unsigned char u8;
extern int func_800879DC(u8 *, u8 *, int);
u32 D_8009F4DC, D_8009F958, D_8009F95C;
u8 D_8009F4E0;
void __main(void) {}

static long sys3(long n, long a, long b, long c)
{
    register long v0 __asm__("$2") = n;
    register long a0 __asm__("$4") = a;
    register long a1 __asm__("$5") = b;
    register long a2 __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v0;
}
static int len(const char *s) { int n = 0; while (s[n]) n++; return n; }
static void say(const char *s) { sys3(4004, 1, (long)s, len(s)); }
static int same(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return *a == *b; }
static void num(int n)
{
    char s[12]; int i = 0;
    if (n == 0) s[i++] = '0';
    while (n > 0) { s[i++] = (char)('0' + n % 10); n /= 10; }
    while (i) { char c = s[--i]; sys3(4004, 1, (long)&c, 1); }
}
static char file[32768], trace[4096];
static int trace_len, overflow;
static union { u32 align; u8 bytes[32768]; } backing;
static int length, pos, first, loop, press_loop, press_port, clock_value, reserve_fault, reserve_calls;
static u32 press_bits;
static void event(char c, int value)
{
    char digits[12]; int n = 0;
    if (trace_len > 4000) { overflow = 1; return; }
    trace[trace_len++] = c;
    if (value >= 0) {
        if (!value) digits[n++] = '0';
        while (value > 0) { digits[n++] = (char)('0' + value % 10); value /= 10; }
        while (n) trace[trace_len++] = digits[--n];
    }
    trace[trace_len++] = ','; trace[trace_len] = 0;
}
u32 func_8001EF8C(int port)
{
    u32 v = (press_loop == -2 || loop == press_loop) && port == press_port ? press_bits : 0;
    if (port == 1) loop++;
    return v;
}
u32 *func_80089404(u32 *slot)
{
    int left;
    reserve_calls++;
    if (first) { first = 0; if (reserve_fault == 1) return 0; return (u32 *)backing.bytes; }
    if (reserve_fault == reserve_calls) return 0;
    if (reserve_fault == 4 && reserve_calls == 3) { *slot = 0; return (u32 *)backing.bytes; }
    left = length - pos;
    if (left < 0) left = 0;
    if (*slot > (u32)left) *slot = (u32)left;
    return (u32 *)(backing.bytes + pos);
}
void func_80089304(u32 n) { event('X', n); pos += n; }
void func_80089A40(u8 *p) { event('V', p - backing.bytes); }
void func_80086FE4(u8 *p) { event('A', p - backing.bytes); }
void func_80087430(u8 *p) { event('B', p - backing.bytes); }
void func_80086DB4(void) { event('S', -1); }
void func_80086F1C(void) { event('E', -1); }
u32 func_80086F58(u32 n) { event('T', n); return 1; }
void func_8008589C(u8 *p, u8 *a, u8 *b) { (void)a; (void)b; event('D', p - backing.bytes); }
u32 func_80086F44(void) { return clock_value; }
void func_80091270(int x) { (void)x; event('W', -1); clock_value++; }
void func_80085914(void) { event('P', -1); }
static int hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static u32 number(const char *s, int base)
{
    u32 v = 0; int d;
    while (*s) { d = hex(*s++); if (d < 0 || d >= base) return 0xffffffff; v = v * base + d; }
    return v;
}
static int decimal(const char *s) { if (*s == '-') return -(int)number(s + 1, 10); return (int)number(s, 10); }
static char *token(char **cursor)
{
    char *p = *cursor, *start;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    while (*p == '#') { while (*p && *p != '\n') p++; while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++; }
    if (!*p) { *cursor = p; return 0; }
    start = p;
    while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
    if (*p) *p++ = 0;
    *cursor = p; return start;
}
int main(int argc, char **argv)
{
    int fd, n, i, cases = 0, failures = 0, ret;
    char *p, *f[19];
    if (argc != 2) { say("demux: missing vector path\n"); return 2; }
    fd = (int)sys3(4005, (long)argv[1], 0, 0);
    n = (int)sys3(4003, fd, (long)file, sizeof(file) - 1);
    sys3(4006, fd, 0, 0);
    if (n <= 0 || n >= (int)sizeof(file) - 1) { say("demux: vector read error\n"); return 2; }
    file[n] = 0; p = file;
    while ((f[0] = token(&p)) != 0) {
        for (i = 1; i < 19; i++) if (!(f[i] = token(&p))) { say("demux: short row\n"); return 2; }
        for (i = 0; i < (int)sizeof(backing.bytes); i++) backing.bytes[i] = 0;
        length = 0;
        if (!same(f[7], "-")) {
            n = len(f[7]);
            if ((n & 1) || n > 16000) { say("demux: bad stream hex\n"); return 2; }
            for (i = 0; i < n; i += 2) {
                int a = hex(f[7][i]), b = hex(f[7][i + 1]);
                if (a < 0 || b < 0) { say("demux: bad hex\n"); return 2; }
                backing.bytes[length++] = (u8)((a << 4) | b);
            }
        }
        D_8009F4DC = number(f[1], 10); press_loop = decimal(f[2]); press_port = decimal(f[3]);
        press_bits = number(f[4], 16); D_8009F958 = number(f[5], 16); D_8009F95C = number(f[6], 16);
        D_8009F4E0 = 0; pos = loop = clock_value = trace_len = overflow = reserve_calls = 0; first = 1; trace[0] = 0;
        reserve_fault = decimal(f[18]);
        ret = func_800879DC(0, 0, 0);
        if (!trace_len) { trace[0] = '-'; trace[1] = 0; }
        cases++;
        if (overflow || ret != decimal(f[8]) || D_8009F4E0 != decimal(f[9]) ||
            D_8009F958 != number(f[10], 16) || D_8009F95C != number(f[11], 16) || !same(trace, f[12])) {
            failures++; say(f[0]); say(": ret="); num(ret); say(" abort="); num(D_8009F4E0);
            say(" trace="); say(trace); say(" expected="); say(f[12]); say("\n");
        }
    }
    if (!cases) { say("demux: no cases\n"); return 2; }
    if (failures) { say("FAILURES "); num(failures); say(" of "); num(cases); say("\n"); return 1; }
    say("ALL PASS (WVE demux C vectors: "); num(cases); say(")\n"); return 0;
}

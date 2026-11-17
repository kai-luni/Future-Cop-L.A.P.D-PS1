/* Executes original rotation C functions against the shared .NET fixtures. */
typedef unsigned int u32;
typedef int s32;
extern void func_80018DC0(s32, s32, s32, s32 *);
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
static int hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static void say(const char *s)
{
    int n = 0; while (s[n]) n++; sys3(4004, 1, (long)s, n);
}
int main(int argc, char **argv)
{
    static char data[32768];
    int fd, n, i, j, count, failures, h;
    char *p;
    u32 v[12];
    s32 actual[9];
    if (argc != 2) { say("rotation: missing fixture\n"); return 2; }
    fd = (int)sys3(4005, (long)argv[1], 0, 0);
    if (fd < 0) { say("rotation: cannot open fixture\n"); return 2; }
    n = (int)sys3(4003, fd, (long)data, sizeof(data)-1);
    sys3(4006, fd, 0, 0);
    if (n < 0) { say("rotation: read error\n"); return 2; }
    data[n] = 0; p = data; count = 0; failures = 0;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;
        if (*p == '#') { while (*p && *p != '\n') p++; continue; }
        for (i = 0; i < 12; i++) {
            while (*p == ' ' || *p == '\t') p++;
            v[i] = 0;
            for (j = 0; j < 8; j++) {
                h = hex(*p++);
                if (h < 0) { say("rotation: malformed fixture\n"); return 2; }
                v[i] = (v[i] << 4) | (u32)h;
            }
        }
        while (*p == ' ' || *p == '\t' || *p == '\r') p++;
        if (*p && *p != '\n') { say("rotation: extra fixture data\n"); return 2; }
        func_80018DC0((s32)v[0], (s32)v[1], (s32)v[2], actual);
        for (i = 0; i < 9; i++) if ((u32)actual[i] != v[i+3]) { failures++; break; }
        count++;
    }
    if (!count || failures) { say("rotation: FAIL\n"); return 1; }
    say("rotation: all shared vectors PASS\n");
    return 0;
}

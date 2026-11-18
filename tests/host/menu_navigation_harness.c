/* Run the unchanged func_800787D8 against the shared menu vectors. */
typedef signed char s8;
typedef unsigned char u8;
typedef int s32;
struct Entry { u8 pad[14]; u8 enabled; s8 group; u8 rest[12]; };
extern void func_800787D8(s8, s8, s8);
u8 *gFontBankTable __asm__("D_8009F468");
void __main(void) {}
static long sys3(long n, long a, long b, long c) {
    register long v __asm__("$2") = n; register long x __asm__("$4") = a;
    register long y __asm__("$5") = b; register long z __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v) : "r"(x), "r"(y), "r"(z) : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25"); return v;
}
static void say(const char *s) { int n=0; while(s[n]) n++; sys3(4004,1,(long)s,n); }
static char data[8192], *p, name[64];
static void space(void) { while (1) { while (*p==' '||*p=='\n'||*p=='\r'||*p=='\t') p++; if (*p!='#') return; while (*p && *p!='\n') p++; } }
static int token(void) { int i=0; space(); if (!*p) return 0; while(*p && *p!=' ' && *p!='\n' && *p!='\r' && i<63) name[i++]=*p++; name[i]=0; return 1; }
static int number(int *v) { int n=0, sign=1; space(); if (*p=='-') {sign=-1;p++;} if (*p<'0'||*p>'9') return 0; while (*p>='0'&&*p<='9') n=n*10+*p++-'0'; *v=n*sign; return 1; }
int main(int argc, char **argv) {
    int fd, n, cases=0, failed=0, count, mask, groups, start, direction, target, previous, expected, i;
    u8 descriptor[48]; struct Entry entries[32];
    if(argc!=2) return 2; fd=sys3(4005,(long)argv[1],0,0); if(fd<0) return 2;
    n=sys3(4003,fd,(long)data,sizeof(data)-1); sys3(4006,fd,0,0); if(n<0) return 2; data[n]=0; p=data;
    while(token()) {
        if(!number(&count)||!number(&mask)||!number(&groups)||!number(&start)||!number(&direction)||!number(&target)||!number(&previous)||!number(&expected)) return 2;
        if(count<1 || count>32 || target<0 || target>1) return 2;
        for(i=0;i<48;i++) descriptor[i]=0; descriptor[2]=(u8)count; descriptor[target]=(u8)previous;
        for(i=0;i<count;i++) { entries[i].enabled=(mask>>i)&1; entries[i].group=(groups>>i)&1; }
        *(struct Entry **)(descriptor+0x28)=entries; gFontBankTable=descriptor;
        func_800787D8((s8)start,(s8)direction,(s8)target); cases++;
        if(descriptor[target]!=(u8)expected) { say("FAIL "); say(name); say("\n"); failed++; }
    }
    if(!cases || failed) return 1; say("menu-navigation: ALL PASS\n"); return 0;
}

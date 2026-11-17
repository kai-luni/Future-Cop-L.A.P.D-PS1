/* Original func_80013CB8, with software models of its GTE primitives. */
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned char u8;
struct Input { s16 pad0[3]; u8 pad6, flags; s16 x,y,z,rx,ry,rz; };
struct Matrix { s32 m[12]; };
struct Matrix D_800A0300[8];
static s16 r[9];
extern void func_80013CB8(s32,struct Input *,s16 *,s32);
void __main(void) {}
static s32 clamp(s32 x) { if(x > 32767) return 32767; if(x < -32768) return -32768; return x; }
void func_80010820(struct Matrix *m)
{
    int i,j;
    for (i=0;i<3;i++) for(j=0;j<3;j++) {
        s32 v=m->m[j*3+i];
        r[i*3+j]=(s16)((i==1 ? -v : v)>>4);
    }
}
void func_8001006C(struct Matrix *src, struct Matrix *dst, s32 count)
{
    int i,j;
    for(i=0;i<count;i++) {
        s32 x=(s16)(src->m[i*3]>>4),y=(s16)(src->m[i*3+1]>>4),z=(s16)(src->m[i*3+2]>>4);
        for(j=0;j<3;j++) {
            s32 value=clamp((r[j*3]*x+r[j*3+1]*y+r[j*3+2]*z)>>12);
            dst->m[i*3+j]=(j==1 ? -value : value)*16;
        }
    }
}
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
    int fd,n,i,j,k,count,failures,h; char *p; u32 v[25];
    s16 table[16]; struct Input e;
    if (argc != 2) { say("compose: missing fixture\n"); return 2; }
    fd=(int)sys3(4005,(long)argv[1],0,0);
    if(fd<0) { say("compose: cannot open fixture\n"); return 2; }
    n=(int)sys3(4003,fd,(long)data,sizeof(data)-1); sys3(4006,fd,0,0);
    if(n<0) { say("compose: read error\n"); return 2; }
    data[n]=0; p=data; count=0; failures=0;
    for(i=0;i<16;i++) table[i]=(s16)(i*101-600);
    while(*p) {
        while(*p==' '||*p=='\t'||*p=='\r'||*p=='\n') p++;
        if(!*p) break;
        if(*p=='#') { while(*p && *p!='\n') p++; continue; }
        for(i=0;i<25;i++) {
            while(*p==' '||*p=='\t') p++;
            v[i]=0;
            for(j=0;j<8;j++) { h=hex(*p++); if(h<0) {say("compose: malformed fixture\n");return 2;} v[i]=(v[i]<<4)|(u32)h; }
        }
        for(i=0;i<12;i++) D_800A0300[0].m[i]=(s32)v[i];
        e.pad0[0]=e.pad0[1]=e.pad0[2]=0; e.pad6=0; e.flags=(u8)v[12];
        if(e.flags) {e.x=55;e.y=-93;e.z=47;e.rx=291;e.ry=544;e.rz=931;}
        else {e.x=0;e.y=1;e.z=2;e.rx=3;e.ry=4;e.rz=5;}
        func_80013CB8(1,&e,table,0);
        for(k=0;k<12;k++) if((u32)D_800A0300[1].m[k]!=v[k+13]) {failures++; break;}
        count++;
    }
    if(!count || failures) {say("compose: FAIL\n");return 1;}
    say("compose: all shared vectors PASS (GTE software model)\n");return 0;
}

/* Runs original func_80052960 with software GTE primitives, driven by
 * tests/golden/joints.vectors (fcop-joints-golden/v2). Every row carries the whole
 * input - offset, root matrix and the record chain - plus the expected fifth and
 * third joint matrices, so the .NET suite reads exactly the same fixture. */
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned char u8;
struct Matrix { s32 m[12]; };
struct Rec { u8 index,pad[6],flags; s16 x,y,z,rx,ry,rz; };
struct Sub { u8 pad[0x14]; struct Rec *entries; s16 *table; };
struct Args { struct Sub *sub; struct Matrix matrix; u8 pad[0x14]; s16 offset; };
extern void func_80052960(struct Args *,struct Matrix *,struct Matrix *);
static s16 r[9];
void __main(void) {}
void func_80010820(struct Matrix *m) { int i,j; for(i=0;i<3;i++) for(j=0;j<3;j++) { s32 v=m->m[j*3+i]; r[i*3+j]=(s16)((i==1?-v:v)>>4); } }
void func_8001006C(struct Matrix *src,struct Matrix *dst,s32 n) { int i,j; for(i=0;i<n;i++) { s32 x=(s16)(src->m[i*3]>>4),y=(s16)(src->m[i*3+1]>>4),z=(s16)(src->m[i*3+2]>>4); for(j=0;j<3;j++) { long long mac=(long long)r[j*3]*x+(long long)r[j*3+1]*y+(long long)r[j*3+2]*z; s32 v=(s32)(mac>>12); if(v>32767)v=32767; if(v< -32768)v=-32768; dst->m[i*3+j]=(j==1?-v:v)*16; } } }
static long sys3(long n,long a,long b,long c) { register long v0 __asm__("$2")=n; register long a0 __asm__("$4")=a; register long a1 __asm__("$5")=b; register long a2 __asm__("$6")=c; __asm__ volatile("syscall" : "+r"(v0) : "r"(a0),"r"(a1),"r"(a2) : "memory","$7","$8","$9","$10","$11","$12","$13","$14","$15","$24","$25"); return v0; }
static int hexval(char c) { if(c>='0'&&c<='9') return c-'0'; if(c>='a'&&c<='f') return c-'a'+10; if(c>='A'&&c<='F') return c-'A'+10; return -1; }
static void say(const char *s) { int n=0; while(s[n]) n++; sys3(4004,1,(long)s,n); }
static void say_dec(int v)
{
    char b[12]; int i=11; b[i]=0;
    if(v<=0) { say("0"); return; }
    while(v>0 && i>0) { b[--i]=(char)('0'+v%10); v/=10; }
    say(&b[i]);
}
static int hexword(char **pp, u32 *out)
{
    u32 w=0; int j,h;
    while(**pp==' '||**pp=='\t'||**pp=='\r') (*pp)++;
    for(j=0;j<8;j++) { h=hexval(**pp); if(h<0) return -1; w=(w<<4)|(u32)h; (*pp)++; }
    *out=w;
    return 0;
}
int main(int argc, char **argv)
{
    static char data[65536];
    static s16 table[64];
    static struct Rec rec[8];
    struct Sub sub; struct Args a; struct Matrix first,second;
    int fd,n,i,k,count,failures,tn,nw,nrec; char *p; u32 v[128];
    if (argc != 2) { say("joints: missing fixture\n"); return 2; }
    fd=(int)sys3(4005,(long)argv[1],0,0);
    if(fd<0) { say("joints: cannot open fixture\n"); return 2; }
    n=(int)sys3(4003,fd,(long)data,sizeof(data)-1); sys3(4006,fd,0,0);
    if(n<0) { say("joints: read error\n"); return 2; }
    data[n]=0; p=data; count=0; failures=0; tn=0;
    while(*p) {
        while(*p==' '||*p=='\t'||*p=='\r'||*p=='\n') p++;
        if(!*p) break;
        if(*p=='#') { while(*p && *p!='\n') p++; continue; }
        if(*p=='T'&&p[1]=='A'&&p[2]=='B'&&p[3]=='L'&&p[4]=='E') { /* TABLE <hex words> */
            p+=5;
            while(*p && *p!='\n') {
                u32 w;
                while(*p==' '||*p=='\t'||*p=='\r') p++;
                if(*p=='\n'||!*p) break;
                if(hexword(&p,&w)) { say("joints: malformed TABLE\n"); return 2; }
                if(tn<64) table[tn++]=(s16)w;
            }
            if(!tn) { say("joints: empty TABLE\n"); return 2; }
            continue;
        }
        nw=0;
        for(;;) {
            while(*p==' '||*p=='\t'||*p=='\r') p++;
            if(!*p || *p=='\n') break;
            if(nw>=128) { say("joints: row too long\n"); return 2; }
            if(hexword(&p,&v[nw])) { say("joints: malformed fixture\n"); return 2; }
            nw++;
        }
        if(!nw) continue;
        if(!tn) { say("joints: TABLE directive missing\n"); return 2; }
        if(nw<15) { say("joints: short row\n"); return 2; }
        nrec=(int)v[14];
        if(nrec<1 || nrec>5) { say("joints: bad record count\n"); return 2; }
        if(nw != 39+8*nrec) { say("joints: row length\n"); return 2; }
        if(nrec<5 && !((u8)v[15+8*(nrec-1)+1] & 0x80)) { say("joints: no terminating 0x80 before the record array ends\n"); return 2; }
        for(i=0;i<12;i++) a.matrix.m[i]=(s32)v[2+i];
        for(i=0;i<nrec;i++) {
            u32 *w=&v[15+8*i];
            rec[i].index=(u8)w[0];
            for(k=0;k<6;k++) rec[i].pad[k]=0;
            rec[i].flags=(u8)w[1];
            rec[i].x=(s16)w[2]; rec[i].y=(s16)w[3]; rec[i].z=(s16)w[4];
            rec[i].rx=(s16)w[5]; rec[i].ry=(s16)w[6]; rec[i].rz=(s16)w[7];
        }
        sub.entries=rec; sub.table=table; a.sub=&sub; a.offset=(s16)v[1];
        for(i=0;i<12;i++) { first.m[i]=0x12345678; second.m[i]=0x12345678; }
        func_80052960(&a,&first,&second);
        for(i=0;i<12;i++) if((u32)first.m[i]  != v[15+8*nrec+i])      { failures++; break; }
        for(i=0;i<12;i++) if((u32)second.m[i] != v[15+8*nrec+12+i])   { failures++; break; }
        count++;
    }
    if(!count || failures) { say("joints: FAIL\n"); return 1; }
    say("joints: all shared vectors PASS ("); say_dec(count);
    say(" cases, GTE software model)\n");
    return 0;
}

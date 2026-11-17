/* Runs original func_80052960 with software GTE primitives. */
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
void func_8001006C(struct Matrix *src,struct Matrix *dst,s32 n) { int i,j; for(i=0;i<n;i++) { s32 x=(s16)(src->m[i*3]>>4),y=(s16)(src->m[i*3+1]>>4),z=(s16)(src->m[i*3+2]>>4); for(j=0;j<3;j++) { s32 v=(r[j*3]*x+r[j*3+1]*y+r[j*3+2]*z)>>12; if(v>32767)v=32767; if(v< -32768)v=-32768; dst->m[i*3+j]=(j==1?-v:v)*16; } } }
static long sys3(long n,long a,long b,long c) { register long v0 __asm__("$2")=n; register long a0 __asm__("$4")=a; register long a1 __asm__("$5")=b; register long a2 __asm__("$6")=c; __asm__ volatile("syscall" : "+r"(v0) : "r"(a0),"r"(a1),"r"(a2) : "memory","$7","$8","$9","$10","$11","$12","$13","$14","$15","$24","$25"); return v0; }
static void hex(u32 v) { char buf[9]; int j; const char *h="0123456789abcdef"; for(j=0;j<8;j++)buf[j]=h[(v>>(28-j*4))&15];buf[8]=' ';sys3(4004,1,(long)buf,9); }
int main(void) { struct Rec rec[5]; struct Sub sub; struct Args a; struct Matrix first,second; s16 table[32]; int c,i,j;
 for(c=0;c<6;c++) { for(i=0;i<32;i++)table[i]=(s16)(i*41-500); for(i=0;i<5;i++) { rec[i].index=(i%3)+1; for(j=0;j<6;j++)rec[i].pad[j]=0; rec[i].flags=(u8)((c%2)?0x3f:0); if(i==c-1 && c>=1 && c<=5)rec[i].flags|=0x80; if(c%2) {rec[i].x=10+i*11;rec[i].y=-20+i*7;rec[i].z=30-i*3;rec[i].rx=33+i*91;rec[i].ry=87-i*21;rec[i].rz=-70+i*13;} else {rec[i].x=i;rec[i].y=i+1;rec[i].z=i+2;rec[i].rx=i+3;rec[i].ry=i+4;rec[i].rz=i+5;} }
 sub.entries=rec;sub.table=table;a.sub=&sub;a.offset=3; for(i=0;i<12;i++) {a.matrix.m[i]=(i==0||i==4||i==8)?65536:(i>=9?(i-8)*160:0);first.m[i]=second.m[i]=0x12345678;} func_80052960(&a,&first,&second);hex(c);for(i=0;i<12;i++)hex(first.m[i]);for(i=0;i<12;i++)hex(second.m[i]);sys3(4004,1,(long)"\n",1); } return 0; }

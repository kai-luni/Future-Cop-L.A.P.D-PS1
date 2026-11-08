typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef struct { s32 m[9]; s32 x,y,z; } Matrix;
typedef struct { char pad[0x24]; s32 x,y,z; } Base;
extern void func_80010820(Base *);
extern void func_8001006C(Matrix *, Matrix *, s32);
u32 func_800619C0(Base *base, s16 *box, char *objects, s32 count, s32 stride)
{
    Matrix a, b;
    register s32 x __asm__("$9"), y __asm__("$7"), z __asm__("$8");
    register s32 xmin __asm__("$13"), xmax __asm__("$9"), ymin __asm__("$12"), ymax __asm__("$7"), zmin __asm__("$11"), zmax __asm__("$8");
    s32 i;
    register s32 n __asm__("$10");
    register u32 result __asm__("$6");
    s16 *p;
    register s32 t0 __asm__("$2"), t1 __asm__("$3"), t2 __asm__("$5"), t3 __asm__("$4");
    a.x=box[0]<<4; a.y=box[1]<<4; a.z=box[2]<<4;
    a.m[0]=box[3]<<4; a.m[1]=0; a.m[2]=0;
    a.m[3]=0; a.m[4]=box[4]<<4; a.m[5]=0;
    a.m[6]=0; a.m[7]=0; a.m[8]=box[5]<<4;
    func_80010820(base);
    func_8001006C(&a,&b,4);
    z=0; y=z; x=b.m[0];
    __asm__ volatile ("" : : "r" (z), "r" (y), "r" (x) : "memory");
    t0=b.y; t1=base->y; t2=base->x;
    __asm__ volatile ("" : "+r" (t0), "+r" (t1), "+r" (t2));
    b.y=t0+t1; __asm__ volatile ("" : : : "memory");
    t0=b.x; t1=b.z; t3=base->z;
    __asm__ volatile ("" : "+r" (t0), "+r" (t1), "+r" (t3));
    b.x=t0+t2; b.z=t1+t3;
    if(x<0) x=-x;
    if(b.m[3]<0) x-=b.m[3]; else x+=b.m[3];
    if(b.m[6]<0) x-=b.m[6]; else x+=b.m[6];
    xmin=b.x-x; xmax=x+b.x;
    __asm__ volatile ("" : : "r" (xmin), "r" (xmax) : "memory");
    if(b.m[1]<0) y-=b.m[1]; else y+=b.m[1];
    if(b.m[4]<0) y-=b.m[4]; else y+=b.m[4];
    if(b.m[7]<0) y-=b.m[7]; else y+=b.m[7];
    ymin=b.y-y; ymax=y+b.y;
    __asm__ volatile ("" : : "r" (ymin), "r" (ymax) : "memory");
    if(b.m[2]<0) z-=b.m[2]; else z+=b.m[2];
    if(b.m[5]<0) z-=b.m[5]; else z+=b.m[5];
    if(b.m[8]<0) z-=b.m[8]; else z+=b.m[8];
    t0=b.z;
    __asm__ volatile ("" : "+r" (t0));
    xmax>>=4; xmin>>=4; ymax>>=4; ymin>>=4;
    result=0; n=count;
    zmin=t0-z; zmax=z+t0; zmax>>=4; zmin>>=4;
    if(count>=2) { n=count-1; objects+=stride; }
    while(n>0) {
        p=(s16 *)objects;
        result <<= 1;
        if(p[0]-p[3]<xmax && xmin<p[0]+p[3] &&
           p[1]-p[4]<ymax && ymin<p[1]+p[4] &&
           p[2]-p[5]<zmax && zmin<p[2]+p[5]) result |= 1;
        n--; objects+=stride;
    }
    return result << (32-count);
}

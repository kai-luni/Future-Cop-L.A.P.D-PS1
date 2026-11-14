#include "game/types.h"

typedef struct { s32 x, y, z; } Vec3;
extern s32 *D_8009F80C;
extern s32 func_80067914(s32 *, s32 *, s32, s32, s32);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")


#define LOADPAIR(a,b,p,q) { \
    register s32 va __asm__("$2") = (p); \
    register s32 vb __asm__("$3") = (q); \
    __asm__("" : "=r"(va), "=r"(vb) : "0"(va), "1"(vb)); \
    (a)=va<<5; (b)=vb<<5; \
}
#define ABS(a) ((((a) >> 31) + (a)) ^ ((a) >> 31))
#define SWAP(a,b) { (a)^=(b); (b)^=(a); (a)^=(b); }
/* The denominator retains the original signed rounding before halving. */
#define RATIO(out,a,b) { \
    s32 divisor; \
    s32 ratio_product, ratio_numerator; \
    if ((a)>0) { \
        divisor=(b); \
        if(divisor>0) { divisor >>= 1; MULU16(out,a,0x80000000U/(u32)divisor); } \
        else { divisor=-divisor; divisor >>= 1; MULU16(ratio_product,a,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
    } else { \
        divisor=(b); ratio_numerator=-(a); \
        if(divisor>0) { divisor >>= 1; MULU16(ratio_product,ratio_numerator,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
        else { divisor=-divisor; divisor >>= 1; MULU16(out,ratio_numerator,0x80000000U/(u32)divisor); } \
    } \
}
#define SINGLE_RATIO(out,a,b) { \
    register s32 divisor __asm__("$3"); \
    register s32 numerator __asm__("$4") = (a); \
    s32 ratio_product; \
    if (numerator>0) { \
        if((b)>0) { divisor=(b)>>1; MULU16(out,numerator,0x80000000U/(u32)divisor); } \
        else { divisor=-(b); divisor >>= 1; MULU16(ratio_product,numerator,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
    } else { \
        numerator=-(a); \
        if((b)>0) { divisor=(b)>>1; MULU16(ratio_product,numerator,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
        else { divisor=-(b); divisor >>= 1; MULU16(out,numerator,0x80000000U/(u32)divisor); } \
    } \
}
#define STEP(out,a,b) { \
    if ((b)>=128) { RATIO(out,a,(b)>>5); } \
    else { (out)=(a)<<10; } \
}
#define NEGSTEP(out,a,b) { \
    s32 divisor, ratio_product; \
    register s32 ratio_numerator __asm__("$4"); \
    if (-(b)>=128) { \
        if ((a)>0) { \
            divisor=(b)>>5; \
            if(divisor>0) { divisor >>= 1; MULU16(ratio_product,a,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
            else { divisor=-divisor; divisor >>= 1; MULU16(out,a,0x80000000U/(u32)divisor); } \
        } else { \
            divisor=(b)>>5; ratio_numerator=-(a); \
            if(divisor>0) { divisor >>= 1; MULU16(out,ratio_numerator,0x80000000U/(u32)divisor); } \
            else { divisor=-divisor; divisor >>= 1; MULU16(ratio_product,ratio_numerator,0x80000000U/(u32)divisor); (out)=-ratio_product; } \
        } \
    } else { register s32 neg __asm__("$2") = -(a); __asm__ volatile("" : "+r"(neg)); (out)=neg<<10; } \
}
#define TRANSFORM(p) { \
    if(mode&2) { (p)[0]=-(p)[0]; (p)[1]=-(p)[1]; } \
    if(mode&1) { SWAP((p)[0],(p)[1]); } \
}

s32 func_800684AC(s32 *p, s32 *q, s32 arg2, s32 arg3, s32 flag)
{
    s32 prev[4], next[4];
    s32 y1, cellstep;
    s32 mode;
    s32 z0;
    register s32 z1 __asm__("$20");
    s32 x0;
    register s32 x1 __asm__("$23");
    s32 dz;
    s32 dx;
    s32 y0;
    s32 dy;
    s32 edgez;
    s32 edgex;
    register s32 crossz __asm__("$16");
    register s32 crossy __asm__("$21");
    register s32 stepz __asm__("$19");
    register s32 stepy __asm__("$18");
    s32 crossx, crosszy, stepx, stepzy;
    s32 lim, cells, prod, ratio, result;
    register s32 *map __asm__("$8");
    mode=0;
    {
        register s32 va __asm__("$2") = p[0];
        register s32 vb __asm__("$3") = q[0];
        map=D_8009F80C;
        __asm__("" : "=r"(va), "=r"(vb), "=r"(map) : "0"(va), "1"(vb), "2"(map));
        x0=va<<5; x1=vb<<5;
    }
    LOADPAIR(z0,z1,p[2],q[2]);
    {
        register s32 dimension __asm__("$2") = map[1];
        register s32 yq __asm__("$3") = q[1];
        __asm__("" : "=r"(dimension), "=r"(yq) : "0"(dimension), "1"(yq));
        {
            register s32 limreg __asm__("$6") = (dimension-1)<<22;
            __asm__("" : "+r"(limreg));
            lim=limreg;
        }
        y1=yq<<5;
        y0=p[1]<<5;
    }
    if(z0<0 || z0>=lim || z1<0 || z1>=lim) return 0;
    lim=(map[2]-1)<<22;
    if(x0<0 || x0>=lim || x1<0 || x1>=lim) return 0;
    dz=z1-z0;
    dx=x1-x0;
    dy=y1-y0;
    if(ABS(dz)<ABS(dx)) {
        mode=1;
        SWAP(z0,x0); SWAP(z1,x1); SWAP(dz,dx);
    }
    if(dz<0) {
        mode|=2;
        z0=-z0; z1=-z1; x0=-x0; x1=-x1; dz=-dz; dx=-dx;
    }
    if((z1>>18)==(z0>>18)) edgez=0x40000000;
    else {
        edgez=((z0>>18)+1)<<18;
        STEP(stepx,dx,dz);
        MUL16(prod,edgez-z0,stepx);
        crossx=x0+(prod>>5);
        STEP(stepzy,dy,dz);
        MUL16(prod,edgez-z0,stepzy);
        crosszy=y0+(prod>>5);
    }
    cells=(x1>>18)-(x0>>18);
    if(cells>0) {
        edgex=((x0>>18)+1)<<18;
        if(cells>=2) {
            register s32 prod __asm__("$2");
            STEP(stepz,dz,dx);
            MUL16(prod,edgex-x0,stepz);
            crossz=z0+(prod>>5);
            STEP(stepy,dy,dx);
            MUL16(prod,edgex-x0,stepy);
            crossy=y0+(prod>>5);
        } else {
            stepz=0x40000000;
            RATIO(ratio,edgex-x0,dx);
            MUL16(prod,dz,ratio);
            crossz=z0+prod;
            SINGLE_RATIO(ratio,edgex-x0,dx);
            MUL16(prod,dy,ratio);
            crossy=y0+prod;
        }
        cellstep=0x40000;
    } else if(cells<0) {
        edgex=(x0>>18)<<18;
        if(cells<-1) {
            NEGSTEP(stepz,dz,dx);
            {
                register s32 scaled __asm__("$2");
                MUL16(scaled,edgex-x0,stepz);
                scaled >>= 5;
                __asm__ volatile("" : "+r"(scaled));
                crossz=z0-scaled;
            }
            __asm__ volatile("" : "+r"(crossz));
            NEGSTEP(stepy,dy,dx);
            {
                register s32 scaled __asm__("$2");
                MUL16(scaled,edgex-x0,stepy);
                scaled >>= 5;
                __asm__ volatile("" : "+r"(scaled));
                crossy=y0-scaled;
            }
        } else {
            stepz=0x40000000;
            RATIO(ratio,edgex-x0,dx);
            MUL16(prod,dz,ratio);
            crossz=z0+prod;
            SINGLE_RATIO(ratio,edgex-x0,dx);
            MUL16(prod,dy,ratio);
            crossy=y0+prod;
        }
        cellstep=-0x40000;
    } else crossz=0x40000000;
    next[0]=(z0+16)>>5;
    next[1]=(x0+16)>>5;
    next[2]=y0>>5;
    TRANSFORM(next);
    stepz>>=3; stepx>>=3; stepy>>=3; stepzy>>=3;
    while(edgez<z1 || crossz<z1) {
        *(Vec3 *)prev=*(Vec3 *)next;
        if(crossz>=edgez) {
            next[0]=(edgez+16)>>5; edgez+=0x40000;
            next[1]=(crossx+16)>>5; crossx+=stepx;
            next[2]=(crosszy+16)>>5; crosszy+=stepzy;
        } else {
            register s32 ex __asm__("$2") = edgex+16;
            register s32 cz __asm__("$3") = crossz+16;
            __asm__("" : "=r"(ex), "=r"(cz) : "0"(ex), "1"(cz));
            next[0]=cz>>5; crossz+=stepz;
            next[1]=ex>>5;
            next[2]=(crossy+16)>>5; crossy+=stepy;
            edgex+=cellstep;
        }
        TRANSFORM(next);
        result=func_80067914(prev,next,arg2,arg3,flag);
        if(result) return result;
    }
    *(Vec3 *)prev=*(Vec3 *)next;
    {
        register s32 tmp __asm__("$2") = z1+16;
        __asm__("" : "+r"(tmp));
        next[0]=tmp>>5;
    }
    {
        register s32 tmp __asm__("$2") = x1+16;
        __asm__("" : "+r"(tmp));
        next[1]=tmp>>5;
    }
    next[2]=y1>>5;
    TRANSFORM(next);
    return func_80067914(prev,next,arg2,arg3,flag);
}

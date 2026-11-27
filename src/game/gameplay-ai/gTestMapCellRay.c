// Categories: Physics > Collision | Resources > World Geometry
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef struct {int x,y,z;} Vec;
extern int *gMapGridHeader __asm__("D_8009F80C");
extern u8 **gTileBlockTable __asm__("D_8009F808");
extern Vec gCellRayStart __asm__("D_800A2600");
extern Vec gCellRayEnd __asm__("D_800A2610");
extern Vec gCellRayWallHit __asm__("D_800A2620");
extern Vec gWallHitPositions[] __asm__("D_800A32A0");
extern u32 gWallHitPolys[] __asm__("D_800A3260");
extern int gWallHitKinds[] __asm__("D_800A3280");
extern int gWallHitTopHeights[] __asm__("D_800A3300");
extern u8 gTileShapeTable[] __asm__("D_8009B1A4");
extern u8 gTileCornerIndices[][4] __asm__("D_8009AFE8");
int gTestMapCellRay(Vec *a, Vec *b, int *outA, int *outB, int flag) __asm__("func_80067914");


/**
 *  @brief Tests a segment against polygons in its midpoint's world cell.
 *
 *  Reads the cell's packed polygon and height data, tests eligible surfaces,
 *  and records wall candidates for selection and material reporting.
 *  @param a Segment start in world coordinates.
 *  @param b Segment end in world coordinates.
 *  @param outA Optional output in y, z, x order.
 *  @param outB Optional output for the selected height or endpoint z.
 *  @param flag Enables tests of polygon types below 71 when nonzero.
 *  @return Zero on no result or a degenerate diagonal; otherwise a kind and material code.
 *  @par Globals
 *  D_8009F80C and D_8009F808 locate the cell; D_8009AFE8 and D_8009B1A4 describe polygon geometry.
 *  D_800A2600, D_800A2610 and D_800A2620 hold local points; D_800A32A0, D_800A3260, D_800A3280 and D_800A3300 hold candidates.
 *  @par Control flow
 *  Iterates cell polygons, returns immediately for an enabled plane hit, and scans multiple wall candidates using an approximate horizontal distance.
 *  @par Calls
 *  No function calls.
 *  @par Side effects
 *  Overwrites global scratch points and candidate arrays and writes optional outputs on a hit.
 *  @par Callers
 *  Confirmed: func_800684AC.
 */
int gTestMapCellRay(Vec *a, Vec *b, int *outA, int *outB, int flag)
{
    int ox,oy;
    const u8 *tile;
    const u32 *polys; u32 poly;
    register const s8 *verts __asm__("$18");
    register int n __asm__("$25"); register int index __asm__("$16"); int type; register int kind __asm__("$14"); int shape;
    register int h0 __asm__("$13"); register int h1 __asm__("$10"); register int h2 __asm__("$9"); register int h3 __asm__("$11"); int off;
    int lo,hi,z0,z1;
    int dx1,dy1; register int dx2 __asm__("$22"); register int dy2 __asm__("$23");
    int nx,ny,nz,t0,t1,t2,p0,p1,p2;
    int v,w,q,den,delta;
    int low, high; register int dist __asm__("$15"); register int bestDist __asm__("$17");
    register Vec *hit __asm__("$12"); register Vec *end __asm__("$12");
    const u8 *indices;
    register int sx __asm__("$2"); register int sy __asm__("$4"); int cell; register int x0 __asm__("$6"); register int *world __asm__("$7");
    {
        register int bx __asm__("$2");
        register int ay __asm__("$4");
        register int by __asm__("$3");
        x0=a->x; bx=b->x; ay=a->y; by=b->y;
        __asm__ __volatile__("" : "+r"(x0), "+r"(bx), "+r"(ay), "+r"(by) : : "memory");
        world=gMapGridHeader;
        __asm__ __volatile__("" : "+r"(world));
        sx=x0+bx; sy=ay+by;
    }
    __asm__ __volatile__("" : "+r"(x0), "+r"(sx), "+r"(sy), "+r"(world));
    sy=sy/2; sx=sx/2;
    oy=sy>>13; ox=sx>>13;
    v=world[9+(sx>>17)+(sy>>17)*world[1]];
    lo=oy&15;
    cell=(lo<<4)+(ox&15);
    lo+=cell;
    ox<<=13; oy<<=13;
    v=(v>>2)&0x3fff;
    __asm__("" : "+r"(v));
    tile=gTileBlockTable[v];
    v=*(u16 *)(tile+0x3cc+cell*2);
    gCellRayStart.x=x0-ox; gCellRayStart.y=a->y-oy;
    polys=(const u32 *)(tile+0x5d0+(v>>4));
    gCellRayStart.z=a->z;
    gCellRayEnd.x=b->x-ox; gCellRayEnd.y=b->y-oy;
    index=-1;
    gCellRayEnd.z=b->z;
    n=v&15;
    verts=(const s8 *)(tile+12+3*lo);
    if(n<=0) goto after_polys;
    __asm__ __volatile__("li %0,-12" : "=r"(off));
    do {
        poly=*polys++;
        type=(poly>>15)&127;
        if(type<71) {
            if(!flag) goto next;
            indices=gTileCornerIndices[type];
            h0=verts[indices[0]]; h1=verts[indices[1]]; h2=verts[indices[2]];
            hi=h0; lo=h0;
            if(h0<h1) hi=h1; else if(h1<h0) lo=h1;
            if(hi<h2) hi=h2; else if(h2<lo) lo=h2;
            z0=a->z>>8; z1=b->z>>8; end=b;
            if(z1<z0) {end=a; z0^=z1; z1^=z0; z0^=z1;}
            if(hi<z0 || z1<lo) goto next;
            h3=h0; kind=0; dx1=32;
            switch(gTileShapeTable[type]>>3) {
                case 0: dy2=0; dx2=0; dy1=32; break;
                case 1: h3=h1; kind=32; dx2=32; dy1=0; dy2=-32; break;
                case 2: dy2=0; dy1=32; dx2=32; break;
                case 3: case 4: dx2=0; dy2=32; dy1=32; break;
            }
            h1-=h0; h2-=h0;
            ny=dy2*h1-h2*dy1;
            nx=h2*dx2-dx1*h1;
            nz=dx1*dy1-dy2*dx2;
            t0=gCellRayStart.y*ny;
            t0+=(gCellRayStart.x-kind)*nx;
            t0+=(gCellRayStart.z-(h3<<8))*nz;
            p0=gCellRayEnd.y*ny;
            p0+=(gCellRayEnd.x-kind)*nx;
            p0+=(gCellRayEnd.z-(h3<<8))*nz;
            if((t0^p0)>0) goto next;
            if(outB) *outB=end->z;
            if(outA) {outA[0]=end->y;outA[1]=end->z;outA[2]=end->x;}
            return (tile[0x370+((poly>>13)&3)]<<8)|5;
        }
        shape=gTileShapeTable[type]; kind=shape&7; shape>>=3;
        switch(kind) {
            case 1: if(gCellRayStart.y>0 && gCellRayEnd.y>0) goto next; break;
            case 2: if(gCellRayStart.x>0 && gCellRayEnd.x>0) goto next; break;
            case 3:
                dist=gCellRayStart.x+gCellRayStart.y-8192;
                bestDist=gCellRayEnd.x+gCellRayEnd.y-8192;
                dist^=bestDist;
                if(bestDist && dist>=0) goto next;
                break;
            case 4:
                dist=gCellRayStart.x-gCellRayStart.y;
                bestDist=gCellRayEnd.x-gCellRayEnd.y;
                dist^=bestDist;
                if(bestDist && dist>=0) goto next;
                break;
        }
        indices=gTileCornerIndices[type];
        h0=verts[indices[0]];h1=verts[indices[1]];h2=verts[indices[2]];h3=verts[indices[3]];
        switch(kind) {
            case 1:
                if(shape==12) {h3=h2;h2=h1;h1=h0;} else if(shape==11) h3=h2;
                hit=&gCellRayEnd;
                if(gCellRayStart.y==0) hit=&gCellRayStart;
                goto load_hit_x;
            case 2:
                if(shape==10) {h3=h2;h2=h1;h1=h0;} else if(shape==9) h3=h2;
                if(gCellRayStart.x==0) hit=&gCellRayStart;
                else hit=&gCellRayEnd;
                dist=hit->y;
                break;
            case 3:
                if(shape==6) {h3=h2;h2=h1;h1=h0;} else if(shape==5) {h3=h1;h1=h2;h2=h3;}
                v=gCellRayStart.x; w=gCellRayEnd.x;
                if(v==w) {hit=&gCellRayStart;dist=v;break;}
                delta=w-v; q=gCellRayStart.y;
                t0=gCellRayEnd.y-q; den=t0+delta;
                if(!den) return 0;
                p0=v*t0; p1=(8192-q)*delta;
                p0=(p0+p1)/den;
                q=gCellRayStart.z;
                t0=(gCellRayEnd.z-q)*(p0-v)/delta;
                hit=&gCellRayWallHit; hit->y=8192-p0; gCellRayWallHit.x=p0;
                goto store_hit_z;
            case 4:
                if(shape==8) {h3=h2;h2=h1;h1=h0;} else if(shape==7) h3=h2;
                v=gCellRayStart.x;w=gCellRayEnd.x;
                if(v==w) {hit=&gCellRayStart;goto load_hit_x;}
                delta=w-v;q=gCellRayStart.y;
                t0=gCellRayEnd.y-q;den=t0-delta;
                if(!den) return 0;
                p0=v*t0;p1=q*delta;p0=(p0-p1)/den;
                q=gCellRayStart.z;
                t0=(gCellRayEnd.z-q)*(p0-v)/delta;
                hit=&gCellRayWallHit; hit->y=p0; gCellRayWallHit.x=p0;
store_hit_z:
                hit->z=q+t0;
load_hit_x:
                dist=hit->x;
                break;
        }
        low=(h0<<8)+(((h3-h0)*dist)>>5);
        high=(h1<<8)+(((h2-h1)*dist)>>5);
        if(high<low) {low^=high;high^=low;low^=high;}
        if(hit->z<high && low<hit->z) {
            index++;off+=12;
            *(Vec *)((u8 *)gWallHitPositions+off)=*hit;
            gWallHitPolys[index]=poly;gWallHitKinds[index]=kind;gWallHitTopHeights[index]=high;
        }
next:
        n--;
    } while(n>0);
after_polys:
    if(index<0) return 0;
    if(index>0) {
        register Vec *scan __asm__("$7");
        register Vec *origin __asm__("$9");
        register int originX __asm__("$8");
        register int vx __asm__("$5");
        register int vy __asm__("$4");
        register int acc __asm__("$6");
        {
            register Vec *scanbase __asm__("$3");
            register int scanoffset __asm__("$2");
            n=index; origin=&gCellRayStart; originX=origin->x;
            __asm__ __volatile__("" : "+r"(originX));
            scanbase=gWallHitPositions; scanoffset=12*index;
            __asm__ __volatile__("" : "+r"(scanbase), "+r"(scanoffset));
            scan=(Vec *)(scanoffset+(int)scanbase);
        }
        do {
            {
                register int scanX __asm__("$2");
                scanX=scan->x;
                __asm__ __volatile__("" : "+r"(scanX));
                vx=scanX-originX;
            }
            if(vx<0)vx=-vx;
            {
                register int scanY __asm__("$3");
                register int originY __asm__("$2");
                scanY=scan->y; originY=origin->y;
                __asm__ __volatile__("" : "+r"(scanY), "+r"(originY));
                vy=scanY-originY;
            }
            if(vy<0)vy=-vy;
            if(vx<vy) {vx^=vy;vy^=vx;vx^=vy;}
            if((vx>>1)<vy) {
                register int part __asm__("$2");
                register int term __asm__("$3");
                acc=vx+vy;
                part=acc<<6; term=acc<<3;
                __asm__ __volatile__("" : "+r"(part), "+r"(term));
                part+=term; term=acc<<1;
                __asm__ __volatile__("" : "+r"(part), "+r"(term));
                part+=term; part+=vx; term=vy<<5;
                __asm__ __volatile__("" : "+r"(part), "+r"(term));
                acc=part+term;vy=vx-vy;vx=acc>>7;
            }
            {
                register int largepart __asm__("$2");
                register int smallpart __asm__("$3");
                largepart=vx+(vy>>2); smallpart=vy>>6;
                __asm__ __volatile__("" : "+r"(largepart), "+r"(smallpart));
                dist=largepart-smallpart;
            }
            if(dist<bestDist) {bestDist=dist;index=n;}
            n--;scan--;
        } while(n>=0);
    }
    if(outB)*outB=gWallHitTopHeights[index];
    if(outA) {
        register int resultOffset __asm__("$3");
        register Vec *resultBase __asm__("$2");
        register Vec *result __asm__("$3");
        resultOffset=12*index;
        __asm__ __volatile__("" : "+r"(resultOffset));
        resultBase=gWallHitPositions;
        result=(Vec *)(resultOffset+(int)resultBase);
        outA[0]=result->y+oy;outA[1]=result->z;outA[2]=result->x+ox;
    }
    {
        register int *kindbase __asm__("$3");
        register u32 *polybase __asm__("$2");
        register int offset __asm__("$4");
        register int *kindptr __asm__("$4");
        kindbase=gWallHitKinds; offset=index<<2; polybase=gWallHitPolys;
        __asm__ __volatile__("" : "+r"(kindbase), "+r"(offset), "+r"(polybase));
        polybase=(u32 *)(offset+(int)polybase);
        kindptr=(int *)(offset+(int)kindbase);
        __asm__ __volatile__("" : "+r"(polybase), "+r"(kindptr));
        {
            register int material __asm__("$2");
            const u8 *materialPtr;
            material=(*polybase>>13)&3;
            __asm__("addu %0,%1,%2" : "=r"(materialPtr) : "r"(tile), "r"(material));
            return *kindptr | (materialPtr[0x370]<<8);
        }
    }
}

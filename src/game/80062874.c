typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef struct Obj { char pad0[8]; s32 active; char padc[0x18]; s32 flags; char pad28[0x34]; s32 z; s32 y; s32 x; s32 radius; } Obj;
typedef struct { s32 count, pad; Obj *items[1]; } List;
typedef struct Node { Obj *obj; struct Node *next; } Node;
typedef struct { s32 x,z,w,h; } Bounds;
typedef struct { s32 x,z; } Pair;
typedef struct { u32 bits; s32 pad[3]; } Cell;
extern List D_800A8F90;
extern Pair D_800A3190[];
extern Bounds D_800A2970;
extern Node D_800A2990[];
extern Cell D_800AB3B8[], D_800AB548[];
extern s32 D_8009F814, D_8009F828, D_8009F82C;
extern Obj **D_8009F818, **D_8009F81C;
extern u8 *D_8009F820;
extern u8 D_8009F834, D_8009F835, D_8009F836, D_8009F837;

void func_80062874(void) {
    register Obj **scan __asm__("$24") = D_800A8F90.items;
    register Pair *pairs __asm__("$3") = D_800A3190;
    Cell *cells = D_800AB3B8;
    register Bounds *bounds __asm__("$2") = &D_800A2970;
    s32 width = bounds->w;
    register s32 count1 __asm__("$13") = D_8009F828;
    register s32 count2 __asm__("$14") = D_8009F82C;
    register s32 remaining __asm__("$15") = D_800A8F90.count;
    register s32 cx1 __asm__("$20") = pairs[0].x;
    register s32 cz1 __asm__("$19") = pairs[0].z;
    register s32 cx2 __asm__("$18") = pairs[1].x;
    register s32 cz2 __asm__("$17") = pairs[1].z;
    s32 height;
    s32 bx;
    s32 bz;
    register s32 n __asm__("$12") = 1;
    register s32 fifteen __asm__("$23");
    register s32 one __asm__("$22");
    Node *nodes;
    register Obj *o __asm__("$10");
    register s32 z __asm__("$11");
    register s32 x __asm__("$9");
    register s32 r __asm__("$7");
    register s32 px __asm__("$8");
    register s32 pz __asm__("$6");
    register s32 dx __asm__("$5");
    register s32 sign __asm__("$3");
    register s32 hit1 __asm__("$25");
    register s32 hit2 __asm__("$7");
    register s32 dz __asm__("$5");
    register u32 bits __asm__("$3");
    Node *node;

    bx = bounds->x;
    height = bounds->h;
    bz = bounds->z;
    if (remaining > 0) {
        fifteen = 15;
        one = n;
        nodes = D_800A2990;
        do {
            o = *scan++;
            if (o->active <= 0) goto next;
            z = o->z; x = o->x; r = o->radius;
            __asm__("" : : "r"(z), "r"(x));
            dx = x - cx1;
            pz = z;
            sign = dx >> 31;
            if (((sign + dx) ^ sign) > r) goto xoutside1;
            px = cx1;
            goto xdone1;
        xoutside1:
            if (dx > 0) px = x - r;
            else px = x + r;
        xdone1:
            dz = pz - cz1;
            sign = dz >> 31;
            if (((sign + dz) ^ sign) <= r) pz = cz1;
            else if (dz > 0) pz -= r;
            else pz += r;
            hit1 = 0;
            {
                register s32 col __asm__("$4");
                register s32 s1 __asm__("$3") = pz >> 17;
                register s32 o1 __asm__("$2") = D_8009F835;
                col = s1 - o1;
                if ((u32)col < 5) {
                    col += col << 2;
                    {
                        register s32 o2 __asm__("$3") = D_8009F834;
                        register s32 s2 __asm__("$2") = px >> 17;
                        dz = s2 - o2;
                    }
                    if ((u32)dz < 5) {
                        col += dz;
                        {
                            register s32 off __asm__("$2") = col << 4;
                            bits = ((Cell *)((s32)off + (s32)cells))->bits;
                        }
                        if (bits != 0) {
                            register s32 sh1 __asm__("$2") = pz >> 13;
                            col = sh1 & 12;
                            {
                                register s32 sh2 __asm__("$2") = px >> 15;
                                sh2 &= 3;
                                __asm__("" : "+r"(sh2));
                                col += sh2;
                            }
                            {
                                register s32 sh3 __asm__("$2") = fifteen - col;
                                sh3 = one << sh3;
                                sh3 = bits & sh3;
                                if (sh3) {
                                    if (count1 >= 256) goto next;
                                    hit1 = 1;
                                    {
                                        register s32 off __asm__("$2") = count1 << 2;
                                        register Obj **arr __asm__("$3") = D_8009F818;
                                        count1++;
                                        arr = (Obj **)((s32)arr + off);
                                        __asm__("" : "+r"(arr));
                                        *arr = o;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            dx = x - cx2;
            pz = z;
            sign = dx >> 31;
            if (((sign + dx) ^ sign) > r) goto xoutside2;
            px = cx2;
            goto xdone2;
        xoutside2:
            if (dx > 0) px = x - r;
            else px = x + r;
        xdone2:
            dz = pz - cz2;
            sign = dz >> 31;
            if (((sign + dz) ^ sign) <= r) pz = cz2;
            else if (dz > 0) pz -= r;
            else pz += r;
            hit2 = 0;
            {
                register s32 col __asm__("$4");
                register s32 s1 __asm__("$3") = pz >> 17;
                register s32 o1 __asm__("$2") = D_8009F837;
                col = s1 - o1;
                if ((u32)col < 5) {
                    col += col << 2;
                    {
                        register s32 o2 __asm__("$3") = D_8009F836;
                        register s32 s2 __asm__("$2") = px >> 17;
                        dz = s2 - o2;
                    }
                    if ((u32)dz < 5) {
                        col += dz;
                        {
                            register s32 off __asm__("$2") = col << 4;
                            bits = ((Cell *)((s32)off + (s32)D_800AB548))->bits;
                        }
                        if (bits != 0) {
                            register s32 sh1 __asm__("$2") = pz >> 13;
                            col = sh1 & 12;
                            {
                                register s32 sh2 __asm__("$2") = px >> 15;
                                sh2 &= 3;
                                __asm__("" : "+r"(sh2));
                                col += sh2;
                            }
                            {
                                register s32 sh3 __asm__("$2") = fifteen - col;
                                sh3 = one << sh3;
                                sh3 = bits & sh3;
                                hit2 = sh3 != 0;
                            }
                            if (count2 >= 256) goto next;
                            {
                                register s32 off __asm__("$2") = count2 << 2;
                                register Obj **arr __asm__("$3") = D_8009F81C;
                                count2++;
                                arr = (Obj **)((s32)arr + off);
                                __asm__("" : "+r"(arr));
                                *arr = o;
                            }
                        }
                    }
                }
            }
            if (o->flags & 4) goto next;
            if (!hit1 && !hit2 && !(o->flags & 0x100)) goto next;
            {
                register s32 col __asm__("$4");
                register s32 sx __asm__("$2") = x >> 15;
                col = sx - bx;
                if (col < 0 || col >= width) goto next;
                {
                    register s32 sz __asm__("$2") = z >> 15;
                    register s32 row __asm__("$3") = sz - bz;
                    u8 *p;
                    u8 old;
                    if (row < 0 || row >= height) goto next;
                    col += row * width;
                    p = &D_8009F820[col];
                    old = *p;
                    *p = n;
                    __asm__ volatile("" : "+r"(n));
                    {
                        register s32 off __asm__("$2") = n << 3;
                        n++;
                        col = off + (s32)nodes;
                        __asm__("" : "+r"(col));
                        node = (Node *)col;
                    }
                    node->obj = o;
                    if (old == 0) {
                        node->next = 0;
                    } else {
                        register s32 off2 __asm__("$2") = old << 3;
                        off2 = off2 + (s32)nodes;
                        __asm__("" : "+r"(off2));
                        node->next = (Node *)off2;
                    }
                }
            }
            __asm__("" : : "r"(x), "r"(z));
        next:
            remaining--;
        } while (remaining > 0);
    }
    __asm__("" : : "r"(cells));
    D_8009F814 = n;
    D_8009F828 = count1;
    D_8009F82C = count2;
}

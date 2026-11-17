// Categories: Graphics > Culling > Object Grid | Graphics > HUD > Minimap
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef struct Obj { char pad0[8]; s32 active; char padc[0x18]; s32 flags; char pad28[0x34]; s32 z; s32 y; s32 x; s32 radius; } Obj;
typedef struct { s32 count, pad; Obj *items[1]; } List;
typedef struct Node { Obj *obj; struct Node *next; } Node;
typedef struct { s32 x,z,w,h; } Bounds;
typedef struct { s32 x,z; } Pair;
typedef struct { u32 bits; s32 pad[3]; } Cell;
extern List gCommandListHead __asm__("D_800A8F90");
extern Pair D_800A3190[];
extern Bounds D_800A2970;
extern Node D_800A2990[];
extern Cell D_800AB3B8[], D_800AB548[];
extern s32 D_8009F814, D_8009F828, D_8009F82C;
extern Obj **D_8009F818, **D_8009F81C;
extern u8 *D_8009F820;
extern u8 D_8009F834, D_8009F835, D_8009F836, D_8009F837;

/**
 * @brief Builds the two per-view object lists and the per-cell object grid of
 *        the full region, testing every entity against both view anchors.
 *
 * @param (none) The function takes no arguments; all its inputs are the globals
 *        D_800A8F90, D_800A3190, D_800AB3B8, D_800AB548, D_8009F834..D_8009F837,
 *        D_800A2970, D_8009F820, D_8009F828 and D_8009F82C.
 * @return Nothing. The results land in D_8009F828/D_8009F818 (view 0),
 *         D_8009F82C/D_8009F81C (view 1), the grid heads in D_8009F820 and the
 *         node counter in D_8009F814.
 *
 * @par Confirmed
 * 1. The entity table D_800A8F90 is walked by count from the head of
 *    D_800A8F90.items; entries whose word at +0x08 is not positive are
 *    skipped. Per entity the z, x and radius words come from +0x5C, +0x64 and
 *    +0x68 and the flags word from +0x24.
 * 2. Clamp to the view 0 anchor (D_800A3190[0], x and z): per axis, when the
 *    distance to the anchor does not exceed the radius the anchor coordinate is
 *    used, otherwise the point moves one radius towards the anchor. The result
 *    is the point of the disc nearest the anchor. The same is repeated with
 *    D_800A3190[1] for view 1.
 * 3. Coverage test of view 0: cell column = (z >> 17) - D_8009F835 and row =
 *    (x >> 17) - D_8009F834, both rejected outside 0..4; cell = 16 bytes at
 *    D_800AB3B8 + (row * 5 + column) * 16 and its first word is the bit mask;
 *    the bit index is 15 - (((z >> 15) & 3) * 4 + ((x >> 15) & 3)).
 * 4. View 0 appends the entity to D_8009F818 when that bit is set, bounded by
 *    256 and with the counter taken from D_8009F828.
 * 5. View 1 uses D_8009F837/D_8009F836, the block D_800AB548, the list
 *    D_8009F81C and the counter D_8009F82C, and it appends as soon as the cell
 *    word is non-zero, the bit result only being recorded in its own hit flag.
 * 6. Entities with bit 4 of the flags word never reach the grid step. Entities
 *    without any coverage hit reach it only when bit 0x100 is set.
 * 7. Grid step: column = (x >> 15) - D_800A2970.x and row = (z >> 15) -
 *    D_800A2970.z, both bounded by D_800A2970.w and .h. The byte
 *    D_8009F820[row * width + column] is read as the previous head, replaced by
 *    n, and node n of D_800A2990 receives the entity and the previous head;
 *    n starts at 1 and ends in D_8009F814.
 * 8. Both counters are read from memory and written back, so the caller has to
 *    zero them for a fresh list, which func_80062FB4 does.
 *
 * @par Likely role
 * The view gather of the region pass: an entity only enters a view's draw list
 * when the point of its disc nearest the view anchor falls inside a covered
 * cell of that view's 5x5 block, and the region rectangle is turned into
 * per-cell buckets so func_80062614 can link neighbouring cells. This is the
 * two-view variant; func_80062C3C is the single-view one.
 *
 * @par Unknown
 * What the 16 bits of a cell encode and in which space the anchors and the
 * grid live; the meaning of entity flag bits 4 and 0x100; why view 1 appends
 * without a bit hit while view 0 requires one.
 *
 * @par Callers
 * func_80062FB4, the region setup, in its non-small-view branch.
 */
void func_80062874(void) {
    register Obj **scan __asm__("$24") = gCommandListHead.items;
    register Pair *pairs __asm__("$3") = D_800A3190;
    Cell *cells = D_800AB3B8;
    register Bounds *bounds __asm__("$2") = &D_800A2970;
    s32 width = bounds->w;
    register s32 count1 __asm__("$13") = D_8009F828;
    register s32 count2 __asm__("$14") = D_8009F82C;
    register s32 remaining __asm__("$15") = gCommandListHead.count;
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

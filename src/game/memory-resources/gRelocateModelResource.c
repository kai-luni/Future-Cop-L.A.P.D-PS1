// Categories: Resources > Data Initialization | Graphics > Animation
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef struct AnimEntry { u8 max, cur, type, flags; s16 rate, timer; u8 *src, *dst; } AnimEntry;
typedef struct AnimNode { struct AnimNode *next; s32 pad; s32 count; AnimEntry entries[1]; } AnimNode;
typedef struct Header { u32 a, b; AnimNode *anim; u32 flags; u32 x10, x14, x18; u8 *p1c, *p20, *p24, *p28, *p2c; u32 p30; u32 *p34; u32 p38; } Header;
extern u32 gModelTriangleListSize __asm__("D_8009F528");
extern Header *gModelGeometryChunk __asm__("D_8009F52C");
extern u8 *gModelBoundingBox __asm__("D_8009F540");
extern u8 *gModelMeshInfo __asm__("D_8009F53C");
extern u8 *gModelHierarchy __asm__("D_8009F538");
extern u8 *gModelTriangleList __asm__("D_8009F534");
extern u8 *gModelPolygonList __asm__("D_8009F530");
extern AnimNode *gAnimNodeList __asm__("D_8009F564");
extern AnimNode *gModelAnimData __asm__("D_8009F544");
extern u32 *gModel3dhsPayload __asm__("D_8009F548");
extern u32 *gModelAnimWaveChunk __asm__("D_8009F550");
extern u32 *gAnimWaveList __asm__("D_8009F568");
extern AnimNode *gModelAnimTrackChunk __asm__("D_8009F54C");
extern u8 **gRefBlockTable __asm__("D_8009F560");
extern u32 gTexpageBaseTable[] __asm__("D_800A8E80");
extern void gIndexModelChunks(void *, u32, u32) __asm__("func_80012E48");
extern void gResolveModelBlockIndices(void *) __asm__("func_8001306C");

void gRelocateModelResource(Header **out, void *data, u32 size, u32 arg3) __asm__("func_800131A0");

/**
 * @brief Indexes a model resource and relocates its header, draw records, and
 * optional animation data in place.
 *
 * @param out Receives the relocated geometry header.
 * @param data Beginning of the model chunks.
 * @param size Length of the chunk buffer in bytes.
 * @param arg3 Word passed in $a2 to gIndexModelChunks, which does not read it.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gIndexModelChunks. Resolves three header references via
 * gRefBlockTable, rewrites type 2 and 3 triangle records through
 * gTexpageBaseTable, links animation chunks, and repacks the first byte of
 * 12-byte polygon records through a jump table. Writes the relocated header to
 * the output pointer.
 *
 * @par Globals
 * Reads gModelGeometryChunk, gModelBoundingBox, gRefBlockTable,
 * gModelPolygonList, gModelTriangleList, gModelTriangleListSize,
 * gModelHierarchy, gModelMeshInfo, gModelAnimData, gModel3dhsPayload,
 * gModelAnimTrackChunk, gModelAnimWaveChunk, and gTexpageBaseTable. Reads and
 * updates gAnimNodeList and gAnimWaveList.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPrepareModelResource.
 *
 * @par Role and path
 * The Cobj loader runs this pass before registering a model for later
 * rendering and animation.
 *
 * @par Unknown
 * The exact meanings of all packed polygon bits and the forwarded fourth
 * argument remain open.
 */
void gRelocateModelResource(Header **out, void *data, u32 size, u32 arg3)
{
    Header *h;
    u8 *end;
    u8 *p;
    u8 *anim_end;
    u8 *q;
    AnimNode *node;
    AnimEntry *e;
    s32 i, count;
    register u32 a __asm__("$7");
    register u32 b __asm__("$5");
    u32 *node2;

    gIndexModelChunks(data, size, arg3);
    h = gModelGeometryChunk;
    h->p34 = (u32 *)gModelBoundingBox;
    h->p34[1] = *(u16 *)&h->flags << 4;
    __asm__ volatile ("" : : : "memory");
    p = h->p20;
    h->p28 = gModelTriangleList;
    h->p24 = gModelPolygonList;
    p = gRefBlockTable[(u32)p]; h->p20 = p + 4; gResolveModelBlockIndices(p);
    p = h->p1c; p = gRefBlockTable[(u32)p]; h->p1c = p + 4; gResolveModelBlockIndices(p);
    p = h->p2c; p = gRefBlockTable[(u32)p]; h->p2c = p + 4; gResolveModelBlockIndices(p);

    p = end = h->p28;
    end += gModelTriangleListSize;
    while (p < end) {
        switch (*p) {
        case 1:
            p += 4;
            break;
        case 0:
            p += 4;
            break;
        case 2:
        case 3:
            {
                u32 val = gTexpageBaseTable[*(u32 *)(p + 12)];
                *(u32 *)(p + 12) = val;
                if (*p == 3) {
                    *(u32 *)(p + 12) = val | 0x20;
                    *p = 2;
                }
                p += 16;
            }
            break;
        }
    }

    if (gModelAnimTrackChunk) {
        node = gModelAnimTrackChunk;
        h->anim = node;
        h->flags |= 0x1000000;
        node->next = gAnimNodeList;
        gAnimNodeList = node;
        e = node->entries;
        anim_end = (u8 *)(e + node->count);
        for (i = 0; i < node->count; i++, e++) {
            u8 *dst = e->dst;
            e->src = (u8 *)((u32)anim_end + (u32)e->src);
            e->dst = h->p28 + (u32)dst;
            if (e->type >= 16) {
                e->rate = 0x20000 / (e->rate * e->max);
            }
        }
    } else {
        h->anim = 0;
        h->flags &= 0xfeffffff;
    }

    a = 0;
    node2 = gModelAnimWaveChunk;
    if (node2) {
        u8 *hp = h->p24;
        u32 *old = gAnimWaveList;
        gAnimWaveList = node2;
        node2[1] = (u32)(hp + 4);
        node2[0] = (u32)old;
    }

    __asm__ volatile ("" : "+r" (a));
    b = a;
    count = *(s32 *)h->p24;
    q = h->p24 + 4;
    while (count > 0) {
        u8 c = *q;
        u8 d = c & 7;

        u8 v = (c & 8) >> 3;
        u8 a2 = c >> 7;
        u32 val;
        switch ((c & 0x70) >> 4) {
        case 0: case 1: a = 0; b = a2; break;
        case 2: case 3: a = 1; b = a2; break;
        case 4: case 5: a = 1; b = 2; if (a2) b = 3; break;
        case 6: a = 0; b = 1; break;
        }
        if (d == 4) b += 4;
        val = b << 4;
        if (v) val |= 2;
        { u32 r = val; r |= a; *q = r; }
        count--;
        q += 12;
    }

    if ((h->flags >> 25) & 1) {
        h->x14 = (u32)gModelHierarchy;
        h->x18 = (u32)gModelMeshInfo;
    }
    h->x10 = (u32)gModelAnimData;
    h->p38 = (u32)gModel3dhsPayload;
    *out = h;
}

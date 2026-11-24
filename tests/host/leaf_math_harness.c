/* Source and arithmetic characterizations of unchanged src/game units on
 * synthetic RAM. Every fixture is written before the call; nothing here reads
 * retail initial memory. Callbacks the units reach through are bound by their
 * retail asm label (fc_testkit.h rule 1) and record what they received.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* --------------------------------------------------------------- stand-ins */

static void *free_args[12];
static int free_calls;

void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeBlock(void *block)
{
    if (free_calls < 12) free_args[free_calls] = block;
    free_calls++;
}

/* 16.16/word matrix-times-vector. The unit under test supplies the operands;
 * the rotated vector itself is a fixture so only negation is under test. */
static void *mv_matrix, *mv_input, *mv_output;
static s32 mv_out[3];
static int mv_calls;

void gMulMatrixVector(void *matrix, void *input, void *output) __asm__("func_8001A81C");
void gMulMatrixVector(void *matrix, void *input, void *output)
{
    s32 *o = output;
    mv_matrix = matrix;
    mv_input = input;
    mv_output = output;
    mv_calls++;
    o[0] = mv_out[0];
    o[1] = mv_out[1];
    o[2] = mv_out[2];
}

static void *alloc_queue[4];
static int alloc_index, alloc_calls;
static s32 alloc_size, alloc_tag;

void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
void *gAllocMemory(s32 size, s32 tag)
{
    alloc_size = size;
    alloc_tag = tag;
    alloc_calls++;
    if (alloc_index < 4) return alloc_queue[alloc_index++];
    return 0;
}

static void *pool_result[3];
static s32 pool_count[3], pool_size[3], pool_tag[3];
static int pool_calls;

void *gCreateFixedPool(s32 count, s32 size, s32 tag) __asm__("func_800194DC");
void *gCreateFixedPool(s32 count, s32 size, s32 tag)
{
    if (pool_calls < 3) {
        pool_count[pool_calls] = count;
        pool_size[pool_calls] = size;
        pool_tag[pool_calls] = tag;
    }
    pool_calls++;
    return pool_calls <= 3 ? pool_result[pool_calls - 1] : 0;
}

static s32 heap_free_args[4];
static int heap_free_calls;

void gHeapFreeRaw(s32 block) __asm__("func_8001E2C0");
void gHeapFreeRaw(s32 block)
{
    if (heap_free_calls < 4) heap_free_args[heap_free_calls] = block;
    heap_free_calls++;
}

static u32 rgb_r, rgb_g, rgb_b;
static int rgb_calls;

u32 gPackRgb24(u32 r, u32 g, u32 b) __asm__("func_8001BC44");
u32 gPackRgb24(u32 r, u32 g, u32 b)
{
    rgb_r = r;
    rgb_g = g;
    rgb_b = b;
    rgb_calls++;
    return 0x5A5A5A5AU;
}

/* Low-level bump allocator. Slot i of heap_alloc_result is what call i gets;
 * an entry left zero makes that call fail, which drives the retry loops. */
static s32 *heap_alloc_result[8];
static s32 heap_alloc_size[8];
static int heap_alloc_calls;

s32 *gAllocHeapBlock(s32 size) __asm__("func_8008CA90");
s32 *gAllocHeapBlock(s32 size)
{
    s32 *result = 0;
    if (heap_alloc_calls < 8) {
        heap_alloc_size[heap_alloc_calls] = size;
        result = heap_alloc_result[heap_alloc_calls];
    }
    heap_alloc_calls++;
    return result;
}

static u8 *rot_dest;
static s32 rot_yaw, rot_pitch, rot_roll;
static u8 *view_src, *view_dest;
static int view_calls;

void gBuildRotationMatrix(s32 yaw, s32 pitch, s32 roll, u8 *rotation) __asm__("func_800188D0");
void gBuildRotationMatrix(s32 yaw, s32 pitch, s32 roll, u8 *rotation)
{
    rot_yaw = yaw;
    rot_pitch = pitch;
    rot_roll = roll;
    rot_dest = rotation;
}

void gBuildViewMatrix(u8 *rotation, u8 *matrix) __asm__("func_80018860");
void gBuildViewMatrix(u8 *rotation, u8 *matrix)
{
    view_src = rotation;
    view_dest = matrix;
    view_calls++;
}

/* ------------------------------------------------------------------ globals */

void *gLevelVertexBlock __asm__("D_8009FA1C");
void *gPartVertexBuffer __asm__("D_8009F578");
void *gClipFlagBuffer __asm__("D_8009F57C");

u8 *gCamera __asm__("D_8009F9E4");
u8 gLightVectorIn[12] __asm__("D_8009A228");
u8 gLightVectorOut[12] __asm__("D_800A02E0");

void *gCameraList __asm__("D_8009F588");
s32 gNextCameraId __asm__("D_8009F58C");
s32 gIdentityMatrix[12] __asm__("D_8009AA38");

s32 gHeapPoolBase __asm__("D_8009FA9C");
s32 gFreeRangeTablePtr __asm__("D_8009F590");
s32 gHeapMainBlockSize __asm__("D_8009F9E8");
s32 gHeapRangeCountThreshold __asm__("D_8009F5A0");
s32 D_8009F59C;
s32 gFreeRangeCount __asm__("D_8009F594");
s32 gHeapSearchCursor __asm__("D_8009F598");

void *gFixedPool1 __asm__("D_8009F9D0");
void *gFixedPool2 __asm__("D_8009FAAC");
void *gFixedPool3 __asm__("D_8009FA4C");

void *gReciprocalTablePtr __asm__("D_8009F5B0");
void *gResourceListHead __asm__("D_8009F5B8");

void *gPacketBufferBases __asm__("D_8009F5C8");
void *gSecondPacketBufferBase __asm__("D_8009F5CC");
void *gRenderNodeTable __asm__("D_8009F5D0");
void *gRenderNodeTableAlt __asm__("D_8009F5D4");
void *gOrderingTableBuffers __asm__("D_8009F5D8");
void *gSecondOrderingTableBuffer __asm__("D_8009F5DC");

s32 gHeapHighWater __asm__("D_80010028");
s32 gHeapHighWaterPeak __asm__("D_8001002C");

/* ----------------------------------------------------------- prototypes */

void gFreeModelRendererBuffers(void) __asm__("func_80013AB4");
void gUpdateLightDirection(void) __asm__("func_80013C80");
void gFreeCameraList(void) __asm__("func_80017764");
struct Vec3s32 { s32 x, y, z; };
void gSetCameraPosition(struct Vec3s32 *position) __asm__("func_800177AC");
void gSetCameraRotation(s32 yaw, s32 pitch, s32 roll) __asm__("func_80017860");
s32 gCreateCamera(void) __asm__("func_80017994");
void gShutdownHeap(void) __asm__("func_80017BA0");
void gInitHeap(void) __asm__("func_80017BE8");
void gInitHeapAndFixedPools(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) __asm__("func_80017C68");
void gFreeFixedPools(void) __asm__("func_80017D20");
void gFreeReciprocalTable(void) __asm__("func_800198DC");
struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
    u8 unk14[4];
    s32 group;
    s32 name;
};
void gScanAndFreeResource(struct Resource *resource) __asm__("func_80019D00");
void gFreeRenderBuffers(void) __asm__("func_8001B448");
u32 gPackClampedRgb16Array(s16 *input) __asm__("func_8001BD70");
int gFreeDefaultCdBuffer(void *block) __asm__("func_8001CDF0");
s32 *gHeapAllocRaw(s32 size) __asm__("func_8001E240");

/* --------------------------------------------------------------- fixtures */

static u32 arena[64];
static s32 range_table[6];

static void reset_free(void)
{
    free_calls = 0;
    lk_fill(free_args, 0, sizeof(free_args));
}

/* ------------------------------------------------------------- 80013AB4 */

static void test_model_renderer_buffers(void)
{
    void *level = &arena[0], *part = &arena[8], *clip = &arena[16];

    /* @leaf 80013AB4 source src/game/80013AB4.c: each saved pointer is freed once in source order and the globals stay published */
    LK_FUNC("func_80013AB4");
    reset_free();
    gLevelVertexBlock = level;
    gPartVertexBuffer = part;
    gClipFlagBuffer = clip;
    gFreeModelRendererBuffers();
    FCK_EQ(free_calls, 3, "three renderer buffers released");
    FCK_CHECK(free_args[0] == level && free_args[1] == part && free_args[2] == clip,
              "release order follows the three globals");
    FCK_CHECK(gLevelVertexBlock == level && gPartVertexBuffer == part && gClipFlagBuffer == clip,
              "pointers remain published after release");
}

/* ------------------------------------------------------------- 80013C80 */

static void test_light_direction(void)
{
    static u8 camera[0x100];
    int i;

    /* @leaf 80013C80 source src/game/80013C80.c: the camera matrix at +0x28, not the camera base, is the first operand */
    LK_FUNC("func_80013C80");
    lk_fill(camera, 0xA5, sizeof(camera));
    for (i = 0; i < 12; i++) {
        gLightVectorIn[i] = (u8)(0x40 + i);
        gLightVectorOut[i] = 0x6D;
    }
    gCamera = camera;
    mv_calls = 0;
    mv_matrix = mv_input = 0;
    gUpdateLightDirection();
    FCK_EQ(mv_calls, 1, "one matrix-vector product per frame");
    FCK_CHECK(mv_matrix == camera + 0x28, "matrix operand is the camera orientation at +0x28");
    FCK_CHECK(mv_input == gLightVectorIn, "fixed input vector forwarded unchanged");
    FCK_CHECK(mv_output == gLightVectorOut, "rotated vector written to the published source");
    FCK_CHECK(lk_all(camera, 0, 0x28, 0xA5), "camera header before the matrix untouched");
}

/* ------------------------------------------------------------- 80017764 */

struct CamListRecord {
    u8 unk00[0x8C];
    struct CamListRecord *next, *previous;
};

static void test_camera_list_free(void)
{
    struct CamListRecord nodes[3], snapshot[3];

    /* @leaf 80017764 source src/game/80017764.c: successor read before release, every record freed once, head cleared, id counter left alone */
    LK_FUNC("func_80017764");
    lk_fill(nodes, 0xA5, sizeof(nodes));
    nodes[0].next = &nodes[1];
    nodes[1].next = &nodes[2];
    nodes[2].next = 0;
    nodes[0].previous = nodes[1].previous = nodes[2].previous = 0;
    memcpy(snapshot, nodes, sizeof(nodes));
    gCameraList = nodes;
    gNextCameraId = 41;
    reset_free();
    gFreeCameraList();
    FCK_EQ(free_calls, 3, "each list record released once");
    FCK_CHECK(free_args[0] == &nodes[0] && free_args[1] == &nodes[1] && free_args[2] == &nodes[2],
              "records released head to tail");
    FCK_CHECK(gCameraList == 0, "list head cleared");
    FCK_EQ(gNextCameraId, 41, "camera id counter deliberately survives");
    FCK_EQ(memcmp(nodes, snapshot, sizeof(nodes)), 0, "released records are not rewritten");

    reset_free();
    gCameraList = 0;
    gFreeCameraList();
    FCK_EQ(free_calls, 0, "empty list releases nothing");
    FCK_CHECK(gCameraList == 0, "empty list still leaves a null head");
}

/* ------------------------------------------------------------- 800177AC */

struct CameraPosition {
    struct Vec3s32 position;
    u8 unk0C[0x1C];
    u8 matrix[0x24];
    struct Vec3s32 inverse;
    u8 unk58[0x24];
    struct Vec3s32 eye;
};

static void test_camera_position(void)
{
    static struct CameraPosition camera;
    struct Vec3s32 wanted;

    /* @leaf 800177AC math src/game/800177AC.c: position copied to eye, and the matrix product is negated component-wise */
    LK_FUNC("func_800177AC");
    lk_fill(&camera, 0xA5, sizeof(camera));
    wanted.x = 7;
    wanted.y = -9;
    wanted.z = 11;
    mv_out[0] = 100;
    mv_out[1] = -200;
    mv_out[2] = 300;
    mv_calls = 0;
    gCamera = (u8 *)&camera;
    gSetCameraPosition(&wanted);
    FCK_EQ(mv_calls, 1, "one matrix-vector product");
    FCK_CHECK(camera.position.x == 7 && camera.position.y == -9 && camera.position.z == 11,
              "camera position takes the requested triple");
    FCK_CHECK(camera.eye.x == 7 && camera.eye.y == -9 && camera.eye.z == 11,
              "eye duplicated from the same triple");
    FCK_CHECK(mv_input == &wanted, "product uses the caller's position");
    FCK_CHECK(mv_matrix == camera.matrix, "product uses the camera matrix at +0x28");
    FCK_CHECK(camera.inverse.x == -100 && camera.inverse.y == 200 && camera.inverse.z == -300,
              "view translation is the negated product");
    FCK_CHECK(lk_all(&camera, 0x0C, 0x4C, 0xA5), "gap and matrix bytes untouched");
    FCK_CHECK(lk_all(&camera, 0x58, 0x7C, 0xA5), "bytes between inverse and eye untouched");
}

/* ------------------------------------------------------------- 80017860 */

struct CameraRotation {
    u8 unk00[0x18];
    s32 roll, pitch, yaw;
    u8 unk24[4];
    u8 matrix[0x30];
    u8 rotation[0x20];
};

static void test_camera_rotation(void)
{
    static struct CameraRotation camera;

    /* @leaf 80017860 source src/game/80017860.c: rotation buffer then view matrix, and the three angles stored in pitch/yaw/roll order */
    LK_FUNC("func_80017860");
    lk_fill(&camera, 0xA5, sizeof(camera));
    gCamera = (u8 *)&camera;
    view_calls = 0;
    rot_dest = view_src = view_dest = 0;
    gSetCameraRotation(0x123, -0x456, 0x7AB);
    FCK_CHECK(rot_dest == camera.rotation, "rotation factory writes the +0x58 rotation buffer");
    FCK_CHECK(rot_yaw == 0x123 && rot_pitch == -0x456 && rot_roll == 0x7AB,
              "factory receives yaw, pitch, roll in that order");
    FCK_EQ(view_calls, 1, "one view matrix rebuild");
    FCK_CHECK(view_src == camera.rotation && view_dest == camera.matrix,
              "view matrix derives from the rotation buffer into +0x28");
    FCK_EQ(camera.yaw, 0x123, "yaw stored at +0x20");
    FCK_EQ(camera.pitch, -0x456, "pitch stored at +0x1C");
    FCK_EQ(camera.roll, 0x7AB, "roll stored at +0x18");
    FCK_CHECK(lk_all(&camera, 0x24, 0x28, 0xA5) && lk_all(&camera, 0x58, sizeof(camera), 0xA5),
              "padding outside matrix and rotation untouched");
    FCK_CHECK(rot_dest == view_src && rot_dest == camera.rotation,
              "view matrix consumes the rotation buffer just built");
}

/* ------------------------------------------------------------- 80017994 */

struct CameraRecord {
    u8 unk00[0x28];
    s32 first[12];
    s32 second[12];
    s32 id;
    struct CameraRecord *next, *previous;
};

static void test_camera_create(void)
{
    static struct CameraRecord nodes[2];
    int i;

    /* @leaf 80017994 source src/game/80017994.c: both matrix copies, id from the counter and an append that sets the previous link */
    LK_FUNC("func_80017994");
    lk_fill(nodes, 0xA5, sizeof(nodes));
    for (i = 0; i < 12; i++) gIdentityMatrix[i] = 0x10000 + 0x37 * i;
    gCameraList = 0;
    gNextCameraId = 5;
    alloc_queue[0] = &nodes[0];
    alloc_queue[1] = &nodes[1];
    alloc_index = 0;
    alloc_calls = 0;
    FCK_EQ(gCreateCamera(), 5, "first record returns the pre-increment id");
    FCK_EQ(alloc_calls, 1, "one allocation per record");
    FCK_CHECK(alloc_size == 0x94 && alloc_tag == 2, "record request is 0x94 bytes with tag two");
    FCK_EQ(gNextCameraId, 6, "id counter advanced");
    FCK_EQ(memcmp(nodes[0].first, gIdentityMatrix, sizeof(gIdentityMatrix)), 0, "first matrix block copied");
    FCK_EQ(memcmp(nodes[0].second, gIdentityMatrix, sizeof(gIdentityMatrix)), 0, "second matrix block copied");
    FCK_CHECK(gCameraList == &nodes[0], "first record becomes the list head");
    FCK_CHECK(nodes[0].next == 0 && nodes[0].previous == 0, "head links terminate");
    FCK_CHECK(lk_all(&nodes[0], 0, 0x28, 0xA5), "record header left as allocated");

    FCK_EQ(gCreateCamera(), 6, "second record gets the next id");
    FCK_CHECK(gCameraList == &nodes[0], "head unchanged by append");
    FCK_CHECK(nodes[0].next == &nodes[1] && nodes[1].previous == &nodes[0],
              "tail walk links the new record behind the old one");
    FCK_CHECK(nodes[1].next == 0, "appended record terminates the list");
    FCK_EQ(memcmp(nodes[1].second, gIdentityMatrix, sizeof(gIdentityMatrix)), 0,
          "second record carries both matrix copies");
}

/* ------------------------------------------------------------- 80017BA0 */

static void test_heap_shutdown(void)
{
    /* @leaf 80017BA0 source src/game/80017BA0.c: each block freed only when non-zero and both slots cleared afterwards */
    LK_FUNC("func_80017BA0");
    gHeapPoolBase = (s32)&arena[0];
    gFreeRangeTablePtr = (s32)&arena[16];
    heap_free_calls = 0;
    gShutdownHeap();
    FCK_EQ(heap_free_calls, 2, "both heap layers released");
    FCK_CHECK(heap_free_args[0] == (s32)&arena[0] && heap_free_args[1] == (s32)&arena[16],
              "main block released before the range table");
    FCK_CHECK(gHeapPoolBase == 0 && gFreeRangeTablePtr == 0, "both publications cleared");

    gHeapPoolBase = 0;
    gFreeRangeTablePtr = (s32)&arena[16];
    heap_free_calls = 0;
    gShutdownHeap();
    FCK_EQ(heap_free_calls, 1, "null main block is skipped");
    FCK_EQ(heap_free_args[0], (s32)&arena[16], "range table still released");
    FCK_CHECK(gHeapPoolBase == 0 && gFreeRangeTablePtr == 0, "cleared pair after partial teardown");

    heap_free_calls = 0;
    gShutdownHeap();
    FCK_EQ(heap_free_calls, 0, "repeated shutdown releases nothing");
}

/* ------------------------------------------------------------- 80017BE8 */

static void test_heap_init(void)
{
    /* Raw returned blocks; the allocator's documented contract publishes the
     * usable area one word past the size header written at word zero. */
    u32 *range = &arena[0], *main_block = &arena[8];

    /* @leaf 80017BE8 source src/game/80017BE8.c: sentinels, 0x810-byte table and a main block retried in 0x1000 steps until granted */
    LK_FUNC("func_80017BE8");
    lk_fill(arena, 0xA5, sizeof(arena));
    gHeapHighWater = 0;
    gHeapHighWaterPeak = 0;
    heap_alloc_calls = 0;
    lk_fill(heap_alloc_result, 0, sizeof(heap_alloc_result));
    heap_alloc_result[0] = (s32 *)range;
    heap_alloc_result[3] = (s32 *)main_block;
    gHeapMainBlockSize = 0;
    gShutdownHeap(); /* start from a cleared publication pair */
    heap_alloc_calls = 0;
    gInitHeap();
    FCK_EQ(D_8009F59C, 0x203, "range sentinel written");
    FCK_EQ(gHeapRangeCountThreshold, 0x182, "count-threshold sentinel written");
    FCK_EQ(heap_alloc_calls, 4, "table request, failing main request and two retries");
    FCK_EQ(heap_alloc_size[0], 0x814, "range table asks for 0x810 usable bytes plus header");
    FCK_EQ(heap_alloc_size[1], 0x15A004, "main block asks for 0x15A000 plus header");
    FCK_EQ(heap_alloc_size[2], 0x159004, "first retry shrinks by 0x1000");
    FCK_EQ(heap_alloc_size[3], 0x158004, "second retry shrinks again");
    FCK_EQ(gFreeRangeTablePtr, (s32)&range[1], "granted range table published past its header");
    FCK_EQ(gHeapPoolBase, (s32)&main_block[1], "granted main block published past its header");
    FCK_EQ(gHeapMainBlockSize, 0x158000, "granted size retained");
    FCK_EQ(range[0], 0x810, "usable size header in front of the range table");
    FCK_EQ(main_block[0], 0x158000, "usable size header in front of the main block");
    /* 0x814 + 0x15A004 + 0x159004 + 0x158004 from a zero high-water mark. */
    FCK_EQ(gHeapHighWater, 0x40B820, "high-water end advances by header plus size per request, including failures");
    FCK_EQ(gHeapHighWaterPeak, 0x40B820, "peak follows the monotonic high-water end");
}

/* ------------------------------------------------------------- 80017C68 */

static void test_heap_and_pools_init(void)
{
    s32 base = (s32)&arena[0];

    /* @leaf 80017C68 source src/game/80017C68.c: three range pairs, then pools 1, 3, 2 with (count, slot size, tag two) */
    LK_FUNC("func_80017C68");
    lk_fill(range_table, 0xA5, sizeof(range_table));
    gFreeRangeTablePtr = (s32)range_table;
    gHeapPoolBase = base;
    gHeapMainBlockSize = 0x2000;
    gFreeRangeCount = -1;
    gHeapSearchCursor = -1;
    gFixedPool1 = gFixedPool2 = gFixedPool3 = (void *)0xDEADBEEF;
    pool_result[0] = (void *)0x1111;
    pool_result[1] = (void *)0x2222;
    pool_result[2] = (void *)0x3333;
    pool_calls = 0;
    gInitHeapAndFixedPools(16, 4, 32, 5, 64, 6);
    FCK_EQ(gFreeRangeCount, 3, "three free ranges published");
    FCK_EQ(gHeapSearchCursor, 1, "search cursor seeded to one");
    FCK_EQ(range_table[0], base, "range zero starts at the heap base");
    FCK_EQ(range_table[1], base, "range zero ends at the heap base");
    FCK_EQ(range_table[2], base + 0x2000, "range one ends at base plus heap size");
    FCK_EQ(range_table[3], base + 0x2000, "range one starts at that same end");
    FCK_EQ(pool_calls, 3, "three fixed pools created");
    FCK_CHECK(pool_count[0] == 4 && pool_size[0] == 16 && pool_tag[0] == 2,
              "first pool built from a1/a0 with tag two");
    FCK_CHECK(pool_count[1] == 6 && pool_size[1] == 64 && pool_tag[1] == 2,
              "third pool built from a5/a4 before the second");
    FCK_CHECK(pool_count[2] == 5 && pool_size[2] == 32 && pool_tag[2] == 2,
              "second pool built last from a3/a2");
    FCK_CHECK(gFixedPool1 == (void *)0x1111 && gFixedPool3 == (void *)0x2222 &&
              gFixedPool2 == (void *)0x3333, "each creation is published in its own slot");
    FCK_EQ(range_table[4], 0xA5A5A5A5, "backed word beyond four range words untouched");
}

/* ------------------------------------------------------------- 80017D20 */

static void test_fixed_pool_free(void)
{
    /* @leaf 80017D20 source src/game/80017D20.c: release order is pool three, two, one and the slots keep their pointers */
    LK_FUNC("func_80017D20");
    reset_free();
    gFixedPool1 = (void *)0x1001;
    gFixedPool2 = (void *)0x1002;
    gFixedPool3 = (void *)0x1003;
    gFreeFixedPools();
    FCK_EQ(free_calls, 3, "all three pools released");
    FCK_CHECK(free_args[0] == (void *)0x1003 && free_args[1] == (void *)0x1002 && free_args[2] == (void *)0x1001,
              "reverse creation order");
    FCK_CHECK(gFixedPool1 == (void *)0x1001 && gFixedPool2 == (void *)0x1002 && gFixedPool3 == (void *)0x1003,
              "pool slots are not cleared");
}

/* ------------------------------------------------------------- 800198DC */

static void test_reciprocal_table_free(void)
{
    /* @leaf 800198DC source src/game/800198DC.c: null test gates the release and the surviving pointer is left dangling */
    LK_FUNC("func_800198DC");
    reset_free();
    gReciprocalTablePtr = &arena[24];
    gFreeReciprocalTable();
    FCK_EQ(free_calls, 1, "table released once when published");
    FCK_CHECK(free_args[0] == &arena[24], "exact table address released");
    FCK_CHECK(gReciprocalTablePtr == &arena[24], "table pointer intentionally left dangling");

    reset_free();
    gReciprocalTablePtr = 0;
    gFreeReciprocalTable();
    FCK_EQ(free_calls, 0, "unpublished table is not released");
}

/* ------------------------------------------------------------- 80019D00 */

static void test_resource_scan(void)
{
    struct Resource first, second, third, candidate;

    /* @leaf 80019D00 source src/game/80019D00.c: key scan over group/name and the caller's descriptor is always released */
    LK_FUNC("func_80019D00");
    lk_fill(&first, 0xA5, sizeof(first));
    lk_fill(&second, 0xA5, sizeof(second));
    lk_fill(&third, 0xA5, sizeof(third));
    lk_fill(&candidate, 0x6D, sizeof(candidate));
    first.group = 1; first.name = 2; first.next = &second;
    second.group = 3; second.name = 4; second.next = &third;
    third.group = 5; third.name = 6; third.next = 0;
    candidate.group = 3; candidate.name = 4;
    gResourceListHead = &first;
    reset_free();
    gScanAndFreeResource(&candidate);
    FCK_EQ(free_calls, 1, "matched key still releases the caller's descriptor");
    FCK_CHECK(free_args[0] == &candidate, "the passed descriptor, not the listed node, is released");
    FCK_CHECK(gResourceListHead == &first && first.next == &second && second.next == &third,
              "registry links untouched by a matching scan");

    candidate.group = 9;
    candidate.name = 8;
    reset_free();
    gScanAndFreeResource(&candidate);
    FCK_EQ(free_calls, 1, "missing key releases the descriptor too");
    FCK_CHECK(free_args[0] == &candidate, "missing key releases the same pointer");

    gResourceListHead = 0;
    reset_free();
    gScanAndFreeResource(&candidate);
    FCK_EQ(free_calls, 1, "empty registry still releases the descriptor");
    FCK_CHECK(first.group == 1 && third.name == 6, "registry contents not edited by the scan");
}

/* ------------------------------------------------------------- 8001B448 */

static void test_render_buffer_free(void)
{
    void *packets = &arena[0], *second_packets = &arena[4], *nodes = &arena[8];
    void *nodes_alt = &arena[12], *ordering = &arena[16], *ordering_alt = &arena[20];

    /* @leaf 8001B448 source src/game/8001B448.c: six render arenas released in declaration order with no store back */
    LK_FUNC("func_8001B448");
    reset_free();
    gPacketBufferBases = packets;
    gSecondPacketBufferBase = second_packets;
    gRenderNodeTable = nodes;
    gRenderNodeTableAlt = nodes_alt;
    gOrderingTableBuffers = ordering;
    gSecondOrderingTableBuffer = ordering_alt;
    gFreeRenderBuffers();
    FCK_EQ(free_calls, 6, "six render allocations released");
    FCK_CHECK(free_args[0] == packets && free_args[1] == second_packets && free_args[2] == nodes &&
              free_args[3] == nodes_alt && free_args[4] == ordering && free_args[5] == ordering_alt,
              "release order matches the six globals");
    FCK_CHECK(gPacketBufferBases == packets && gRenderNodeTable == nodes &&
              gSecondOrderingTableBuffer == ordering_alt, "all six slots keep their pointers");
}

/* ------------------------------------------------------------- 8001BD70 */

static void test_rgb_clamp(void)
{
    s16 input[6];
    s16 snapshot[6];

    /* @leaf 8001BD70 math src/game/8001BD70.c: odd elements become the byte channels and any bit above 0xFF saturates to 255 */
    LK_FUNC("func_8001BD70");
    input[0] = 0x7777; input[1] = 0x12;
    input[2] = 0x7777; input[3] = 0x34;
    input[4] = 0x7777; input[5] = 0x56;
    memcpy(snapshot, input, sizeof(input));
    rgb_calls = 0;
    FCK_EQ(gPackClampedRgb16Array(input), 0x5A5A5A5A, "return value is the packer's result");
    FCK_EQ(rgb_calls, 1, "one pack call");
    FCK_CHECK(rgb_r == 0x12 && rgb_g == 0x34 && rgb_b == 0x56,
              "elements 1, 3 and 5 are red, green and blue");
    FCK_EQ(memcmp(input, snapshot, sizeof(input)), 0, "input components never written");

    input[1] = 0x01FF; /* 511: bit 8 set */
    input[3] = -1;     /* sign-extended to 0xFFFFFFFF */
    input[5] = 0x0100; /* exactly 256 */
    gPackClampedRgb16Array(input);
    FCK_CHECK(rgb_r == 255 && rgb_g == 255 && rgb_b == 255,
              "values with any bit above the low byte clamp to 255");

    input[1] = 0x00FF; /* 255 stays */
    input[3] = 0;
    input[5] = 0x80;
    gPackClampedRgb16Array(input);
    FCK_CHECK(rgb_r == 255 && rgb_g == 0 && rgb_b == 128,
              "low-byte values pass through without saturation");

    input[1] = 0x0100;
    input[3] = 0x0001;
    input[5] = -32768;
    gPackClampedRgb16Array(input);
    FCK_CHECK(rgb_r == 255 && rgb_g == 1 && rgb_b == 255,
              "clamping is per component and independent");
}

/* ------------------------------------------------------------- 8001CDF0 */

static void test_cd_buffer_free(void)
{
    /* @leaf 8001CDF0 source src/game/8001CDF0.c: pointer forwarded to the shared free and zero always returned */
    LK_FUNC("func_8001CDF0");
    reset_free();
    FCK_EQ(gFreeDefaultCdBuffer(&arena[32]), 0, "deallocator callback returns zero");
    FCK_EQ(free_calls, 1, "one release per call");
    FCK_CHECK(free_args[0] == &arena[32], "supplied buffer forwarded unchanged");

    reset_free();
    FCK_EQ(gFreeDefaultCdBuffer(0), 0, "null buffer still reports success");
    FCK_EQ(free_calls, 1, "null buffer still reaches the free routine");
    FCK_CHECK(free_args[0] == 0, "null pointer passed through unchanged");
}

/* ------------------------------------------------------------- 8001E240 */

static void test_heap_alloc_raw(void)
{
    u32 *block = &arena[40];
    u32 *second = &arena[48];

    /* @leaf 8001E240 math src/game/8001E240.c: header word, +4 high-water advance and monotonic peak, even when the request fails */
    LK_FUNC("func_8001E240");
    lk_fill(arena, 0xA5, sizeof(arena));
    heap_alloc_calls = 0;
    lk_fill(heap_alloc_result, 0, sizeof(heap_alloc_result));
    gHeapHighWater = 100;
    gHeapHighWaterPeak = 1000;
    heap_alloc_result[0] = (s32 *)block;
    FCK_CHECK(gHeapAllocRaw(0x20) == (s32 *)&block[1], "usable pointer sits past the size header");
    FCK_EQ(heap_alloc_calls, 1, "one underlying request");
    FCK_EQ(heap_alloc_size[0], 0x24, "underlying request includes the four-byte header");
    FCK_EQ(block[0], 0x20, "header stores the usable size");
    FCK_EQ(gHeapHighWater, 136, "high water advances by header plus size");
    FCK_EQ(gHeapHighWaterPeak, 1000, "peak keeps the larger previous value");

    FCK_CHECK(gHeapAllocRaw(0) == 0, "zero-byte request returns null");
    FCK_EQ(heap_alloc_calls, 1, "zero-byte request does not reach the allocator");
    FCK_EQ(gHeapHighWater, 136, "zero-byte request leaves the high water alone");

    gHeapHighWater = 200;
    gHeapHighWaterPeak = 400;
    FCK_CHECK(gHeapAllocRaw(0x10) == 0, "failed request returns null");
    FCK_EQ(heap_alloc_calls, 2, "failed request still reaches the allocator");
    FCK_EQ(heap_alloc_size[1], 0x14, "failed request still asks for header plus size");
    FCK_EQ(gHeapHighWater, 220, "high water advances even when the block is null");
    FCK_EQ(gHeapHighWaterPeak, 400, "peak unchanged while the end stays below it");

    gHeapHighWater = 0x1000;
    gHeapHighWaterPeak = 0x1000;
    heap_alloc_result[2] = (s32 *)second;
    FCK_CHECK(gHeapAllocRaw(0x100) == (s32 *)&second[1], "next request succeeds after a failure");
    FCK_EQ(gHeapHighWater, 0x1104, "end advances past the previous peak");
    FCK_EQ(gHeapHighWaterPeak, 0x1104, "peak rises to the new end");
    FCK_EQ(second[0], 0x100, "second header stores its own size");
    FCK_EQ(arena[39], 0xA5A5A5A5U, "word before the first header untouched");
}

int main(void)
{
    test_model_renderer_buffers();
    test_light_direction();
    test_camera_list_free();
    test_camera_position();
    test_camera_rotation();
    test_camera_create();
    test_heap_shutdown();
    test_heap_init();
    test_heap_and_pools_init();
    test_fixed_pool_free();
    test_reciprocal_table_free();
    test_resource_scan();
    test_render_buffer_free();
    test_rgb_clamp();
    test_cd_buffer_free();
    test_heap_alloc_raw();
    return lk_done("leaf_math");
}

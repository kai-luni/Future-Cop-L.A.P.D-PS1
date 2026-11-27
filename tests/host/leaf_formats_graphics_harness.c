/* Original graphics transfer clients; no VRAM, GPU or DMA emulation. */
/* @mock gpu_mock */
#include "leaf_kit.h"
#include "mocks/gpu_mock.h"

struct TextureNode {
    u8 *asset;
};

void load_texture_chunks(struct TextureNode *node) __asm__("func_8005A414");
int upload_image(u16 *rect, u32 *data) __asm__("func_8009446C");

void **texture_table __asm__("D_8009F7D8");
s32 texture_count __asm__("D_8009F7DC");

enum { ATLAS_DATA = 236, ATLAS_BYTES = 0xF000 };
static u32 asset_words[(ATLAS_DATA + ATLAS_BYTES) / 4];
static struct TextureNode asset_node;
static void *descriptors[2];
static u32 tiles_a[10], tiles_b[4];
static void *alloc_result[3];
static s32 allocations, build_result;

static const char alloc_call[] = "texture allocation";
static const char build_call[] = "build sprite descriptors";
static const char cleanup_call[] = "discard texture descriptors";
static const char free_call[] = "free input node";

void *alloc_texture(s32 size, s32 tag) __asm__("func_80017EBC");
void *alloc_texture(s32 size, s32 tag)
{
    s32 index = allocations++;
    fck_record(alloc_call, size, tag, 0);
    return index < 3 ? alloc_result[index] : 0;
}

s32 build_descriptors(void **table, s32 count) __asm__("func_8005A2BC");
s32 build_descriptors(void **table, s32 count)
{
    fck_record(build_call, (s32)table, count, fcm_gpu.syncs);
    return build_result;
}

void discard_descriptors(void) __asm__("func_8005A220");
void discard_descriptors(void)
{
    fck_record(cleanup_call, (s32)texture_table, texture_count, 0);
}

void free_texture(void *block) __asm__("func_800181D4");
void free_texture(void *block)
{
    fck_record(free_call, (s32)block, 0, 0);
}

static void reset_texture(s32 count)
{
    static const u8 first[] = {0x12, 0x34, 4, 0x56};
    static const u8 second[] = {0x78, 0x9A, 1, 0xBC};
    u8 *asset = (u8 *)asset_words;

    fck_log_reset();
    fcm_gpu_reset();
    fcm_gpu.copy_bytes = 1;
    lk_fill(asset_words, 0, sizeof(asset_words));
    lk_fill(tiles_a, 0x9C, sizeof(tiles_a));
    lk_fill(tiles_b, 0x9C, sizeof(tiles_b));
    asset_words[2] = count;
    memcpy(asset + 12, first, sizeof(first));
    lk_fill(asset + 16, 0xAB, 32);
    memcpy(asset + 48, second, sizeof(second));
    lk_fill(asset + 52, 0xCD, 8);
    asset[68] = 'A';
    asset[100] = 'B';
    asset[132] = 'C';
    asset[164] = 'D';
    asset[196] = 'E';
    asset[ATLAS_DATA] = 'F';
    asset_node.asset = asset;
    texture_table = 0;
    texture_count = 0;
    descriptors[0] = descriptors[1] = 0;
    alloc_result[0] = descriptors;
    alloc_result[1] = tiles_a;
    alloc_result[2] = tiles_b;
    allocations = 0;
    build_result = 0x101;
}

static void check_texture_alloc(s32 index, s32 bytes)
{
    struct FckCall call = fck_call_n(alloc_call, index);
    FCK_EQ(call.a0, bytes, "Cpyr allocation size");
    FCK_EQ(call.a1, 2, "Cpyr allocation tag");
}

static void check_transfer(s32 index, s32 x, s32 y, s32 w, s32 h,
                           s32 offset, u8 first)
{
    const struct FcmGpuLoad *load = &fcm_gpu.load[index];

    FCK_EQ(load->rect.x, x, "Cpyr transfer x");
    FCK_EQ(load->rect.y, y, "Cpyr transfer y");
    FCK_EQ(load->rect.w, w, "Cpyr transfer width");
    FCK_EQ(load->rect.h, h, "Cpyr transfer height");
    FCK_EQ(load->data, asset_node.asset + offset, "Cpyr transfer payload pointer");
    FCK_EQ(load->copied, 1, "Cpyr records first payload byte");
    FCK_EQ(load->bytes[0], first, "Cpyr transfer payload byte");
    FCK_EQ(fcm_gpu.sync_modes[index], 0, "Cpyr synchronizes the transfer");
}

static void check_input_release(void)
{
    FCK_EQ(fck_call_n(free_call, 0).a0, &asset_node, "Cpyr releases input node");
}

static void test_texture_chunks(void)
{
    static const char *success[] = {alloc_call, alloc_call, alloc_call, build_call,
                                    free_call};
    static const char *empty[] = {cleanup_call, free_call};
    static const char *alloc_failure[] = {alloc_call, cleanup_call, free_call};
    static const char *build_failure[] = {alloc_call, alloc_call, alloc_call, build_call,
                                          cleanup_call, free_call};
    struct FckCall call;
    u8 *a = (u8 *)tiles_a;
    u8 *b = (u8 *)tiles_b;

    /* @leaf 8005A414 source src/game/memory-resources/gLoadTextureChunksToVram.c: two packed entries, strip row rollover, trailing atlas, low-byte build result and delegated cleanup/ownership */
    LK_FUNC("func_8005A414");

    reset_texture(2);
    load_texture_chunks(&asset_node);
    fck_expect_order(success, 5, "Cpyr success allocator/build/free order");
    check_input_release();
    FCK_EQ(texture_table, descriptors, "Cpyr publishes allocated table");
    FCK_EQ(texture_count, 2, "Cpyr publishes entry count");
    FCK_EQ(descriptors[0], tiles_a, "Cpyr table first entry");
    FCK_EQ(descriptors[1], tiles_b, "Cpyr table second entry");
    check_texture_alloc(0, 8);
    check_texture_alloc(1, 40);
    check_texture_alloc(2, 16);
    FCK_EQ(tiles_a[0], 0x56043412, "Cpyr header of four-tile entry");
    FCK_EQ(tiles_b[0], 0xBC019A78, "Cpyr header of one-tile entry");
    FCK_CHECK(lk_all(a, 4, 6, 0), "Cpyr first entry starts at x=0 y=0");
    FCK_EQ(b[4], 0, "Cpyr next entry returns to x=0");
    FCK_EQ(b[5], 1, "Cpyr next entry starts on row 1");
    FCK_CHECK(lk_all(a, 6, 8, 0x9C) && lk_all(b, 6, 8, 0x9C),
              "Cpyr descriptor reserved bytes are untouched");
    FCK_CHECK(lk_all(a, 8, 40, 0xAB), "Cpyr first copied descriptor body");
    FCK_CHECK(lk_all(b, 8, 16, 0xCD), "Cpyr second copied descriptor body");
    FCK_EQ(fcm_gpu.loads, 6, "Cpyr five tiles and one atlas upload");
    FCK_EQ(fcm_gpu.syncs, 6, "Cpyr sync count equals upload count");
    check_transfer(0, 320, 480, 16, 1,  68, 'A');
    check_transfer(1, 336, 480, 16, 1, 100, 'B');
    check_transfer(2, 352, 480, 16, 1, 132, 'C');
    check_transfer(3, 368, 480, 16, 1, 164, 'D');
    check_transfer(4, 320, 481, 16, 1, 196, 'E');
    check_transfer(5, 320,   0, 64, 480, ATLAS_DATA, 'F');
    call = fck_call_n(build_call, 0);
    FCK_EQ(call.a0, descriptors, "Cpyr builder receives descriptor table");
    FCK_EQ(call.a1, 2, "Cpyr builder receives entry count");
    FCK_EQ(call.a2, 6, "Cpyr builder runs after the last sync");

    reset_texture(0);
    load_texture_chunks(&asset_node);
    fck_expect_order(empty, 2, "Cpyr empty input delegates cleanup without allocating");
    FCK_EQ(fcm_gpu.loads, 0, "Cpyr empty input does not upload");
    FCK_EQ(fcm_gpu.syncs, 0, "Cpyr empty input does not synchronize");
    check_input_release();

    reset_texture(2);
    alloc_result[0] = 0;
    load_texture_chunks(&asset_node);
    fck_expect_order(alloc_failure, 3, "Cpyr table allocation failure delegates cleanup");
    check_texture_alloc(0, 8);
    FCK_EQ(fck_call_n(cleanup_call, 0).a0, 0, "Cpyr failed table remains null at cleanup");
    FCK_EQ(fcm_gpu.loads, 0, "Cpyr allocation failure does not upload");
    FCK_EQ(fcm_gpu.syncs, 0, "Cpyr allocation failure does not synchronize");
    check_input_release();

    reset_texture(2);
    build_result = 0x100;
    load_texture_chunks(&asset_node);
    fck_expect_order(build_failure, 6, "Cpyr zero low byte delegates cleanup before free");
    FCK_EQ(fck_call_n(cleanup_call, 0).a0, descriptors, "Cpyr cleanup sees live table");
    FCK_EQ(fck_call_n(cleanup_call, 0).a1, 2, "Cpyr cleanup sees descriptor count");
    FCK_EQ(fcm_gpu.loads, 6, "Cpyr builder failure comes after upload");
    check_input_release();
}

u16 max_width __asm__("D_8009DDF4");
u16 max_height __asm__("D_8009DDF6");
u32 *volatile gp0_ptr __asm__("D_8009DEC4");
volatile u32 *gp1_ptr __asm__("D_8009DEC8");
volatile u32 *dma_addr_ptr __asm__("D_8009DECC");
volatile u32 *dma_block_ptr __asm__("D_8009DED0");
volatile u32 *dma_control_ptr __asm__("D_8009DED4");

enum { GPU_READY = 0x04000000, REG_CANARY = 0x11223344 };
static u32 gp0, gp1, dma_addr, dma_block, dma_control;
static s32 armed, polls, ready_on_poll, fails_on_poll;
static union { u16 fields[4]; u32 words[2]; } rect;
static u32 pixels[35];

void arm_timeout(void) __asm__("func_800951D8");
void arm_timeout(void)
{
    ++armed;
}

int poll_timeout(void) __asm__("func_8009520C");
int poll_timeout(void)
{
    ++polls;
    if (polls == ready_on_poll)
        gp1 = GPU_READY;
    return polls == fails_on_poll;
}

static void reset_upload(u16 w, u16 h, u32 initial_gp1)
{
    s32 i;

    gp0_ptr = &gp0;
    gp1_ptr = &gp1;
    dma_addr_ptr = &dma_addr;
    dma_block_ptr = &dma_block;
    dma_control_ptr = &dma_control;
    gp0 = dma_addr = dma_block = dma_control = REG_CANARY;
    gp1 = initial_gp1;
    max_width = 35;
    max_height = 2;
    armed = polls = ready_on_poll = fails_on_poll = 0;
    rect.fields[0] = 0x123;
    rect.fields[1] = 0x056;
    rect.fields[2] = w;
    rect.fields[3] = h;
    for (i = 0; i < 35; ++i)
        pixels[i] = 0x17200000 + i;
}

static void check_upload(s32 result, u16 w, u16 h, s32 expected_polls)
{
    FCK_EQ(upload_image(rect.fields, pixels), result, "GPU upload result");
    FCK_EQ(rect.fields[0], 0x123, "GPU preserves X");
    FCK_EQ(rect.fields[1], 0x056, "GPU preserves Y");
    FCK_EQ(rect.fields[2], w, "GPU clamps signed width in place");
    FCK_EQ(rect.fields[3], h, "GPU clamps signed height in place");
    FCK_EQ(armed, 1, "GPU arms timeout once");
    FCK_EQ(polls, expected_polls, "GPU readiness poll count");
}

static void check_dma_idle(void)
{
    FCK_EQ(dma_addr, REG_CANARY, "GPU leaves DMA address untouched");
    FCK_EQ(dma_block, REG_CANARY, "GPU leaves DMA block untouched");
    FCK_EQ(dma_control, REG_CANARY, "GPU leaves DMA control untouched");
}

static void check_dma_launch(u32 *source, u32 blocks)
{
    FCK_EQ(gp1, 0x04000002, "GPU final GP1 selects CPU-to-GP0 DMA");
    FCK_EQ(dma_addr, source, "GPU DMA starts after any CPU words");
    FCK_EQ(dma_block, blocks, "GPU DMA word and block counts");
    FCK_EQ(dma_control, 0x01000201, "GPU DMA start command");
}

static void test_gpu_upload(void)
{
    /* @leaf 8009446C source src/game/gpu-2d-hud/gUploadImageToGpu.c: signed clamps, empty and busy/timeout exits, odd CPU-only words and exact/mixed DMA blocks with RAM-backed registers */
    LK_FUNC("func_8009446C");

    reset_upload(0xFFFE, 7, 0);
    check_upload(-1, 0, 2, 0);
    FCK_EQ(gp0, REG_CANARY, "GPU empty transfer does not send GP0 words");
    FCK_EQ(gp1, 0, "GPU empty transfer does not change GP1");
    check_dma_idle();

    reset_upload(3, 0xFFFF, GPU_READY);
    check_upload(-1, 3, 0, 0);
    FCK_EQ(gp0, REG_CANARY, "GPU negative height does not send GP0 words");
    check_dma_idle();

    reset_upload(32, 1, 0);
    fails_on_poll = 1;
    check_upload(-1, 32, 1, 1);
    FCK_EQ(gp0, REG_CANARY, "GPU timeout does not send GP0 words");
    FCK_EQ(gp1, 0, "GPU timeout does not change GP1");
    check_dma_idle();

    reset_upload(3, 1, 0);
    ready_on_poll = 2;
    check_upload(0, 3, 1, 2);
    FCK_EQ(gp0, 0x17200001, "GPU three pixels round up to two words");
    FCK_EQ(gp1, GPU_READY, "GPU CPU-only command leaves DMA disabled");
    check_dma_idle();

    reset_upload(32, 1, GPU_READY);
    check_upload(0, 32, 1, 0);
    FCK_EQ(gp0, 0x00010020, "GPU exact block leaves dimensions as final GP0 word");
    check_dma_launch(pixels, 0x00010010);

    reset_upload(140, 7, GPU_READY);
    check_upload(0, 35, 2, 0);
    FCK_EQ(gp0, 0x17200002, "GPU 35 words leave three CPU remainder words");
    check_dma_launch(pixels + 3, 0x00020010);
}

int main(void)
{
    test_texture_chunks();
    test_gpu_upload();
    return lk_done("leaf-formats-graphics");
}

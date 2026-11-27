/* Original media/format routines with synthetic buffers and bus stand-ins. */
#include "leaf_kit.h"
#include "mocks/bios_mock.h"

s32 read_path_table(void) __asm__("func_80090218");
void push_movie_audio(u8 *chunk) __asm__("func_80086FE4");
void build_card_frame(char *name, s32 index) __asm__("func_80020F34");

enum { ISO_SECTOR = 2048, PATH_LIMIT = 128, PATH_LBA = 0x010203,
       AUDIO_RING = 0x3C00, AUDIO_CANARY = 0xD7,
       CARD_FRAME = 0x80, CARD_OFFSET = 0x9B0 };

struct PathRow {
    s32 id, parent;
    u32 extent;
    u8 name[32];
};

/* These symbols name fields of one 44-byte-stride retail table. */
struct PathRow path_rows[PATH_LIMIT + 1] __asm__("D_800A4480");
__asm__(".globl D_800A4484\n.set D_800A4484,D_800A4480+4\n"
        ".globl D_800A4488\n.set D_800A4488,D_800A4480+8");

u32 cd_sector[(ISO_SECTOR + 12) / 4] __asm__("D_800A5A80");
s32 cd_log_level __asm__("D_8009C8B0");
s32 cd_cached_index __asm__("D_8009CBC0");
u8 pvd_read_error[] __asm__("D_80012638") = "PVD read error";
u8 iso_magic[] __asm__("D_80012664") = "CD001";
u8 iso_sig_error[] __asm__("D_8001266C") = "signature error";
u8 path_read_error[] __asm__("D_8001269C") = "path read error";
u8 path_header[] __asm__("D_800126C0") = "path table";
u8 path_entry[] __asm__("D_800126E0") = "entry";
u8 path_count[] __asm__("D_800126F4") = "count";

static u8 pvd_fixture[ISO_SECTOR], path_fixture[ISO_SECTOR];
static s32 pvd_result, path_result;
static const char read_call[] = "read sectors";
static const char compare_call[] = "compare signature";
static const char print_call[] = "CD log";

s32 read_cd_sectors(s32 count, s32 lba, u8 *dst) __asm__("func_8009081C");
s32 read_cd_sectors(s32 count, s32 lba, u8 *dst)
{
    fck_record(read_call, count, lba, (s32)dst);
    if (count != 1 || dst != (u8 *)cd_sector)
        return 0;
    if (lba == 0x10) {
        if (pvd_result == 1)
            memcpy(dst, pvd_fixture, ISO_SECTOR);
        return pvd_result;
    }
    if (lba == PATH_LBA) {
        if (path_result == 1)
            memcpy(dst, path_fixture, ISO_SECTOR);
        return path_result;
    }
    return 0;
}

s32 compare_cd_signature(u8 *a, u8 *b, s32 n) __asm__("func_80090890");
s32 compare_cd_signature(u8 *a, u8 *b, s32 n)
{
    fck_record(compare_call, (s32)a, (s32)b, n);
    while (n > 0 && *a == *b && *a != 0) {
        ++a;
        ++b;
        --n;
    }
    return n > 0 ? (s32)*a - (s32)*b : 0;
}

void cd_print(u8 *format, ...) __asm__("func_8008E000");
void cd_print(u8 *format, ...)
{
    fck_record(print_call, (s32)format, 0, 0);
}

static void reset_iso(void)
{
    fck_log_reset();
    fcm_bios_reset();
    lk_fill(pvd_fixture, 0, sizeof(pvd_fixture));
    lk_fill(path_fixture, 0, sizeof(path_fixture));
    lk_fill(cd_sector, 0xAB, sizeof(cd_sector));
    /* A valid-looking record just past the sector must never be parsed. */
    memcpy((u8 *)cd_sector + ISO_SECTOR, "\x02\x00\x01\x00\x00\x00\x01\x00" "ZZ", 10);
    lk_fill(path_rows, 0xA5, sizeof(path_rows));
    pvd_fixture[0] = 1;
    memcpy(pvd_fixture + 1, "CD001", 5);
    memcpy(pvd_fixture + 0x8C, "\x03\x02\x01\x00", 4);
    pvd_result = path_result = 1;
    cd_cached_index = 37;
    cd_log_level = 1;
}

static void check_iso_calls(s32 reads, s32 compares, u8 *error)
{
    struct FckCall call;

    FCK_EQ(fck_calls_to(read_call), reads, "CD read call count");
    call = fck_call_n(read_call, 0);
    FCK_EQ(call.a0, 1, "CD PVD read count");
    FCK_EQ(call.a1, 0x10, "CD PVD location");
    FCK_EQ(call.a2, cd_sector, "CD PVD destination");
    if (reads == 2) {
        call = fck_call_n(read_call, 1);
        FCK_EQ(call.a0, 1, "CD path read count");
        FCK_EQ(call.a1, PATH_LBA, "CD path location decoded from PVD");
        FCK_EQ(call.a2, cd_sector, "CD path destination");
    }
    FCK_EQ(fck_calls_to(compare_call), compares, "CD signature compare count");
    if (compares) {
        call = fck_call_n(compare_call, 0);
        FCK_EQ(call.a0, (u8 *)cd_sector + 1, "CD signature location");
        FCK_EQ(call.a1, iso_magic, "CD signature reference");
        FCK_EQ(call.a2, 5, "CD signature compare length");
    }
    FCK_EQ(fck_calls_to(print_call), error != 0, "CD level-one diagnostics count");
    if (error)
        FCK_EQ(fck_call_n(print_call, 0).a0, error, "CD failure diagnostic");
}

static void check_iso_failure(s32 reads, s32 compares, u8 *error)
{
    FCK_EQ(read_path_table(), 0, "CD failure result");
    check_iso_calls(reads, compares, error);
    FCK_EQ(cd_cached_index, 37, "CD error retains cached directory");
    FCK_CHECK(lk_all(path_rows, 0, sizeof(path_rows), 0xA5),
              "CD error does not write path rows");
    FCK_EQ(fcm_bios.calls, 0, "CD error does not copy path names");
}

static void check_path_row(s32 index, s32 parent, u32 extent,
                           const void *name, s32 length)
{
    const struct PathRow *row = &path_rows[index];

    FCK_EQ(row->id, index + 1, "CD one-based directory ID");
    FCK_EQ(row->parent, parent, "CD parent ID");
    FCK_EQ(row->extent, extent, "CD little-endian extent");
    FCK_EQ(memcmp(row->name, name, length), 0, "CD name bytes");
    FCK_EQ(row->name[length], 0, "CD name terminator");
    FCK_CHECK(lk_all(row->name, length + 1, sizeof(row->name), 0xA5),
              "CD name copy does not touch the row tail");
}

static void check_path_end(s32 index)
{
    FCK_EQ(path_rows[index].parent, 0, "CD marks the first unused parent slot");
    FCK_EQ(path_rows[index].id, 0xA5A5A5A5, "CD leaves the unused ID intact");
    FCK_CHECK(lk_all(&path_rows[index], 8,
                     sizeof(path_rows) - index * sizeof(struct PathRow), 0xA5),
              "CD keeps the rest of the unused rows intact");
}

static void test_iso_path_table(void)
{
    static const u8 records[] = {
        1, 0, 0x78, 0x56, 0x34, 0x12, 1, 0, 0, 0xE1,
        3, 0, 0xBC, 0x9A, 0x78, 0x56, 1, 0, 'S', 'U', 'B', 0xE2,
        4, 0, 0xEF, 0xCD, 0xAB, 0x09, 2, 0, 'D', 'E', 'E', 'P',
        0
    };
    static const u8 short_record[] = {2, 0, 3, 2, 1, 0, 1, 0, 'A', 'B'};
    static const u8 sector_end_record[32] = {
        24, 0, 9, 0, 0, 0, 1, 0,
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'
    };
    s32 i;

    /* @leaf 80090218 source src/game/80090218.c: PVD/signature/path read failures, odd/even records, empty table, sector end and 128-row cap on synthetic sectors */
    LK_FUNC("func_80090218");

    reset_iso();
    pvd_result = 0;
    check_iso_failure(1, 0, pvd_read_error);

    reset_iso();
    pvd_fixture[5] = '2';
    check_iso_failure(1, 1, iso_sig_error);

    reset_iso();
    path_result = 0;
    check_iso_failure(2, 1, path_read_error);

    reset_iso();
    memcpy(path_fixture, records, sizeof(records));
    FCK_EQ(read_path_table(), 1, "CD parsed path table");
    check_iso_calls(2, 1, 0);
    FCK_EQ(fcm_bios.calls, 3, "CD copies exactly three names");
    check_path_row(0, 1, 0x12345678, "\0", 1);
    check_path_row(1, 1, 0x56789ABC, "SUB", 3);
    check_path_row(2, 2, 0x09ABCDEF, "DEEP", 4);
    check_path_end(3);
    FCK_EQ(cd_cached_index, 0, "CD success invalidates cached directory");

    reset_iso();
    FCK_EQ(read_path_table(), 1, "CD accepts immediate record terminator");
    FCK_EQ(fcm_bios.calls, 0, "CD empty table copies no names");
    check_path_end(0);
    FCK_EQ(cd_cached_index, 0, "CD empty success invalidates cache");

    reset_iso();
    for (i = 0; i < 64; ++i)
        memcpy(path_fixture + i * 32, sector_end_record, 32);
    FCK_EQ(read_path_table(), 1, "CD accepts a record ending at the sector boundary");
    FCK_EQ(fcm_bios.calls, 64, "CD does not scan beyond the sector");
    check_path_row(63, 1, 9, "ABCDEFGHIJKLMNOPQRSTUVWX", 24);
    check_path_end(64);

    reset_iso();
    for (i = 0; i < PATH_LIMIT + 1; ++i)
        memcpy(path_fixture + i * 10, short_record, 10);
    FCK_EQ(read_path_table(), 1, "CD accepts a full path table");
    FCK_EQ(fcm_bios.calls, PATH_LIMIT, "CD ignores a 129th record inside the sector");
    check_path_row(PATH_LIMIT - 1, 1, 0x010203, "AB", 2);
    FCK_CHECK(lk_all(&path_rows[PATH_LIMIT], 0, sizeof(struct PathRow), 0xA5),
              "CD full table writes no sentinel or data past the 128th row");
}

struct AudioState {
    u32 read_cursor, write_cursor;
    u8 unused[8];
    u8 left[AUDIO_RING], right[AUDIO_RING];
    u32 guard;
};

static struct AudioState audio_state;
struct AudioState *movie_audio_state __asm__("D_8009F4D8");

static void run_audio_chunk(const u8 *chunk, s32 bytes, u32 write, u32 read)
{
    union { u32 words[8]; u8 bytes[32]; } input;

    lk_fill(&audio_state, AUDIO_CANARY, sizeof(audio_state));
    movie_audio_state = &audio_state;
    audio_state.read_cursor = read;
    audio_state.write_cursor = write;
    lk_fill(&input, 0xFF, sizeof(input));
    memcpy(input.bytes, chunk, bytes);
    fcm_bios_reset();
    push_movie_audio(input.bytes);
    FCK_EQ(memcmp(input.bytes, chunk, bytes), 0, "WVE source chunk is unchanged");
    FCK_EQ(audio_state.read_cursor, read, "WVE preserves consumer cursor");
    FCK_CHECK(lk_all(audio_state.unused, 0, 8, AUDIO_CANARY), "WVE header guard");
    FCK_EQ(audio_state.guard, 0xD7D7D7D7, "WVE post-ring guard");
}

static void check_ring(const u8 *ring, s32 offset, const char *tail, s32 tail_length,
                       const char *head, s32 head_length)
{
    FCK_EQ(memcmp(ring + offset, tail, tail_length), 0, "WVE bytes at write position");
    FCK_EQ(memcmp(ring, head, head_length), 0, "WVE wrapped bytes at ring head");
    FCK_CHECK(lk_all(ring, head_length, offset, AUDIO_CANARY),
              "WVE leaves bytes before write position unchanged");
    FCK_CHECK(lk_all(ring, offset + tail_length, AUDIO_RING, AUDIO_CANARY),
              "WVE leaves bytes after write unchanged");
}

static void test_movie_audio(void)
{
    static const u8 short_mono[] = "Ad10\x00\x00\x00\x10" "xxxx" "abcd";
    static const u8 long_mono[] = "ad10\x00\x00\x00\x14" "xxxxxxxx" "wxyz";
    static const u8 stereo[] = "ad2X\x00\x00\x00\x18" "xxxxxxxx" "ABCD" "1234";
    static const u8 empty[] = "ad10\x00\x00\x00\x10" "xxxxxxxx";

    /* @leaf 80086FE4 source src/game/80086FE4.c: short/long mono, split stereo, exact ring tail, wrap, zero payload and producer cursor on an already-ready consumer fixture */
    LK_FUNC("func_80086FE4");

    run_audio_chunk(short_mono, sizeof(short_mono) - 1, 7, 3);
    FCK_EQ(audio_state.write_cursor, 11, "WVE short mono advances four bytes");
    FCK_EQ(fcm_bios.calls, 2, "WVE contiguous mono issues two copies");
    check_ring(audio_state.left, 7, "abcd", 4, "", 0);
    check_ring(audio_state.right, 7, "abcd", 4, "", 0);

    run_audio_chunk(long_mono, sizeof(long_mono) - 1, AUDIO_RING - 4, 0);
    FCK_EQ(audio_state.write_cursor, AUDIO_RING, "WVE exact fit advances to ring size");
    FCK_EQ(fcm_bios.calls, 2, "WVE exact fit has no wrap copy");
    check_ring(audio_state.left, AUDIO_RING - 4, "wxyz", 4, "", 0);
    check_ring(audio_state.right, AUDIO_RING - 4, "wxyz", 4, "", 0);

    run_audio_chunk(long_mono, sizeof(long_mono) - 1, AUDIO_RING - 1, 3);
    FCK_EQ(audio_state.write_cursor, AUDIO_RING + 3, "WVE mono wraps stored data, not cursor");
    FCK_EQ(fcm_bios.calls, 4, "WVE wrapped mono copies two pieces per channel");
    check_ring(audio_state.left, AUDIO_RING - 1, "w", 1, "xyz", 3);
    check_ring(audio_state.right, AUDIO_RING - 1, "w", 1, "xyz", 3);

    run_audio_chunk(stereo, sizeof(stereo) - 1, AUDIO_RING + 9, 20);
    FCK_EQ(audio_state.write_cursor, AUDIO_RING + 13, "WVE stereo advances per channel");
    FCK_EQ(fcm_bios.calls, 2, "WVE contiguous stereo issues two copies");
    check_ring(audio_state.left, 9, "ABCD", 4, "", 0);
    check_ring(audio_state.right, 9, "1234", 4, "", 0);

    run_audio_chunk(stereo, sizeof(stereo) - 1, AUDIO_RING - 2, 2);
    FCK_EQ(audio_state.write_cursor, AUDIO_RING + 2, "WVE split stereo wraps both channels");
    FCK_EQ(fcm_bios.calls, 4, "WVE wrapped stereo copies two pieces per channel");
    check_ring(audio_state.left, AUDIO_RING - 2, "AB", 2, "CD", 2);
    check_ring(audio_state.right, AUDIO_RING - 2, "12", 2, "34", 2);

    run_audio_chunk(empty, sizeof(empty) - 1, AUDIO_RING, 0);
    FCK_EQ(audio_state.write_cursor, AUDIO_RING, "WVE empty payload leaves cursor");
    FCK_CHECK(lk_all(audio_state.left, 0, AUDIO_RING, AUDIO_CANARY), "WVE empty left");
    FCK_CHECK(lk_all(audio_state.right, 0, AUDIO_RING, AUDIO_CANARY), "WVE empty right");
}

static u32 card_storage[(CARD_OFFSET + 17 * CARD_FRAME) / 4];
u8 *card_operation __asm__("D_8009AB08");

void *card_fill(void *dst, int value, unsigned int bytes) __asm__("func_8001E4C0");
void *card_fill(void *dst, int value, unsigned int bytes)
{
    return memset(dst, value, bytes);
}

u8 *card_copy(u8 *dst, u8 *src, u32 bytes) __asm__("func_8001E318");
u8 *card_copy(u8 *dst, u8 *src, u32 bytes)
{
    return memcpy(dst, src, bytes);
}

static void check_card_frame(char *name, s32 index, s32 name_bytes, u8 checksum)
{
    static const u8 prefix[] = {0x51, 0, 0, 0, 0, 0x20, 0, 0, 0xFF, 0xFF};
    s32 start = CARD_OFFSET + index * CARD_FRAME;
    u8 *frame = (u8 *)card_storage + start;

    card_operation = (u8 *)card_storage;
    lk_fill(card_storage, 0xA7, sizeof(card_storage));
    fcm_bios_reset();

    build_card_frame(name, index);

    FCK_EQ(memcmp(frame, prefix, sizeof(prefix)), 0, "card type, size and unlinked marker");
    FCK_EQ(memcmp(frame + 0xA, name, name_bytes), 0, "card filename bytes");
    FCK_CHECK(lk_all(frame, 0xA + name_bytes, 0x7F, 0), "card clears unused frame bytes");
    FCK_EQ(frame[0x7F], checksum, "card directory XOR checksum");
    FCK_CHECK(lk_all(card_storage, 0, start, 0xA7), "card preserves earlier bytes");
    FCK_CHECK(lk_all(card_storage, start + CARD_FRAME, sizeof(card_storage), 0xA7),
              "card preserves later bytes");
}

static void test_card_directory(void)
{
    /* @leaf 80020F34 source src/game/80020F34.c: selected 128-byte frame, little-endian fields, empty and 20-byte names, zero fill, exact XOR and neighboring canaries */
    LK_FUNC("func_80020F34");

    check_card_frame("", 1, 0, 0x71);
    check_card_frame("SAVE-0123456789ABCDE", 15, 20, 0x1D);
}

int main(void)
{
    test_iso_path_table();
    test_movie_audio();
    test_card_directory();
    return lk_done("leaf-formats");
}

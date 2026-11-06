typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u8 *D_8009AB08;
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_80020B28(void *records);
extern s32 func_8001F180(void);

int func_80020B64(void)
{
    u8 *records;
    u8 *record;
    u8 *end;
    u8 *p;
    u32 off;
    u32 i;
    u32 checksum;
    u32 b;
    u32 tt;
    u8 *g;
    u8 *base;
    u8 *first;
    register u32 h1 __asm__("$13");
    register u32 h2 __asm__("$12");
    register u32 mk __asm__("$11");
    register u32 ff __asm__("$10");

    base = D_8009AB08;
    records = base + 0x9B0;
    func_8001E4C0(records, 0, 0x1200);
    h1 = 0x4D;
    h2 = 0x43;
    mk = 0xA0;
    ff = 0xFFFF;
    __asm__ volatile("" : "=r"(h1), "=r"(h2), "=r"(mk), "=r"(ff) : "0"(h1), "1"(h2), "2"(mk), "3"(ff));
    off = 0;
    record = records;
    first = records;
    end = base + 0x11B0;
    do {
        if (record == first) {
            *(volatile u8 *)(D_8009AB08 + 0x9B0) = h1;
            *(volatile u8 *)(D_8009AB08 + 0x9B1) = h2;
        } else {
            record[0] = mk;
            *(u16 *)(record + 8) = ff;
        }
        checksum = 0;
        __asm__("" : "=r"(checksum) : "0"(checksum));
        p = records + off;
        for (i = 0; i < 0x7F; i++) {
            b = *p++;
            tt = checksum ^ b;
            checksum = tt;
        }
        *p = tt;
        off += 0x80;
        record += 0x80;
    } while (record < end);
    func_80020B28(records);
    if (func_8001F180() != 0) {
        return 1;
    }
    g = D_8009AB08;
    *(s32 *)(g + 0x998) = 1;
    *(s32 *)(g + 0x99C) = 0x23;
    *(s32 *)(g + 0x910) = (s32)(g + 0xA30);
    *(s32 *)(g + 0x90C) = 0;
    *(s32 *)(g + 0x994) = 0;
    *(s32 *)(g + 0x9A4) = 0;
    *(s32 *)(g + 0x1BB4) = 0;
    *(s32 *)(g + 0x8F8) = 5;
    return 0;
}

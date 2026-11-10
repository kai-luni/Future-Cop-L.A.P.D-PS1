typedef int s32;
typedef unsigned char u8;

struct Ctx {
    u8 pad000[0x8F4];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    u8 pad908[0x9A4 - 0x908];
    s32 unk9A4;
    s32 unk9A8;
    u8 pad9AC[0x1BB0 - 0x9AC];
    s32 unk1BB0;
};

extern volatile struct Ctx *D_8009AB08;
extern void func_8008D640(s32);

s32 func_8001FE40(void)
{
    register volatile struct Ctx *ctx __asm__("$2");
    D_8009AB08->unk1BB0 = 0;
    D_8009AB08->unk9A4 = 1;
    switch (D_8009AB08->unk8F4) {
    case 2:
    case 3:
    case 6:
        ctx = D_8009AB08;
        ctx->unk900 = -3;
        goto reset;
    case 0:
        if (D_8009AB08->unk9A8 > 0) {
            D_8009AB08->unk9A8--;
        } else {
            D_8009AB08->unk900 = -3;
            D_8009AB08->unk8F8 = 0;
            D_8009AB08->unk8F4 = 0;
            D_8009AB08->unk904 = 0;
            D_8009AB08->unk9A8 = 1;
        }
        ctx = D_8009AB08;
        break;
    default:
        D_8009AB08->unk900 = -3;
    case 1:
        ctx = D_8009AB08;
    reset:
        ctx->unk8F8 = 0;
        ctx->unk8F4 = 0;
        ctx->unk904 = 0;
        break;
    }
    func_8008D640(ctx->unk8FC);
    return 0;
}

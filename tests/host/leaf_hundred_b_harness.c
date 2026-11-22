/* Characterization of unchanged MIPS functions using synthetic fixtures. */
#include "leaf_kit.h"

unsigned int gGetNextPrim(unsigned int *value) __asm__("func_80092324");
void gSetPolyFT3Packet(u8 *arg0) __asm__("func_80092474");
struct PolyFlat { u8 padding0[3]; u8 length; u8 padding4[3]; u8 code; u8 padding8[12]; u32 end; };
struct PolyGouraud { u8 padding0[3]; u8 length; u8 padding4[3]; u8 code; u8 padding8[28]; u32 end; };
void gInitFlatThreePointPolylinePacket(struct PolyFlat *state) __asm__("func_800925B4");
void gInitGouraudFourPointPolylinePacket(struct PolyGouraud *state) __asm__("func_80092614");
int gGetFontBitsPerPixel(unsigned char *value) __asm__("func_800489EC");
void gComputeQuadAverages(u32 a0, u32 a1, u32 a2, u32 a3, u32 *arg) __asm__("func_8004F00C");
struct VertexIn { s16 x, y, z; s8 i0, i1; };
struct VertexOut { s16 a, b, c, pad; };
void gScaleVertexStream(struct VertexOut *dst, s16 *tbl, s16 count, struct VertexIn *src, s16 scale) __asm__("func_8006CAAC");

static void test_next_prim(void)
{
    unsigned int v;
    /* @leaf 80092324 source low 24 bits retained, bit 31 set; input unchanged */
    LK_FUNC("func_80092324");
    v = 0; FCK_EQ(gGetNextPrim(&v), 0x80000000U, "zero pointer field"); FCK_EQ(v, 0, "zero unchanged");
    v = 0x7f123456U; FCK_EQ(gGetNextPrim(&v), 0x80123456U, "high byte replaced"); FCK_EQ(v, 0x7f123456U, "word unchanged");
    v = 0xffffffffU; FCK_EQ(gGetNextPrim(&v), 0x80ffffffU, "max address field");
}

static void test_packets(void)
{
    u8 packet[16];
    struct PolyFlat flat, want_flat;
    struct PolyGouraud smooth, want_smooth;
    /* @leaf 80092474 source packet byte constants and preservation of surrounding bytes */
    LK_FUNC("func_80092474");
    lk_fill(packet, 0xa5, sizeof(packet)); gSetPolyFT3Packet(packet);
    FCK_EQ(packet[3], 7, "packet length"); FCK_EQ(packet[7], 36, "packet command");
    FCK_CHECK(lk_all(packet,0,3,0xa5) && lk_all(packet,4,7,0xa5) && lk_all(packet,8,16,0xa5), "other bytes unchanged");
    /* @leaf 800925B4 source flat polyline length, opcode and terminator; other bytes retained */
    LK_FUNC("func_800925B4");
    lk_fill(&flat, 0xa5, sizeof(flat)); memcpy(&want_flat, &flat, sizeof(flat));
    want_flat.length=5; want_flat.code=0x48; want_flat.end=0x55555555U;
    gInitFlatThreePointPolylinePacket(&flat);
    FCK_CHECK(memcmp(&flat,&want_flat,sizeof(flat)) == 0, "flat packet complete representation");
    /* @leaf 80092614 source gouraud polyline length, opcode and terminator; other bytes retained */
    LK_FUNC("func_80092614");
    lk_fill(&smooth, 0x5a, sizeof(smooth)); memcpy(&want_smooth, &smooth, sizeof(smooth));
    want_smooth.length=9; want_smooth.code=0x5c; want_smooth.end=0x55555555U;
    gInitGouraudFourPointPolylinePacket(&smooth);
    FCK_CHECK(memcmp(&smooth,&want_smooth,sizeof(smooth)) == 0, "gouraud packet complete representation");
}

static void test_font(void)
{
    u8 value;
    /* @leaf 800489EC source format 0x44 selects 1, all others select 4 */
    LK_FUNC("func_800489EC");
    value=0x44; FCK_EQ(gGetFontBitsPerPixel(&value),1,"format 44"); FCK_EQ(value,0x44,"format preserved");
    value=0x40; FCK_EQ(gGetFontBitsPerPixel(&value),4,"format 40");
    value=0x43; FCK_EQ(gGetFontBitsPerPixel(&value),4,"below 44");
    value=0x45; FCK_EQ(gGetFontBitsPerPixel(&value),4,"above 44");
    value=0xff; FCK_EQ(gGetFontBitsPerPixel(&value),4,"unknown format");
}

static void test_quad(void)
{
    u32 out[6];
    /* @leaf 8004F00C source rounded averages and unsigned transform with guarded output */
    LK_FUNC("func_8004F00C");
    lk_fill(out,0xa5,sizeof(out));
    gComputeQuadAverages(0, 16, 32, 48, out+1);
    FCK_EQ(out[0],0xa5a5a5a5U,"leading guard"); FCK_EQ(out[5],0xa5a5a5a5U,"trailing guard");
    FCK_EQ(out[1],16,"zero"); FCK_EQ(out[2],16,"rounded pair 8"); FCK_EQ(out[3],15,"pair 16"); FCK_EQ(out[4],15,"four average 24");
    gComputeQuadAverages(0xffffffffU,0xffffffffU,0xffffffffU,0xffffffffU,out+1);
    FCK_EQ(out[1],16,"wrapped first"); FCK_EQ(out[2],15,"wrapped pair"); FCK_EQ(out[3],15,"wrapped other pair"); FCK_EQ(out[4],15,"wrapped total");
}

static void test_vertices(void)
{
    struct VertexIn src[2] = { { 8,-9,20,2,0 }, { -13,3,-21,0,3 } };
    s16 tbl[12] = { 2048,4096,8192,77, 4096,2048,13,8192, 4096,4096,4096,4096 };
    struct VertexOut dst[5];
    /* @leaf 8006CAAC source Q12 selected scaling, shifted destination and repeated input */
    LK_FUNC("func_8006CAAC");
    lk_fill(dst,0xa5,sizeof(dst));
    gScaleVertexStream(dst,tbl,2,src,1);
    FCK_CHECK(lk_all(dst,0,16,0xa5),"offset leaves first two vertices untouched");
    FCK_EQ(dst[2].a,16,"selected x scale"); FCK_EQ(dst[2].b,-9,"selected y scale"); FCK_EQ(dst[2].c,10,"selected z scale");
    FCK_EQ(dst[3].a,0,"table advances x"); FCK_EQ(dst[3].b,-5,"negative fractional y rounds down"); FCK_EQ(dst[3].c,20,"same source z with next table");
    FCK_EQ((u16)dst[2].pad,0xa5a5,"first padding untouched"); FCK_EQ((u16)dst[3].pad,0xa5a5,"second padding untouched");
    FCK_CHECK(lk_all(&dst[4],0,sizeof(dst[4]),0xa5),"following vertex untouched");
    lk_fill(dst,0x5a,sizeof(dst)); gScaleVertexStream(dst,tbl,0,src,1);
    FCK_CHECK(lk_all(dst,0,sizeof(dst),0x5a),"zero count leaves destination untouched");
    gScaleVertexStream(dst,tbl,-1,src,1);
    FCK_CHECK(lk_all(dst,0,sizeof(dst),0x5a),"negative count leaves destination untouched");
}

int main(void)
{
    test_next_prim(); test_packets(); test_font(); test_quad(); test_vertices();
    return lk_done("leaf_hundred_b");
}

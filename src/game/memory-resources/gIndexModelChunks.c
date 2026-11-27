// Categories: Resources > Chunk Parsing
typedef unsigned int u32;
typedef unsigned char u8;
extern u32 gModelTriangleListSize __asm__("D_8009F528");
extern u8 *gModelBoundingBox __asm__("D_8009F540");
extern u8 *gModelMeshInfo __asm__("D_8009F53C");
extern u8 *gModelHierarchy __asm__("D_8009F538");
extern u8 *gModelTriangleList __asm__("D_8009F534");
extern u8 *gModelPolygonList __asm__("D_8009F530");
extern u8 *gModelGeometryChunk __asm__("D_8009F52C");
extern u8 *gModel3dhsPayload __asm__("D_8009F548");
extern u8 *gModelAnimTrackChunk __asm__("D_8009F54C");
extern u8 *gModelAnimWaveChunk __asm__("D_8009F550");
extern u8 *gModelAnimData __asm__("D_8009F544");
extern u8 **gRefBlockTable __asm__("D_8009F560");
extern u8 **gRelocBlockTable __asm__("D_8009F55C");
extern u8 **gNormalBlockTable __asm__("D_8009F558");
extern u8 **gVertexBlockTable __asm__("D_8009F554");

void gIndexModelChunks(u8 *base, u32 size) __asm__("func_80012E48");

/**
 * @brief Indexes model chunks by tag within a size-delimited buffer.
 *
 * @param base Start of the chunk buffer.
 * @param size Number of bytes in the chunk buffer.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the tag, length, and index at offsets 0, 4, and 8. The index is scaled
 * by four for table writes. The animation tag 0x416E6D44 replaces its index
 * with (length - 12) >> 4. Each iteration advances by the chunk length.
 *
 * @par Globals
 * gVertexBlockTable, gNormalBlockTable, gRelocBlockTable, and gRefBlockTable
 * supply indexed destinations. gModelGeometryChunk, gModelBoundingBox,
 * gModelPolygonList, gModelTriangleList, gModelTriangleListSize,
 * gModelHierarchy, and gModelMeshInfo receive chunk locations or lengths.
 * gModelAnimData, gModel3dhsPayload, gModelAnimTrackChunk, and
 * gModelAnimWaveChunk are cleared and then set if present.
 *
 * @par Callers
 * Direct calls in the retail listing are from gRelocateModelResource.
 *
 * @par Role and path
 * The model loader collects pointers for the later in-place relocation pass.
 *
 * @par Unknown
 * The tag alone gives no format for the 3DHS payload.
 */
void gIndexModelChunks(u8 *base, u32 size)
{
    u8 *p = base;
    u32 tag, length, index;
    u8 *data;
    gModelAnimData = 0;
    gModel3dhsPayload = 0;
    gModelAnimTrackChunk = 0;
    gModelAnimWaveChunk = 0;
    if (size) do {
        tag = *(u32 *)p;
        length = *(u32 *)(p + 4);
        index = *(u32 *)(p + 8);
        data = p + 12;
        switch (tag) {
        case 0x3444564c: gVertexBlockTable[index] = data; break;
        case 0x34444e4c: gNormalBlockTable[index] = data; break;
        case 0x3344524c: gRelocBlockTable[index] = data; break;
        case 0x33445246: gRefBlockTable[index] = data; break;
        case 0x33444242: gModelBoundingBox = p + 8; break;
        case 0x34444749: gModelGeometryChunk = p; break;
        case 0x3344514c: gModelPolygonList = data; break;
        case 0x3344544c: gModelTriangleList = data; gModelTriangleListSize = length - 12; break;
        case 0x33444859: gModelHierarchy = data; break;
        case 0x33444d49: gModelMeshInfo = data; break;
        case 0x416e6d44: *(u32 *)(p + 8) = (length - 12) >> 4; gModelAnimData = p + 8; break;
        case 0x33444853: gModel3dhsPayload = p + 8; break;
        case 0x33445441: gModelAnimTrackChunk = p; break;
        case 0x3344414c: gModelAnimWaveChunk = p; break;
        }
        p += length;
    } while ((u32)(p - base) < size);
}

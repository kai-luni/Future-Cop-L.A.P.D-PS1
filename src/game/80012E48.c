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
extern u8 *D_8009F548, *D_8009F54C, *D_8009F550;
extern u8 *gModelAnimData __asm__("D_8009F544");
extern u8 **D_8009F55C, **D_8009F560;
extern u8 **gNormalBlockTable __asm__("D_8009F558");
extern u8 **gVertexBlockTable __asm__("D_8009F554");

/**
 *  @brief Indexes recognized chunks in a length-delimited resource buffer.
 *
 *  Each chunk supplies a tag, length, index, and data beginning at offset 12.
 *  Recognized tags populate global pointers or indexed pointer tables.
 *  @param base Start of the chunk buffer.
 *  @param size Number of bytes defining the traversal boundary.
 *  @par Globals
 *  D_8009F554, D_8009F558, D_8009F55C, and D_8009F560 are indexed tables.
 *  D_8009F52C through D_8009F550 receive chunk pointers; D_8009F528 receives a data length.
 *  @par Control flow
 *  Clears four pointers, then advances by each chunk length until size is reached.
 *  A tag switch selects the destination; unrecognized tags are skipped.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates global pointers and rewrites the index word of tag 0x416e6d44 as a count.
 *  @par Callers
 *  Confirmed: func_800131A0.
 */
void func_80012E48(u8 *base, u32 size)
{
    u8 *p = base;
    u32 tag, length, index;
    u8 *data;
    gModelAnimData = 0;
    D_8009F548 = 0;
    D_8009F54C = 0;
    D_8009F550 = 0;
    if (size) do {
        tag = *(u32 *)p;
        length = *(u32 *)(p + 4);
        index = *(u32 *)(p + 8);
        data = p + 12;
        switch (tag) {
        case 0x3444564c: gVertexBlockTable[index] = data; break;
        case 0x34444e4c: gNormalBlockTable[index] = data; break;
        case 0x3344524c: D_8009F55C[index] = data; break;
        case 0x33445246: D_8009F560[index] = data; break;
        case 0x33444242: gModelBoundingBox = p + 8; break;
        case 0x34444749: gModelGeometryChunk = p; break;
        case 0x3344514c: gModelPolygonList = data; break;
        case 0x3344544c: gModelTriangleList = data; gModelTriangleListSize = length - 12; break;
        case 0x33444859: gModelHierarchy = data; break;
        case 0x33444d49: gModelMeshInfo = data; break;
        case 0x416e6d44: *(u32 *)(p + 8) = (length - 12) >> 4; gModelAnimData = p + 8; break;
        case 0x33444853: D_8009F548 = p + 8; break;
        case 0x33445441: D_8009F54C = p; break;
        case 0x3344414c: D_8009F550 = p; break;
        }
        p += length;
    } while ((u32)(p - base) < size);
}

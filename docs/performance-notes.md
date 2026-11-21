# Future Cop: L.A.P.D. (SLUS_00739) - performance techniques, code-evidenced

Scope: what the reconstructed retail code in src/game (and asm/us/main.s) actually does to stay fast on PS1 hardware. Status: confirmed = implemented and cited; likely = mechanism cited, one link inferred; hypothesis = not shown in code, missing piece named.
Every `src/game/<unit>.c:<line>` citation is listed with its exact code fragment in the citation index at the end; `tests/test_performance_notes.py` re-checks that each fragment is still on its cited line.

## Index

1. Adaptive depth-bucket sort of all primitives into the ordering table at frame end - confirmed - 7 units
2. Double-buffered packet pools and OT with a VBlank-gated flip - confirmed - 4 units
3. Bump-pointer primitive pools with saturating last-slot reuse - confirmed - 5 units
4. GTE batch vertex transforms with hand-scheduled (software-pipelined) loops - confirmed - 8 units
5. Single-vertex RTPS projection with per-vertex clip codes and one-test batch reject - confirmed - 6 units
6. Cheap backface culling: GTE NCLIP for terrain, integer 2D cross product for models - confirmed - 4 units
7. Hierarchical view-volume culling of the 5x5 map-cell window into 16-bit subcell masks - confirmed - 10 units
8. Per-instance bounding-sphere view-cone test before any vertex work - confirmed - 6 units
9. Multiply-free octagonal distance approximations and a fixed activity radius - confirmed - 5 units
10. Proximity streaming of deferred object spawns - confirmed - 3 units
11. Per-cell bucket grid for map models (spatial hashing by cell index) - confirmed - 3 units
12. Scratchpad (1 KB on-chip RAM) as stack for hot passes and as fast table storage - confirmed - 10 units
13. Table-driven trigonometry: quarter-wave sine table, 4096-step angles, table atan2 and sqrt - confirmed - 5 units
14. Integer fixed-point math everywhere (16.16 and 4.12), no FPU and no soft-float helpers - confirmed - 4 units
15. Division-free vector normalisation with the GTE leading-zero counter and an inverse-sqrt table - confirmed - 4 units
16. Lighting split: terrain colours baked into map data, models lit by GTE NCCS - confirmed - 9 units
17. Variable timestep with a 4-frame averaged, clamped frame delta - confirmed - 3 units
18. Interrupt-driven CD sector reader that reads whole sectors straight into the destination - confirmed - 2 units
19. Hardware MDEC decoding for movies and in-game animated portraits, serviced at frame end - confirmed - 6 units
20. SPU-interrupt-driven audio streaming in a ring of ADPCM blocks - confirmed - 4 units
21. Arena allocator with front/back carving and fixed-size node pools with O(1) take/release - confirmed - 4 units
22. Ordering table handed to GPU DMA channel 2 once the GPU is ready - likely - 2 units
23. Not used in this binary: GTE RTPT triple projection, AVSZ depth averaging, DPCS depth cueing - hypothesis - 5 units
24. Distance-based LOD or per-frame polygon budget - hypothesis - 1 units

### 1. Adaptive depth-bucket sort of all primitives into the ordering table at frame end
- Status: confirmed
- Mechanism: Emitters never touch the ordering table: each packet stores its depth in the 24-bit link field of its tag word. At frame end one pass finds min/max depth over both packet pools, derives a power-of-two bucket shift from the depth span via the GTE leading-zero counter (one divide per frame, shifts per primitive), and relinks every packet into its OT bucket. The OT resolution therefore adapts to the depth range actually on screen, and per-primitive cost is a mask, subtract, shift and two stores.
- Evidence:
  - `src/game/8001B4A8.c:153`
  - `src/game/8001B4A8.c:182`
  - `src/game/8001B4A8.c:185`
  - `src/game/8001B4A8.c:213`
  - `src/game/8001B4A8.c:225`
  - `src/game/8001B4A8.c:362`
  - `src/game/800648E8.c:214`
  - `src/game/800164F4.c:146`
  - `src/game/800172A8.c:99`
  - `src/game/8001B2C8.c:89`
  - `src/game/80093178.c:62`
- Units:
  - `src/game/8001B4A8.c` - manifest 0x8001B4A8, size 0x704 (current binding: gSubmitRenderFrame)
  - `src/game/8001B2C8.c` - manifest 0x8001B2C8, size 0x180 (current binding: gAllocateRenderBuffers)
  - `src/game/80093178.c` - manifest 0x80093178, size 0xAC (current binding: gClearOrderingTableReverse)
  - `src/game/80095C14.c` - manifest 0x80095C14, size 0x18 (current binding: gGteLeadingZeros)
  - `src/game/800648E8.c` - manifest 0x800648E8, size 0x84C (current binding: gEmitCellPolygon)
  - `src/game/800164F4.c` - manifest 0x800164F4, size 0x570 (current binding: gDrawModelFrame)
  - `src/game/800172A8.c` - manifest 0x800172A8, size 0x260 (current binding: gEmitFlatShadedPoly)
- Naming proposals:
  - `func_8001B4A8` -> `gBucketSortAndSubmitFrame`: finds depth extrema over both pools, derives the bucket shift and relinks every packet before submitting the OT (8001B4A8.c:153-249,362).
  - `func_8001B2C8` -> `gAllocateRenderBuffers`: keeps current name: allocates two packet pools, two node pools and two (n+2)-word OTs (8001B2C8.c:78-90).
  - `func_80093178` -> `gClearOrderingTableReverse`: logs the ClearOTagR format string and writes the terminator word through the GPU callback block (80093178.c:60-77).
  - `func_80095C14` -> `gGteLeadingZeros`: keeps current name: mtc2 to LZCS ($30), mfc2 from LZCR ($31) (80095C14.c:34-37).
  - `func_800648E8` -> `gEmitCellPolygon`: keeps current name: writes the mean corner depth into the tag before filling a terrain packet (800648E8.c:205,214).
  - `func_800164F4` -> `gDrawModelFrame`: keeps current name: depth-tags each model triangle (800164F4.c:142-146).
  - `func_800172A8` -> `gEmitFlatShadedPoly`: keeps current name: copies the 24-bit depth into the packet tag (800172A8.c:99).
  - global `D_8009F5D8` -> `gOrderingTableBuffers`: keep: per-buffer OT base array indexed by the buffer index (8001B4A8.c:202-205).
  - global `D_8009F5FC` -> `gOtBucketCount`: divisor of the depth span and OT length passed to the clear (8001B4A8.c:182,399).
  - global `D_8009F600` -> `gRenderBufferIndex`: selects the active OT and is flipped to 1-index after submit (8001B4A8.c:199-205,368-384).

### 2. Double-buffered packet pools and OT with a VBlank-gated flip
- Status: confirmed
- Mechanism: Two sets of packet pools and two OTs are allocated; while the GPU draws one set, the CPU builds the next frame into the other. Submission raises a flip-pending flag, and the vertical-blank callback clears it and flips the display only once the GPU reports idle, so the CPU only waits at the start of the next submit if the GPU is still behind.
- Evidence:
  - `src/game/8001B2C8.c:81`
  - `src/game/8001B2C8.c:90`
  - `src/game/8001B4A8.c:121`
  - `src/game/8001B4A8.c:365`
  - `src/game/8001B4A8.c:384`
  - `src/game/8001B4A8.c:390`
  - `src/game/8001EFF8.c:61`
  - `src/game/8001EFF8.c:65`
  - `src/game/8001EFF8.c:68`
- Units:
  - `src/game/8001B4A8.c` - manifest 0x8001B4A8, size 0x704 (current binding: gSubmitRenderFrame)
  - `src/game/8001B2C8.c` - manifest 0x8001B2C8, size 0x180 (current binding: gAllocateRenderBuffers)
  - `src/game/8001EFF8.c` - manifest 0x8001EFF8, size 0xC0 (current binding: gFrameCallback)
  - `src/game/8001BFA8.c` - manifest 0x8001BFA8, size 0x30 (current binding: gFlipDisplayLayer)
- Naming proposals:
  - `func_8001B4A8` -> `gBucketSortAndSubmitFrame`: same unit as finding 1; it also swaps the pool bases and clears the next OT (8001B4A8.c:368-399).
  - `func_8001B2C8` -> `gAllocateRenderBuffers`: allocates the second packet pool and second OT (8001B2C8.c:81,90).
  - `func_8001EFF8` -> `gVBlankFrameCallback`: installed as a per-frame interrupt callback; adds 5 to the clocks, clears the flip flag when the GPU poll returns 0 and flips (8001EFF8.c:56-68).
  - `func_8001BFA8` -> `gFlipDisplayLayer`: keeps current name: called only after the GPU-idle check in the VBlank callback (8001EFF8.c:65).
  - global `D_8009FA3C` -> `gFlipPending`: raised after submit (8001B4A8.c:365), awaited before the next submit (8001B4A8.c:121), cleared by the VBlank callback when the GPU is idle (8001EFF8.c:61-63).

### 3. Bump-pointer primitive pools with saturating last-slot reuse
- Status: confirmed
- Mechanism: Every GPU packet comes from one of two flat per-frame pools (0x34-byte and 0x1C-byte slots) via a pointer bump; there is no per-primitive allocator, free, or OT insertion. When a pool is full the emitter keeps overwriting the last slot instead of branching to an error path, so overload costs image quality, never time.
- Evidence:
  - `src/game/800172A8.c:89`
  - `src/game/800172A8.c:95`
  - `src/game/800648E8.c:207`
  - `src/game/800648E8.c:212`
  - `src/game/8006F21C.c:74`
  - `src/game/8001B2C8.c:106`
  - `src/game/8001B2C8.c:112`
  - `src/game/8001B4A8.c:393`
- Units:
  - `src/game/800172A8.c` - manifest 0x800172A8, size 0x260 (current binding: gEmitFlatShadedPoly)
  - `src/game/800648E8.c` - manifest 0x800648E8, size 0x84C (current binding: gEmitCellPolygon)
  - `src/game/8006F21C.c` - manifest 0x8006F21C, size 0xF4 (current binding: gEmitTracerGouraudQuad)
  - `src/game/80082EF0.c` - manifest 0x80082EF0, size 0xB4 (current binding: gEmitFlatQuadRect)
  - `src/game/8002549C.c` - manifest 0x8002549C, size 0x1F4 (current binding: gEmitSplitScreenBlit)
- Naming proposals:
  - `func_800172A8` -> `gEmitFlatShadedPoly`: keeps current name; the pool-take idiom at 800172A8.c:89-95.
  - `func_800648E8` -> `gEmitCellPolygon`: keeps current name; same idiom at 800648E8.c:207-213.
  - `func_8006F21C` -> `gEmitTracerGouraudQuad`: keeps current name; takes a 0x34-byte packet from the same pool (8006F21C.c:74).
  - `func_80082EF0` -> `gEmitFlatQuadRect`: keeps current name; reads gPrimPoolEnd for the same saturating take (80082EF0.c:11).
  - `func_8002549C` -> `gEmitSplitScreenBlit`: keeps current name; uses the 0x1C pool bounded by gPacketLimit (8002549C.c:20).
  - global `D_8009F9F0` -> `gPrimPoolNext`: keep: bump pointer of the 0x34-byte pool (800172A8.c:95).
  - global `D_8009FA28` -> `gPrimPoolEnd`: keep: last usable 0x34 slot, base + (n-1)*0x34 (8001B2C8.c:100-106).
  - global `D_8009FA90` -> `gSmallPrimPoolNext`: bump pointer of the 0x1C-byte pool; reset to the node table each frame (8001B2C8.c:95, 8001B4A8.c:393).
  - global `D_8009F9C0` -> `gSmallPrimPoolEnd`: last usable 0x1C slot, base + (0x400-1)*0x1C (8001B2C8.c:108-112).

### 4. GTE batch vertex transforms with hand-scheduled (software-pipelined) loops
- Status: confirmed
- Mechanism: All model, effect and cell vertices go through small hand-written COP2 loops: one mvmva per vertex with the rotation/translation loaded once per object. The loads of the next vertex are placed in the GTE latency slots between mvmva and the mfc2 reads, so the R3000 and the GTE overlap; 32-bit world coordinates are pre-shifted by 4 to fit the GTE's 16-bit inputs.
- Evidence:
  - `src/game/8001006C.c:46`
  - `src/game/8001006C.c:58`
  - `src/game/8001006C.c:38`
  - `src/game/80010234.c:42`
  - `src/game/8001016C.c:46`
  - `src/game/80010750.c:42`
  - `src/game/80084398.c:226`
  - `src/game/80084398.c:48`
  - `asm/us/main.s:71`
  - `asm/us/main.s:210`
- Units:
  - `src/game/8001006C.c` - manifest 0x8001006C, size 0xA0 (current binding: gGteTransformWords)
  - `src/game/8001010C.c` - manifest 0x8001010C, size 0x60 (current binding: gGteTransformVertices)
  - `src/game/8001016C.c` - manifest 0x8001016C, size 0x68 (no readable binding)
  - `src/game/800101D4.c` - manifest 0x800101D4, size 0x60 (current binding: gGteTransformVerticesV0)
  - `src/game/80010234.c` - manifest 0x80010234, size 0x6C (current binding: gGteTransformVerticesWide)
  - `src/game/80010750.c` - manifest 0x80010750, size 0xD0 (current binding: gGteLoadMatrix)
  - `src/game/80010820.c` - manifest 0x80010820, size 0xB8 (current binding: gGteLoadRotation)
  - `src/game/80084398.c` - manifest 0x80084398, size 0x448 (current binding: gDrawCellWindowModels)
- Naming proposals:
  - `func_8001006C` -> `gGteTransformWords`: keeps the callers' name: 12-byte s32 vertices, >>4 into VXY0/VZ0, mvmva, <<4 back (8001006C.c:36-67).
  - `func_8001010C` -> `gGteTransformVertices`: keeps the callers' name: same loop on 8-byte s16 vertices with s16 output.
  - `func_8001016C` -> `gGteTransformVerticesYScale15`: same loop but stores y - (y >> 4), a 15/16 Y scale (8001016C.c:45-47).
  - `func_800101D4` -> `gGteTransformVerticesV0`: keeps the callers' name: mvmva with the V0 operand (800101D4.c:36).
  - `func_80010234` -> `gGteTransformVerticesWide`: keeps the callers' name: 32-bit output, 0xC stride (80010234.c:42-48).
  - `func_80010750` -> `gGteLoadMatrix`: keeps the callers' name: ctc2 of rotation rows plus translation (80010750.c:42-69).
  - `func_80010820` -> `gGteLoadRotation`: keeps the callers' name: rotation only, zero translation (80010820.c:40-49).
  - `func_80084398` -> `gDrawCellWindowModels`: keeps current name: one matrix load then one batch transform per animated model (80084398.c:226-231).

### 5. Single-vertex RTPS projection with per-vertex clip codes and one-test batch reject
- Status: confirmed
- Mechanism: Projection runs one rtps per vertex in a hand-scheduled loop that writes screen XY and Z plus a clip byte per vertex (near, left/right, bottom; FLAG overflow forces reject) and returns the OR of all FLAG words. Polygon emitters AND/OR the clip bytes of their corners to drop whole polygons before any packet is built, and models can reject the whole vertex batch with one test.
- Evidence:
  - `src/game/8001047C.c:60`
  - `src/game/8001047C.c:65`
  - `src/game/8001047C.c:68`
  - `src/game/8001047C.c:70`
  - `src/game/8001047C.c:79`
  - `src/game/800648E8.c:187`
  - `src/game/800648E8.c:188`
  - `src/game/80013F08.c:270`
  - `src/game/800646BC.c:101`
  - `src/game/80044260.c:204`
  - `asm/us/main.s:388`
  - `asm/us/main.s:98550`
- Units:
  - `src/game/8001047C.c` - manifest 0x8001047C, size 0xD0 (current binding: gProjectVertexBatch)
  - `src/game/80010704.c` - manifest 0x80010704, size 0x28 (current binding: gGteProjectPoint)
  - `src/game/8001072C.c` - manifest 0x8001072C, size 0x24 (current binding: gGteProjectVertex)
  - `src/game/800646BC.c` - manifest 0x800646BC, size 0x200 (current binding: gEmitTexturedSurface)
  - `src/game/80044260.c` - manifest 0x80044260, size 0x350 (current binding: gDrawObjectPointTiles)
  - `src/game/80013F08.c` - manifest 0x80013F08, size 0x25C (current binding: gProjectModelInstance)
- Naming proposals:
  - `func_8001047C` -> `gProjectVertexBatch`: keeps the callers' name: rtps loop, clip byte per vertex, returns OR of FLAG (8001047C.c:60-70).
  - `func_80010704` -> `gGteProjectPoint`: keeps current name: one rtps, swc2 SXY2/SZ3 (80010704.c:43-44).
  - `func_8001072C` -> `gGteProjectVertex`: keeps current name: one rtps, swc2 SXY2 only (8001072C.c:38).
  - `func_800646BC` -> `gEmitTexturedSurface`: keeps current name: inline rtps in the terrain strip builder (800646BC.c:101).
  - `func_80044260` -> `gDrawObjectPointTiles`: keeps current name: inline rtps per object point (80044260.c:204).
  - `func_80013F08` -> `gProjectModelInstance`: keeps current name: batch-projects the model vertices into the clip-flag buffer (80013F08.c:270).

### 6. Cheap backface culling: GTE NCLIP for terrain, integer 2D cross product for models
- Status: confirmed
- Mechanism: Back-facing polygons are dropped before a packet is allocated. Terrain triangles use the GTE NCLIP instruction (sign of MAC0), model triangles use a two-multiply screen-space cross product on packed 16.16 screen words, both costing a handful of cycles against the cost of a GPU packet.
- Evidence:
  - `src/game/800648BC.c:33`
  - `src/game/800648BC.c:36`
  - `src/game/800648E8.c:198`
  - `src/game/80015AF4.c:35`
  - `src/game/80015B68.c:147`
  - `asm/us/main.s:98633`
- Units:
  - `src/game/800648BC.c` - manifest 0x800648BC, size 0x2C (no readable binding)
  - `src/game/80015AF4.c` - manifest 0x80015AF4, size 0x74 (current binding: gEmitFrontFacingTriangle)
  - `src/game/80015B68.c` - manifest 0x80015B68, size 0x3AC (current binding: gPrepareModelPart)
  - `src/game/800648E8.c` - manifest 0x800648E8, size 0x84C (current binding: gEmitCellPolygon)
- Naming proposals:
  - `func_800648BC` -> `gGteNclipIsBackFacing`: mtc2 SXY0..2, nclip, returns MAC0 < 0 (800648BC.c:29-36); its only caller skips the polygon on 1 (800648E8.c:198).
  - `func_80015AF4` -> `gEmitFrontFacingTriangle`: keeps current name: returns 1 for negative winding, otherwise writes the three corners (80015AF4.c body).
  - `func_80015B68` -> `gPrepareModelPart`: keeps current name: calls the winding test per triangle/quad half (80015B68.c:147,163).
  - `func_800648E8` -> `gEmitCellPolygon`: keeps current name: NCLIP test gated by bit 0 of the polygon word (800648E8.c:189-198).

### 7. Hierarchical view-volume culling of the 5x5 map-cell window into 16-bit subcell masks
- Status: confirmed
- Mechanism: Once per view, each of the 25 cells around the camera is rotated into view space with one GTE mvmva and tested as a sphere against the view pyramid using only shifts and adds; only if the cell passes are its 4 quadrants and then 4x4 subcells tested. The result is a 16-bit visibility mask per cell, so terrain subcells and cell models are rejected with a single AND each, and fully hidden cells (mask 0) cost nothing in the three draw layers.
- Evidence:
  - `src/game/80066C9C.c:111`
  - `src/game/80064460.c:103`
  - `src/game/80064460.c:113`
  - `src/game/8006429C.c:51`
  - `src/game/8006429C.c:52`
  - `src/game/8006429C.c:66`
  - `src/game/80063FF8.c:45`
  - `src/game/80063FF8.c:58`
  - `src/game/8006531C.c:79`
  - `src/game/80084398.c:141`
  - `src/game/80084398.c:153`
  - `src/game/80083A10.c:426`
  - `asm/us/main.s:98071`
- Units:
  - `src/game/80064460.c` - manifest 0x80064460, size 0x25C (current binding: gSampleCellWindow)
  - `src/game/8006429C.c` - manifest 0x8006429C, size 0x1C4 (no readable binding)
  - `src/game/8006411C.c` - manifest 0x8006411C, size 0x180 (no readable binding)
  - `src/game/80063FF8.c` - manifest 0x80063FF8, size 0xE0 (current binding: gIsInViewFrustumWithMargin)
  - `src/game/800640D8.c` - manifest 0x800640D8, size 0x44 (no readable binding)
  - `src/game/80066C9C.c` - manifest 0x80066C9C, size 0x1FC (current binding: gUpdateCameraAnchor)
  - `src/game/8006531C.c` - manifest 0x8006531C, size 0x1DC (current binding: gDrawRegionTiles)
  - `src/game/80084398.c` - manifest 0x80084398, size 0x448 (current binding: gDrawCellWindowModels)
  - `src/game/80083764.c` - manifest 0x80083764, size 0xD0 (current binding: gBuildSubcellMask)
  - `src/game/80083A10.c` - manifest 0x80083A10, size 0x700 (current binding: gSpawnSceneNode)
- Naming proposals:
  - `func_80064460` -> `gSampleCellWindow`: keeps current name: per cell calls the cull cascade and packs the hit bits into out->mask (80064460.c:103-113).
  - `func_8006429C` -> `gCullCellQuadtree`: rotates the cell centre, sphere-tests the cell, then descends into the four quadrant probes with shifted result bits (8006429C.c:46-69).
  - `func_8006411C` -> `gCullCellQuadrants`: four GTE-rotate + sphere tests at +step offsets, one result bit per sub-square (8006411C.c body).
  - `func_80063FF8` -> `gIsInViewFrustumWithMargin`: rejects z*16+margin < 0x601 and |x|,|y| beyond a depth-proportional limit, shifts/adds only (80063FF8.c:43-65).
  - `func_800640D8` -> `gGteRotatePointS16`: lwc2 V0, mvmva rotation, IR1..3 stored as s16 with y negated, returns FLAG (800640D8.c:30-45).
  - `func_80066C9C` -> `gUpdateCameraAnchor`: keeps current name: loads the camera rotation into the GTE right before the window sample (80066C9C.c:111,194).
  - `func_8006531C` -> `gDrawRegionTiles`: keeps current name: consumes the mask one bit per subcell (8006531C.c:77-80).
  - `func_80084398` -> `gDrawCellWindowModels`: keeps current name: skips mask-0 cells and ANDs each model's subcell mask (80084398.c:141,153).
  - `func_80083764` -> `gBuildSubcellMask`: keeps current name: precomputes a model's 4x4 subcell coverage at spawn (80083A10.c:426).
  - `func_80083A10` -> `gSpawnSceneNode`: keeps current name: stores the precomputed mask at node +0x4E (80083A10.c:426).

### 8. Per-instance bounding-sphere view-cone test before any vertex work
- Status: confirmed
- Mechanism: Each model instance carries a radius copied from its frame-0 data at bind time; the model path tests the instance centre plus that radius against the view cone (shift-only arithmetic) and returns before loading a matrix or transforming a single vertex. Glow effects use a sibling test with a fixed near plane.
- Evidence:
  - `src/game/80013AF0.c:83`
  - `src/game/80013F08.c:192`
  - `src/game/80019E70.c:54`
  - `src/game/80019E70.c:67`
  - `src/game/80070158.c:143`
  - `src/game/80017508.c:42`
- Units:
  - `src/game/80019E70.c` - manifest 0x80019E70, size 0xE4 (current binding: gTestFrustumMargin)
  - `src/game/80019D90.c` - manifest 0x80019D90, size 0xE0 (current binding: gTestFrustumFixedNear)
  - `src/game/80013F08.c` - manifest 0x80013F08, size 0x25C (current binding: gProjectModelInstance)
  - `src/game/80013AF0.c` - manifest 0x80013AF0, size 0xE8 (current binding: gInitModelInstance)
  - `src/game/80017508.c` - manifest 0x80017508, size 0x98 (current binding: gSubmitModelInstance)
  - `src/game/80070158.c` - manifest 0x80070158, size 0x250 (current binding: gDrawGlowNodeList)
- Naming proposals:
  - `func_80019E70` -> `gTestFrustumMargin`: keeps current name: returns 2 inside, 1 too near, 0 outside, shifts/adds only (80019E70.c:54-72).
  - `func_80019D90` -> `gTestFrustumFixedNear`: keeps current name: same cone with a fixed near threshold (80019D90.c:41-71).
  - `func_80013F08` -> `gProjectModelInstance`: keeps current name: early return when the cone test is not 2 (80013F08.c:192-193).
  - `func_80013AF0` -> `gInitModelInstance`: keeps current name: precomputes the instance radius at bind time (80013AF0.c:83).
  - `func_80017508` -> `gSubmitModelInstance`: keeps current name: the projection/cull gate before part drawing (80017508.c:42-44).
  - `func_80070158` -> `gDrawGlowNodeList`: keeps current name: distance then cone rejection per glow node (80070158.c:129,143).

### 9. Multiply-free octagonal distance approximations and a fixed activity radius
- Status: confirmed
- Mechanism: Distances are never computed with a square root on the hot paths: 3D length is max + (mid+min)/4, or a 2D octagonal estimate applied twice, built from abs, compare, add and shifts. One fixed radius (0x1FFFF world units) gates drawing of node-list models and glow nodes and the spawning of deferred objects.
- Evidence:
  - `src/game/80019F54.c:60`
  - `src/game/8001A074.c:54`
  - `src/game/8001A074.c:58`
  - `src/game/8001A100.c:44`
  - `src/game/8003CF6C.c:77`
  - `src/game/80070158.c:129`
  - `src/game/8003FF7C.c:179`
- Units:
  - `src/game/80019F54.c` - manifest 0x80019F54, size 0x78 (current binding: gApproxVectorLength)
  - `src/game/8001A074.c` - manifest 0x8001A074, size 0x8C (current binding: gApproximateLength2)
  - `src/game/8001A100.c` - manifest 0x8001A100, size 0x3C (current binding: gVectorLength3D)
  - `src/game/8003CF6C.c` - manifest 0x8003CF6C, size 0x140 (current binding: gDrawNodeListModels)
  - `src/game/80070158.c` - manifest 0x80070158, size 0x250 (current binding: gDrawGlowNodeList)
- Naming proposals:
  - `func_80019F54` -> `gApproxVectorLength3D`: abs, two swaps, return max + (mid+min)>>2 (80019F54.c:60).
  - `func_8001A074` -> `gApproxLength2D`: octagonal 2D estimate from shifts only (8001A074.c:54-58).
  - `func_8001A100` -> `gApproxLength3D`: applies the 2D estimate to (x,y) then to (xy,z) (8001A100.c body).
  - `func_8003CF6C` -> `gDrawNodeListModels`: keeps current name: skips nodes beyond radius + 0x1FFFF (8003CF6C.c:77).
  - `func_80070158` -> `gDrawGlowNodeList`: keeps current name: same distance gate (80070158.c:129).

### 10. Proximity streaming of deferred object spawns
- Status: confirmed
- Mechanism: Level objects can sit as lightweight deferred records; each tick their distance to each player's view anchor is checked and the full object is instantiated when it comes within range and torn down (message 10 to its nodes) when it leaves. Only nearby objects consume update time, memory and draw passes.
- Evidence:
  - `src/game/8003FF7C.c:148`
  - `src/game/8003FF7C.c:179`
  - `src/game/8003FF7C.c:203`
  - `src/game/8003FF7C.c:207`
  - `src/game/8003FE0C.c:61`
  - `src/game/8003FE90.c:58`
- Units:
  - `src/game/8003FF7C.c` - manifest 0x8003FF7C, size 0x360 (current binding: gStreamDeferredSpawns)
  - `src/game/8003FE0C.c` - manifest 0x8003FE0C, size 0x84 (no readable binding)
  - `src/game/8003FE90.c` - manifest 0x8003FE90, size 0xEC (current binding: gRetireDeferredNodes)
- Naming proposals:
  - `func_8003FF7C` -> `gStreamDeferredSpawns`: keeps current name: walks the deferred list and toggles spawn state by range (8003FF7C.c:179-207).
  - `func_8003FE0C` -> `gActivateDeferredSpawn`: creates the model node and runs gSpawnObjectFromTable, setting flag bit 1 on success (8003FE0C.c:61).
  - `func_8003FE90` -> `gRetireDeferredNodes`: unlinks every list node owned by the record from the seven list heads and sends message 10, then clears flag bit 1 (8003FE90.c:58).
  - global `D_8009F30C` -> `gDeferredSpawnList`: keep: head of the deferred spawn record list walked each tick (8003FF7C.c:64).

### 11. Per-cell bucket grid for map models (spatial hashing by cell index)
- Status: confirmed
- Mechanism: Map-cell model nodes are stored in a width x height grid of list heads allocated at map load, so the model layer only walks the lists of the up to 25 visible window cells instead of all objects in the level.
- Evidence:
  - `src/game/800834C4.c:53`
  - `src/game/80084398.c:144`
  - `src/game/80084398.c:145`
  - `src/game/80083A10.c:426`
- Units:
  - `src/game/800834C4.c` - manifest 0x800834C4, size 0x68 (current binding: gAllocCellBucketGrid)
  - `src/game/80083A10.c` - manifest 0x80083A10, size 0x700 (current binding: gSpawnSceneNode)
  - `src/game/80084398.c` - manifest 0x80084398, size 0x448 (current binding: gDrawCellWindowModels)
- Naming proposals:
  - `func_800834C4` -> `gAllocCellBucketGrid`: keeps current name: allocates and zeroes width*height two-word heads (800834C4.c:53).
  - `func_80083A10` -> `gSpawnSceneNode`: keeps current name: inserts a model node into its cell bucket with its subcell mask (80083A10.c:426).
  - `func_80084398` -> `gDrawCellWindowModels`: keeps current name: indexes the grid by window cell (80084398.c:144-145).
  - global `D_8009F938` -> `gCellBucketGrid`: keep: grid of per-cell list heads (800834C4.c:4).
  - global `D_8009F93C` -> `gCellGridWidth`: keep: row stride of the grid (80084398.c:144).

### 12. Scratchpad (1 KB on-chip RAM) as stack for hot passes and as fast table storage
- Status: confirmed
- Mechanism: The hottest per-frame passes run with the stack pointer moved into the 1 KB scratchpad at 0x1F800000 (single-cycle, no cache misses), via tiny trampolines that save $sp in a global. The terrain layer copies its tile-shape table there, terrain packets stage remapped UVs there, mesh fracture keeps bitmaps there, and the MDEC path copies its dither tables there.
- Evidence:
  - `src/game/8003F530.c:34`
  - `src/game/8003F530.c:35`
  - `src/game/80026E1C.c:34`
  - `src/game/8006531C.c:61`
  - `src/game/800648E8.c:219`
  - `src/game/8004B5F8.c:242`
  - `src/game/80085228.c:159`
  - `src/game/800861E4.c:179`
- Units:
  - `src/game/8003F530.c` - manifest 0x8003F530, size 0x40 (current binding: gRunSceneListPassesOnScratch)
  - `src/game/80026E1C.c` - manifest 0x80026E1C, size 0x40 (current binding: gRunRegionPass)
  - `src/game/80026E5C.c` - manifest 0x80026E5C, size 0x40 (current binding: gUpdateViewAnchorsOnScratchpad)
  - `src/game/80026DDC.c` - manifest 0x80026DDC, size 0x40 (current binding: gRunMapScreenOnScratch)
  - `src/game/80026D1C.c` - manifest 0x80026D1C, size 0x40 (current binding: gRunEntityUpdatesOnScratchpad)
  - `src/game/80026D5C.c` - manifest 0x80026D5C, size 0x40 (current binding: gRunHudPassOnScratchpad)
  - `src/game/80026D9C.c` - manifest 0x80026D9C, size 0x40 (current binding: gTickScriptTimersOnScratchpad)
  - `src/game/800586D4.c` - manifest 0x800586D4, size 0x40 (no readable binding)
  - `src/game/8006531C.c` - manifest 0x8006531C, size 0x1DC (current binding: gDrawRegionTiles)
  - `src/game/8004B5F8.c` - manifest 0x8004B5F8, size 0x8F0 (current binding: gSpawnMeshFractureDebris)
- Naming proposals:
  - `func_8003F530` -> `gRunSceneListPassesOnScratch`: keeps current name: sp = 0x1F8003F8, jal func_8003F4A0 (8003F530.c:34-36).
  - `func_80026E1C` -> `gRunRegionPassOnScratch`: renamed from gRunRegionPass for family consistency: sp = 0x1F8003F8, jal func_80066E98 (80026E1C.c:34-36).
  - `func_80026E5C` -> `gUpdateViewAnchorsOnScratchpad`: keeps current name: sp to scratchpad, calls func_80066C9C (80026E5C.c:35-36).
  - `func_80026DDC` -> `gRunMapScreenOnScratch`: keeps current name: sp = 0x1F8003F0, calls func_800267C8 (80026DDC.c:52).
  - `func_80026D1C` -> `gRunEntityUpdatesOnScratchpad`: keeps current name: sp = 0x1F8003F8, jal func_8004EC74 (80026D1C.c:39-41).
  - `func_80026D5C` -> `gRunHudPassOnScratchpad`: keeps current name: sp = 0x1F800400, jal func_80062FB4 (80026D5C.c:39-41).
  - `func_80026D9C` -> `gTickScriptTimersOnScratchpad`: keeps current name: sp = 0x1F8003FC, jal func_80049564 (80026D9C.c:41-43).
  - `func_800586D4` -> `gRunUnitTickOnScratchpad`: sp = 0x1F8003F4 then jal func_80055988 (800586D4.c:50-52); name follows the callee, which is still unnamed.
  - `func_8006531C` -> `gDrawRegionTiles`: keeps current name: copies the tile-shape table into 0x1F800000..0x1F80006F (8006531C.c:61).
  - `func_8004B5F8` -> `gSpawnMeshFractureDebris`: keeps current name: face-selection bitmaps at 0x1F800000/0x1F800020 (8004B5F8.c:240-242).
  - global `D_8009FAD8` -> `gScratchpadSavedSp`: holds the caller's $sp while a pass runs on the scratchpad stack (8003F530.c:4,30-37).

### 13. Table-driven trigonometry: quarter-wave sine table, 4096-step angles, table atan2 and sqrt
- Status: confirmed
- Mechanism: Angles are 12-bit integers (0x1000 per turn), so wrapping is a mask. Sine and cosine come from one 0x401-entry quarter-wave table with quadrant mirroring; atan2 is a ratio lookup into a 1024-entry table; the square root normalises with the GTE leading-zero counter and reads a half-word table. No transcendental is ever computed.
- Evidence:
  - `src/game/8001846C.c:36`
  - `src/game/8001846C.c:38`
  - `src/game/8001846C.c:39`
  - `src/game/80018534.c:43`
  - `src/game/800188D0.c:47`
  - `src/game/80095C30.c:72`
  - `src/game/80095C30.c:79`
  - `src/game/80095900.c:46`
  - `src/game/80095900.c:71`
- Units:
  - `src/game/8001846C.c` - manifest 0x8001846C, size 0xC8 (current binding: gSinCos)
  - `src/game/80018534.c` - manifest 0x80018534, size 0x4C (current binding: gSineLookup)
  - `src/game/800188D0.c` - manifest 0x800188D0, size 0x4F0 (current binding: gBuildRotationMatrix)
  - `src/game/80095C30.c` - manifest 0x80095C30, size 0x174 (current binding: gAtan2)
  - `src/game/80095900.c` - manifest 0x80095900, size 0x94 (current binding: gSqrt12)
- Naming proposals:
  - `func_8001846C` -> `gSinCos`: keeps current name: one masked index, two table reads, quadrant switch (8001846C.c:38-60).
  - `func_80018534` -> `gSineLookup`: keeps current name: single sine with mirror/negate (80018534.c:40-49).
  - `func_800188D0` -> `gBuildRotationMatrix`: keeps current name: builds the camera/object matrices from gSinCos pairs (800188D0.c:47,59).
  - `func_80095C30` -> `gAtan2`: keeps current name: min*1024/max indexes the atan table, mirrored by 0x400 - value (80095C30.c:66-79).
  - `func_80095900` -> `gSqrt12`: keeps current name: LZCS/LZCR normalisation then a half-word table at 0x8009DF20 (80095900.c:46-71).
  - global `D_8009A234` -> `gQuarterSineTable`: indexed 0..0x400 and mirrored for the other quadrants (8001846C.c:38-39).
  - global `D_8009E0A0` -> `gAtanTable`: keep: ratio-indexed arctangent table (80095C30.c:72,79).
  - global `D_8009DF20` -> `gSqrtTable`: hypothesis: the address is only formed as lui 0x800A / lh -0x20E0 in 80095900.c:66-71 and has no symbol in analysis/symbol_addrs.txt yet.

### 14. Integer fixed-point math everywhere (16.16 and 4.12), no FPU and no soft-float helpers
- Status: confirmed
- Mechanism: World positions and matrices are 32-bit 16.16 integers, GTE data is 4.12/16-bit; software matrix and vector products use one mult and take the middle 32 bits from HI/LO. The compiler profile is -msoft-float yet no float code is emitted: the retail listing contains no COP1 instructions and the symbol list has no libgcc float helpers.
- Evidence:
  - `src/game/8001A81C.c:4`
  - `src/game/8001A81C.c:46`
  - `src/game/8001AB00.c:52`
  - `src/game/8001AC28.c:49`
  - `src/game/8001006C.c:38`
- Units:
  - `src/game/8001A81C.c` - manifest 0x8001A81C, size 0x154 (current binding: gMulMatrixVector)
  - `src/game/8001AB00.c` - manifest 0x8001AB00, size 0x78 (current binding: gVectorDot)
  - `src/game/8001AC28.c` - manifest 0x8001AC28, size 0x7C (current binding: gVectorMulAdd)
  - `src/game/8001AB78.c` - manifest 0x8001AB78, size 0x68 (current binding: gVectorScaleInPlace)
- Naming proposals:
  - `func_8001A81C` -> `gMulMatrixVector`: keeps current name: 3x3 by vector with the 16.16 FMUL macro (8001A81C.c:4,46).
  - `func_8001AB00` -> `gVectorDot16`: 16.16 dot product from three mult/mfhi/mflo pairs (8001AB00.c:52-65).
  - `func_8001AC28` -> `gVectorMulAdd`: keeps current name: a + b*scale per component with one mult each (8001AC28.c:49).
  - `func_8001AB78` -> `gVectorScaleInPlace`: keeps current name: hand-written in-place 16.16 scale.

### 15. Division-free vector normalisation with the GTE leading-zero counter and an inverse-sqrt table
- Status: confirmed
- Mechanism: To normalise a vector, the largest component's magnitude is measured with the GTE LZCS/LZCR registers, the vector is shifted into a fixed range, the squared length is formed with three multiplies, and the reciprocal square root comes from a table indexed after another LZC normalisation with a halved shift. No divide and no iterative square root on this path; the frame-end OT sort uses the same LZC trick to turn a divisor into a shift.
- Evidence:
  - `src/game/8001A180.c:97`
  - `src/game/8001A180.c:122`
  - `src/game/80019904.c:35`
  - `src/game/80019904.c:46`
  - `src/game/8001B4A8.c:185`
  - `src/game/8001A314.c:85`
- Units:
  - `src/game/8001A180.c` - manifest 0x8001A180, size 0x194 (current binding: gNormalizeVector)
  - `src/game/80019904.c` - manifest 0x80019904, size 0x68 (no readable binding)
  - `src/game/80095C14.c` - manifest 0x80095C14, size 0x18 (current binding: gGteLeadingZeros)
  - `src/game/8001A314.c` - manifest 0x8001A314, size 0x1D4 (current binding: gNormalizeGetLength)
- Naming proposals:
  - `func_8001A180` -> `gNormalizeVector`: keeps current name: LZC pre-scale, sum of squares, table inverse sqrt, three multiplies (8001A180.c:97-129).
  - `func_80019904` -> `gLookupInvSqrt`: LZCS/LZCR, even shift, half-word table lookup, result shifted by half the exponent (80019904.c:35-55).
  - `func_80095C14` -> `gGteLeadingZeros`: keeps current name: bare LZCS/LZCR access.
  - `func_8001A314` -> `gNormalizeGetLength`: keeps current name; counter-example: it uses gSqrt12 plus one real divide (8001A314.c:82-85).

### 16. Lighting split: terrain colours baked into map data, models lit by GTE NCCS
- Status: confirmed
- Mechanism: Terrain polygons get their vertex colours straight from level data (a per-polygon grey byte, a colour table, or the tileset's precomputed pattern buffer) with no lighting math per frame. Only models are lit at runtime, by one NCCS GTE instruction per vertex against a light matrix and colour matrix that are uploaded once per view, with the light vector rotated into camera space once per frame in software.
- Evidence:
  - `src/game/800648E8.c:232`
  - `src/game/800648E8.c:275`
  - `src/game/800648E8.c:296`
  - `src/game/80015F14.c:39`
  - `src/game/80015F38.c:97`
  - `src/game/80013ECC.c:43`
  - `src/game/80013C80.c:39`
  - `src/game/80027510.c:245`
  - `src/game/80013A14.c:49`
  - `src/game/800959D0.c:29`
  - `src/game/800959A0.c:30`
  - `asm/us/main.s:7393`
- Units:
  - `src/game/800648E8.c` - manifest 0x800648E8, size 0x84C (current binding: gEmitCellPolygon)
  - `src/game/80015F14.c` - manifest 0x80015F14, size 0x24 (current binding: gGteShadeNccs)
  - `src/game/80015F38.c` - manifest 0x80015F38, size 0x5BC (current binding: gEmitShadedPart)
  - `src/game/800172A8.c` - manifest 0x800172A8, size 0x260 (current binding: gEmitFlatShadedPoly)
  - `src/game/80013ECC.c` - manifest 0x80013ECC, size 0x3C (current binding: gGteSetLightVector)
  - `src/game/80013C80.c` - manifest 0x80013C80, size 0x38 (current binding: gUpdateLightDirection)
  - `src/game/80013A14.c` - manifest 0x80013A14, size 0x8C (current binding: gInitModelRendererBuffers)
  - `src/game/800959D0.c` - manifest 0x800959D0, size 0x20 (current binding: gGteSetBackColor)
  - `src/game/800959A0.c` - manifest 0x800959A0, size 0x30 (current binding: gGteSetColorMatrix)
- Naming proposals:
  - `func_800648E8` -> `gEmitCellPolygon`: keeps current name: copies baked colours from the polygon word, CLUT table or pattern buffer (800648E8.c:232-306).
  - `func_80015F14` -> `gGteShadeNccs`: keeps current name: lwc2 normal and RGBC, NCCS, swc2 RGB2 (80015F14.c:34-40).
  - `func_80015F38` -> `gEmitShadedPart`: keeps current name: calls NCCS per corner for lit model parts (80015F38.c:97).
  - `func_800172A8` -> `gEmitFlatShadedPoly`: keeps current name: NCCS-shaded debris polygons (800172A8.c:129).
  - `func_80013ECC` -> `gGteSetLightVector`: keeps current name: ctc2 into the light matrix registers 8..10 (80013ECC.c:43-45).
  - `func_80013C80` -> `gUpdateLightDirection`: keeps current name: one software matrix x vector per view (80013C80.c:39).
  - `func_80013A14` -> `gInitModelRendererBuffers`: keeps current name: sets background colour and light colour matrix once (80013A14.c:49-53).
  - `func_800959D0` -> `gGteSetBackColor`: ctc2 into RBK/GBK/BBK (control registers 13..15) (800959D0.c:27-29); the map's 'Depth Cueing' label does not match these registers.
  - `func_800959A0` -> `gGteSetColorMatrix`: five ctc2 into control registers 16..20, the light colour matrix (800959A0.c:28-32).

### 17. Variable timestep with a 4-frame averaged, clamped frame delta
- Status: confirmed
- Mechanism: The game does not lock to a fixed frame rate: a VBlank callback adds 5 to a clock each vertical blank, and each game frame measures the elapsed ticks (at least one VBlank), averages it with the previous three samples plus the carried remainder, clamps it to 0x3C, and passes that delta to animation and entity updates. Heavy scenes drop frames without slowing gameplay, and the average smooths jitter.
- Evidence:
  - `src/game/8001EFF8.c:58`
  - `src/game/8002703C.c:72`
  - `src/game/8002703C.c:85`
  - `src/game/8002703C.c:93`
  - `src/game/8002703C.c:113`
  - `src/game/80028618.c:110`
- Units:
  - `src/game/8002703C.c` - manifest 0x8002703C, size 0x168 (current binding: gStepGameplayFrameTime)
  - `src/game/80028618.c` - manifest 0x80028618, size 0x1DC (current binding: gRunGameFrameLoop)
  - `src/game/8001EFF8.c` - manifest 0x8001EFF8, size 0xC0 (current binding: gFrameCallback)
- Naming proposals:
  - `func_8002703C` -> `gStepGameplayFrameTime`: keeps current name: measures, averages and clamps the delta and feeds the tick functions (8002703C.c:72-113).
  - `func_80028618` -> `gRunGameFrameLoop`: keeps current name: seeds the delta history with 8 before the loop (80028618.c:109-118).
  - `func_8001EFF8` -> `gVBlankFrameCallback`: same unit as finding 2; advances the clocks by 5 per interrupt (8001EFF8.c:58-60).
  - global `D_8009F9FC` -> `gFrameDelta`: keep: averaged, clamped ticks per frame (8002703C.c:85-95).
  - global `D_8009FA60` -> `gGameClock`: keep: VBlank-driven clock, +5 per interrupt in play states (8001EFF8.c:58).
  - global `D_8009F690` -> `gFrameDeltaHistory`: first of four bytes D_8009F690..D_8009F693 used as the delta history and remainder (8002703C.c:16-19,79-89).

### 18. Interrupt-driven CD sector reader that reads whole sectors straight into the destination
- Status: confirmed
- Mechanism: File data is pulled in by a data-ready callback, not by polling: each sector's header is checked, full 2 KB sectors are read directly into the caller's buffer and only partial head/tail sectors go through a bounce buffer and a copy. The CPU is free between sectors and most data is touched once.
- Evidence:
  - `src/game/8001D130.c:102`
  - `src/game/8001C808.c:170`
  - `src/game/8001C808.c:179`
  - `src/game/8001C808.c:184`
- Units:
  - `src/game/8001C808.c` - manifest 0x8001C808, size 0x464 (current binding: gCdSectorCallback)
  - `src/game/8001D130.c` - manifest 0x8001D130, size 0x118 (current binding: gProbeCdDriveMode)
- Naming proposals:
  - `func_8001C808` -> `gCdSectorCallback`: keeps current name: per-sector state machine of the reader (8001C808.c:149-190).
  - `func_8001D130` -> `gInitCdSectorReader`: resets the reader state and installs the sector callback with gSetCdReadyCallback (8001D130.c:91-102); the current name gProbeCdDriveMode does not describe that.
  - global `D_800A85C0` -> `gCdSectorBounceBuffer`: receives a 0x200-word sector only when the wanted span is shorter than 0x800 bytes, then partially copied out (8001C808.c:170-179).

### 19. Hardware MDEC decoding for movies and in-game animated portraits, serviced at frame end
- Status: confirmed
- Mechanism: Compressed images are decoded by the MDEC, not the CPU: the movie path feeds slices by DMA, and in-game portrait frames are queued in a four-entry ring and decoded straight to VRAM during the frame-end submit. The CPU only converts the 24-bit MDEC output to 15-bit with a dither lookup, and those lookup tables are first copied into the scratchpad.
- Evidence:
  - `src/game/80029748.c:84`
  - `src/game/8001BBEC.c:52`
  - `src/game/8001B4A8.c:349`
  - `src/game/800861E4.c:210`
  - `src/game/800861E4.c:215`
  - `src/game/800861E4.c:179`
  - `src/game/800847E0.c:14`
- Units:
  - `src/game/800861E4.c` - manifest 0x800861E4, size 0x5CC (current binding: gDecodeMdecFrameToVram)
  - `src/game/80085228.c` - manifest 0x80085228, size 0x408 (current binding: gMovieSliceFeedDma)
  - `src/game/80085074.c` - manifest 0x80085074, size 0x1B4 (current binding: gUploadMovieStrip)
  - `src/game/800847E0.c` - manifest 0x800847E0, size 0x894 (no readable binding)
  - `src/game/8001BBEC.c` - manifest 0x8001BBEC, size 0x58 (current binding: gQueueVramUpload)
  - `src/game/80029748.c` - manifest 0x80029748, size 0x1A4 (current binding: gStepSynchronizedPortrait)
- Naming proposals:
  - `func_800861E4` -> `gDecodeMdecFrameToVram`: keeps current name: starts MDEC decode and output DMA, then uploads (800861E4.c:208-213).
  - `func_80085228` -> `gMovieSliceFeedDma`: keeps current name: slice feed on the interrupt path, copies tables to the scratchpad (80085228.c:159-162).
  - `func_80085074` -> `gUploadMovieStrip`: keeps current name: frame-complete callback that converts and uploads one strip.
  - `func_800847E0` -> `gDitherRgb24RowToRgb15`: per row: threshold-bumped channel bytes mapped through gByteTable and packed 5:5:5, two pixels per word (800847E0.c:14).
  - `func_8001BBEC` -> `gQueueMdecUpload`: writes five values into the four-slot ring that 8001B4A8 drains through gDecodeMdecFrameToVram (8001BBEC.c:52, 8001B4A8.c:346-357).
  - `func_80029748` -> `gStepPortraitFrames`: queues one portrait frame to VRAM 0x3C0,0x1CC per step (80029748.c:84).
  - global `D_800A0660` -> `gMdecUploadRing`: four 20-byte entries written by 8001BBEC and decoded by 8001B4A8.c:348-350.
  - global `D_8009F9D8` -> `gDitherByteTable`: keep-or-rename: byte map indexed after the dither bump (800847E0.c:14); currently gByteTable.
  - global `D_8009FABC` -> `gDitherThresholdTable`: 16x16 threshold rows cycled per output row (800847E0.c:77); currently gCharTable.

### 20. SPU-interrupt-driven audio streaming in a ring of ADPCM blocks
- Status: confirmed
- Mechanism: Streamed audio is refilled block by block from an SPU interrupt armed at the playback address: the handler queues the next block's DMA to SPU RAM and marks the block's ADPCM loop flags, so music and dialogue need only a small SPU ring and no per-frame CPU polling.
- Evidence:
  - `src/game/80022F44.c:43`
  - `src/game/80022FA8.c:71`
  - `src/game/80022FA8.c:93`
  - `src/game/80022D84.c:116`
  - `src/game/80022D84.c:121`
  - `src/game/80022D84.c:128`
- Units:
  - `src/game/80022FA8.c` - manifest 0x80022FA8, size 0x25C (current binding: gHandleAudioStreamSpuIrq)
  - `src/game/80022F44.c` - manifest 0x80022F44, size 0x38 (current binding: gArmAudioStreamSpuIrq)
  - `src/game/80022D84.c` - manifest 0x80022D84, size 0x1C0 (current binding: gSubmitAudioStreamTransfer)
  - `src/game/80022D1C.c` - manifest 0x80022D1C, size 0x68 (current binding: gMarkAudioStreamAdpcmBlock)
- Naming proposals:
  - `func_80022FA8` -> `gHandleAudioStreamSpuIrq`: keeps current name: SPU IRQ handler that submits the next block (80022FA8.c:71-93).
  - `func_80022F44` -> `gArmAudioStreamSpuIrq`: keeps current name: sets the IRQ address to base+offset and enables it (80022F44.c:41-43).
  - `func_80022D84` -> `gSubmitAudioStreamTransfer`: keeps current name: marks loop flags and queues the SPU DMA request (80022D84.c:116-128).
  - `func_80022D1C` -> `gMarkAudioStreamAdpcmBlock`: keeps the callers' name: called with mode 0..3 for the block position (80022D84.c:116).

### 21. Arena allocator with front/back carving and fixed-size node pools with O(1) take/release
- Status: confirmed
- Mechanism: Level-lifetime buffers are carved from the front or back end of free ranges (no headers walk, one table update), small objects come from fixed-stride pools, and per-frame object nodes use intrusive free lists where take and release are a few pointer swaps. Nothing on the frame path runs a general-purpose malloc.
- Evidence:
  - `src/game/80017EBC.c:132`
  - `src/game/80017EBC.c:146`
  - `src/game/80017EBC.c:164`
  - `src/game/80017EBC.c:178`
  - `src/game/80017EBC.c:190`
  - `src/game/8003BEF0.c:47`
- Units:
  - `src/game/80017EBC.c` - manifest 0x80017EBC, size 0x290 (current binding: gAllocMemory)
  - `src/game/8003BEF0.c` - manifest 0x8003BEF0, size 0x2C (current binding: gTakePoolNode)
  - `src/game/8003BF1C.c` - manifest 0x8003BF1C, size 0x34 (current binding: gReleasePoolNode)
  - `src/game/800181D4.c` - manifest 0x800181D4, size 0x28C (current binding: gFreeBlock)
- Naming proposals:
  - `func_80017EBC` -> `gAllocMemory`: keeps current name: tag 1/2 carve from the back/front of a free range, tag 3 picks a fixed pool (80017EBC.c:132-190).
  - `func_8003BEF0` -> `gTakePoolNode`: keeps current name: pops the free-list tail and links it active in three stores (8003BEF0.c:42-50).
  - `func_8003BF1C` -> `gReleasePoolNode`: keeps current name: the inverse unlink/push (8003BF1C.c:44).
  - `func_800181D4` -> `gFreeBlock`: keeps current name: frees into the owning fixed pool or range (callers 8003FF7C.c, 80084398 family).
  - global `D_8009F590` -> `gFreeRangeTablePtr`: keep: pairs of [start,end) free ranges carved by tags 1 and 2 (80017EBC.c:14,146,164).

### 22. Ordering table handed to GPU DMA channel 2 once the GPU is ready
- Status: likely
- Mechanism: The finished OT is not walked by the CPU: the submit path waits until DMA channel 2 is idle and GPUSTAT reports ready for DMA, installs the DMA-complete callback and issues DrawOTag through the library callback block, after which the GPU fetches the primitive chain itself while the CPU starts the next frame. The linked-list DMA mode is inside the library callback and is not visible in this unit, hence likely.
- Evidence:
  - `src/game/8009570C.c:62`
  - `src/game/8009570C.c:72`
  - `src/game/8009570C.c:72`
  - `src/game/80093280.c:50`
  - `src/game/8001B4A8.c:362`
- Units:
  - `src/game/8009570C.c` - manifest 0x8009570C, size 0xFC (no readable binding)
  - `src/game/80093280.c` - manifest 0x80093280, size 0x70 (no readable binding)
- Naming proposals:
  - `func_8009570C` -> `gDrawOTagWhenGpuReady`: polls DMA2 CHCR bit 24 and GPUSTAT bit 26, sets the DMA2 callback, calls the draw entry (8009570C.c:60-72).
  - `func_80093280` -> `gDrawOTag`: logs and forwards the OT head to the callback block's draw entry (80093280.c:50); called with the OT end from 8001B4A8.c:362.
  - global `D_8009DED4` -> `gDma2ChannelCtrlPtr`: keep: points at 0x1F8010A8 per docs/hardware-pointers.md:40.
  - global `D_8009DEC8` -> `gGpuGp1PortPtr`: keep: points at 0x1F801814 (GPUSTAT) per docs/hardware-pointers.md:36.

### 23. Not used in this binary: GTE RTPT triple projection, AVSZ depth averaging, DPCS depth cueing
- Status: hypothesis
- Mechanism: The retail code links libgte-style RTPT, AVSZ3/AVSZ4 and DPCT/INTPL routines, but no jal to them exists in asm/us/main.s; the game instead projects with single RTPS loops (finding 5), averages depth in software (z sums and shifts) and uses no hardware fog. Missing piece: data-section function-pointer tables are not part of asm/us/main.s, so an indirect call cannot be fully excluded.
- Evidence:
  - `asm/us/main.s:267`
  - `asm/us/main.s:155607`
  - `asm/us/main.s:155549`
  - `src/game/800648E8.c:205`
  - `src/game/800164F4.c:142`
- Units:
  - `src/game/800102A0.c` - manifest 0x800102A0, size 0x1DC (no readable binding)
  - `src/game/8001054C.c` - manifest 0x8001054C, size 0xE4 (no readable binding)
  - `src/game/80010630.c` - manifest 0x80010630, size 0xD4 (no readable binding)
  - `src/game/80095B20.c` - manifest 0x80095B20, size 0x20 (no readable binding)
  - `src/game/80095B40.c` - manifest 0x80095B40, size 0x24 (no readable binding)
- Naming proposals:
  - `func_800102A0` -> `gProjectVertexBatch3`: rtpt loop with the same clip-byte logic as 8001047C (800102A0.c:58); no direct caller.
  - `func_8001054C` -> `gProjectVertexBatch3Depth`: rtpt loop returning positions, depths and flags (8001054C.c:44); no direct caller.
  - `func_80010630` -> `gProjectVertexBatch3Xy`: rtpt loop storing only SXY (80010630.c:44); no direct caller.
  - `func_80095B20` -> `gGteAverageZ3`: avsz3 wrapper (asm/us/main.s:155607); no direct caller.
  - `func_80095B40` -> `gGteAverageZ4`: avsz4 wrapper (asm/us/main.s:155620); no direct caller.

### 24. Distance-based LOD or per-frame polygon budget
- Status: hypothesis
- Mechanism: No distance-selected model LOD and no explicit polygon counter was found. What exists is a fixed activity/draw radius (finding 9), cone culling (findings 7-8), a flag-selected cheap flat path for some nodes, and pool saturation (finding 3) as the only hard budget. Missing piece: a code path choosing a model variant by distance; the only 'LOD' text is a doc comment, not code.
- Evidence:
  - `src/game/8001A100.c:28`
  - `src/game/80072F7C.c:118`
- Units:
  - `src/game/80072F7C.c` - manifest 0x80072F7C, size 0x534 (current binding: gDrawFlatModelNode)
- Naming proposals:
  - `func_80072F7C` -> `gDrawFlatModelNode`: keeps current name: the cheap flat/line path of beam nodes, selected by node flag 0x4000 rather than distance.

## Citation index

Each entry is checked by `tests/test_performance_notes.py` (the fragment must
occur on the cited line).

| citation | code fragment |
|---|---|
| `src/game/8001B4A8.c:153` | `xbase = v & 0xFFFFFF;` |
| `src/game/8001B4A8.c:182` | `quot = (gNodeTableLength + v - min) / gNodeTableLength;` |
| `src/game/8001B4A8.c:185` | `v = gGteLeadingZeros(quot);` |
| `src/game/8001B4A8.c:213` | `max = ((max - min) >> shift) + 1;` |
| `src/game/8001B4A8.c:225` | `*(u32 *)xbase = v;` |
| `src/game/8001B4A8.c:362` | `func_80093280(table + (max << 2));` |
| `src/game/800648E8.c:214` | `*(u32 *)o = (*(u32 *)o & 0xFF000000) | (avg & 0xFFFFFF);` |
| `src/game/800164F4.c:146` | `packet->tag = (packet->tag & 0xff000000) | masked_depth;` |
| `src/game/800172A8.c:99` | `prim->w[0] = (prim->w[0] & 0xFF000000) | (arg3->s4 & 0xFFFFFF);` |
| `src/game/8001B2C8.c:89` | `gOrderingTableBuffers = gAllocMemory((arg1 + 2) * 4, 2);` |
| `src/game/80093178.c:62` | `gLogPrintf(gClearOTagFormat, (s32)arg0, arg1);` |
| `src/game/8001B2C8.c:81` | `gSecondPacketBufferBase = gAllocMemory(allocationSize, 2);` |
| `src/game/8001B2C8.c:90` | `gSecondOrderingTableBuffer = gAllocMemory((arg1 + 2) * 4, 2);` |
| `src/game/8001B4A8.c:121` | `while (gFrameBarrier) {` |
| `src/game/8001B4A8.c:365` | `gFrameBarrier = 1;` |
| `src/game/8001B4A8.c:384` | `gNodeTableIndex = mask;` |
| `src/game/8001B4A8.c:390` | `gPrimPoolNext = shift;` |
| `src/game/8001EFF8.c:61` | `if (gFrameBarrier != 0) {` |
| `src/game/8001EFF8.c:65` | `gFlipDisplayLayer();` |
| `src/game/8001EFF8.c:68` | `gDisplaySetupToggle = 1 - gDisplaySetupToggle;` |
| `src/game/800172A8.c:89` | `if (gPrimPoolNext < gPrimPoolEnd) goto L_alloc;` |
| `src/game/800172A8.c:95` | `gPrimPoolNext = (Prim17 *)((u8 *)prim + 0x34);` |
| `src/game/800648E8.c:207` | `if (gPrimPoolNext >= gPrimPoolEnd) {` |
| `src/game/800648E8.c:212` | `gPrimPoolNext = (u32 *)o + 13;` |
| `src/game/8006F21C.c:74` | `gPrimPoolNext = (Prim *)((u8 *)prim + 0x34);` |
| `src/game/8001B2C8.c:106` | `gPrimPoolEnd = value;` |
| `src/game/8001B2C8.c:112` | `gPacketLimit = value;` |
| `src/game/8001B4A8.c:393` | `gPacketCursor = val;` |
| `src/game/8001006C.c:46` | `".word 0x4A486012   /* mvmva      1, 0, 0, 3, 0 */\n"` |
| `src/game/8001006C.c:58` | `".word 0x480B4800   /* mfc2       $t3, $9 */\n"` |
| `src/game/8001006C.c:38` | `".word 0x00094903   /* sra        $t1, $t1, 4 */\n"` |
| `src/game/80010234.c:42` | `".word 0x4A480012   /* mvmva      1, 0, 0, 0, 0 */\n"` |
| `src/game/8001016C.c:46` | `".word 0x016A5822   /* sub        $t3, $t3, $t2 */\n"` |
| `src/game/80010750.c:42` | `".word 0x48CE0000   /* ctc2       $t6, $0 */\n"` |
| `src/game/80084398.c:226` | `gGteLoadMatrix(&o->unk04);` |
| `src/game/80084398.c:48` | `extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");` |
| `asm/us/main.s:71` | `/* 8A4 800100A4 1260484A */  mvmva      1, 0, 0, 3, 0` |
| `asm/us/main.s:210` | `/* A50 80010250 1200484A */  mvmva      1, 0, 0, 0, 0` |
| `src/game/8001047C.c:60` | `".word 0x4A180001   /* rtps */\n"` |
| `src/game/8001047C.c:65` | `".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"` |
| `src/game/8001047C.c:68` | `".word 0x484FF800   /* cfc2       $t7, $31 */\n"` |
| `src/game/8001047C.c:70` | `".word 0x004F1025   /* or         $v0, $v0, $t7 */\n"` |
| `src/game/8001047C.c:79` | `".word 0x29CF0400   /* slti       $t7, $t6, 0x400 */\n"` |
| `src/game/800648E8.c:187` | `if (f & 1) return;` |
| `src/game/800648E8.c:188` | `if (!(f & 2) && x2) return;` |
| `src/game/80013F08.c:270` | `gProjectVertexBatch(t0, a1_pts, flg, count);` |
| `src/game/800646BC.c:101` | `".word 0x4A180001   /* rtps */\n"` |
| `src/game/80044260.c:204` | `".word 0x4A180001   /* rtps */\n"` |
| `asm/us/main.s:388` | `/* CC8 800104C8 0100184A */  rtps` |
| `asm/us/main.s:98550` | `/* 54FB8 800647B8 0100184A */  rtps` |
| `src/game/800648BC.c:33` | `".word 0x4B400006   /* nclip */\n"` |
| `src/game/800648BC.c:36` | `".word 0x28420000   /* slti       $v0, $v0, 0x0 */\n"` |
| `src/game/800648E8.c:198` | `if (func_800648BC(pa0, pa1, pa2)) return;` |
| `src/game/80015AF4.c:35` | `((s16)second - (s16)origin) * ((first >> 16) - (origin >> 16)) < 0) {` |
| `src/game/80015B68.c:147` | `if (gEmitFrontFacingTriangle(vtx[i0].u.xy, vtx[i1].u.xy, vtx[i2].u.xy, work)) {` |
| `asm/us/main.s:98633` | `/* 550D0 800648D0 0600404B */  nclip` |
| `src/game/80066C9C.c:111` | `gGteLoadRotation(gCamera->matrix);` |
| `src/game/80064460.c:103` | `flags=gProbeMapCellVisibility(node,v);` |
| `src/game/80064460.c:113` | `out->mask=a|(((flags&0x8000)>>12)|(((u32)flags>>14)&4)|(((u32)flags>>18)&2)|(((u32)flags>>20)&1));` |
| `src/game/8006429C.c:51` | `func_800640D8(&temp, &transformed);` |
| `src/game/8006429C.c:52` | `result = gIsInViewFrustumWithMargin((s16 *)&transformed, data[0] << 4);` |
| `src/game/8006429C.c:66` | `result |= gBuildFourCellVisibilityMask(&point, 0x800, (char *)data + 0x40) << 16;` |
| `src/game/80063FF8.c:45` | `if (depth + margin < 0x601) {` |
| `src/game/80063FF8.c:58` | `if (limit < extentX) {` |
| `src/game/8006531C.c:79` | `if (mask & 1) gEmitCellGeometry(obj, base, dx, dy, a + b);` |
| `src/game/80084398.c:141` | `if (mask == 0) {` |
| `src/game/80084398.c:153` | `if (mask & o->unk4E) {` |
| `src/game/80083A10.c:426` | `node->unk4E = gBuildSubcellMask(dst[3].z, dst[3].x,` |
| `asm/us/main.s:98071` | `/* 548E8 800640E8 1260484A */  mvmva      1, 0, 0, 3, 0` |
| `src/game/80013AF0.c:83` | `instance->radius = model->frames->frames[0].radius << 4;` |
| `src/game/80013F08.c:192` | `if (gTestFrustumMargin(point, instance->unk40) != 2) {` |
| `src/game/80019E70.c:54` | `if (depth + margin < 0x406) {` |
| `src/game/80019E70.c:67` | `if (limit < extentX) {` |
| `src/game/80070158.c:143` | `if ((gTestFrustumFixedNear(dst, n->unk12 << 4) & 0xFF) != 2) {` |
| `src/game/80017508.c:42` | `visible = gProjectModelInstance(instance);` |
| `src/game/80019F54.c:60` | `return a + (b >> 2);` |
| `src/game/8001A074.c:54` | `sum = (sum << 6) + (sum << 3) + (sum << 1) + a + (b << 5);` |
| `src/game/8001A074.c:58` | `return a + (b >> 2) - (b >> 6);` |
| `src/game/8001A100.c:44` | `return func_8001A074(planar);` |
| `src/game/8003CF6C.c:77` | `if (gVectorLength3D(vec) - node->obj->unk40 > 0x1FFFF) {` |
| `src/game/80070158.c:129` | `if (gVectorLength3D(vec) - (n->unk12 << 4) > 0x1FFFF) {` |
| `src/game/8003FF7C.c:179` | `in_range = gVectorLength3D(&work.delta.x) <= 0x1FFFF;` |
| `src/game/8003FF7C.c:148` | `node = gDeferredSpawnList;` |
| `src/game/8003FF7C.c:203` | `gRetireDeferredNodes(node);` |
| `src/game/8003FF7C.c:207` | `func_8003FE0C(node);` |
| `src/game/8003FE0C.c:61` | `if (gSpawnObjectFromTable(object, 0, node->type, node->arg18,` |
| `src/game/8003FE90.c:58` | `node->callback(10, node, zero, zero);` |
| `src/game/800834C4.c:53` | `cells = gAllocMemory(count * 8, 2);` |
| `src/game/80084398.c:144` | `idx = x + a + gCellGridWidth * (y + b);` |
| `src/game/80084398.c:145` | `o = gCellBucketGrid[idx].head;` |
| `src/game/8003F530.c:34` | `"lui $sp, 0x1F80\n\t"` |
| `src/game/8003F530.c:35` | `"ori $sp, $sp, 0x3F8\n\t"` |
| `src/game/80026E1C.c:34` | `"lui $sp, 0x1F80\n\t"` |
| `src/game/8006531C.c:61` | `do { *dst++ = *src++; } while ((s32)dst <= 0x1F80006F);` |
| `src/game/800648E8.c:219` | `volatile u16 *d = (volatile u16 *)0x1F800070;` |
| `src/game/8004B5F8.c:242` | `bm = (u32 *)0x1F800000;` |
| `src/game/80085228.c:159` | `memcpy((void *)0x1F800000, gScratchTable, 0x100);` |
| `src/game/800861E4.c:179` | `*(Block100 *)0x1F800000 = gScratchTable;` |
| `src/game/8001846C.c:36` | `angle &= 0xFFF;` |
| `src/game/8001846C.c:38` | `s = gSineTable[index] << 1;` |
| `src/game/8001846C.c:39` | `c = gSineTable[0x400 - index] << 1;` |
| `src/game/80018534.c:43` | `index = 0x400 - index;` |
| `src/game/800188D0.c:47` | `gSinCos(x, &sx, &cx);` |
| `src/game/80095C30.c:72` | `result = gAtanTable[index];` |
| `src/game/80095C30.c:79` | `result = 0x400 - gAtanTable[index];` |
| `src/game/80095900.c:46` | `"mtc2\t$4, $30\n\t"` |
| `src/game/80095900.c:71` | `"lh\t$13, -0x20E0($13)\n\t"` |
| `src/game/8001A81C.c:4` | `#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9` |
| `src/game/8001A81C.c:46` | `FMUL(p0, x, b);` |
| `src/game/8001AB00.c:52` | `__asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(y));` |
| `src/game/8001AC28.c:49` | `__asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"` |
| `src/game/8001A180.c:97` | `shift = gGteLeadingZeros((u32)ay);` |
| `src/game/8001A180.c:122` | `scale = func_80019904(sum);` |
| `src/game/80019904.c:35` | `".word 0x4884F000   /* mtc2       $a0, $30 */\n"` |
| `src/game/80019904.c:46` | `".word 0x000A5043   /* sra        $t2, $t2, 1 */\n"` |
| `src/game/8001A314.c:85` | `scale = (s32)(0x80000000U / (u32)(t >> 1));` |
| `src/game/800648E8.c:232` | `c = t3 & 0xFC;` |
| `src/game/800648E8.c:275` | `e = cl[t3 & 0xFF];` |
| `src/game/800648E8.c:296` | `u8 *tab = gTilePatternBuf;` |
| `src/game/80015F14.c:39` | `"cop2 0x0108041B\n\t"` |
| `src/game/80015F38.c:97` | `gGteShadeNccs(light + part->index[0] * 8, &tmp, &prim->w[1]);` |
| `src/game/80013ECC.c:43` | `"ctc2 $t1, $8\n\t"` |
| `src/game/80013C80.c:39` | `gMulMatrixVector(gCamera + 0x28, gLightVectorIn, gLightVectorOut);` |
| `src/game/80027510.c:245` | `gUpdateLightDirection();` |
| `src/game/80013A14.c:49` | `gGteSetBackColor(0x40, 0x40, 0x40);` |
| `src/game/800959D0.c:29` | `"ctc2 $4, $13\n\t"` |
| `src/game/800959A0.c:30` | `"ctc2 $t0, $16\n\t"` |
| `asm/us/main.s:7393` | `/* 6728 80015F28 1B04084B */  nccs` |
| `src/game/8001EFF8.c:58` | `gGameClock += 5;` |
| `src/game/8002703C.c:72` | `} while (diff < 5);` |
| `src/game/8002703C.c:85` | `gFrameDelta = sum >> 2;` |
| `src/game/8002703C.c:93` | `if (gFrameDelta >= 0x3C) {` |
| `src/game/8002703C.c:113` | `gStepAnimations(gFrameDelta);` |
| `src/game/80028618.c:110` | `gFrameDelta = work;` |
| `src/game/8001D130.c:102` | `func_80010000.unk18 = gSetCdReadyCallback((s32)gCdSectorReadyCallbackAddress);` |
| `src/game/8001C808.c:170` | `if (len < 0x800) {` |
| `src/game/8001C808.c:179` | `func_8008CD10(q->buf, gLengthTable + n, cnt);` |
| `src/game/8001C808.c:184` | `if (!gCdRead((void *)q->buf, 0x200))` |
| `src/game/80029748.c:84` | `gQueueVramUpload(0, (u8 *)*frame + index, 0x3C0, 0x1CC, 0);` |
| `src/game/8001BBEC.c:52` | `entry = base + gUploadRingWriteIdx;` |
| `src/game/8001B4A8.c:349` | `gDecodeMdecFrameToVram(*(u32 *)xbase, *(u8 *)(xbase + 8),` |
| `src/game/800861E4.c:210` | `gMdecStartDecode(gMovieSliceContext->buffer28[gMovieSliceContext->index], gStripScaleEnable);` |
| `src/game/800861E4.c:215` | `gStartMdecOutDma(gMovieSliceContext->buffer24, ((s32)(width << 16) >> 16) * ((s32)(height << 16) >> 16) / 2);` |
| `src/game/800847E0.c:14` | `#define K(o, e, tp, i) o = (e); if ((o & 7) >= (tp)[i]) o += 8; o = gByteTable[o];` |
| `src/game/80022F44.c:43` | `gSpuSetIrqEnable(1);` |
| `src/game/80022FA8.c:71` | `gDisarmAudioStreamSpuIrq();` |
| `src/game/80022FA8.c:93` | `gSubmitAudioStreamTransfer();` |
| `src/game/80022D84.c:116` | `gMarkAudioStreamAdpcmBlock((void *)gMixerState.channels[channel].start, mode, gMixerState.channels[channel].length);` |
| `src/game/80022D84.c:121` | `req.callback = gCompleteAudioStreamTransfer;` |
| `src/game/80022D84.c:128` | `func_800212F8(&req);` |
| `src/game/80017EBC.c:132` | `case 2:` |
| `src/game/80017EBC.c:146` | `gFreeRangeTablePtr[index] += size;` |
| `src/game/80017EBC.c:164` | `gFreeRangeTablePtr[index + 1] -= size;` |
| `src/game/80017EBC.c:178` | `case 3:` |
| `src/game/80017EBC.c:190` | `result = func_800195A4((struct LinkState *)pool);` |
| `src/game/8003BEF0.c:47` | `node->tail = tail->previous;` |
| `src/game/8009570C.c:62` | `if (!((*gDma2ChannelCtrlPtr & 0x01000000) || !(*gGpuGp1PortPtr & 0x04000000))) goto success;` |
| `src/game/8009570C.c:72` | `gSetDmaCallback(2, callback);` |
| `src/game/8009570C.c:74` | `gVramWriteCallback->unk18(arg0);` |
| `src/game/80093280.c:50` | `dispatch->callback(dispatch->unk18, a1, zero, 0);` |
| `asm/us/main.s:267` | `/* B10 80010310 3000284A */  rtpt` |
| `asm/us/main.s:155607` | `/* 86330 80095B30 2D00584B */  avsz3` |
| `asm/us/main.s:155549` | `/* 86288 80095A88 2A00F84A */  dpct` |
| `src/game/800648E8.c:205` | `avg = (v0->h4 + v1->h4 + v2->h4 + v3->h4) >> 2;` |
| `src/game/800164F4.c:142` | `depth += center + (depth << 2);` |
| `src/game/8001A100.c:28` | `* the rest of the polygon/particle paths. Likely used for LOD and range tests.` |
| `src/game/80072F7C.c:118` | `if (node->flags & 0x4000) {` |

## Renames applied with this document

These 27 retail symbols were bound to the readable names proposed by the
findings above. Each binding is a declaration in the unit's own file
(`<name>(...) __asm__("func_<addr>");`), so the retail symbol, the object layout
and every instruction stay unchanged; the byte match is re-verified per unit.

| retail symbol | C name | unit |
|---|---|---|
| `func_8003B65C` | `gQuarterIndex` | `src/game/8003B65C.c` |
| `func_80049EF8` | `gIsTriangleFrontFacing` | `src/game/80049EF8.c` |
| `func_800536B4` | `gComputeDepthCueRange` | `src/game/800536B4.c` |
| `func_8006062C` | `gCosineClamped` | `src/game/8006062C.c` |
| `func_80063FF8` | `gIsInViewFrustumWithMargin` | `src/game/80063FF8.c` |
| `func_80069A38` | `gSampleHeightGridBilinear` | `src/game/80069A38.c` |
| `func_800886C8` | `gMinS32` | `src/game/800886C8.c` |
| `func_80091CC0` | `gInitDmaCallbacks` | `src/game/80091CC0.c` |
| `func_80091F60` | `gSetGpuDrawOffset` | `src/game/80091F60.c` |
| `func_80091F74` | `gGetGpuDrawOffset` | `src/game/80091F74.c` |
| `func_8009226C` | `gGetClutId` | `src/game/8009226C.c` |
| `func_80092340` | `gIsPrimListEnd` | `src/game/80092340.c` |
| `func_8009235C` | `gAddPrim` | `src/game/8009235C.c` |
| `func_800923D4` | `gSetPrimNextLink` | `src/game/800923D4.c` |
| `func_800923F8` | `gSetPrimListEnd` | `src/game/800923F8.c` |
| `func_80092410` | `gSetPrimSemiTrans` | `src/game/80092410.c` |
| `func_80092438` | `gSetPrimRawTexture` | `src/game/80092438.c` |
| `func_800930B0` | `gClearOrderingTable` | `src/game/800930B0.c` |
| `func_80093178` | `gClearOrderingTableReverse` | `src/game/80093178.c` |
| `func_80093488` | `gCopyDrawEnv` | `src/game/80093488.c` |
| `func_800938F0` | `gCopyDispEnv` | `src/game/800938F0.c` |
| `func_80094960` | `gSendGpuWordList` | `src/game/80094960.c` |
| `func_80095830` | `gMemset` | `src/game/80095830.c` |
| `func_800959A0` | `gGteSetColorMatrix` | `src/game/800959A0.c` |
| `func_800959D0` | `gGteSetBackColor` | `src/game/800959D0.c` |
| `func_800959F0` | `gGteSetScreenOffset` | `src/game/800959F0.c` |
| `func_800966B0` | `gDivSigned64` | `src/game/800966B0.c` |

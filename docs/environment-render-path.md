# Environment Render Path

How one gameplay frame draws the environment (terrain tiles, map-cell objects,
cell models) and the object lists on top of it, derived from the matching C
units in `src/game/`. Every `src/game/<unit>.c:<line>` below was resolved by a
text search in the current sources and is listed with its exact code fragment
in the [citation index](#citation-index); `tests/test_environment_path.py`
re-checks that each fragment is still on its cited line.

Readable names are the C names bound to the retail symbol with
`__asm__("func_XXXXXXXX")`. Handwritten (GTE) units marked *(asm)* export only
`.globl func_XXXXXXXX`; their readable name is the one their callers declare.

## 1. Overview: frame order

The order below is the order of the calls in the code. One correction against
the commonly quoted chain "gRunViewScenePasses -> gRunSceneListPasses ->
gRunPolygonPass": `gRunViewScenePasses` does **not** call
`gRunSceneListPasses`. The scene-list passes (ending in `gRunPolygonPass`) are
reached from the entity world tick, which the frame driver runs *before* the
region pass.

1. **Game loop** - `gRunGameFrameLoop` (`0x80028618`) runs one frame driver per
   iteration: `gRunSoloSessionFrame` (`0x800271A4`) or `gRunSessionFrame`
   (`0x80027510`). Evidence: `src/game/boot-frontend/gRunGameFrameLoop.c:131`, `src/game/boot-frontend/gRunGameFrameLoop.c:133`.
2. **Entity world tick** - the driver calls `gRunEntityWorldTick`
   (`0x8003F570`), which ends with `gRunSceneListPassesOnScratch`
   (`0x8003F530`); that wrapper switches to the scratchpad stack and calls
   `gRunSceneListPasses` (`0x8003F4A0`). Evidence: `src/game/boot-frontend/gRunSessionFrame.c:231`, `src/game/gameplay-ai/gRunSoloSessionFrame.c:183`, `src/game/boot-frontend/gRunEntityWorldTick.c:103`, `src/game/boot-frontend/gRunSceneListPassesOnScratch.c:36`.
3. **Scene-list passes** - `gRunSceneListPasses` makes eight calls over
   `gListHeads` (`D_800A1070`), e.g. `gTickEffectNodes` on list 2,
   `gUpdateObjectLights` on list 0, and last `gRunPolygonPass` (`0x80075A84`)
   on list 5, which culls/lights each particle node and draws it through
   `func_800751A4`. Evidence: `src/game/renderer-scene/gRunSceneListPasses.c:45`, `src/game/renderer-scene/gRunSceneListPasses.c:47`, `src/game/renderer-scene/gRunSceneListPasses.c:50`.
4. **Camera anchor + cell window** - per present player, `func_80026E5C`
   runs `gUpdateCameraAnchor` (`0x80066C9C`) on the scratchpad. It selects the
   view record, loads the camera rotation and calls `gSampleCellWindow`
   (`0x80064460`, emitted as the raw word `0x0c019118` = `jal 0x80064460`) to
   fill the 25-entry window block in `D_800AB3B8` that the three layers read.
   Evidence: `src/game/boot-frontend/gRunSessionFrame.c:233`, `src/game/gameplay-ai/gRunSoloSessionFrame.c:190`, `src/game/boot-frontend/gUpdateViewAnchorsOnScratchpad.c:37`, `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:110`, `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:111`, `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:194`.
5. **View select + light** - before each region pass the driver selects that
   player's view record (`gSelectViewRecord`, `0x80017B60`) and calls
   `gUpdateLightDirection` (`0x80013C80`). In the solo driver the light update
   comes just before the anchor call (step 4), not after it. Evidence: `src/game/boot-frontend/gRunSessionFrame.c:244`, `src/game/boot-frontend/gRunSessionFrame.c:245`, `src/game/boot-frontend/gRunSessionFrame.c:283`, `src/game/boot-frontend/gRunSessionFrame.c:284`, `src/game/gameplay-ai/gRunSoloSessionFrame.c:189`.
6. **Region pass** - `gRunRegionPass` (`0x80026E1C`, player index 0 or 1)
   moves to the scratchpad stack and calls `gDrawViewLayers`. Evidence:
   `src/game/boot-frontend/gRunSessionFrame.c:250`, `src/game/boot-frontend/gRunSessionFrame.c:288`, `src/game/gameplay-ai/gRunSoloSessionFrame.c:194`, `src/game/boot-frontend/gRunRegionPass.c:36`.
7. **Three layers** - `gDrawViewLayers` (`0x80066E98`) selects the view
   record, uploads the camera rotation (`gGteLoadRotation`, `0x80010820`, on
   `gCamera + 0x28`), draws layer 1 `gDrawRegionTiles` (`0x8006531C`),
   re-uploads the rotation, then layer 2 `gDrawCellWindowObjects`
   (`0x8004A924`) and layer 3 `gDrawCellWindowModels` (`0x80084398`), all on
   the same `(x, y, region)` triple from `D_800AB3B8`. Evidence: `src/game/gpu-2d-hud/gDrawViewLayers.c:63`, `src/game/gpu-2d-hud/gDrawViewLayers.c:64`, `src/game/gpu-2d-hud/gDrawViewLayers.c:68`, `src/game/gpu-2d-hud/gDrawViewLayers.c:69`, `src/game/gpu-2d-hud/gDrawViewLayers.c:70`, `src/game/gpu-2d-hud/gDrawViewLayers.c:71`.
   - Layer 1 internals: `src/game/gpu-2d-hud/gDrawRegionTiles.c:72`, `src/game/gpu-2d-hud/gDrawRegionTiles.c:74`, `src/game/gpu-2d-hud/gDrawRegionTiles.c:76`, `src/game/gpu-2d-hud/gDrawRegionTiles.c:79` (tileset pattern, colour word,
     `gEmitTexturedSurface` `0x800646BC`, `gEmitCellGeometry` `0x80065134`).
   - Layer 2 internals: `src/game/renderer-scene/gDrawCellWindowObjects.c:91` (`gDrawMapCellNode`, `0x8004A048`).
   - Layer 3 internals: `src/game/renderer-scene/gDrawCellWindowModels.c:181`, `src/game/renderer-scene/gDrawCellWindowModels.c:226`, `src/game/renderer-scene/gDrawCellWindowModels.c:231`, `src/game/renderer-scene/gDrawCellWindowModels.c:235`, `src/game/renderer-scene/gSubmitModelInstance.c:42`, `src/game/renderer-scene/gSubmitModelInstance.c:44` (frame, `gGteLoadMatrix` +
     `gGteTransformVerticesWide`, then `gSubmitModelInstance` ->
     `gProjectModelInstance` / `gDrawModelInstanceParts`).
8. **Close of the view** - `gRunRegionObjectPass` (`0x80062EB8`, draws
   visible entities via `gDrawModelFrame` `src/game/gpu-2d-hud/gRunRegionObjectPass.c:72`) and
   `gRunViewScenePasses` (`0x8003F6E4`), which resolves the view position and
   draws the object lists (`gDrawNodeListModels`, `gDrawDebrisFragments`,
   `gDrawGlowNodeList`, `gDrawRibbonList`, plus five still-unnamed `func_` calls).
   Evidence: `src/game/gpu-2d-hud/gDrawViewLayers.c:72`, `src/game/gpu-2d-hud/gDrawViewLayers.c:73`, `src/game/renderer-scene/gRunViewScenePasses.c:89`, `src/game/renderer-scene/gRunViewScenePasses.c:90`, `src/game/renderer-scene/gRunViewScenePasses.c:93`, `src/game/renderer-scene/gRunViewScenePasses.c:94`.

## 2. Camera / view

| step | function | what it does | evidence |
|---|---|---|---|
| placement | `gPlacePlayerViews` `0x80056634`, `gPlaceCameraFromPreset` `0x8005678C`, `gUpdateChaseCamera` `0x800569EC`, `gResetActorAndPlaceView` `0x800573A4` | the four view placements; each calls both setters | `src/game/gpu-2d-hud/gPlacePlayerViews.c:111`, `src/game/gpu-2d-hud/gPlacePlayerViews.c:112`, `src/game/gpu-2d-hud/gPlaceCameraFromPreset.c:83`, `src/game/gpu-2d-hud/gPlaceCameraFromPreset.c:84`, `src/game/gpu-2d-hud/gUpdateChaseCamera.c:512`, `src/game/gpu-2d-hud/gUpdateChaseCamera.c:563`, `src/game/gameplay-ai/gResetActorAndPlaceView.c:283`, `src/game/gameplay-ai/gResetActorAndPlaceView.c:284` |
| setter 1 | `gSetCameraPosition` `0x800177AC` | writes the eye position of the active view record and its rotated (inverse) form through `gMulMatrixVector` | `src/game/gpu-2d-hud/gSetCameraPosition.c:55` |
| setter 2 | `gSetCameraRotation` `0x80017860` | yaw/pitch/roll -> 3x3 rotation (`gBuildRotationMatrix` `0x800188D0`), then the view matrix (`gBuildViewMatrix` `0x80018860`) | `src/game/gpu-2d-hud/gSetCameraRotation.c:49`, `src/game/gpu-2d-hud/gSetCameraRotation.c:50` |
| matrix build | `gBuildViewMatrix` `0x80018860` | transposes the rotation (`gTransposeMatrix3x3` `0x80019470`) and rotates the translation in software (`gMulMatrixVector` `0x8001A81C`, no GTE) | `src/game/gte/gBuildViewMatrix.c:56`, `src/game/gte/gBuildViewMatrix.c:58` |
| angles | `gBuildRotationMatrix` `0x800188D0` | sine/cosine pairs from `gSinCos` `0x8001846C` | `src/game/utilities/gBuildRotationMatrix.c:47` |
| GTE upload | `gGteLoadRotation` `0x80010820` (asm) | uploads the camera rotation (no translation); the environment pass calls it before layer 1 and again before layers 2/3 | `src/game/gpu-2d-hud/gDrawViewLayers.c:64`, `src/game/gpu-2d-hud/gDrawViewLayers.c:69` |
| GTE upload (models) | `gGteLoadMatrix` `0x80010750` (asm) | uploads matrix + translation, rows y/z negated; used per model instance (e.g. layer 3), **not** by `gDrawViewLayers` for the camera | `src/game/renderer-scene/gDrawCellWindowModels.c:226` |
| view record selection | `gSelectViewRecord` `0x80017B60` | `gCamera = gFindInstanceById(id)` (`0x80017B28`); called by the driver, the camera anchor and `gDrawViewLayers` | `src/game/gpu-2d-hud/gSelectViewRecord.c:31`, `src/game/boot-frontend/gRunSessionFrame.c:244`, `src/game/gpu-2d-hud/gDrawViewLayers.c:63` |
| light | `gUpdateLightDirection` `0x80013C80` | light vector = camera matrix x light input (software); must follow the setters | `src/game/renderer-scene/gUpdateLightDirection.c:39` |

Note: the `@par Environment path` paragraph of `0x80010750` calls it
"minimal-environment step 4" (view matrix upload). In the code the camera upload
of the environment pass is `gGteLoadRotation` (`0x80010820`); `gGteLoadMatrix`
is the per-instance upload. The paragraph is kept as written in `src/`; this
page follows the call sites.

## 3. Minimal scene

Short list - what has to run for terrain to appear through a camera:

1. `gCreateCamera` `0x80017994` - a view record exists in the camera list `D_8009F588`, which `gFindInstanceById` `0x80017B28` walks by id.
2. `gSetCameraPosition` `0x800177AC` + `gSetCameraRotation` `0x80017860`
   (with `gBuildRotationMatrix` `0x800188D0`, `gSinCos` `0x8001846C`,
   `gBuildViewMatrix` `0x80018860`, `gTransposeMatrix3x3` `0x80019470`,
   `gMulMatrixVector` `0x8001A81C`).
3. `gUpdateCameraAnchor` `0x80066C9C` -> `gSampleCellWindow` `0x80064460` - fills the cell window the layers read.
4. `gSelectViewRecord` `0x80017B60`, `gGteLoadRotation` `0x80010820`.
5. `gRunRegionPass` `0x80026E1C` -> `gDrawViewLayers` `0x80066E98` -> layer 1
   `gDrawRegionTiles` `0x8006531C` (+ `gBuildTilePattern` `0x800654F8`,
   `gWorldToRadarCell` `0x80065744`, `gEmitTexturedSurface` `0x800646BC`,
   `gEmitCellGeometry` `0x80065134`).

Optional (the scene still shows terrain without them):

- Layer 2 `gDrawCellWindowObjects` `0x8004A924` / `gDrawMapCellNode` `0x8004A048` - walls and props as map-cell objects.
- Layer 3 `gDrawCellWindowModels` `0x80084398` and the model path
  (`gSubmitModelInstance` `0x80017508`, `gProjectModelInstance` `0x80013F08`,
  `gDrawModelInstanceParts` `0x80016A64`, `gPrepareModelPart` `0x80015B68`,
  `gEmitFrontFacingTriangle` `0x80015AF4`, batch transforms `0x8001006C`,
  `0x8001010C`, `0x80010234`, `0x8001047C`).
- `gUpdateLightDirection` `0x80013C80` - light vector for shading; the three layers do not call it, whether their output reads its result is not tested.
- `gRunRegionObjectPass` `0x80062EB8`, `gRunViewScenePasses` `0x8003F6E4`,
  `gRunSceneListPasses` `0x8003F4A0` / `gRunPolygonPass` `0x80075A84` - actors,
  effects and particles over the terrain.
- Everything else in the table below (entity handlers, effects, collision, minimap).

The split is a reading of the call graph, not a tested claim: no test in this
repo renders a minimal scene.

## 4. Test table

Test kinds:

- **vector** - the unmodified decomp C unit is compiled with the retail
  compiler and run under `qemu-mipsel` by a script in `tests/host/`, compared
  against shared golden vectors (`../future_cop_dotnet/tests/golden/*.vectors`,
  a sibling repo that must be present). Proves: on those inputs the compiled C
  returns the expected outputs. Runs on 2026-11-18: `env_test.sh` "all 58
  vectors PASS (retail C + retail GTE asm words, cop2 via gte_ref-model trap)",
  `rotation_test.sh` "all shared vectors PASS", `compose_test.sh` "all shared
  vectors PASS (GTE software model)", `joints_test.sh` "all shared vectors PASS
  (7 cases, GTE software model)", `build.sh` "ALL PASS".
- **contract** - covered only by `tests/test_environment_path.py`: the unit has
  its `@par Environment path` paragraph, a readable name bound to its retail
  symbol, and every `func_` it cites exists. Proves documentation/symbol
  consistency only, nothing about behaviour.
- **skip** - no behavioural test, with the reason given.

All units additionally sit in the byte-exact matching build, which proves the
code assembles to the retail bytes, not what it computes.

**GTE caveat:** no test runs PS1 hardware. Two things are covered and should not
be confused: (a) the retail instruction words of the handwritten units now do
execute under qemu - `env_harness.c` traps exactly the cop2 instruction,
computes it with the `tests/host/gte_ref.c` model and resumes, so loops, bit
packing, sign handling and stores are the retail code; (b) the GTE arithmetic
itself is that software model, derived from psx-spx and the .NET port. Where a
harness links a unit that is not in the link (stand-ins in `compose_harness.c`),
the unit itself does not run. A passing vector proves agreement with those
models and the golden files, not with a real GTE. Known model deviation: the
clip byte of `gte_ref.c` compares `(s16)sx < width`, while the retail unit
zero-extends before comparing, so a negative SX never sets bit 0x40; the PROJ
vector records the retail result (0x81) and does not rely on the model there.

Totals: 18 vector, 326 contract, 2 skip (346 units).

| address | readable name | test | role (from `@par Environment path`) | proved by / reason |
|---|---|---|---|---|
| `0x80010030` | `gSetGeomOffsetAndH` (asm) | contract | Sets the GTE projection centre and distance: it forwards (x, y) to func_800959F0, which writes x << 16 and y << 16 to... | structure only |
| `0x8001006C` | `gGteTransformWords` (asm) | vector | Batch vertex transform with 12-byte 32-bit vertices | `tests/host/env_test.sh` (env.vectors, XF6C 3 cases): retail asm words, cop2 model |
| `0x8001010C` | `gGteTransformVertices` (asm) | vector | Batch vertex transform of 8-byte vertices | `tests/host/env_test.sh` (env.vectors, XF10C 2 cases): retail asm words, cop2 model |
| `0x800101D4` | `gGteTransformVerticesV0` (asm) | vector | Same contract as the 8-byte batch transform, but it multiplies the GTE V0 vector instead of IR. Called by the light s... | `tests/host/env_test.sh` (env.vectors, XF1D4 2 cases): retail asm words, cop2 model |
| `0x80010234` | `gGteTransformVerticesWide` (asm) | vector | Batch vertex transform with 32-bit output and stride 0xC. Its 39 callers are the map-cell drawers (func_8002B588, fun... | `tests/host/env_test.sh` (env.vectors, XF234 2 cases): retail asm words, cop2 model |
| `0x8001047C` | `gProjectVertexBatch` (asm) | vector | The batched RTPS projection step: it loads each 8-byte source vertex into VXY0/VZ0, runs rtps, stores SXY2 and SZ3 in... | `tests/host/env_test.sh` (env.vectors, PROJ 5 cases): retail asm words, cop2 model, screen-width read handled per case |
| `0x80010704` | `gGteProjectPoint` | contract | Perspective-transforms one vertex with GTE RTPS using the currently loaded rotation/translation and stores the screen... | structure only |
| `0x80010750` | `gGteLoadMatrix` (asm) | vector | The per-model matrix upload: moves a finished model matrix into the GTE (rows 1..3 as F*M^T with F = diag(1,-1,1), y ... | `tests/host/env_test.sh` (env.vectors, LOADMAT 2 cases): retail asm words run, cop2 trap feeds a gte_ref model |
| `0x80010820` | `gGteLoadRotation` (asm) | vector | Same upload without translation, i.e | `tests/host/env_test.sh` (env.vectors, LOADROT 2 cases): retail asm words, cop2 model |
| `0x800108D8` | - (asm) | skip | Cross product variant that restores the caller's GTE matrix afterwards | no caller in the US binary and no readable binding; only checks (a)/(c) apply |
| `0x80010974` | `gGteCrossProduct` (asm) | contract | Cross product used to build a normal from two vectors: the light setups (func_800192AC, func_80019310, func_8001937C,... | structure only |
| `0x800136C8` | `gStepAnimations` | contract | Advances the two global texture-animation lists once per frame | structure only |
| `0x80013AA0` | `gSetModelUvOffset` | contract | Stores (high << 8) / low into D_8009F580, the UV offset that the model part emitters add to every u/v coordinate (fun... | structure only |
| `0x80013AF0` | `gInitModelInstance` | contract | Binds a model to a model instance: it stores the model pointer, copies the frame-0 radius shifted left by four togeth... | structure only |
| `0x80013C80` | `gUpdateLightDirection` | contract | Derives the frame's light direction from the camera matrix | structure only |
| `0x80013CB8` | `gComposeBoneMatrix` | vector | Builds the local transform of one hierarchy level and composes it into the bone matrix array D_800A0300: each of the ... | `tests/host/env_test.sh` (env.vectors, COMPOSE 3 cases) and `compose_test.sh`; GTE callees are software stand-ins |
| `0x80013ECC` | `gGteSetLightVector` | contract | Loads one light direction into the GTE light matrix: control register 8 gets (x >> 4) / (y >> 4) << 16 (L11, L12), re... | structure only |
| `0x80013F08` | `gProjectModelInstance` | contract | Computes one model instance's camera-relative matrix, tests the frame's reference vertex against the view volume and ... | structure only |
| `0x80014164` | `gComposeTransform` | contract | Composes a part transform with its owner's matrix | structure only |
| `0x8001428C` | `gProjectBoneParts` | contract | Transforms and projects a jointed model part by part without emitting any primitive | structure only |
| `0x80014490` | `gEmitRayBurst` | contract | Emits a ray burst: one 0x1C-byte header packet (draw mode 0xE10006A0) and a set of semi-transparent Gouraud triangles... | structure only |
| `0x8001484C` | `gEmitGouraudDisc` | contract | Emits one gouraud-shaded triangle fan as a filled disc: it returns immediately when depth minus the depth offset is n... | structure only |
| `0x80014C64` | `gDrawBeamQuad` | contract | Projects two world points relative to the camera centre, loads the camera matrix with func_80010820, projects both po... | structure only |
| `0x80015830` | `gBuildLineQuad` | contract | Builds the screen-space quad of a type-7 model part from two transformed vertices: it rejects the part when either en... | structure only |
| `0x80015AF4` | `gEmitFrontFacingTriangle` | vector | Backface/winding test for a triangle in packed 16.16 coordinates | `tests/host/build.sh` (harness.c vs render.vectors): `w_keep`, `w_cull`, `w_flat` |
| `0x80015B68` | `gPrepareModelPart` | vector | Prepares one model part: backface and on-screen rejection, then the screen-space corners and the depth into the work ... | `tests/host/build.sh` (harness.c vs render.vectors): `p_tri`, `p_tri_bias`, `p_tri_clip1..3`, `p_quad`, `p_bill` |
| `0x80015F14` | `gGteShadeNccs` | contract | Shades one vertex normal with the GTE NCCS command (light matrix, background colour, then multiplied by the input col... | structure only |
| `0x80015F38` | `gEmitShadedPart` | contract | Turns one prepared model part into a single GPU packet from the gPrimPoolNext pool: the part flags select flat/gourau... | structure only |
| `0x800164F4` | `gDrawModelFrame` | contract | Draws one animated model frame as textured, depth-cued triangles | structure only |
| `0x80016A64` | `gDrawModelInstanceParts` | vector | Draws the masked parts of one model instance, with a billboard path for flag-0x40 models | `tests/host/build.sh` (harness.c vs render.vectors): `k_mask8`, `k_maskf7` |
| `0x80016C88` | `gComputeEnvMapUV` | contract | Computes one environment-map texture coordinate pair for a polygon corner: the vector from the vertex to the referenc... | structure only |
| `0x80016E6C` | `gEmitEnvMapPolygon` | contract | Emits one environment-mapped textured polygon for a model part: it takes a 0x34-byte packet from gPrimPoolNext (bound... | structure only |
| `0x80017058` | `gDrawModelPartsMasked` | contract | Part-submission stage of the jointed-model path: func_800175A0 calls it right after func_8001428C has projected the p... | structure only |
| `0x800172A8` | `gEmitFlatShadedPoly` | contract | Allocates one 0x34-byte packet from gPrimPoolNext/gPrimPoolEnd and emits a flat-shaded triangle or quad (codes 0x20, ... | structure only |
| `0x80017508` | `gSubmitModelInstance` | contract | Visibility gate and submission for one model instance | structure only |
| `0x800175A0` | `gProjectAndRenderMesh` | contract | Updates skinned model bone parts via gProjectBoneParts and submits the model's part list for rendering via gDrawModel... | structure only |
| `0x800175F8` | `gSetModelFrame` | contract | Commits a new animation frame on a model instance: it returns when the frame index is unchanged, otherwise it stores ... | structure only |
| `0x80017668` | `gTransformPartVertex` | contract | Fetches one vertex of a model part for the instance's current animation frame and transforms it: gGteLoadMatrix (func... | structure only |
| `0x80017764` | `gFreeCameraList` | contract | Walks the camera record list D_8009F588 (0x94-byte records, next pointer at +0x8C) and frees each record with gFreeBl... | structure only |
| `0x800177AC` | `gSetCameraPosition` | contract | Minimal-environment step 1: writes the eye position of a view record and its inverse | structure only |
| `0x80017860` | `gSetCameraRotation` | contract | Minimal-environment step 2: turns yaw/pitch/roll of a view record into its 3x3 matrix and the uploadable view matrix ... | structure only |
| `0x800178DC` | `gTransformToViewSpace` | contract | Transforms four vertices into view space relative to the eye | structure only |
| `0x80017994` | `gCreateCamera` | contract | Allocates a 0x94-byte camera record with func_80017EBC(0x94, 2), initialises both of its 12-word matrix blocks (+0x28... | structure only |
| `0x80017B28` | `gFindInstanceById` | contract | Resolves a draw instance by its id (+0x88) | structure only |
| `0x80017B60` | `gSelectViewRecord` | contract | Picks the view record the environment is drawn from | structure only |
| `0x80017EBC` | `gAllocMemory` | contract | The universal allocator: mode 0 goes to the general allocator func_8001E240, modes 1 and 2 carve from the front or th... | structure only |
| `0x800181D4` | `gFreeBlock` | contract | Frees a block obtained from func_80017EBC: it first tests whether the block lies inside one of the three fixed pools ... | structure only |
| `0x8001846C` | `gSinCos` | vector | Sine/cosine pair of the 4096-step table: the rotation factories func_800188D0/func_80018DC0 and the object builders c... | `tests/host/env_test.sh` (env.vectors, SINCOS 12 cases); sine table read from the retail ROM |
| `0x80018534` | `gSineLookup` | contract | Sine lookup over the quarter-wave table gSineTable: the angle is masked to 12 bits, the index is mirrored as 0x400 - ... | structure only |
| `0x80018860` | `gBuildViewMatrix` | vector | Builds the view matrix from a pose record | `tests/host/env_test.sh` (env.vectors, VIEW 2 cases) |
| `0x800188D0` | `gBuildRotationMatrix` | vector | Rotation factory behind the camera setter: three angles in 4096 units per turn produce the 16.16 matrix the view path... | `tests/host/env_test.sh` (env.vectors, ROT 9 cases) |
| `0x80018DC0` | `gBuildRotationMatrixVariant` | vector | Rotation factory with the same contract as func_800188D0 but a shifted multiply sequence | `tests/host/env_test.sh` (env.vectors, ROTV 9 cases) and `rotation_test.sh` |
| `0x800192AC` | `gBuildBasisFromPair` | contract | Builds a 3x3 basis from two vectors: rows[0] is first x second normalised by func_8001A180, rows[2] is rows[0] x firs... | structure only |
| `0x80019310` | `gBuildBasisZFromPair` | contract | Builds a 3x3 basis whose third row is the first vector: rows[0] = second x first normalised by func_8001A180, rows[1]... | structure only |
| `0x8001937C` | `gBuildBasisFromAxisYZ` | contract | Builds a 3x3 basis whose third row is the axis, with the first row (0, -axis.z, axis.x) normalised by func_8001A180 a... | structure only |
| `0x800193F8` | `gBuildBasisFromAxis` | contract | Builds a 3x3 basis whose third row is the given axis: the first row is the horizontal perpendicular (axis.z, 0, -axis... | structure only |
| `0x80019470` | `gTransposeMatrix3x3` | vector | Transposes a 3x3 s32 matrix into the destination with nine element assignments and no loop, branch or call | `tests/host/env_test.sh` (env.vectors, TRANSPOSE 2 cases) |
| `0x800195F0` | `gCountTrailingZeros` | contract | Pure bit utility that feeds the texture descriptor path, not a draw routine: returns 0..31 as the lowest set bit posi... | structure only |
| `0x8001996C` | `gParseTextureContainer` | contract | Parses a chunked texture container in memory and wires up the pixel and CLUT pointers for each texture entry, returni... | structure only |
| `0x80019BA4` | `gReleaseAllResources` | contract | Releases every node of the resource registry D_8009F5B8 through func_80019AF8, which runs the node's release callback... | structure only |
| `0x80019D88` | - | contract | Empty function (jr $ra | structure only |
| `0x80019D90` | `gTestFrustumFixedNear` | contract | Frustum visibility test for one point plus a safety margin, with a fixed near threshold: depth = z << 4 returns 1 as ... | structure only |
| `0x80019E70` | `gTestFrustumMargin` | contract | View-frustum test for a view-space s16 point (x, y, z) with an extent margin: it returns 1 when z*16 + margin < 0x406... | structure only |
| `0x80019F54` | `gApproxVectorLength` | contract | Cheap 3D length estimate: the absolute components are sorted and the result is max + (mid + min) / 4, with no multipl... | structure only |
| `0x8001A100` | `gVectorLength3D` | contract | Returns an integer approximation of the 3D length of a vector by applying the 2D length helper func_8001A074 to (x, y... | structure only |
| `0x8001A180` | `gNormalizeVector` | contract | Normalises a three-component s32 vector in place: it finds the largest absolute component, uses the GTE leading-zero ... | structure only |
| `0x8001A314` | `gNormalizeGetLength` | contract | Normalises a three-component vector in place to 16.16 unit length and returns its original length: it pre-scales smal... | structure only |
| `0x8001A6F8` | `gEaseVectorQuarter` | contract | Moves a three-component s32 vector a quarter of the way toward a target in place: for each axis dst = (3*dst + src) >... | structure only |
| `0x8001A81C` | `gMulMatrixVector` | vector | Software matrix-times-vector in 16.16 used whenever the GTE must not be disturbed, e.g | `tests/host/env_test.sh` (env.vectors, MULMV 3 cases) |
| `0x8001AB00` | `gVectorDot` | contract | Returns the 16.16 fixed-point dot product of two three-component vectors (middle 32 bits of each 64-bit product, summ... | structure only |
| `0x8001AB78` | `gVectorScaleInPlace` (asm) | contract | Hand-written in-place scale of a three-component vector by a 16.16 factor (argument order: scale, vector) | structure only |
| `0x8001AC28` | `gVectorMulAdd` | contract | Computes out = a + b * scale per component with the middle 32 bits of the 64-bit product (16.16 fixed point) | structure only |
| `0x8001C148` | `gEmitSolidTileRect` | contract | Emits a 2D filled rectangle (GP0 0x60/0x62 TILE) paired with a texpage word for the given RGB color, HalfRect bounds,... | structure only |
| `0x8001C214` | `gEmitTranslucentTileRect` | contract | Converts 15-bit color and screen coordinates (x, y, w, h) and delegates to func_8001C148 to emit a solid or semi-tran... | structure only |
| `0x8001F0FC` | `gSetScreenModeHook` | contract | Registers the screen-mode/subsystem frame callback pointer once if not already set and returns registration status (c... | structure only |
| `0x800211FC` | `gInitSpuTransferHook` | contract | Not on the draw path: it hooks the transfer callback of the sound side | structure only |
| `0x80021B50` | `gResetCvkSubsystem` | contract | Clears the 8-entry sprite/asset subsystem table and invokes SPU and subsystem initialization/reset helpers func_80021... | structure only |
| `0x8002333C` | `gInitCvkStreamSubsystem` | contract | Brings up the CVK sprite/asset subsystem by resetting tables via func_80021B50, registering the frame callback via fu... | structure only |
| `0x80023478` | `gShutdownCvkSystem` | contract | Not on the draw path: it is subsystem teardown | structure only |
| `0x80023930` | `gRegisterCvkChunk` | contract | Registers asset chunks routed here by gLoadQueuedAssets (func_80024EC8, 80024EC8.c:185-188): a 'Cvkh' (0x43766B68) no... | structure only |
| `0x80024EC8` | `gLoadQueuedAssets` | contract | Drains finished asset requests and dispatches them by four-character tag: every node returned by func_800295E8 is app... | structure only |
| `0x80025370` | `gShowLoadingScreen` | contract | Loading-screen draw step: only when gLoadScreenRequest is 0 and gLoadScreenIndex is below 23, it formats the file nam... | structure only |
| `0x8002549C` | `gEmitSplitScreenBlit` | contract | Allocates four 28-byte packets from the gPacketCursor pool (bounded by gPacketLimit, reusing the last slot when full)... | structure only |
| `0x80025690` | `gDrawScreenFadeQuad` | contract | Draws the front-end transition progress/fade bar driven by func_80028FD4() | structure only |
| `0x800260B8` | `gDrawOptionsSubPanel` | contract | Draws two blended backdrop rectangles via func_8001C214 and prints the six-row options list via func_80049100 using g... | structure only |
| `0x800267C8` | `gUpdateMapScreenView` | contract | Per-player, per-frame update of the in-game map screen, run on the scratchpad stack by gRunMapScreenOnScratch (func_8... | structure only |
| `0x80026DDC` | `gRunMapScreenOnScratch` | contract | Inline-assembly trampoline: it pushes $ra, saves $sp in D_8009FAD8, moves the stack to 0x1F8003F0 at the top of the s... | structure only |
| `0x80026E1C` | `gRunRegionPass` | contract | Runs the environment pass func_80066E98 on the scratchpad stack | structure only |
| `0x80026E9C` | `gEmitScreenFadeBar` | contract | Allocates two 28-byte packets from the gPacketCursor pool, both with link field 0: texpage E10006A0 (page 0,0, blend ... | structure only |
| `0x800271A4` | `gRunSoloSessionFrame` | contract | One-player frame driver, called from func_80028618 after func_8002703C when gPlayerCount == 1 (gRunSessionFrame, func... | structure only |
| `0x80027510` | `gRunSessionFrame` | contract | The frame driver: runs the environment pass through func_80026E1C and the light direction update once per frame, arou... | structure only |
| `0x80028618` | `gRunGameFrameLoop` | contract | The in-game frame loop: it clears the game clock, sets the frame delta to 8, resets gSceneFrameCounter and seeds the ... | structure only |
| `0x8002A5AC` | - | contract | Empty function (jr $ra | structure only |
| `0x8002A810` | `gRetireEntityBase` | contract | Shared retire step of an entity: it returns 0 if flag 0x01000000 of +0x24 is already set, otherwise sets 0x01000010, ... | structure only |
| `0x8002AD58` | `gEntityBaseHandler` | contract | The shared base entity message handler: message 1 returns the 0xA0 base block size, message 2 runs the base spawn fun... | structure only |
| `0x8002B588` | `gTurnEntityToAngle` | contract | Turns an entity towards a target yaw at a limited speed: when the wrapped difference is no larger than 0x40 it does n... | structure only |
| `0x8002C584` | `gInitObjectFromDef` | contract | Message-2 (init) handler for the object classes dispatched through func_8002D664 (class ids 6, 8, 30, 33 and 36 in th... | structure only |
| `0x8002CC30` | `gUpdatePropTick` | contract | Message-6 tick of the object classes dispatched by func_8002D664 (the same classes whose init is func_8002C584): it s... | structure only |
| `0x8002DAA4` | `gInitGroundObject` | contract | Message-2 (init) handler of object type 13, called from its class handler func_8002DB9C: after the shared setup func_... | structure only |
| `0x8002DC94` | `gInitType11Entity` | contract | Spawn initializer of the type-11 entity, called only from message 2 of func_8002DEA8 | structure only |
| `0x8002E630` | `gTickActor` | contract | Message-6 (per-frame) body of the type-27 handler func_8002ED7C. It services the command queue with func_8002E528 and... | structure only |
| `0x8002EED8` | `gInitMoverObject` | contract | Message-2 body of one object-type behaviour: it calls the shared func_8002AD58 first, then sets the ground height fro... | structure only |
| `0x8002F30C` | `gStepMoverObject` | contract | Message-6 tick of a one-degree-of-freedom mover that func_8002F7C8 dispatches at message 6: depending on the mode byt... | structure only |
| `0x8003015C` | `gHandleRotatingPropMsg` | contract | Message handler of one entity class: message 1 answers 0xD8, message 2 runs the placement half func_8002FA20, message... | structure only |
| `0x800302E8` | `gInitTerrainEntity` | contract | Spawn initializer of the type-32 entity, called only from its handler func_80030960 | structure only |
| `0x80030AD4` | `gInitEntityGroundPair` | contract | Spawn step (message 2) of the entity class handled by func_80030D3C (message 1 there reports size 0xB0), which calls ... | structure only |
| `0x80030C60` | `gWarpEntityToSelf` | contract | Message-11 body of the type-29 handler func_80030D3C: it moves the entity that sent the message (or that entity's chi... | structure only |
| `0x80031238` | `gTestEntityOverlap` | contract | Overlap test between two entities: three branch-free absolute differences against the summed half extents scaled by 1... | structure only |
| `0x800313F0` | `gFollowAttachTarget` | contract | Keeps an attached entity on its target: when the re-resolve byte +0xB1 is set it is cleared, the target is looked up ... | structure only |
| `0x800317BC` | `gInitFromSourceModel` | contract | Spawn helper of the entity class handled by func_8003190C (message 2): it forwards message 2 to func_8002AD58, rebind... | structure only |
| `0x80031990` | `gFreeMapZoneList` | contract | Frees every zone node of gMapZoneList (D_8009F6F8) with gFreeBlock (func_800181D4), reading next at +0x24 before each... | structure only |
| `0x800319D4` | `gLoadMapZone` | contract | Load-time handler for actor resources of type 0x5F: it locates the 'tACT' (0x74414354) record with func_80019C38 (hea... | structure only |
| `0x80031B40` | `gTestCameraZoneOverlap` | contract | Returns 1 when a position widened by a radius overlaps any zone in gMapZoneList (D_8009F6F8) whose flag byte (+0x14) ... | structure only |
| `0x800324E0` | `gUpdateHoverEnemy` | contract | Shared flyer motion tick, called from the message handlers func_80032F3C and func_8003532C: it advances the animation... | structure only |
| `0x80036E6C` | `gHandleProjectileMsg` | contract | Message handler of a projectile entity class (size 0xD4), reached only through its handler table via the jump table j... | structure only |
| `0x80037708` | `gHandleProjectileMessage` | contract | Message handler of the kind-0xF projectile class | structure only |
| `0x800378BC` | `gInitRailMoverEntity` | contract | Spawn helper of the class-5 handler func_800382B4 (message 2): it runs the base handler, initialises the entity's att... | structure only |
| `0x80037EB4` | `gOrientEntityBasis` | contract | Orients an entity's model basis according to flags 0x200 and 0x100 | structure only |
| `0x8003802C` | `gUpdateFallMotion` | contract | Per-tick motion step used by the tick handlers func_8002B784 and func_800382B4: it first runs the path/motion step fu... | structure only |
| `0x80038754` | `gInitActorFromSpawn` | contract | Message-2 (init) handler of the object class whose dispatcher is func_80039298 | structure only |
| `0x80038A00` | `gUpdateMountedTurret` | contract | Per-tick update of a child entity mounted on a parent (the +0x158 child of func_800395A8 and child[0] of func_800398E... | structure only |
| `0x80039C58` | `gHandleCameraOverrideMsg` | contract | Message handler of the camera-override object class (size 0x120 on message 1) | structure only |
| `0x8003A160` | `gUpdateForceField` | contract | Per-frame tick of a force-field object, called from message 6 of its own handler func_8003A844 | structure only |
| `0x8003A90C` | `gFindVertexRangeEntry` | contract | Walks an array of 0x14-byte entries and returns the first whose byte range [start (+0x03), start + count (+0x04)) con... | structure only |
| `0x8003AA94` | `gSpawnModelDebris` | contract | Message-2 (spawn) case of the object class handled by func_8003B2D0: it allocates a 0xF0-byte piece array with func_8... | structure only |
| `0x8003B898` | `gSpawnEntitySlotTypes` | contract | Spawns the definition types configured for an entity through gSpawnTypeAtOwner (func_800402DC), always with bank func... | structure only |
| `0x8003BA50` | `gFlagQuartileChange` | contract | Sets flag 0x4000 in an entity's +0x24 word when the counter at +0x1C, measured against the maximum at +0x1E, lies in ... | structure only |
| `0x8003BEF0` | `gTakePoolNode` | contract | Allocates one node from a fixed pool laid out as {storage, active, free}: it pops the head of the free list at +0x08,... | structure only |
| `0x8003BF1C` | `gReleasePoolNode` | contract | Returns a node to a fixed pool laid out as {storage, active, free}: it unlinks the node from the active list at +0x04... | structure only |
| `0x8003C404` | `gInstallSpawnRecord` | contract | Installs one copied actor record into the spawn table D_800A7040[D_8009F310][record +0x14]: it first clears the low b... | structure only |
| `0x8003CF6C` | `gDrawNodeListModels` | contract | One of the scene passes of func_8003F6E4: it walks the node list D_8009F320, skips nodes without an object or without... | structure only |
| `0x8003D240` | `gSyncAttachedNodes` | contract | Per-frame walk of the 0x4C-byte anchor-node list D_8009F320 that func_8003D680 builds for spawned objects: a node who... | structure only |
| `0x8003D680` | `gCreateModelNode` | contract | Allocates a 0x4C-byte node (tag 1) and links it at the head of D_8009F320: with a source object it stores the source ... | structure only |
| `0x8003D9C4` | `gGetObjectTransform` | contract | Returns the transform block (3x3 rotation followed by an s32 position at words 9..11) an object should be drawn with:... | structure only |
| `0x8003D9F8` | `gResolveNodeTransform` | contract | Resolves an object's world transform from a parent, but only when flag 0x100 is set | structure only |
| `0x8003DB70` | `gBuildNodePlacement` | contract | Resolves the placement of a spawned node into a 12-word block: a nine-word basis in out[0..8] and a world position in... | structure only |
| `0x8003E984` | `gSpawnObjectFromTable` | contract | Instantiates one object from definition gSpawnRecordMap[bank][type] (D_800A7040, line 263): it builds the placement m... | structure only |
| `0x8003F4A0` | `gRunSceneListPasses` | contract | Runs the six per-layer render passes over the global scene lists in a fixed order | structure only |
| `0x8003F530` | `gRunSceneListPassesOnScratch` | contract | Runs gRunSceneListPasses on the scratchpad stack, so the passes get their own stack for the frame. | structure only |
| `0x8003F570` | `gRunEntityWorldTick` | contract | The per-frame tick of the entity world, called with the frame delta by both session updates, func_800271A4 (one playe... | structure only |
| `0x8003F6E4` | `gRunViewScenePasses` | contract | Resolves the view position (from gFocusPositions, otherwise func_80069580) and then runs nine list passes in a fixed ... | structure only |
| `0x8003F854` | `gTeardownWorldObjects` | contract | World teardown called once by gResetMissionSubsystems (func_80024D14): every object in the seven bucket lists gListHe... | structure only |
| `0x8003FAD4` | `gFinishLevelLoad` | contract | Post-load finalisation run once per module start by gRunScreenModule (func_800287F4, 800287F4.c:82), after the module... | structure only |
| `0x8003FF7C` | `gStreamDeferredSpawns` | contract | Per-tick proximity streaming of the deferred spawn requests in D_8009F30C (queued by func_8003FD74 from gSpawnTypeAtO... | structure only |
| `0x800402DC` | `gSpawnTypeAtOwner` | contract | Common entry for spawning an object of definition type 'column' from bank 'row' next to an owner: it returns 0 unless... | structure only |
| `0x80040874` | `gHandleLightObjectMsg` | contract | Message handler of spawn kind 0x5D objects, stored at +0x24 by their constructor func_80040E60 | structure only |
| `0x80040A00` | `gUpdateObjectLights` | contract | Walks a linked object list and integrates one time step per node: with bit 3 of the flags byte clear and an inner rec... | structure only |
| `0x80040DCC` | `gRegisterActorDef5D` | contract | Registers one 0x58-byte actor definition of type 0x5D: func_8003C2A4 first queues a deferred placement for records wi... | structure only |
| `0x80041070` | `gLoadActorDef5D` | contract | Chunk loader for 'Cact' actor type 0x5D: it finds the 'tACT' (0x74414354) record of the request's section id (+0x1C) ... | structure only |
| `0x80041488` | `gInitElevator` | contract | Message-2 (init) handler of the lift object stepped by func_80041880 (gStepElevator) | structure only |
| `0x80041728` | `gRunEntityCommands` | contract | Processes the four-slot command queue at +0x0C of a waypoint-following entity | structure only |
| `0x80041880` | `gStepElevator` | contract | Tick of a vertical lift object (sole caller func_80041B6C): after optional func_80041728 and the hold/delay gates, it... | structure only |
| `0x80041C6C` | `gHandlePlacedPropMsg` | contract | Message handler of another entity class: message 1 answers 0xA4, message 2 rebuilds the orientation from the descript... | structure only |
| `0x80042004` | `gEmitTexturedQuadPacket` | contract | Emits a textured, semi-transparent GP0 0x2C (POLY_FT4) quad primitive from the supplied points and flags arrays with ... | structure only |
| `0x80042128` | `gDrawPortraitScreen` | contract | Draw case (message 3 of the type-34 handler func_80042330) of an in-world screen that shows the animated comm portrai... | structure only |
| `0x800441DC` | `gTickChainEffectList` | contract | Per-frame update pass over object bucket gListHeads[3], called from gRunSceneListPasses (func_8003F4A0, 8003F4A0.c:46... | structure only |
| `0x80044260` | `gDrawObjectPointTiles` (asm) | contract | Draws an object's point list as small screen-space rectangles | structure only |
| `0x800445B0` | `gDrawPointTileList` | contract | Draw pass over one object bucket: for every node (next at +0x00) whose flag byte +0x0F lacks bit 3 it calls gDrawObje... | structure only |
| `0x80044770` | `gRegisterActorDef5E` | contract | Registers one 0x60-byte actor definition of type 0x5E: func_8003C2A4 first queues a deferred placement for records wi... | structure only |
| `0x8004488C` | `gLoadActorDef5E` | contract | Chunk loader for 'Cact' actor type 0x5E: it finds the 'tACT' (0x74414354) record of the request's section id (+0x1C) ... | structure only |
| `0x80044988` | `gSetAnimClipTable` | contract | Installs an animation clip table on a track: it stores the clip array at data + 4 and the clip count from data[0], se... | structure only |
| `0x80044F8C` | `gInitSoundState` | contract | Not on the draw path: initializes the sound/audio subsystem state by enabling audio and setting volume/stream paramet... | structure only |
| `0x80047888` | `gDrawTargetMarker` | contract | Draws an object's target reticle and leader line when state->target is set and flag 0x1000000 is clear | structure only |
| `0x80047BE8` | `gEmitHudGouraudBar` | contract | Draws a vertical HUD health/energy gauge at (x, y_in) with height (z+1)/2 (called by func_80047F30) | structure only |
| `0x80047F30` | `gDrawInGameHud` | contract | Draws the in-game 2D HUD when D_8009FA00 == 4 | structure only |
| `0x800485AC` | `gEmitHudPanelSprite` | contract | Allocates two 28-byte packets from the gPacketCursor pool with link field 0: texpage E100011F (page X 960, Y 256, ble... | structure only |
| `0x80048BF8` | `gLoadFontAsset` | contract | Uploads a font bank's glyph bitmap to VRAM (0x1E0, 0) and palette to (0, 0x1F0) via func_80092F38/func_80092C8C, comp... | structure only |
| `0x80048F98` | `gEmitFontGlyphSprite` | contract | Looks up character in the current font glyph table, emits an 8x8 textured sprite (SPRT, code 0x64) at (x, y) with the... | structure only |
| `0x80049100` | `gDrawAlignedString` | contract | Draws a NUL-terminated string at (x, y) in palette color using the current font bank by emitting per-character glyph ... | structure only |
| `0x8004963C` | `gReleaseMapLightData` | contract | Frees the map light and sprite-descriptor data: the descriptor table D_8009F77C (if set), the 0x180 x 0x44-byte light... | structure only |
| `0x800496D0` | `gAllocLightGrid` | contract | Allocates the per-cell bucket array D_8009F780 (width x height list heads, allocation tag 2), stores the dimensions i... | structure only |
| `0x80049EA8` | `gLoadActorLightNode` | contract | Handles a type-0x61 resource request: func_80019C38 locates the 'tACT' (0x74414354) chunk with section id +0x1C insid... | structure only |
| `0x8004A048` | `gDrawMapCellNode` | contract | Draws one map-cell object as a camera-facing billboard or a four-corner quad, after a perspective cull, a clip-flag t... | structure only |
| `0x8004A924` | `gDrawCellWindowObjects` | contract | Layer 2 of the environment pass: draws the map-cell objects (walls, props) of the 5x5 cell window, filtered by the pe... | structure only |
| `0x8004ABE4` | `gLaunchDebrisObject` | contract | Starts the ballistic flight of a debris object when it is destroyed | structure only |
| `0x8004AFC0` | `gStepDebrisFlight` | contract | Advances a launched debris object by one step | structure only |
| `0x8004B5F8` | `gSpawnMeshFractureDebris` | contract | Selects up to n faces from an entity's 3D model using scratchpad bitmaps (0x1F800000/0x1F800020) and spawns debris fr... | structure only |
| `0x8004C228` | `gTickDebrisList` | contract | Per-frame update pass over object bucket gListHeads[4]: every node (next at +0x00) whose flag byte +0x0F lacks bit 3 ... | structure only |
| `0x8004C5C8` | `gDrawDebrisFragments` | contract | Draws the fragment polygons of the debris objects in one scene list | structure only |
| `0x8004C884` | `gRegisterActorDef5A` | contract | Registers one 0x4C-byte actor definition of type 0x5A: func_8003C2A4 first queues a deferred placement for records wi... | structure only |
| `0x8004CA98` | `gLoadActorDef5A` | contract | Chunk loader for 'Cact' actor type 0x5A: it finds the 'tACT' (0x74414354) record of the request's section id (+0x1C) ... | structure only |
| `0x8004EDC8` | `gDrawTargetMarkers` | contract | Prepares the object via func_8005A9C4, emits a 0xE100069E texpage packet into gPacketCursor/gPacketLimit, and iterate... | structure only |
| `0x8005027C` | `gStepPathMover` | contract | Advances an object along its path-network route by one step | structure only |
| `0x80050E0C` | `gInitRailSegment` | contract | Seats a route-following mover on its route: with a route descriptor it marks the render object (+0x24 bit 31), finds ... | structure only |
| `0x800518A8` | `gUpdateMorphBounds` | contract | Updates the player unit's progress-dependent footprint for the form change: from the model's current frame element it... | structure only |
| `0x80051C50` | `gBuildObjectBasis` | contract | Builds an object's orientation matrix into the 3x3 block passed as the last argument (the caller passes the object's ... | structure only |
| `0x80051CC4` | `gPushFromPartBox` | contract | Collision push against one box-shaped model part, called from the masked part loop of func_80051EB8 for every part bi... | structure only |
| `0x80052410` | `gTestFootprintEdges` | contract | Builds the object's basis with func_80051C50, loads it with func_80010750 and transforms the four footprint corners o... | structure only |
| `0x80052960` | `gComposeJointChain` | vector | Composes a joint chain into a local matrix stack seeded with the object's coordinate matrix: for each record the thre... | `tests/host/joints_test.sh` (joints.vectors, 7 cases); GTE software model |
| `0x80052D7C` | `gUpdateTwoPartRig` | contract | Refreshes the two auxiliary model instances held in an entity's state block (object->+0xB4->+0xAC, instances at +0x17... | structure only |
| `0x80053A74` | `gTickHoverPhysics` | contract | Drive integrator of the player unit in form 1 (state + 0xAD == 1, the hover form in the caller's documentation) | structure only |
| `0x80055EE8` | `gTestCameraSightLine` | contract | Line-of-sight test between the view target (from) and a candidate camera eye (to) on top of the map raycast func_8006... | structure only |
| `0x8005605C` | `gEaseAngleShortest` | contract | Eases a 12-bit angle toward a target along the shorter way round the circle: both are folded to -0x800..0x7FF, curren... | structure only |
| `0x8005612C` | `gComputeCameraEye` | contract | Computes a camera eye position around a base point: the per-view offset block is scaled by (0x100 - arg7/2)/256, lowe... | structure only |
| `0x800564A8` | `gComputeBoomDistance` | contract | Maps a pitch angle to a camera distance: value = 0x10000 - ((pitch - 0x14A) << 16) / 600, then 0x20000 * (0x10000 - v... | structure only |
| `0x80056530` | `gSpringFollowHeight` | contract | Spring-eases the follow height kept at +0xD4 of the camera info block (the pointer at object+0xAC) toward the followe... | structure only |
| `0x80056634` | `gPlacePlayerViews` | contract | Places both players' view objects before the frame is drawn | structure only |
| `0x8005678C` | `gPlaceCameraFromPreset` | contract | Computes a camera position from a preset offset around a follow target and, when asked, applies it through gSetCamera... | structure only |
| `0x80056870` | `gFindClearCameraPreset` | contract | Searches the alternate camera presets 9..16 for a view of the followed object that is not blocked: it starts at a ran... | structure only |
| `0x80056994` | `gStepDemoCameraTable` | contract | Counts the s16 timer at +0x2FE of the camera info block down by the frame delta | structure only |
| `0x800569EC` | `gUpdateChaseCamera` | contract | Chase-camera step: selects the preset, probes it against the map and smooths yaw, pitch, boom and eye offset into the... | structure only |
| `0x800573A4` | `gResetActorAndPlaceView` | contract | Start-position step: resets an actor from its spawn record and places the third-person view that belongs to it, which... | structure only |
| `0x800576A8` | `gTranslatePadButtons` | contract | Converts the raw pad word D_8009F7C0[index] into the game's button mask written to *out: the bits 0x80000, 0x40000, 0... | structure only |
| `0x8005780C` | `gDecodeButtonEdge` | contract | Four-phase button decoder: a set bit in the current mask moves the action record to the press edge (phase 0) or to he... | structure only |
| `0x80057E40` | `gSlewEntityAim` | contract | Aims the player unit's two-part gun assembly (the +0xDC companion passed by func_800588D4, its only caller): it place... | structure only |
| `0x80058714` | `gRespawnPlayerEntity` | contract | Respawns a player entity when its respawn countdown has run out | structure only |
| `0x800588D4` | `gUpdatePlayerEntity` | contract | Per-frame tick of the player unit, called from its class handler func_8005908C: it resets state +0x2FC, re-arms the m... | structure only |
| `0x800592D0` | `gDrawOtherPlayerArrow` | contract | Two-player overlay drawn from the player-unit draw handler func_800597C0: it returns unless gPlayerCount is at least ... | structure only |
| `0x800596C4` | `gEmitObjectBeamSegment` | contract | Transforms an active object part position via gTransformPartVertex, picks a random descriptor using func_80019668, an... | structure only |
| `0x800597C0` | `gDrawGlowDiscEffect` | contract | Draw handler for the player/effect object (called from func_80059ABC:87) | structure only |
| `0x80059ABC` | `gHandleCompanionMessage` | contract | Message handler of the kind-0x12 companion object that func_80057914 queues on a part of the player unit, so it has n... | structure only |
| `0x80059DB0` | - | contract | Empty function (jr $ra | structure only |
| `0x8005A068` | - | contract | Empty function (jr $ra | structure only |
| `0x8005A220` | `gFreeTextureDescriptors` | contract | Frees each per-entry sprite/tile descriptor block in gTextureDescriptorTable in reverse order via gFreeBlock, frees t... | structure only |
| `0x8005A2BC` | `gBuildSpriteDescTable` | contract | Texture descriptor step of the Cpyr atlas loader, not a primitive emitter: for each source block it stores byte 2 (ti... | structure only |
| `0x8005A414` | `gLoadTextureChunksToVram` | contract | Consumes a 'Cpyr' texture/tile atlas blob from func_80024EC8:221, allocates gTextureDescriptorTable and per-entry blo... | structure only |
| `0x8005A740` | `gEmitAngledLine` | contract | Computes a direction vector from angle id using gSinCos and emits a flat GP0 0x40 line primitive from origin into the... | structure only |
| `0x8005A828` | `gEmitRadarTriangle` | contract | Takes one 28-byte packet from the gPacketCursor pool (not advancing when gPacketLimit is reached) with tag word 0x040... | structure only |
| `0x8005A9C4` | `gDrawRadarMarkers` | contract | Draws one object's radar overlay into the gPacketCursor pool through its callees: a heading line via func_8005A740 at... | structure only |
| `0x8005BDE4` | `gAttachNearPlayerEffect` | contract | Attaches a timed effect node to an owner object, but only if the model instance's world position (+0x34/+0x38/+0x3C) ... | structure only |
| `0x8005CED4` | `gTakeArmedLobePoint` | contract | Helper of the damage-field update func_8005D8F4 (its only caller): it scans the twelve 0x1C-byte lobes from +0x58 for... | structure only |
| `0x8005D060` | `gSpawnLobeArc` | contract | Steps one of the twelve lobes of the damage field updated by func_8005D8F4 (its only caller): once the frame time D_8... | structure only |
| `0x800619C0` | `gBoxOverlapBitmask` | contract | Turns one oriented box into a world axis-aligned box and returns a per-entry overlap bitmask: it builds a diagonal sc... | structure only |
| `0x80061CAC` | `gTestPartPairHit` | contract | Narrow-phase hit test between two coord records: bit 27 of each model flag word gives a two-bit mode, and the functio... | structure only |
| `0x80062EB8` | `gRunRegionObjectPass` | contract | Update and draw pass over the entity table of one region: message 3 to every object, then the polygons of the visible... | structure only |
| `0x80063C5C` | `gLoadCbmpTexture` | contract | Texture load path for one Cbmp slot (index s0 = unk1C - 1): func_8001996C unpacks the resource, the 0x80 x 0xFC pixel... | structure only |
| `0x80063DC8` | `gInitMapFromGrdb` | contract | Handler for the 'Cptc' (0x43707463) chunk in gLoadQueuedAssets (80024EC8.c:151-152): it stores the chunk in D_8009F81... | structure only |
| `0x80064460` | `gSampleCellWindow` | contract | Fills the 25 window entries (mask, node, world position) that the three region layers then draw. | structure only |
| `0x800646BC` | `gEmitTexturedSurface` (asm) | contract | Already renamed to gEmitTexturedSurface in src/game/renderer-scene/gEmitTexturedSurface.c: builds and emits triangle strips of a textured surfac... | structure only |
| `0x800648E8` | `gEmitCellPolygon` | contract | Emits one textured polygon of a map cell into the shared primitive pool: the primitive word's bits 15..21 select four... | structure only |
| `0x80065134` | `gEmitCellGeometry` | contract | Emits the visible cells of one region as geometry | structure only |
| `0x8006531C` | `gDrawRegionTiles` | contract | Layer 1 of the environment pass: draws the 5x5 window of map regions (the tiled ground surfaces) around the camera po... | structure only |
| `0x800654F8` | `gBuildTilePattern` | contract | Builds the 256-byte pattern uploaded for a tileset | structure only |
| `0x80065744` | `gWorldToRadarCell` | contract | Maps a world coordinate onto the tile/radar grid coordinate | structure only |
| `0x80065834` | `gDrawRadarBlip` | contract | Draws one entity onto the radar/minimap | structure only |
| `0x80065CA8` | `gDrawMapSubtilePolys` | contract | Emits the top-down map polygons of one of the 16x16 sub-tiles of a map tile record: the sub-tile's u16 descriptor at ... | structure only |
| `0x800665E8` | `gClampMapViewToBounds` | contract | Clamps the map-view centre in place to the world bounds D_800A2950 built by func_80063DC8: z (arg0[2]) into [bounds[0... | structure only |
| `0x80066684` | `gDrawMinimapMarkers` | contract | Draws up to eight point markers of one map object as gouraud discs: it returns at once for a null object or when no s... | structure only |
| `0x8006680C` | `gDrawMapView` | contract | Draws the map view around a position for the map screen | structure only |
| `0x80066C9C` | `gUpdateCameraAnchor` | contract | Per-view look-at update: it selects the player's view record with gSelectViewRecord, loads the camera rotation, and i... | structure only |
| `0x80066E98` | `gDrawViewLayers` | contract | The environment pass of one view: selects the view record, loads the camera rotation, then draws the same region in t... | structure only |
| `0x80066F78` | `gGetGroundHeight` | contract | Returns the ground height under a world position by walking one map cell: gMapGridHeader (D_8009F80C) gives the grid ... | structure only |
| `0x8006943C` | `gClampToWorldBounds` | contract | Clamps a moving entity's position in place to the world bounds D_800A2950 shrunk by 0x800 on every side: z (arg0[2]) ... | structure only |
| `0x800694C8` | `gWorldToMinimapCell` | contract | Converts a world x/z position into cell indices of the 54x54 map grid: origin x/z from D_800A2950 is subtracted, each... | structure only |
| `0x80069580` | `gGetViewGroundAnchor` | contract | Copies entry 'index' of the pair table D_800A3190 into an (x, 0, z) triple, with the middle component zeroed and no b... | structure only |
| `0x800695A8` | `gBuildCoarseHeightGrid` | contract | Builds the smoothed signed-byte height grid published in D_800A2940: for every map cell of the GRDB header D_8009F80C... | structure only |
| `0x8006BF44` | `gEmitStatusSlotIcon` | contract | Allocates a 0x34-byte packet from gPrimPoolNext/gPrimPoolEnd and emits a 16x16 textured SPRT primitive (code 0x64, le... | structure only |
| `0x8006C080` | `gDrawThreeSlotStatusBar` | contract | Draws a three-slot HUD status bar by looping i = 0..2 and calling func_8006BF44 for each 16x16 icon (highlighted when... | structure only |
| `0x8006C4C4` | `gGenDomePatchVertices` | contract | Generates a 3D s16 vertex grid (4-s16 stride) for a spherical octant dome-patch mesh into dst (called by func_8006C84... | structure only |
| `0x8006C768` | `gBuildDomeFaceIndices` | contract | Geometry feeding the effect renderer, not a primitive emitter: writes rows - 1 triangle records (count 3, apex vertex... | structure only |
| `0x8006C84C` | `gBuildDomePatchMesh` | contract | Allocates and populates a spherical dome-patch mesh for effect objects (called by func_8005D3FC and func_8006E814) | structure only |
| `0x8006CB58` | `gDrawShadedModel` | contract | Draws one procedural effect shape with a colour gradient: it generates up to sixteen mirrored corner points (+-size o... | structure only |
| `0x8006D520` | `gDrawGroundMarker` | contract | Draws an entity's ground shadow/target ring for func_8006D770 | structure only |
| `0x8006D770` | `gTickEffectNodes` | contract | Per-frame update of the effect objects in gListHeads bucket 2 | structure only |
| `0x8006DC24` | `gDrawFlatRing` | contract | Draws one model-list node as a flat ring (annulus) in its local x/z plane | structure only |
| `0x8006E988` | `gDrawAttachedModelList` | contract | Walks a linked list of model nodes (next at +0x00) for one view and skips nodes whose flag byte +0x0F has bit 3 | structure only |
| `0x8006EAF0` | `gRegisterActorDef5C` | contract | Registers one 0x54-byte actor definition of type 0x5C: func_8003C2A4 first queues a deferred placement for records wi... | structure only |
| `0x8006ED78` | `gLoadActorDef5C` | contract | Chunk loader for 'Cact' actor type 0x5C: it finds the 'tACT' (0x74414354) record of the request's section id (+0x1C) ... | structure only |
| `0x8006F170` | `gTickTracerPool` | contract | Per-frame update of the beam/tracer node pool D_800A3538 (created by func_8006EDC4 with 10 nodes of 0x48 bytes, fille... | structure only |
| `0x8006F21C` | `gEmitTracerGouraudQuad` | contract | Takes a 0x34-byte packet from the gPrimPoolNext pool, sets length 8 and the low 24 link bits to the tag argument, the... | structure only |
| `0x8006F310` | `gDrawTracerBand` | contract | Draws one beam/tracer node (endpoints at +0x10 and +0x1C) as a screen-space band: it returns early when both endpoint... | structure only |
| `0x8006F844` | `gDrawTracerList` | contract | Draw pass for the beam/tracer pool: walks D_800A353C, which is the active-list field (+4) of the pool header D_800A35... | structure only |
| `0x8006FF44` | `gHandleActor59Msg` | contract | Message handler of the 0x9C-byte kind-0x59 object built by func_8007043C, which stores this function's address in the... | structure only |
| `0x80070048` | `gIntegrateActor59List` | contract | Advances every node of the scene list gListHeads[1] (D_800A1070[1], passed by gRunSceneListPasses, 8003F4A0.c:48) by ... | structure only |
| `0x80070158` | `gDrawGlowNodeList` | contract | Walks the node list from the argument, resolves each node's transform with func_8003D9F8, culls it by radius against ... | structure only |
| `0x800703A8` | `gRegisterActorDef59` | contract | Registers one 0x58-byte actor definition of type 0x59: func_8003C2A4 first queues a deferred placement for records wi... | structure only |
| `0x800706C4` | `gLoadActorDef59` | contract | Chunk loader for 'Cact' actor type 0x59: it finds the 'tACT' (0x74414354) record of the request's section id (+0x1C) ... | structure only |
| `0x80070730` | `gComputeEffectEndPoint` | contract | Computes one end point of a beam or ribbon strip and returns the index of the player entity it locked onto, or -1 | structure only |
| `0x80071B40` | `gStepBeamNode` | contract | Per-frame step of one beam node, called by the list pass func_8007364C over gBeamNodeList (D_8009F38C) once the node ... | structure only |
| `0x80071FA4` | `gDrawBeamNode` | contract | Draws one beam node, a set of point strips stretched between two end objects: per strip it makes the points camera-re... | structure only |
| `0x80072F7C` | `gDrawFlatModelNode` | contract | Draws a model node on the cheap flat/line renderer path when node->flags & 0x4000 is clear | structure only |
| `0x800734BC` | `gRetireAllRibbons` | contract | Walks the ribbon/beam node list D_8009F38C (pushed by the constructor func_8007377C, 8007377C.c:185-186), sets retire... | structure only |
| `0x80073500` | `gAllocPositionNode` | contract | Allocates a 0x20-byte node with gAllocMemory (func_80017EBC, tag 1), returns 0 if that fails, otherwise clears the tw... | structure only |
| `0x8007364C` | `gTickBeamNodeList` | contract | Per-frame maintenance of the beam/ribbon node list D_8009F38C (nodes built by func_8007377C): walking with a predeces... | structure only |
| `0x80073700` | `gDrawBeamNodeList` | contract | Per-view draw pass over the beam/ribbon node list D_8009F38C: a node with either of its two record pointers at +0x04/... | structure only |
| `0x80073BBC` | `gHandleBeamObjectMsg` | contract | Message handler of a two-point beam object (no direct caller | structure only |
| `0x80074104` | `gDrawRibbonList` | contract | Renders every entry of a linked object list as camera-facing ribbons: per entry it builds the strip transform with fu... | structure only |
| `0x800753D0` | `gUpdateParticleLight` | contract | Keeps the light node at +0x24 of a particle in sync: with a node, the particle's colour bytes +0x15..+0x17 are copied... | structure only |
| `0x80075478` | `gReleaseParticleLight` | contract | If the light-node pointer at +0x24 of a particle is non-null, releases it through func_800498F0 (which unlinks the no... | structure only |
| `0x800754B8` | `gInitEmitterParticle` | contract | Initialises one particle node that an emitter object has just taken from its pool: it writes the world position, the ... | structure only |
| `0x80075A84` | `gRunPolygonPass` | contract | Polygon pass over one scene pool: walks the active particle list, culls and lights each node and draws it through fun... | structure only |
| `0x800762B0` | `gCreateEmitterObject` | contract | Constructor for spawn kind 0x57, called only from gSpawnObjectFromTable (func_8003E984): it allocates a 0xA0-byte obj... | structure only |
| `0x800770A8` | `gDrawSevenSegmentChar` | contract | Maps an ASCII character (0-9, A-Z, a-z) to a 7-bit segment mask in gSegTable and draws each active segment from gSegC... | structure only |
| `0x80078B98` | `gDrawSlotStatusLine` | contract | Walks the two queued display text slots in D_800A3578 and draws each active slot's status string at the bottom of the... | structure only |
| `0x8007F1F8` | `gUpdatePasswordScreen` | contract | Jump-table state machine (states 0..5 via jtbl_800121A4) for the 10-character A..Z password entry panel | structure only |
| `0x8008125C` | `gHandleProfileSlotWidget` | contract | Front-end profile-slot widget handler gated by D_8009F8BD and D_8009F8BE. Action 0 draws the 8-cell glyph editor with... | structure only |
| `0x80082358` | `gUploadSaveProfileIcon` | contract | Extracts selected image and palette chunks from a tagged resource node via func_8001E318, encodes the profile name gl... | structure only |
| `0x800824CC` | `gLoadCtdtImages` | contract | Texture load path for a Ctdt resource: checks header type 2 and magic 0x35BFBC73, then walks count 8-byte entries, ca... | structure only |
| `0x800825AC` | `gValidateAtlasSpriteId` | contract | Registers and validates a loaded 2D texture atlas header resource from object, freeing temporary data via gFreeBlock ... | structure only |
| `0x800826AC` | `gComposeScreenSlices` | contract | Composes a 320x240 front-end/pause screen from descriptor table D_8009B32C[idx] and caches idx in D_8009F874 | structure only |
| `0x80082AB0` | `gEmitAtlasSpriteSimple` | contract | Thin wrapper on the 2D sprite path: forwards to func_80082B0C, which emits a sprite from the texture atlas table, rep... | structure only |
| `0x80082B0C` | `gDrawAtlasSprite` | contract | Looks up entry idx in gAtlasTable, resolves -1 dimensions and alignment flags (flagX/flagY), allocates a 0x34-byte pa... | structure only |
| `0x80082EF0` | `gEmitFlatQuadRect` | contract | Takes a 0x34-byte packet from the gPrimPoolNext pool (overwriting the last slot when gPrimPoolEnd is reached) and bui... | structure only |
| `0x80082FA4` | `gEmitFlatQuadRectMode` | contract | Takes a 0x34-byte packet from the gPrimPoolNext pool and writes length 5, code (code or 0x28) with the rgb colour, an... | structure only |
| `0x80083060` | `gEmitFlatQuad` | contract | Emits an untextured flat-shaded four-corner quad (POLY_F4, code 0x28 or arg code) from eight explicit coordinates (x0... | structure only |
| `0x80083280` | `gDrawBlinkSelectFrame` | contract | 2D overlay helper: computes a grey level gHighlightShadeTable[(gFrameTickCounter shifted right 4) and 7] + 0x80 and d... | structure only |
| `0x800834C4` | `gAllocCellBucketGrid` | contract | Allocates and zeroes the per-cell bucket grid D_8009F938 (two words per cell) and records its width and height in D_8... | structure only |
| `0x80083764` | `gBuildSubcellMask` | contract | Builds a 16-bit coverage mask of the 4x4 subcells (0x8000 units each, cell size 0x20000) that the rectangle left..rig... | structure only |
| `0x80083A10` | `gSpawnSceneNode` | contract | Creates one map-cell model node and inserts it into the cell bucket grid: it takes a 0x5C-byte node from the 250-entr... | structure only |
| `0x80084398` | `gDrawCellWindowModels` | contract | Layer 3 of the environment pass: animates and submits the models of the same cell window, i.e | structure only |
| `0x80085074` | `gUploadMovieStrip` | contract | Video draw path, run once per decoded strip: when gStripScaleMode == 1 and gStripScaleEnable != 0 the strip rectangle... | structure only |
| `0x800860CC` | `gAllocDecodeBuffers` | contract | Not a draw routine, but it provisions the image decode/upload path: allocates the static working set used by func_800... | structure only |
| `0x800861E4` | `gDecodeMdecFrameToVram` | contract | Decodes a compressed frame with the hardware MDEC pipeline and transfers it to VRAM using a 0x130-byte gMovieSliceCon... | structure only |
| `0x80086AC0` | `gServiceMovieAudioIrq` | contract | SPU IRQ service routine of the movie audio stream whose state block is pointed to by D_8009F4D8: it returns at once w... | structure only |
| `0x80091F90` | `gLoadTexturePage` | contract | VRAM upload helper of the texture path: builds a rectangle at x,y with height h and a width narrowed for the pixel ty... | structure only |
| `0x80092078` | `gLoadClut256` | contract | CLUT upload helper for 8-bit textures: sends 256 palette entries to VRAM at a,b through func_80092F38 and returns the... | structure only |
| `0x800920DC` | `gLoadClut16` | contract | CLUT upload helper: sends 16 palette entries from main memory to VRAM at a,b through the LoadImage-tagged transfer fu... | structure only |
| `0x80092140` | `gSetDefDrawEnv` | contract | Initializes a DRAWENV structure with its clip rectangle (x, y, z, size), drawing offset, and default draw-mode/area/o... | structure only |
| `0x800921F4` | `gSetDefDispEnv` | contract | Display set-up on the draw path: initialises a 20-byte display environment block with the display rectangle, a zero s... | structure only |
| `0x80092230` | `gGetTPage` | contract | Packs texture depth mode (type), semi-transparency blend rate, and VRAM coordinates (x, y) into a 16-bit PS1 texture-... | structure only |
| `0x80092514` | `gInitPolyF3Packet` | contract | Initializes a graphics primitive packet header with length 3 and GP0 command code 0x20 (POLY_F3). | structure only |
| `0x80092528` | `gSetSpritePacket` | contract | Primitive initialiser for a free-size sprite: sets length 4 (colour/code, xy, uv plus CLUT, wh) and code 0x64, opaque... | structure only |
| `0x8009253C` | `gInitPolyFT3Packet` | contract | Initializes a graphics primitive packet header with length 4 and GP0 command code 0x24 (POLY_FT3). | structure only |
| `0x80092550` | `gSetTile8Packet` | contract | Primitive initialiser for an 8x8 tile: sets length 2 (colour/code word plus xy word) and code 0x70, leaving colour an... | structure only |
| `0x80092564` | `gInitPolyF4Packet` | contract | Initializes a graphics primitive packet header with length 5 and GP0 command code 0x28 (POLY_F4). | structure only |
| `0x80092634` | `gSetDrawModePacket` | contract | Builds a draw-mode packet: tpage and 0x9FF keeps page X/Y, blend bits 5-6, colour depth bits 7-8 and texture-disable ... | structure only |
| `0x80092660` | `gSetDrawMovePacket` | contract | Builds a VRAM copy packet: length 5 (0 when the rectangle has no area), GP0 01 to flush the texture cache, GP0 80 cop... | structure only |
| `0x800926C0` | `gBuildVramTransferPkt` | contract | Builds a CPU-to-VRAM transfer packet (GP0 0xA0) with its rectangle parameters and terminator word into packet p (not ... | structure only |
| `0x80092E10` | `gClearImage` | contract | VRAM fill request: after the rectangle check (tag string ClearImage) it calls the GPU library table slot at +8 with t... | structure only |
| `0x80092F38` | `gLoadImage` | contract | Transfers a rectangular pixel buffer from main RAM to VRAM by checking clipping bounds and issuing an 8-word GPU DMA ... | structure only |
| `0x80092F98` | `gStoreImage` | contract | Transfers a rectangular pixel region from VRAM to main RAM by validating bounds and issuing an 8-word GPU command blo... | structure only |
| `0x80092FF8` | `gMoveImage` | contract | VRAM-to-VRAM copy request: after func_80092CF4 validates the rectangle (tag string MoveImage), an empty rectangle ret... | structure only |
| `0x80093224` | `gDrawPrimImmediate` | contract | Immediate primitive submit: waits or selects through gGpuCallbackTable slot +0x3C(0), then hands the packet body (aft... | structure only |
| `0x800933B0` | `gDrawOTagEnv` | contract | PsyQ DrawOTagEnv implementation: copies a DRAWENV block via func_8008CD10, builds its GPU primitive packet via func_8... | structure only |
| `0x800934BC` | `gPutDispEnvCached` | contract | Applies a 0x14-byte display/drawing-area state block to the GPU via gVramWriteCallback->fn (GP0/GP1 05h, 06h, 07h, 08... | structure only |
| `0x80093954` | `gSetTexWindow` | contract | Builds a 2-word GP0 texture-window primitive packet (PsyQ SetTexWindow) using func_800940C4 to encode the 8-byte wind... | structure only |
| `0x80093A4C` | `gSetDrawMaskPacket` | contract | Builds a 1-word GP0 0xE6 mask-bit setting primitive packet encoding flag1 and flag2 into the packet word. | structure only |
| `0x80093A74` | `gSetMaskBitPacket` | contract | Builds the GP0 E6 mask setting packet: bit 0 (force the mask bit on drawn pixels) follows the flag, bit 1 (check mask... | structure only |
| `0x80093A9C` | `gSetDrawTPage` | contract | Builds a two-word GPU draw-mode packet containing a GP0 0xE1 command from func_80093F58 and a GP0 0xE2 texture-window... | structure only |
| `0x80093AF0` | `gBuildSpritePacket` | contract | Builds a 2D GPU sprite command packet from a rectangle descriptor | structure only |
| `0x80093CE8` | `gBuildRectDrawList` | contract | Builds a GPU display list from a rectangle descriptor for func_800932F0 and func_800933B0 | structure only |
| `0x80093F58` | `gBuildDrawModeCommand` | contract | Encodes dfe, dtd, and tpage bits into a 32-bit GP0 0xE1 draw-mode (texture page) command word and returns it (called ... | structure only |
| `0x80093F78` | `gBuildDrawAreaCommands` | contract | Clamps coordinates (x, y) to valid VRAM draw-area bounds and encodes a GP0 0xE3/0xE4 drawing-area command word (calle... | structure only |
| `0x80094010` | `gPackDrawAreaBottomRight` | contract | Builds the GP0 E4 drawing-area bottom-right word: x and y are clamped to 0..gMaxDrawWidth - 1 and 0..gMaxDrawHeight -... | structure only |
| `0x800940A8` | `gPackDrawOffset` | contract | Packs the GP0 E5 drawing-offset command, the x,y added to every vertex of later primitives (not a texture window, whi... | structure only |
| `0x800940C4` | `gBuildTexWindowCommand` | contract | Encodes an 8-byte texture window rectangle into a GP0 0xE2 texture-window command word and returns it (called by func... | structure only |
| `0x8009446C` | `gUploadImageToGpu` | contract | Uploads a pixel rectangle to GPU VRAM via GP0 and DMA2 | structure only |
| `0x80094CEC` | `gWaitImageTransferFrame` | contract | Waits for the image-transfer engine to reach a frame boundary in a hand-written assembly loop | structure only |
| `0x80095900` | `gSqrt12` | contract | Hand-written 20.12 fixed-point square root (result = 64 * sqrt(v), the libgte SquareRoot12 scheme): the GTE leading-z... | structure only |
| `0x80095B64` | - (asm) | skip | Cross product through a temporary matrix | no caller in the US binary and no readable binding; only checks (a)/(c) apply |
| `0x80095C14` | `gGteLeadingZeros` | contract | Returns the GTE leading-zero/one count of its argument (mtc2 to LZCS, mfc2 from LZCR) | structure only |
| `0x80095C30` | `gAtan2` | contract | Two-argument arctangent returning a 12-bit angle (0x1000 = full turn) measured from the +y argument toward the +x arg... | structure only |
| `0x80096070` | `gStartMdecOutDma` | contract | Video decode support: starts the next decode-out DMA into the destination buffer with the given size, so a strip can ... | structure only |
| `0x80096090` | `gMdecInSync` | contract | Video decode support, not a draw routine: blocks until the decode-input side is idle (mode 0, timeout inside func_800... | structure only |
| `0x800960CC` | `gMdecOutSync` | contract | Video decode support rather than primitive drawing: blocks until the decode-out transfer finishes (mode 0, with timeo... | structure only |
| `0x80096484` | `gIsMdecBusy` | contract | Reads the MDEC transfer status register via D_8009E9F4 and returns bit 29 indicating whether MDEC DMA output/control ... | structure only |

## Citation index

Each entry is checked by `tests/test_environment_path.py` (the fragment must
occur on the cited line).

| citation | code fragment |
|---|---|
| `src/game/boot-frontend/gRunGameFrameLoop.c:131` | `gRunSoloSessionFrame();` |
| `src/game/boot-frontend/gRunGameFrameLoop.c:133` | `gRunSessionFrame();` |
| `src/game/boot-frontend/gRunSessionFrame.c:231` | `gRunEntityWorldTick(` |
| `src/game/gameplay-ai/gRunSoloSessionFrame.c:183` | `gRunEntityWorldTick(` |
| `src/game/boot-frontend/gRunEntityWorldTick.c:103` | `gRunSceneListPassesOnScratch(arg0);` |
| `src/game/boot-frontend/gRunSceneListPassesOnScratch.c:36` | `jal func_8003F4A0` |
| `src/game/renderer-scene/gRunSceneListPasses.c:45` | `gTickEffectNodes(gListHeads[2], arg);` |
| `src/game/renderer-scene/gRunSceneListPasses.c:47` | `gUpdateObjectLights(gListHeads[0], arg);` |
| `src/game/renderer-scene/gRunSceneListPasses.c:50` | `gRunPolygonPass(gListHeads[5], arg);` |
| `src/game/boot-frontend/gRunSessionFrame.c:233` | `func_80026E5C(0,` |
| `src/game/gameplay-ai/gRunSoloSessionFrame.c:190` | `func_80026E5C(0,` |
| `src/game/boot-frontend/gUpdateViewAnchorsOnScratchpad.c:37` | `jal func_80066C9C` |
| `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:110` | `gSelectViewRecord(` |
| `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:111` | `gGteLoadRotation(gCamera->matrix);` |
| `src/game/gpu-2d-hud/gUpdateCameraAnchor.c:194` | `.word 0x0c019118` |
| `src/game/boot-frontend/gRunSessionFrame.c:244` | `gSelectViewRecord(` |
| `src/game/boot-frontend/gRunSessionFrame.c:245` | `gUpdateLightDirection();` |
| `src/game/boot-frontend/gRunSessionFrame.c:283` | `gSelectViewRecord(` |
| `src/game/boot-frontend/gRunSessionFrame.c:284` | `gUpdateLightDirection();` |
| `src/game/gameplay-ai/gRunSoloSessionFrame.c:189` | `gUpdateLightDirection();` |
| `src/game/boot-frontend/gRunSessionFrame.c:250` | `gRunRegionPass(0,` |
| `src/game/boot-frontend/gRunSessionFrame.c:288` | `gRunRegionPass(1,` |
| `src/game/gameplay-ai/gRunSoloSessionFrame.c:194` | `gRunRegionPass(0,` |
| `src/game/boot-frontend/gRunRegionPass.c:36` | `jal func_80066E98` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:63` | `gSelectViewRecord(object->info->id);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:64` | `gGteLoadRotation(gCamera + 0x28);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:68` | `gDrawRegionTiles(x, y, region);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:69` | `gGteLoadRotation(gCamera + 0x28);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:70` | `gDrawCellWindowObjects(x, y, region);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:71` | `gDrawCellWindowModels(x, y, region);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:72` | `gRunRegionObjectPass(index);` |
| `src/game/gpu-2d-hud/gDrawViewLayers.c:73` | `gRunViewScenePasses(index);` |
| `src/game/renderer-scene/gRunViewScenePasses.c:89` | `gDrawNodeListModels(` |
| `src/game/renderer-scene/gRunViewScenePasses.c:90` | `gDrawDebrisFragments(` |
| `src/game/renderer-scene/gRunViewScenePasses.c:93` | `gDrawGlowNodeList(` |
| `src/game/renderer-scene/gRunViewScenePasses.c:94` | `gDrawRibbonList(` |
| `src/game/gpu-2d-hud/gDrawRegionTiles.c:72` | `gBuildTilePattern(` |
| `src/game/gpu-2d-hud/gDrawRegionTiles.c:74` | `gWorldToRadarCell(` |
| `src/game/gpu-2d-hud/gDrawRegionTiles.c:76` | `gEmitTexturedSurface(` |
| `src/game/gpu-2d-hud/gDrawRegionTiles.c:79` | `gEmitCellGeometry(` |
| `src/game/renderer-scene/gDrawCellWindowObjects.c:91` | `gDrawMapCellNode(` |
| `src/game/renderer-scene/gDrawCellWindowModels.c:181` | `gSetModelFrame(o, s);` |
| `src/game/renderer-scene/gDrawCellWindowModels.c:226` | `gGteLoadMatrix(&o->unk04);` |
| `src/game/renderer-scene/gDrawCellWindowModels.c:231` | `gGteTransformVerticesWide(` |
| `src/game/renderer-scene/gDrawCellWindowModels.c:235` | `gSubmitModelInstance(o, &local, 1);` |
| `src/game/renderer-scene/gSubmitModelInstance.c:42` | `gProjectModelInstance(instance);` |
| `src/game/renderer-scene/gSubmitModelInstance.c:44` | `gDrawModelInstanceParts(` |
| `src/game/gpu-2d-hud/gRunRegionObjectPass.c:72` | `gDrawModelFrame(object->data);` |
| `src/game/gpu-2d-hud/gPlacePlayerViews.c:111` | `gSetCameraPosition(&out);` |
| `src/game/gpu-2d-hud/gPlacePlayerViews.c:112` | `gSetCameraRotation(angle, entry->f2, 0);` |
| `src/game/gpu-2d-hud/gPlaceCameraFromPreset.c:83` | `gSetCameraRotation(` |
| `src/game/gpu-2d-hud/gPlaceCameraFromPreset.c:84` | `gSetCameraPosition(out);` |
| `src/game/gpu-2d-hud/gUpdateChaseCamera.c:512` | `gSetCameraRotation(yaw, pitch, 0);` |
| `src/game/gpu-2d-hud/gUpdateChaseCamera.c:563` | `gSetCameraPosition(&local.out);` |
| `src/game/gameplay-ai/gResetActorAndPlaceView.c:283` | `gSetCameraRotation(` |
| `src/game/gameplay-ai/gResetActorAndPlaceView.c:284` | `gSetCameraPosition(&position);` |
| `src/game/gpu-2d-hud/gSetCameraPosition.c:55` | `gMulMatrixVector(gCamera->matrix, position, &rotated);` |
| `src/game/gpu-2d-hud/gSetCameraRotation.c:49` | `gBuildRotationMatrix(yaw, pitch, roll, gCamera->rotation);` |
| `src/game/gpu-2d-hud/gSetCameraRotation.c:50` | `gBuildViewMatrix(gCamera->rotation, gCamera->matrix);` |
| `src/game/gte/gBuildViewMatrix.c:56` | `gTransposeMatrix3x3(` |
| `src/game/gte/gBuildViewMatrix.c:58` | `gMulMatrixVector(` |
| `src/game/utilities/gBuildRotationMatrix.c:47` | `gSinCos(x, &sx, &cx);` |
| `src/game/gpu-2d-hud/gSelectViewRecord.c:31` | `gCamera = gFindInstanceById(id);` |
| `src/game/renderer-scene/gUpdateLightDirection.c:39` | `gMulMatrixVector(gCamera + 0x28, gLightVectorIn, gLightVectorOut);` |

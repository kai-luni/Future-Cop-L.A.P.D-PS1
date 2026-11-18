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
   (`0x80027510`). Evidence: `src/game/80028618.c:131`, `src/game/80028618.c:133`.
2. **Entity world tick** - the driver calls `gRunEntityWorldTick`
   (`0x8003F570`), which ends with `gRunSceneListPassesOnScratch`
   (`0x8003F530`); that wrapper switches to the scratchpad stack and calls
   `gRunSceneListPasses` (`0x8003F4A0`). Evidence: `src/game/80027510.c:231`, `src/game/800271A4.c:183`, `src/game/8003F570.c:103`, `src/game/8003F530.c:36`.
3. **Scene-list passes** - `gRunSceneListPasses` makes eight calls over
   `gListHeads` (`D_800A1070`), e.g. `gTickEffectNodes` on list 2,
   `gUpdateObjectLights` on list 0, and last `gRunPolygonPass` (`0x80075A84`)
   on list 5, which culls/lights each particle node and draws it through
   `func_800751A4`. Evidence: `src/game/8003F4A0.c:45`, `src/game/8003F4A0.c:47`, `src/game/8003F4A0.c:50`.
4. **Camera anchor + cell window** - per present player, `func_80026E5C`
   runs `gUpdateCameraAnchor` (`0x80066C9C`) on the scratchpad. It selects the
   view record, loads the camera rotation and calls `gSampleCellWindow`
   (`0x80064460`, emitted as the raw word `0x0c019118` = `jal 0x80064460`) to
   fill the 25-entry window block in `D_800AB3B8` that the three layers read.
   Evidence: `src/game/80027510.c:233`, `src/game/800271A4.c:190`, `src/game/80026E5C.c:37`, `src/game/80066C9C.c:110`, `src/game/80066C9C.c:111`, `src/game/80066C9C.c:194`.
5. **View select + light** - before each region pass the driver selects that
   player's view record (`gSelectViewRecord`, `0x80017B60`) and calls
   `gUpdateLightDirection` (`0x80013C80`). In the solo driver the light update
   comes just before the anchor call (step 4), not after it. Evidence: `src/game/80027510.c:244`, `src/game/80027510.c:245`, `src/game/80027510.c:283`, `src/game/80027510.c:284`, `src/game/800271A4.c:189`.
6. **Region pass** - `gRunRegionPass` (`0x80026E1C`, player index 0 or 1)
   moves to the scratchpad stack and calls `gDrawViewLayers`. Evidence:
   `src/game/80027510.c:250`, `src/game/80027510.c:288`, `src/game/800271A4.c:194`, `src/game/80026E1C.c:36`.
7. **Three layers** - `gDrawViewLayers` (`0x80066E98`) selects the view
   record, uploads the camera rotation (`gGteLoadRotation`, `0x80010820`, on
   `gCamera + 0x28`), draws layer 1 `gDrawRegionTiles` (`0x8006531C`),
   re-uploads the rotation, then layer 2 `gDrawCellWindowObjects`
   (`0x8004A924`) and layer 3 `gDrawCellWindowModels` (`0x80084398`), all on
   the same `(x, y, region)` triple from `D_800AB3B8`. Evidence: `src/game/80066E98.c:63`, `src/game/80066E98.c:64`, `src/game/80066E98.c:68`, `src/game/80066E98.c:69`, `src/game/80066E98.c:70`, `src/game/80066E98.c:71`.
   - Layer 1 internals: `src/game/8006531C.c:72`, `src/game/8006531C.c:74`, `src/game/8006531C.c:76`, `src/game/8006531C.c:79` (tileset pattern, colour word,
     `gEmitTexturedSurface` `0x800646BC`, `gEmitCellGeometry` `0x80065134`).
   - Layer 2 internals: `src/game/8004A924.c:91` (`gDrawMapCellNode`, `0x8004A048`).
   - Layer 3 internals: `src/game/80084398.c:181`, `src/game/80084398.c:226`, `src/game/80084398.c:231`, `src/game/80084398.c:235`, `src/game/80017508.c:42`, `src/game/80017508.c:44` (frame, `gGteLoadMatrix` +
     `gGteTransformVerticesWide`, then `gSubmitModelInstance` ->
     `gProjectModelInstance` / `gDrawModelInstanceParts`).
8. **Close of the view** - `gRunRegionObjectPass` (`0x80062EB8`, draws
   visible entities via `gDrawModelFrame` `src/game/80062EB8.c:72`) and
   `gRunViewScenePasses` (`0x8003F6E4`), which resolves the view position and
   draws the object lists (`gDrawNodeListModels`, `gDrawDebrisFragments`,
   `gDrawGlowNodeList`, `gDrawRibbonList`, plus five still-unnamed `func_` calls).
   Evidence: `src/game/80066E98.c:72`, `src/game/80066E98.c:73`, `src/game/8003F6E4.c:89`, `src/game/8003F6E4.c:90`, `src/game/8003F6E4.c:93`, `src/game/8003F6E4.c:94`.

## 2. Camera / view

| step | function | what it does | evidence |
|---|---|---|---|
| placement | `gPlacePlayerViews` `0x80056634`, `gPlaceCameraFromPreset` `0x8005678C`, `gUpdateChaseCamera` `0x800569EC`, `gResetActorAndPlaceView` `0x800573A4` | the four view placements; each calls both setters | `src/game/80056634.c:111`, `src/game/80056634.c:112`, `src/game/8005678C.c:83`, `src/game/8005678C.c:84`, `src/game/800569EC.c:512`, `src/game/800569EC.c:563`, `src/game/800573A4.c:283`, `src/game/800573A4.c:284` |
| setter 1 | `gSetCameraPosition` `0x800177AC` | writes the eye position of the active view record and its rotated (inverse) form through `gMulMatrixVector` | `src/game/800177AC.c:55` |
| setter 2 | `gSetCameraRotation` `0x80017860` | yaw/pitch/roll -> 3x3 rotation (`gBuildRotationMatrix` `0x800188D0`), then the view matrix (`gBuildViewMatrix` `0x80018860`) | `src/game/80017860.c:49`, `src/game/80017860.c:50` |
| matrix build | `gBuildViewMatrix` `0x80018860` | transposes the rotation (`gTransposeMatrix3x3` `0x80019470`) and rotates the translation in software (`gMulMatrixVector` `0x8001A81C`, no GTE) | `src/game/80018860.c:56`, `src/game/80018860.c:58` |
| angles | `gBuildRotationMatrix` `0x800188D0` | sine/cosine pairs from `gSinCos` `0x8001846C` | `src/game/800188D0.c:47` |
| GTE upload | `gGteLoadRotation` `0x80010820` (asm) | uploads the camera rotation (no translation); the environment pass calls it before layer 1 and again before layers 2/3 | `src/game/80066E98.c:64`, `src/game/80066E98.c:69` |
| GTE upload (models) | `gGteLoadMatrix` `0x80010750` (asm) | uploads matrix + translation, rows y/z negated; used per model instance (e.g. layer 3), **not** by `gDrawViewLayers` for the camera | `src/game/80084398.c:226` |
| view record selection | `gSelectViewRecord` `0x80017B60` | `gCamera = gFindInstanceById(id)` (`0x80017B28`); called by the driver, the camera anchor and `gDrawViewLayers` | `src/game/80017B60.c:31`, `src/game/80027510.c:244`, `src/game/80066E98.c:63` |
| light | `gUpdateLightDirection` `0x80013C80` | light vector = camera matrix x light input (software); must follow the setters | `src/game/80013C80.c:39` |

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

Totals: 18 vector, 127 contract, 2 skip (147 units).

| address | readable name | test | role (from `@par Environment path`) | proved by / reason |
|---|---|---|---|---|
| `0x8001006C` | `gGteTransformWords` (asm) | vector | Batch vertex transform with 12-byte 32-bit vertices | `tests/host/env_test.sh` (env.vectors, XF6C 3 cases): retail asm words, cop2 model |
| `0x8001010C` | `gGteTransformVertices` (asm) | vector | Batch vertex transform of 8-byte vertices | `tests/host/env_test.sh` (env.vectors, XF10C 2 cases): retail asm words, cop2 model |
| `0x800101D4` | `gGteTransformVerticesV0` (asm) | vector | Same contract as the 8-byte batch transform, but it multiplies the GTE V0 vector instead of IR. Called by the light s... | `tests/host/env_test.sh` (env.vectors, XF1D4 2 cases): retail asm words, cop2 model |
| `0x80010234` | `gGteTransformVerticesWide` (asm) | vector | Batch vertex transform with 32-bit output and stride 0xC. Its 39 callers are the map-cell drawers (func_8002B588, fun... | `tests/host/env_test.sh` (env.vectors, XF234 2 cases): retail asm words, cop2 model |
| `0x8001047C` | `gProjectVertexBatch` (asm) | vector | The batched RTPS projection step: it loads each 8-byte source vertex into VXY0/VZ0, runs rtps, stores SXY2 and SZ3 in... | `tests/host/env_test.sh` (env.vectors, PROJ 5 cases): retail asm words, cop2 model, screen-width read handled per case |
| `0x80010750` | `gGteLoadMatrix` (asm) | vector | The per-model matrix upload: moves a finished model matrix into the GTE (rows 1..3 as F*M^T with F = diag(1,-1,1), y ... | `tests/host/env_test.sh` (env.vectors, LOADMAT 2 cases): retail asm words run, cop2 trap feeds a gte_ref model |
| `0x80010820` | `gGteLoadRotation` (asm) | vector | Same upload without translation, i.e | `tests/host/env_test.sh` (env.vectors, LOADROT 2 cases): retail asm words, cop2 model |
| `0x800108D8` | - (asm) | skip | Cross product variant that restores the caller's GTE matrix afterwards | no caller in the US binary and no readable binding; only checks (a)/(c) apply |
| `0x80010974` | `gGteCrossProduct` (asm) | contract | Cross product used to build a normal from two vectors: the light setups (func_800192AC, func_80019310, func_8001937C,... | structure only |
| `0x800136C8` | `gStepAnimations` | contract | Advances the two global texture-animation lists once per frame | structure only |
| `0x80013AA0` | `gSetModelUvOffset` | contract | Stores (high << 8) / low into D_8009F580, the UV offset that the model part emitters add to every u/v coordinate (fun... | structure only |
| `0x80013AF0` | `gInitModelInstance` | contract | Binds a model to a model instance: it stores the model pointer, copies the frame-0 radius shifted left by four togeth... | structure only |
| `0x80013C80` | `gUpdateLightDirection` | contract | Derives the frame's light direction from the camera matrix | structure only |
| `0x80013CB8` | `gComposeBoneMatrix` | vector | Builds the local transform of one hierarchy level and composes it into the bone matrix array D_800A0300: each of the ... | `tests/host/env_test.sh` (env.vectors, COMPOSE 3 cases) and `compose_test.sh`; GTE callees are software stand-ins |
| `0x80013F08` | `gProjectModelInstance` | contract | Computes one model instance's camera-relative matrix, tests the frame's reference vertex against the view volume and ... | structure only |
| `0x80014164` | `gComposeTransform` | contract | Composes a part transform with its owner's matrix | structure only |
| `0x8001428C` | `gProjectBoneParts` | contract | Transforms and projects a jointed model part by part without emitting any primitive | structure only |
| `0x80014490` | `gEmitRayBurst` | contract | Emits a ray burst: one 0x1C-byte header packet (draw mode 0xE10006A0) and a set of semi-transparent Gouraud triangles... | structure only |
| `0x8001484C` | `gEmitGouraudDisc` | contract | Emits one gouraud-shaded triangle fan as a filled disc: it returns immediately when depth minus the depth offset is n... | structure only |
| `0x80014C64` | `gDrawBeamQuad` | contract | Projects two world points relative to the camera centre, loads the camera matrix with func_80010820, projects both po... | structure only |
| `0x80015830` | `gBuildLineQuad` | contract | Builds the screen-space quad of a type-7 model part from two transformed vertices: it rejects the part when either en... | structure only |
| `0x80015AF4` | `gEmitFrontFacingTriangle` | vector | Backface/winding test for a triangle in packed 16.16 coordinates | `tests/host/build.sh` (harness.c vs render.vectors): `w_keep`, `w_cull`, `w_flat` |
| `0x80015B68` | `gPrepareModelPart` | vector | Prepares one model part: backface and on-screen rejection, then the screen-space corners and the depth into the work ... | `tests/host/build.sh` (harness.c vs render.vectors): `p_tri`, `p_tri_bias`, `p_tri_clip1..3`, `p_quad`, `p_bill` |
| `0x80015F38` | `gEmitShadedPart` | contract | Turns one prepared model part into a single GPU packet from the gPrimPoolNext pool: the part flags select flat/gourau... | structure only |
| `0x800164F4` | `gDrawModelFrame` | contract | Draws one animated model frame as textured, depth-cued triangles | structure only |
| `0x80016A64` | `gDrawModelInstanceParts` | vector | Draws the masked parts of one model instance, with a billboard path for flag-0x40 models | `tests/host/build.sh` (harness.c vs render.vectors): `k_mask8`, `k_maskf7` |
| `0x80016C88` | `gComputeEnvMapUV` | contract | Computes one environment-map texture coordinate pair for a polygon corner: the vector from the vertex to the referenc... | structure only |
| `0x80016E6C` | `gEmitEnvMapPolygon` | contract | Emits one environment-mapped textured polygon for a model part: it takes a 0x34-byte packet from gPrimPoolNext (bound... | structure only |
| `0x80017058` | `gDrawModelPartsMasked` | contract | Part-submission stage of the jointed-model path: func_800175A0 calls it right after func_8001428C has projected the p... | structure only |
| `0x80017508` | `gSubmitModelInstance` | contract | Visibility gate and submission for one model instance | structure only |
| `0x800175F8` | `gSetModelFrame` | contract | Commits a new animation frame on a model instance: it returns when the frame index is unchanged, otherwise it stores ... | structure only |
| `0x80017668` | `gTransformPartVertex` | contract | Fetches one vertex of a model part for the instance's current animation frame and transforms it: gGteLoadMatrix (func... | structure only |
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
| `0x80019470` | `gTransposeMatrix3x3` | vector | Transposes a 3x3 s32 matrix into the destination with nine element assignments and no loop, branch or call | `tests/host/env_test.sh` (env.vectors, TRANSPOSE 2 cases) |
| `0x80019D90` | `gTestFrustumFixedNear` | contract | Frustum visibility test for one point plus a safety margin, with a fixed near threshold: depth = z << 4 returns 1 as ... | structure only |
| `0x8001A81C` | `gMulMatrixVector` | vector | Software matrix-times-vector in 16.16 used whenever the GTE must not be disturbed, e.g | `tests/host/env_test.sh` (env.vectors, MULMV 3 cases) |
| `0x80026E1C` | `gRunRegionPass` | contract | Runs the environment pass func_80066E98 on the scratchpad stack | structure only |
| `0x800271A4` | `gRunSoloSessionFrame` | contract | One-player frame driver, called from func_80028618 after func_8002703C when gPlayerCount == 1 (gRunSessionFrame, func... | structure only |
| `0x80027510` | `gRunSessionFrame` | contract | The frame driver: runs the environment pass through func_80026E1C and the light direction update once per frame, arou... | structure only |
| `0x80028618` | `gRunGameFrameLoop` | contract | The in-game frame loop: it clears the game clock, sets the frame delta to 8, resets gSceneFrameCounter and seeds the ... | structure only |
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
| `0x80030C60` | `gWarpEntityToSelf` | contract | Message-11 body of the type-29 handler func_80030D3C: it moves the entity that sent the message (or that entity's chi... | structure only |
| `0x80031238` | `gTestEntityOverlap` | contract | Overlap test between two entities: three branch-free absolute differences against the summed half extents scaled by 1... | structure only |
| `0x800317BC` | `gInitFromSourceModel` | contract | Spawn helper of the entity class handled by func_8003190C (message 2): it forwards message 2 to func_8002AD58, rebind... | structure only |
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
| `0x8003AA94` | `gSpawnModelDebris` | contract | Message-2 (spawn) case of the object class handled by func_8003B2D0: it allocates a 0xF0-byte piece array with func_8... | structure only |
| `0x8003CF6C` | `gDrawNodeListModels` | contract | One of the scene passes of func_8003F6E4: it walks the node list D_8009F320, skips nodes without an object or without... | structure only |
| `0x8003D9F8` | `gResolveNodeTransform` | contract | Resolves an object's world transform from a parent, but only when flag 0x100 is set | structure only |
| `0x8003DB70` | `gBuildNodePlacement` | contract | Resolves the placement of a spawned node into a 12-word block: a nine-word basis in out[0..8] and a world position in... | structure only |
| `0x8003F4A0` | `gRunSceneListPasses` | contract | Runs the six per-layer render passes over the global scene lists in a fixed order | structure only |
| `0x8003F530` | `gRunSceneListPassesOnScratch` | contract | Runs gRunSceneListPasses on the scratchpad stack, so the passes get their own stack for the frame. | structure only |
| `0x8003F570` | `gRunEntityWorldTick` | contract | The per-frame tick of the entity world, called with the frame delta by both session updates, func_800271A4 (one playe... | structure only |
| `0x8003F6E4` | `gRunViewScenePasses` | contract | Resolves the view position (from gFocusPositions, otherwise func_80069580) and then runs nine list passes in a fixed ... | structure only |
| `0x80040A00` | `gUpdateObjectLights` | contract | Walks a linked object list and integrates one time step per node: with bit 3 of the flags byte clear and an inner rec... | structure only |
| `0x80041488` | `gInitElevator` | contract | Message-2 (init) handler of the lift object stepped by func_80041880 (gStepElevator) | structure only |
| `0x80041728` | `gRunEntityCommands` | contract | Processes the four-slot command queue at +0x0C of a waypoint-following entity | structure only |
| `0x80041880` | `gStepElevator` | contract | Tick of a vertical lift object (sole caller func_80041B6C): after optional func_80041728 and the hold/delay gates, it... | structure only |
| `0x80041C6C` | `gHandlePlacedPropMsg` | contract | Message handler of another entity class: message 1 answers 0xA4, message 2 rebuilds the orientation from the descript... | structure only |
| `0x80042128` | `gDrawPortraitScreen` | contract | Draw case (message 3 of the type-34 handler func_80042330) of an in-world screen that shows the animated comm portrai... | structure only |
| `0x80044260` | `gDrawObjectPointTiles` (asm) | contract | Draws an object's point list as small screen-space rectangles | structure only |
| `0x80044988` | `gSetAnimClipTable` | contract | Installs an animation clip table on a track: it stores the clip array at data + 4 and the clip count from data[0], se... | structure only |
| `0x8004A048` | `gDrawMapCellNode` | contract | Draws one map-cell object as a camera-facing billboard or a four-corner quad, after a perspective cull, a clip-flag t... | structure only |
| `0x8004A924` | `gDrawCellWindowObjects` | contract | Layer 2 of the environment pass: draws the map-cell objects (walls, props) of the 5x5 cell window, filtered by the pe... | structure only |
| `0x8004ABE4` | `gLaunchDebrisObject` | contract | Starts the ballistic flight of a debris object when it is destroyed | structure only |
| `0x8004AFC0` | `gStepDebrisFlight` | contract | Advances a launched debris object by one step | structure only |
| `0x8004C5C8` | `gDrawDebrisFragments` | contract | Draws the fragment polygons of the debris objects in one scene list | structure only |
| `0x8005027C` | `gStepPathMover` | contract | Advances an object along its path-network route by one step | structure only |
| `0x800518A8` | `gUpdateMorphBounds` | contract | Updates the player unit's progress-dependent footprint for the form change: from the model's current frame element it... | structure only |
| `0x80051C50` | `gBuildObjectBasis` | contract | Builds an object's orientation matrix into the 3x3 block passed as the last argument (the caller passes the object's ... | structure only |
| `0x80051CC4` | `gPushFromPartBox` | contract | Collision push against one box-shaped model part, called from the masked part loop of func_80051EB8 for every part bi... | structure only |
| `0x80052410` | `gTestFootprintEdges` | contract | Builds the object's basis with func_80051C50, loads it with func_80010750 and transforms the four footprint corners o... | structure only |
| `0x80052960` | `gComposeJointChain` | vector | Composes a joint chain into a local matrix stack seeded with the object's coordinate matrix: for each record the thre... | `tests/host/joints_test.sh` (joints.vectors, 7 cases); GTE software model |
| `0x80052D7C` | `gUpdateTwoPartRig` | contract | Refreshes the two auxiliary model instances held in an entity's state block (object->+0xB4->+0xAC, instances at +0x17... | structure only |
| `0x80053A74` | `gTickHoverPhysics` | contract | Drive integrator of the player unit in form 1 (state + 0xAD == 1, the hover form in the caller's documentation) | structure only |
| `0x8005612C` | `gComputeCameraEye` | contract | Computes a camera eye position around a base point: the per-view offset block is scaled by (0x100 - arg7/2)/256, lowe... | structure only |
| `0x800564A8` | `gComputeBoomDistance` | contract | Maps a pitch angle to a camera distance: value = 0x10000 - ((pitch - 0x14A) << 16) / 600, then 0x20000 * (0x10000 - v... | structure only |
| `0x80056634` | `gPlacePlayerViews` | contract | Places both players' view objects before the frame is drawn | structure only |
| `0x8005678C` | `gPlaceCameraFromPreset` | contract | Computes a camera position from a preset offset around a follow target and, when asked, applies it through gSetCamera... | structure only |
| `0x80056870` | `gFindClearCameraPreset` | contract | Searches the alternate camera presets 9..16 for a view of the followed object that is not blocked: it starts at a ran... | structure only |
| `0x800569EC` | `gUpdateChaseCamera` | contract | Chase-camera step: selects the preset, probes it against the map and smooths yaw, pitch, boom and eye offset into the... | structure only |
| `0x800573A4` | `gResetActorAndPlaceView` | contract | Start-position step: resets an actor from its spawn record and places the third-person view that belongs to it, which... | structure only |
| `0x8005780C` | `gDecodeButtonEdge` | contract | Four-phase button decoder: a set bit in the current mask moves the action record to the press edge (phase 0) or to he... | structure only |
| `0x80057E40` | `gSlewEntityAim` | contract | Aims the player unit's two-part gun assembly (the +0xDC companion passed by func_800588D4, its only caller): it place... | structure only |
| `0x80058714` | `gRespawnPlayerEntity` | contract | Respawns a player entity when its respawn countdown has run out | structure only |
| `0x800588D4` | `gUpdatePlayerEntity` | contract | Per-frame tick of the player unit, called from its class handler func_8005908C: it resets state +0x2FC, re-arms the m... | structure only |
| `0x800592D0` | `gDrawOtherPlayerArrow` | contract | Two-player overlay drawn from the player-unit draw handler func_800597C0: it returns unless gPlayerCount is at least ... | structure only |
| `0x80059ABC` | `gHandleCompanionMessage` | contract | Message handler of the kind-0x12 companion object that func_80057914 queues on a part of the player unit, so it has n... | structure only |
| `0x8005CED4` | `gTakeArmedLobePoint` | contract | Helper of the damage-field update func_8005D8F4 (its only caller): it scans the twelve 0x1C-byte lobes from +0x58 for... | structure only |
| `0x8005D060` | `gSpawnLobeArc` | contract | Steps one of the twelve lobes of the damage field updated by func_8005D8F4 (its only caller): once the frame time D_8... | structure only |
| `0x800619C0` | `gBoxOverlapBitmask` | contract | Turns one oriented box into a world axis-aligned box and returns a per-entry overlap bitmask: it builds a diagonal sc... | structure only |
| `0x80061CAC` | `gTestPartPairHit` | contract | Narrow-phase hit test between two coord records: bit 27 of each model flag word gives a two-bit mode, and the functio... | structure only |
| `0x80062EB8` | `gRunRegionObjectPass` | contract | Update and draw pass over the entity table of one region: message 3 to every object, then the polygons of the visible... | structure only |
| `0x80064460` | `gSampleCellWindow` | contract | Fills the 25 window entries (mask, node, world position) that the three region layers then draw. | structure only |
| `0x800646BC` | `gEmitTexturedSurface` (asm) | contract | Emits the textured triangle strips of one region surface with per-strip clipping | structure only |
| `0x80065134` | `gEmitCellGeometry` | contract | Emits the visible cells of one region as geometry | structure only |
| `0x8006531C` | `gDrawRegionTiles` | contract | Layer 1 of the environment pass: draws the 5x5 window of map regions (the tiled ground surfaces) around the camera po... | structure only |
| `0x800654F8` | `gBuildTilePattern` | contract | Builds the 256-byte pattern uploaded for a tileset | structure only |
| `0x80065744` | `gWorldToRadarCell` | contract | Maps a world coordinate onto the tile/radar grid coordinate | structure only |
| `0x80065834` | `gDrawRadarBlip` | contract | Draws one entity onto the radar/minimap | structure only |
| `0x8006680C` | `gDrawMapView` | contract | Draws the map view around a position for the map screen | structure only |
| `0x80066C9C` | `gUpdateCameraAnchor` | contract | Per-view look-at update: it selects the player's view record with gSelectViewRecord, loads the camera rotation, and i... | structure only |
| `0x80066E98` | `gDrawViewLayers` | contract | The environment pass of one view: selects the view record, loads the camera rotation, then draws the same region in t... | structure only |
| `0x80066F78` | `gGetGroundHeight` | contract | Returns the ground height under a world position by walking one map cell: gTileMapTable (D_8009F80C) gives the grid s... | structure only |
| `0x8006CB58` | `gDrawShadedModel` | contract | Draws one procedural effect shape with a colour gradient: it generates up to sixteen mirrored corner points (+-size o... | structure only |
| `0x8006D770` | `gTickEffectNodes` | contract | Per-frame update of the effect objects in gListHeads bucket 2 | structure only |
| `0x8006DC24` | `gDrawFlatRing` | contract | Draws one model-list node as a flat ring (annulus) in its local x/z plane | structure only |
| `0x80070158` | `gDrawGlowNodeList` | contract | Walks the node list from the argument, resolves each node's transform with func_8003D9F8, culls it by radius against ... | structure only |
| `0x80070730` | `gComputeEffectEndPoint` | contract | Computes one end point of a beam or ribbon strip and returns the index of the player entity it locked onto, or -1 | structure only |
| `0x80071B40` | `gStepBeamNode` | contract | Per-frame step of one beam node, called by the list pass func_8007364C over gModelNodeList (D_8009F38C) once the node... | structure only |
| `0x80071FA4` | `gDrawBeamNode` | contract | Draws one beam node, a set of point strips stretched between two end objects: per strip it makes the points camera-re... | structure only |
| `0x80074104` | `gDrawRibbonList` | contract | Renders every entry of a linked object list as camera-facing ribbons: per entry it builds the strip transform with fu... | structure only |
| `0x800754B8` | `gInitEmitterParticle` | contract | Initialises one particle node that an emitter object has just taken from its pool: it writes the world position, the ... | structure only |
| `0x80075A84` | `gRunPolygonPass` | contract | Polygon pass over one scene pool: walks the active particle list, culls and lights each node and draws it through fun... | structure only |
| `0x80083A10` | `gSpawnSceneNode` | contract | Creates one map-cell model node and inserts it into the cell bucket grid: it takes a 0x5C-byte node from the 250-entr... | structure only |
| `0x80084398` | `gDrawCellWindowModels` | contract | Layer 3 of the environment pass: animates and submits the models of the same cell window, i.e | structure only |
| `0x80095B64` | - (asm) | skip | Cross product through a temporary matrix | no caller in the US binary and no readable binding; only checks (a)/(c) apply |

## Citation index

Each entry is checked by `tests/test_environment_path.py` (the fragment must
occur on the cited line).

| citation | code fragment |
|---|---|
| `src/game/80028618.c:131` | `gRunSoloSessionFrame();` |
| `src/game/80028618.c:133` | `gRunSessionFrame();` |
| `src/game/80027510.c:231` | `gRunEntityWorldTick(` |
| `src/game/800271A4.c:183` | `gRunEntityWorldTick(` |
| `src/game/8003F570.c:103` | `gRunSceneListPassesOnScratch(arg0);` |
| `src/game/8003F530.c:36` | `jal func_8003F4A0` |
| `src/game/8003F4A0.c:45` | `gTickEffectNodes(gListHeads[2], arg);` |
| `src/game/8003F4A0.c:47` | `gUpdateObjectLights(gListHeads[0], arg);` |
| `src/game/8003F4A0.c:50` | `gRunPolygonPass(gListHeads[5], arg);` |
| `src/game/80027510.c:233` | `func_80026E5C(0,` |
| `src/game/800271A4.c:190` | `func_80026E5C(0,` |
| `src/game/80026E5C.c:37` | `jal func_80066C9C` |
| `src/game/80066C9C.c:110` | `gSelectViewRecord(` |
| `src/game/80066C9C.c:111` | `gGteLoadRotation(gCamera->matrix);` |
| `src/game/80066C9C.c:194` | `.word 0x0c019118` |
| `src/game/80027510.c:244` | `gSelectViewRecord(` |
| `src/game/80027510.c:245` | `gUpdateLightDirection();` |
| `src/game/80027510.c:283` | `gSelectViewRecord(` |
| `src/game/80027510.c:284` | `gUpdateLightDirection();` |
| `src/game/800271A4.c:189` | `gUpdateLightDirection();` |
| `src/game/80027510.c:250` | `gRunRegionPass(0,` |
| `src/game/80027510.c:288` | `gRunRegionPass(1,` |
| `src/game/800271A4.c:194` | `gRunRegionPass(0,` |
| `src/game/80026E1C.c:36` | `jal func_80066E98` |
| `src/game/80066E98.c:63` | `gSelectViewRecord(object->info->id);` |
| `src/game/80066E98.c:64` | `gGteLoadRotation(gCamera + 0x28);` |
| `src/game/80066E98.c:68` | `gDrawRegionTiles(x, y, region);` |
| `src/game/80066E98.c:69` | `gGteLoadRotation(gCamera + 0x28);` |
| `src/game/80066E98.c:70` | `gDrawCellWindowObjects(x, y, region);` |
| `src/game/80066E98.c:71` | `gDrawCellWindowModels(x, y, region);` |
| `src/game/80066E98.c:72` | `gRunRegionObjectPass(index);` |
| `src/game/80066E98.c:73` | `gRunViewScenePasses(index);` |
| `src/game/8003F6E4.c:89` | `gDrawNodeListModels(` |
| `src/game/8003F6E4.c:90` | `gDrawDebrisFragments(` |
| `src/game/8003F6E4.c:93` | `gDrawGlowNodeList(` |
| `src/game/8003F6E4.c:94` | `gDrawRibbonList(` |
| `src/game/8006531C.c:72` | `gBuildTilePattern(` |
| `src/game/8006531C.c:74` | `gWorldToRadarCell(` |
| `src/game/8006531C.c:76` | `gEmitTexturedSurface(` |
| `src/game/8006531C.c:79` | `gEmitCellGeometry(` |
| `src/game/8004A924.c:91` | `gDrawMapCellNode(` |
| `src/game/80084398.c:181` | `gSetModelFrame(o, s);` |
| `src/game/80084398.c:226` | `gGteLoadMatrix(&o->unk04);` |
| `src/game/80084398.c:231` | `gGteTransformVerticesWide(` |
| `src/game/80084398.c:235` | `gSubmitModelInstance(o, &local, 1);` |
| `src/game/80017508.c:42` | `gProjectModelInstance(instance);` |
| `src/game/80017508.c:44` | `gDrawModelInstanceParts(` |
| `src/game/80062EB8.c:72` | `gDrawModelFrame(object->data);` |
| `src/game/80056634.c:111` | `gSetCameraPosition(&out);` |
| `src/game/80056634.c:112` | `gSetCameraRotation(angle, entry->f2, 0);` |
| `src/game/8005678C.c:83` | `gSetCameraRotation(` |
| `src/game/8005678C.c:84` | `gSetCameraPosition(out);` |
| `src/game/800569EC.c:512` | `gSetCameraRotation(yaw, pitch, 0);` |
| `src/game/800569EC.c:563` | `gSetCameraPosition(&local.out);` |
| `src/game/800573A4.c:283` | `gSetCameraRotation(` |
| `src/game/800573A4.c:284` | `gSetCameraPosition(&position);` |
| `src/game/800177AC.c:55` | `gMulMatrixVector(gCamera->matrix, position, &rotated);` |
| `src/game/80017860.c:49` | `gBuildRotationMatrix(yaw, pitch, roll, gCamera->rotation);` |
| `src/game/80017860.c:50` | `gBuildViewMatrix(gCamera->rotation, gCamera->matrix);` |
| `src/game/80018860.c:56` | `gTransposeMatrix3x3(` |
| `src/game/80018860.c:58` | `gMulMatrixVector(` |
| `src/game/800188D0.c:47` | `gSinCos(x, &sx, &cx);` |
| `src/game/80017B60.c:31` | `gCamera = gFindInstanceById(id);` |
| `src/game/80013C80.c:39` | `gMulMatrixVector(gCamera + 0x28, gLightVectorIn, gLightVectorOut);` |

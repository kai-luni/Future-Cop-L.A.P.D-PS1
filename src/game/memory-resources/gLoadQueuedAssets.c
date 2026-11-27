// Categories: Resources > Asset Loader | Graphics > Asset Dispatch
#include "game/types.h"

struct Node24EC8 {
    u8 pad00[0xC];
    struct Node24EC8 *next;
    struct Node24EC8 *prev;
    u8 pad14[4];
    u32 tag;
    s32 unk1C;
    u8 pad20[4];
    s32 unk24;
    s32 unk28;
    s32 unk2C;
};

extern struct Node24EC8 *gRequestListHead __asm__("D_8009F668");
extern struct Node24EC8 *gRequestListTail __asm__("D_8009F66C");
extern s32 gRequestOffsetBias __asm__("D_8009F9F4");
extern s32 gScreenState __asm__("D_8009FA00");

extern struct Node24EC8 *func_800295E8(s32 arg0);
extern void func_80013550(struct Node24EC8 *node);
extern void gFreeBlock(struct Node24EC8 *node) __asm__("func_800181D4");
extern void gRegisterCvkChunk(struct Node24EC8 *node) __asm__("func_80023930");
extern void gRegisterPortraitStreamTable(struct Node24EC8 *node) __asm__("func_80029E20");
extern void gLoadCsfxResource(struct Node24EC8 *node) __asm__("func_8003C110");
extern void gLoadCfntResource(struct Node24EC8 *node) __asm__("func_80048E9C");
extern void gLoadScriptTimers(struct Node24EC8 *node) __asm__("func_80049420");
extern void gLoadCdcsResource(struct Node24EC8 *node) __asm__("func_80049734");
extern void gLoadActorChunk(struct Node24EC8 *node) __asm__("func_8004D6D4");
extern void gRegisterResourceWithBias(struct Node24EC8 *node) __asm__("func_8004D924");
extern void gLoadPathNodePositions(struct Node24EC8 *node) __asm__("func_800511E4");
extern void gLoadTextureChunksToVram(struct Node24EC8 *node) __asm__("func_8005A414");
extern void gRegisterUnitSpawner(struct Node24EC8 *node) __asm__("func_80060E74");
extern void gLoadMapTileCatalog(struct Node24EC8 *node) __asm__("func_80063C20");
extern void gLoadCbmpTexture(struct Node24EC8 *node) __asm__("func_80063C5C");
extern void gInitMapFromGrdb(struct Node24EC8 *node) __asm__("func_80063DC8");
extern void gUploadSaveProfileIcon(struct Node24EC8 *node) __asm__("func_80082358");
extern void gLoadCtdtImages(struct Node24EC8 *node) __asm__("func_800824CC");
extern void gValidateAtlasSpriteId(struct Node24EC8 *node) __asm__("func_800825AC");
extern void gRegisterPublishedBlock(struct Node24EC8 *node) __asm__("func_8008263C");
extern void gLoadTextLookupTable(struct Node24EC8 *node) __asm__("func_800899A4");
void gLoadQueuedAssets(void) __asm__("func_80024EC8");


/**
 * @brief Loads every queued asset of the level and frees the request nodes.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Walks the request list D_8009F668/D_8009F66C (a doubly linked list with
 * `next`/`prev`), takes the next node first, and dispatches on `node->tag`
 * through a switch whose arms call one loader per asset class: func_800899A4,
 * func_800825AC, func_800824CC, func_8008263C, func_80063C20, func_80063C5C,
 * func_80063DC8, func_80048E9C, func_80013550, func_8004D6D4, func_8004D924,
 * func_80060E74, func_80049420, func_80023930, func_80082358, func_80029E20,
 * func_800511E4, func_80049734 and func_8005A414. Every node is released with
 * func_800181D4 afterwards.
 *
 * @par Likely role
 * The central "load what this level needs" pass: one tag per asset type
 * (textures, models, animation tables, sound banks), called once per frame
 * while the state machine is not in state 4 (D_8009FA00).
 *
 * @par Callers
 * func_80027EBC (the frame step of a scene) and func_800295E8.
 *
 * @par Texture load path
 * Request nodes are popped from the streaming queue D_800A0B70 by func_800295E8
 * and appended to D_8009F668/D_8009F66C here. Runs once per frame from
 * func_80027EBC while D_8009FA00 != 4,
 * dispatching pixel-bearing request nodes by ASCII tag to their loaders before
 * freeing each node with func_800181D4: `0x43707972` ("Cpyr") -> func_8005A414,
 * `0x43746474` ("Ctdt") -> func_800824CC, `0x4374696E` ("Ctin") -> func_800825AC,
 * `0x43626D70` ("Cbmp") -> func_80063C5C, `0x436D6963` ("Cmic") -> func_80082358,
 * and `0x436F626A` ("Cobj") -> func_80013550 (geometry/animation, no pixel upload).
 *
 * @par Environment path
 * Drains finished asset requests and dispatches them by four-character tag:
 * every node returned by func_800295E8 is appended to D_8009F668/D_8009F66C,
 * then each list node is unlinked (skipped and kept while gScreenState
 * D_8009FA00 == 4), its words +0x24/+0x28/+0x2C are rebased by D_8009F9F4 and
 * the tag selects the handler, e.g. 'Cptc' func_80063DC8 (GRDB map), 'Cact'
 * func_8004D6D4 (actor records), 'Csfx' func_8003C110 (spawn-record stream),
 * 'Ctil'/'Catl' func_80063C20, 'Cbmp' func_80063C5C, 'Cobj' func_80013550,
 * 'RPNS' func_8004D924 and 'Cvkh'/'Cvkb'/'Cshd' func_80023930; 'Cctr' sets
 * gScreenState to 2 or 4, 'Cnon' is dropped without a free and unknown tags
 * are freed. It is polled every frame by the loading and front-end frame
 * ticks func_80025348, func_80025690, gPrepareAndSubmitFrame (func_800259D8),
 * func_80027D9C and func_80027EBC. All per-level construction that arrives as
 * chunks (map grid, spawn tables) is started from this switch, in the order
 * the stream delivers the chunks.
 */
void gLoadQueuedAssets(void)
{
    struct Node24EC8 *node;
    struct Node24EC8 *next;
    s32 flag;
    s32 mode;

    flag = 1;
    goto loop_test;
loop_body:
    node->next = 0;
    node->prev = 0;
    if (gRequestListTail != 0) {
        gRequestListTail->next = node;
        node->prev = gRequestListTail;
    } else {
        gRequestListHead = node;
    }
    gRequestListTail = node;
    flag = 0;
loop_test:
    node = func_800295E8(flag);
    if (node != 0) {
        goto loop_body;
    }

    if (gRequestListHead != 0) {
      node = gRequestListHead;
      do {
        next = node->next;
        if (gScreenState != 4) {
            if (node->prev != 0) {
                node->prev->next = next;
            } else {
                gRequestListHead = next;
                if (next != 0) {
                    next->prev = 0;
                }
            }

            if (node->next != 0) {
                node->next->prev = node->prev;
            } else {
                gRequestListTail = node->prev;
                if (gRequestListTail != 0) {
                    gRequestListTail->next = 0;
                }
            }

            node->unk24 += gRequestOffsetBias;
            node->unk28 += gRequestOffsetBias;
            node->unk2C += gRequestOffsetBias;

            switch (node->tag) {
            case 0x43747273:
                gLoadTextLookupTable(node);
                break;
            case 0x4374696E:
                gValidateAtlasSpriteId(node);
                break;
            case 0x43746474:
                gLoadCtdtImages(node);
                break;
            case 0x436D6463:
                gRegisterPublishedBlock(node);
                break;
            case 0x4361746C:
            case 0x4374696C:
                gLoadMapTileCatalog(node);
                break;
            case 0x43626D70:
                gLoadCbmpTexture(node);
                break;
            case 0x43707463:
                gInitMapFromGrdb(node);
                break;
            case 0x43666E74:
                gLoadCfntResource(node);
                break;
            case 0x43637472:
                switch (node->unk1C) {
                case 0:
                    mode = 2;
                    break;
                case 1:
                    mode = 4;
                    break;
                default:
                    goto default_action;
                }
                gScreenState = mode;
                goto default_action;
            case 0x436F626A:
                func_80013550(node);
                break;
            case 0x43616374:
                gLoadActorChunk(node);
                break;
            case 0x52504E53:
                gRegisterResourceWithBias(node);
                break;
            case 0x43736163:
                gRegisterUnitSpawner(node);
                break;
            case 0x4366756E:
                gLoadScriptTimers(node);
                break;
            case 0x43736864:
            case 0x43766B68:
            case 0x43766B62:
                gRegisterCvkChunk(node);
                break;
            case 0x436D6963:
                gUploadSaveProfileIcon(node);
                break;
            case 0x43746F73:
                gRegisterPortraitStreamTable(node);
                break;
            case 0x436E6574:
                gLoadPathNodePositions(node);
                break;
            case 0x43646373:
                gLoadCdcsResource(node);
                break;
            case 0x43707972:
                gLoadTextureChunksToVram(node);
                break;
            case 0x43736678:
                gLoadCsfxResource(node);
                break;
            case 0x436E6F6E:
                break;
            default:
            default_action:
                gFreeBlock(node);
                break;
            }
        }
        node = next;
      } while (node != 0);
    }
}

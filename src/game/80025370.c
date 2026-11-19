// Categories: Graphics > Image Loading & Display | Resources > File Access
typedef unsigned char u8;
typedef signed char s8;
struct Entry { char *name; s8 type; u8 value; char pad[2]; };
extern u8 gUploadRequestFlag __asm__("D_8009F3E5");
extern s8 gModuleLoadState __asm__("D_8009F3F0");
extern struct Entry gScreenIndexTable[] __asm__("D_8009B42C");
extern char gFormatNamePair[] __asm__("D_8009F2E4");
extern int gModulePrefixCW __asm__("D_8009F3D8");
extern int gModulePrefixPA __asm__("D_8009F3E0");
extern int gFileSearchGroup __asm__("D_8009F238");
extern int gUploadInProgress __asm__("D_8009F9C4");
extern void func_8008CD30(char *, char *, int, char *);
extern void func_8001E110(char *, int, void **, int *, int);
extern void gDecodeMdecFrameToVram(void *, int, int, int) __asm__("func_800861E4");
extern void gFreeBlock(void *) __asm__("func_800181D4");
extern void gEmitSplitScreenBlit(int) __asm__("func_8002549C");
extern void func_8001B4A8(int);
int gShowLoadingScreen(void) __asm__("func_80025370");


/**
 * @brief Loads and displays the loading-screen image (*.lsc) selected by the
 *        global screen index, unless a screen is already pending.
 *
 * @return 1 if an image was loaded and displayed, 0 if nothing was done
 *         (either D_8009F3E5 was already set, the index was out of range, or
 *         the load returned no data).
 *
 * @par Confirmed flow
 * 1. Guard: only runs when D_8009F3E5 == 0 and D_8009F3F0 < 23.
 * 2. entry = &D_8009B42C[D_8009F3F0]; the table holds 23 entries of
 *    { char *name; s8 type; u8 value; } - the *.lsc loading-screen files
 *    ("feld.lsc", "m4a1ld.lsc", ... "alegal.lsc", "jokepo.lsc").
 * 3. The directory prefix depends on the entry type: type 1 selects
 *    D_8009F3E0 ("PA\\"), anything else D_8009F3D8 ("CW\\").
 * 4. sprintf(buffer, "%s%s", prefix, entry->name) and
 *    func_8001E110(buffer, D_8009F238, &data, &size, 0) loads the file into a
 *    freshly allocated block (data/size out-parameters).
 * 5. If data != 0 the global flag D_8009F9C4 is saved, set to 1 for the
 *    duration of the upload, and func_800861E4(data + 8, entry->value, 320, 0)
 *    plus a second call with the offset taken from the first word of the
 *    block (+8) and VRAM x = 256 transfer the image as two 320-pixel halves
 *    (the second one at x = 256, which only makes sense with the 0x100 screen
 *    offset from func_8001B2C8/8001C034).
 * 6. The flag is restored, the block freed and func_8002549C(0) /
 *    func_8001B4A8(1) notified - the "screen is up" signal.
 * 7. D_8009F3E5 is cleared unconditionally at the end.
 *
 * @par Globals
 * D_8009F3E5 (request flag, cleared), D_8009F3F0 (screen index),
 * D_8009B42C (23-entry *.lsc table), D_8009F2E4 ("%s%s"),
 * D_8009F3D8/D_8009F3E0 ("CW\\"/"PA\\"), D_8009F238 (load parameter),
 * D_8009F9C4 (upload guard), D_8009F3D0/D_8009F3D8 also appear in
 * func_8008CD30 call sites of the front-end.
 *
 * @par Likely role
 * This is the visible half of the loading screen: the front-end sets
 * D_8009F3F0 to the mission/menu entry index and D_8009F3E5 to request the
 * matching *.lsc artwork.
 *
 * @par Unknown
 * The meaning of the per-entry @c value byte (0 or 1) beyond being passed to
 * the upload routine, and of D_8009F238 (zero in the retail image).
 *
 * @par Environment path
 * Loading-screen draw step: only when gUploadRequestFlag is 0 and
 * gModuleLoadState is below 23, it formats the file name with func_8008CD30
 * (format gFormatNamePair, prefix gModulePrefixPA for type 1 else
 * gModulePrefixCW), loads the file with func_8001E110 into a fresh block and,
 * with gUploadInProgress forced to 1, decodes the two images with
 * func_800861E4 to destination arguments 320,0 and 320,256. It then frees the
 * block, emits the two-sprite screen blit via func_8002549C(0) and calls
 * func_8001B4A8(1); gUploadRequestFlag is cleared on every exit. Callers are
 * the front-end frame func_800259D8 and the main loop func_80076A2C, which
 * writes the index.
 */
int gShowLoadingScreen(void)
{
    char buffer[40];
    void *data = 0;
    int size;
    struct Entry *entry;
    int saved;
    if (!gUploadRequestFlag && gModuleLoadState < 23) {
        entry = &gScreenIndexTable[gModuleLoadState];
        {
            int arg;
            if (entry->type == 1)
                arg = gModulePrefixPA;
            else
                arg = gModulePrefixCW;
            func_8008CD30(buffer, gFormatNamePair, arg, entry->name);
        }
        func_8001E110(buffer, gFileSearchGroup, &data, &size, 0);
        if (data) {
            saved = gUploadInProgress;
            gUploadInProgress = 1;
            gDecodeMdecFrameToVram((char *)data + 8, entry->value, 320, 0);
            {
                register char *base __asm__("$2");
                base = (char *)data;
                gDecodeMdecFrameToVram(base + (*(int *)base + 8), entry->value, 320, 256);
            }
            gUploadInProgress = saved;
            gFreeBlock(data);
            gEmitSplitScreenBlit(0);
            func_8001B4A8(1);
        }
    }
    gUploadRequestFlag = 0;
    return data != 0;
}

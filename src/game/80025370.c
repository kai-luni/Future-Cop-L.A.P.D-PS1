// Categories: Graphics > Image Loading & Display | Resources > File Access
typedef unsigned char u8;
typedef signed char s8;
struct Entry { char *name; s8 type; u8 value; char pad[2]; };
extern u8 gUploadRequestFlag __asm__("D_8009F3E5");
extern s8 D_8009F3F0;
extern struct Entry D_8009B42C[];
extern char gFormatNamePair[] __asm__("D_8009F2E4");
extern int D_8009F3E0, D_8009F3D8, D_8009F238;
extern int gUploadInProgress __asm__("D_8009F9C4");
extern void func_8008CD30(char *, char *, int, char *);
extern void func_8001E110(char *, int, void **, int *, int);
extern void func_800861E4(void *, int, int, int);
extern void func_800181D4(void *);
extern void func_8002549C(int);
extern void func_8001B4A8(int);

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
 */
int func_80025370(void)
{
    char buffer[40];
    void *data = 0;
    int size;
    struct Entry *entry;
    int saved;
    if (!gUploadRequestFlag && D_8009F3F0 < 23) {
        entry = &D_8009B42C[D_8009F3F0];
        {
            int arg;
            if (entry->type == 1)
                arg = D_8009F3E0;
            else
                arg = D_8009F3D8;
            func_8008CD30(buffer, gFormatNamePair, arg, entry->name);
        }
        func_8001E110(buffer, D_8009F238, &data, &size, 0);
        if (data) {
            saved = gUploadInProgress;
            gUploadInProgress = 1;
            func_800861E4((char *)data + 8, entry->value, 320, 0);
            {
                register char *base __asm__("$2");
                base = (char *)data;
                func_800861E4(base + (*(int *)base + 8), entry->value, 320, 256);
            }
            gUploadInProgress = saved;
            func_800181D4(data);
            func_8002549C(0);
            func_8001B4A8(1);
        }
    }
    gUploadRequestFlag = 0;
    return data != 0;
}

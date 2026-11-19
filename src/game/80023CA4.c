// Categories: Platform > Requests
extern int gSpuBufferPtr __asm__("D_8009F278");
extern int gCvkSpuUploadCursor __asm__("D_8009F9BC");
extern volatile int gCvkSpuUploadComplete __asm__("D_8009F650");
extern void gMarkCvkSpuUploadComplete(void) __asm__("D_80023C94");
struct Request { int address; unsigned int size; void *data; void (*callback)(void); int unused; int mode; };
extern void func_800212F8(struct Request *);

int gUploadCvkSpuDataAndWait(void *data, unsigned int size, int mode) __asm__("func_80023CA4");

/**
 * @brief Submits a CVK SPU upload and waits for its completion flag.
 *
 * @param data Buffer passed to the request.
 * @param size Requested byte count, rounded up to a multiple of 64.
 * @param mode Resets the address cursor when equal to 1.
 * @return 1 if completion is observed before timeout, otherwise 0.
 *
 * @par Confirmed mechanics
 * Requires a nonzero SPU base and resets the cursor to that base for mode 1. Rounds the byte
 * count up to 64, fills the DMA request with the source and completion callback, clears the
 * flag and polls from 0xFFFFF. Advances the cursor by the rounded count only with a positive
 * remainder.
 *
 * @par Globals
 * Reads gSpuBufferPtr and reads or writes gCvkSpuUploadCursor and gCvkSpuUploadComplete.
 *
 * @par Callers
 * Direct calls from gParseStreamedModuleChunks (func_8002902C).
 *
 * @par Role and path
 * The module loader uses this synchronous upload for embedded sound data.
 *
 * @par Unknown
 * The timeout is measured in loop iterations.
 */
int gUploadCvkSpuDataAndWait(void *data, unsigned int size, int mode)
{
    struct Request req;
    int result = 0;
    int timeout = 0xfffff;
    if (gSpuBufferPtr) {
        if (mode == 1) gCvkSpuUploadCursor = gSpuBufferPtr;
        size = (size + 63) >> 6 << 6;
        req.mode = 1;
        req.address = gCvkSpuUploadCursor;
        req.size = size;
        req.data = data;
        req.callback = gMarkCvkSpuUploadComplete;
        gCvkSpuUploadComplete = 0;
        func_800212F8(&req);
        while (!gCvkSpuUploadComplete && timeout-- > 0) {}
        if (timeout > 0) {
            gCvkSpuUploadCursor += size;
            result = 1;
        }
    }
    return result;
}

// Categories: Platform > Requests
extern int D_8009F278;
extern int D_8009F9BC;
extern volatile int D_8009F650;
extern void D_80023C94(void);
struct Request { int address; unsigned int size; void *data; void (*callback)(void); int unused; int mode; };
extern void func_800212F8(struct Request *);

/**
 *  @brief Submits a request and waits for its completion flag or a timeout.
 *
 *  The request uses a 64-byte-rounded size and a global address cursor;
 *  mode 1 resets that cursor to the configured base before submission.
 *  @param data Buffer passed to the request.
 *  @param size Requested byte count, rounded up to a multiple of 64.
 *  @param mode Resets the address cursor when equal to 1.
 *  @return 1 if completion is observed before timeout, otherwise 0.
 *  @par Globals
 *  D_8009F278 enables requests and supplies the base; D_8009F9BC is the cursor;
 *  D_8009F650 is cleared and polled; D_80023C94 is the request callback.
 *  @par Control flow
 *  Skips submission if disabled; otherwise polls with a bounded countdown and
 *  advances the cursor only when the remaining timeout is positive.
 *  @par Calls
 *  func_800212F8 submits the request.
 *  @par Side effects
 *  Updates the cursor and completion flag and submits a request using data.
 *  @par Callers
 *  Confirmed: func_8002902C.
 */
int func_80023CA4(void *data, unsigned int size, int mode)
{
    struct Request req;
    int result = 0;
    int timeout = 0xfffff;
    if (D_8009F278) {
        if (mode == 1) D_8009F9BC = D_8009F278;
        size = (size + 63) >> 6 << 6;
        req.mode = 1;
        req.address = D_8009F9BC;
        req.size = size;
        req.data = data;
        req.callback = D_80023C94;
        D_8009F650 = 0;
        func_800212F8(&req);
        while (!D_8009F650 && timeout-- > 0) {}
        if (timeout > 0) {
            D_8009F9BC += size;
            result = 1;
        }
    }
    return result;
}

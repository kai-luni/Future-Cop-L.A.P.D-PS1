// Categories: Game Engine > Request Queue
typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;
extern Request gPendingEntryList[] __asm__("D_800A8EA8");
extern int gSpuTransferWriteIndex __asm__("D_8009F998");
extern int gSpuTransferDispatching __asm__("D_8009FAD0");
extern int gSpuTransferActive __asm__("D_8009FA34");
extern int gSlotIndexLimit __asm__("D_8009FA80");
extern void gStartSpuTransfer(Request *) __asm__("func_80021490");

void gEnqueueSpuTransfer(Request *req) __asm__("func_800212F8");

/**
 * @brief Appends a six-word descriptor to the nine-slot SPU transfer ring and
 * starts it when the ring was empty.
 *
 * @param req Request descriptor copied into the ring.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Uses index times 24 to copy six words. Compares the write index with the
 * read index before advancing, calls gStartSpuTransfer on equality, then
 * increments the write index and wraps it to zero at nine.
 *
 * @par Globals
 * Reads gSlotIndexLimit. Updates gPendingEntryList and gSpuTransferWriteIndex.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021898, func_80022D84,
 * func_80023CA4.
 *
 * @par Role and path
 * SPU transfer producers use this queue to serialize requests.
 *
 * @par Unknown
 * The producer is responsible for avoiding overwrite of unread requests.
 */
void gEnqueueSpuTransfer(Request *req)
{
    register Request *dst __asm__("$2") = &gPendingEntryList[gSpuTransferWriteIndex];
    dst->a = req->a;
    dst->b = req->b;
    dst->c = req->c;
    dst->callback = req->callback;
    dst->context = req->context;
    dst->mode = req->mode;
    if (gSpuTransferWriteIndex == gSlotIndexLimit)
        gStartSpuTransfer(req);
    gSpuTransferWriteIndex++;
    if (gSpuTransferWriteIndex >= 9)
        gSpuTransferWriteIndex = 0;
}


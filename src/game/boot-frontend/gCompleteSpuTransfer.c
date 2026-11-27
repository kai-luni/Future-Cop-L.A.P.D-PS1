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

void gCompleteSpuTransfer(void) __asm__("func_8002139C");

/**
 * @brief Handles SPU transfer completion and starts the next queued request
 * when present.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Returns if guarded or if both indices match. Copies the current six-word
 * request to the stack, sets the guard, and calls the optional callback at
 * +0x0C with context +0x10. Wraps the read index at nine, dispatches the next
 * descriptor or clears the active flag, and clears the guard.
 *
 * @par Globals
 * Reads gPendingEntryList and gSpuTransferWriteIndex. Reads and updates
 * gSlotIndexLimit and gSpuTransferDispatching. Clears gSpuTransferActive when
 * the ring empties.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * The SPU queue initializer installs this address as the transfer-completion
 * callback.
 *
 * @par Unknown
 * The request owner chooses the completion callback and context.
 */
void gCompleteSpuTransfer(void)
{
    Request r;
    if (gSpuTransferDispatching)
        return;
    if (gSlotIndexLimit == gSpuTransferWriteIndex)
        return;
    r = gPendingEntryList[gSlotIndexLimit];
    gSpuTransferDispatching = 1;
    if (r.callback)
        r.callback(r.context);
    gSlotIndexLimit++;
    if (gSlotIndexLimit >= 9)
        gSlotIndexLimit = 0;
    if (gSpuTransferWriteIndex != gSlotIndexLimit)
        gStartSpuTransfer(&gPendingEntryList[gSlotIndexLimit]);
    else
        gSpuTransferActive = 0;
    gSpuTransferDispatching = 0;
}

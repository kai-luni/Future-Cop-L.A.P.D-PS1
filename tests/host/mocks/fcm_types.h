/* Shared integer types for the host mocks.
 *
 * Every mock lives in its own translation unit, so it cannot use the types from
 * fc_testkit.h: that header also defines __main/memcpy/memset as external
 * symbols, and pulling it into a second object file would give the link two (or
 * more) definitions of each. This header carries only the typedefs.
 *
 * Both this file and fc_testkit.h guard the block against each other, so a
 * harness may include either one first.
 */
#ifndef FCM_TYPES_H
#define FCM_TYPES_H
#define FCM_TYPES_INCLUDED 1

/* fc_testkit.h owns the same typedef block under FCK_TYPES_H. Claim the guard
 * too, so the two headers can be included in either order and the second one
 * skips its copy instead of redefining the types. */
#ifndef FCK_TYPES_H
#define FCK_TYPES_H
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
#endif

/* The mocks spell their prototypes with s32/u32. If a future toolchain ever
 * made those anything but 32 bits the ABI match would silently break, so pin
 * the widths here instead of trusting the comments. */
typedef char fcm_type_width_check[(sizeof(s32) == 4 && sizeof(u32) == 4
                                   && sizeof(void *) == 4) ? 1 : -1];

#endif /* FCM_TYPES_H */

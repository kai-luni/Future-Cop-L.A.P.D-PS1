/* Shared stand-ins for the PS1 BIOS vector layer, i.e. the A0/B0 thunks the
 * retail image calls for string and memory work.
 *
 * Why a mock at all: in retail these are `addiu $t2, 0xA0; jr $t2; ...` hand
 * written thunks (src/game/8008CCxx.c) that tail-jump into the console BIOS.
 * There is no BIOS under qemu-mipsel user mode, so every unit that formats a
 * path or copies a string needs the semantics supplied by hand.
 *
 * The important difference from a plain stub: an *unsupported* call is loud.
 *
 *  - `fcm_bios_dispatch` is the one place a vector is decoded. It recognises
 *    exactly the services listed in FCM_BIOS_VEC_* and calls fcm_abort() for
 *    anything else, printing the vector and the service index. A missing mock
 *    therefore stops the run with a named error - it never returns 0 and lets
 *    the unit under test continue on a value the BIOS would not have produced.
 *  - Each supported service implements the documented A0/B0 semantics, not an
 *    approximation, because the units' behaviour depends on them (func_8001DC48
 *    formats an ISO-9660 path out of strlen/strcpy/strcat/toupper alone).
 *
 * Binding: the exported symbols are the retail labels (func_8008CC80 ...
 * func_8008CD20), each defined through an asm alias, so a wrong binding is a
 * link error. When a unit under test is *linked* rather than mocked (the real
 * src/game/8008CCxx.c thunk), do not link this object's same-named symbol too:
 * pick one, per test.
 *
 * Two register-level notes, both taken from the retail thunk listing:
 *  - The thunks pass the service index in $t1 and the table in $t2, so the
 *    *first* argument stays in $a0 and the second in $a1. Easy to get wrong
 *    when re-deriving them from the C prototypes, which is why the vector
 *    numbers are spelled out per service below.
 *  - A0:2A (memcpy) and A0:2B (memset) return the destination in $v0.
 */
#ifndef FCM_BIOS_MOCK_H
#define FCM_BIOS_MOCK_H

#include "fcm_types.h"

/* ---- the A0 service indices, as used by the retail thunks ---- */
#define FCM_BIOS_A0_STRCMP   0x17
#define FCM_BIOS_A0_STRCPY   0x19
#define FCM_BIOS_A0_STRCAT   0x15
#define FCM_BIOS_A0_STRNCPY  0x1A
#define FCM_BIOS_A0_STRLEN   0x1B
#define FCM_BIOS_A0_STRCHR   0x1E
#define FCM_BIOS_A0_STRRCHR  0x1F
#define FCM_BIOS_A0_TOUPPER  0x25
#define FCM_BIOS_A0_TOLOWER  0x26
#define FCM_BIOS_A0_BZERO    0x28
#define FCM_BIOS_A0_MEMCPY   0x2A
#define FCM_BIOS_A0_MEMSET   0x2B

/* ---- the B0 service index ---- */
#define FCM_BIOS_B0_EXIT     0x38

/* Large enough for a whole formatting pass: the join costs ~5 service calls and
 * the upper-case walk one per character, so a 64-byte name needs ~70. A test
 * that overruns this sees seq_overflow set rather than a silently truncated
 * sequence. */
#define FCM_BIOS_LOG_MAX 128

struct FcmBiosMock {
    /* Calls seen, so a test can assert that a unit reached the BIOS at all. */
    s32 calls;
    s32 last_table;      /* 0xA0 or 0xB0 */
    s32 last_service;    /* the service index, e.g. 0x1B */

    /* The whole sequence, in order, so a test can assert the call chain a unit
     * really makes (strlen -> strcpy -> strcat -> strlen -> toupper ...) rather
     * than just its final string. */
    s32 services[FCM_BIOS_LOG_MAX];
    s32 seq_overflow;    /* 1 = more calls than the log holds */

    /* exit(2) is deliberately not taken for B(0x38): the unit under test may
     * have a fatal path that is worth asserting, so it is recorded and the
     * call returns. Set `exit_stops` to 1 if a test wants the run to end. */
    s32 exit_calls;
    s32 exit_code;
    s32 exit_stops;
};

extern struct FcmBiosMock fcm_bios;

void fcm_bios_reset(void);

/* The single dispatch point. `table` is 0xA0 or 0xB0; returns whatever the
 * service returns ($v0 in retail). Aborts on an unknown table or service. */
s32 fcm_bios_dispatch(s32 table, s32 service, s32 a0, s32 a1, s32 a2);

/* The stand-ins, each bound to its retail label. One per retail thunk, with the
 * service index the thunk encodes. A harness should call these rather than
 * fcm_bios_dispatch so that a unit under test and the test share the same entry
 * point. */
s32  gBiosStrcmp(void *a, void *b) __asm__("func_8008CCA0");
void gBiosStrcpy(void *dst, void *src) __asm__("func_8008CCB0");
void gBiosStrcat(void *dst, void *src) __asm__("func_8008CC90");
s32  gBiosStrlen(void *s) __asm__("func_8008CCC0");
void *gBiosStrchr(void *s, s32 c) __asm__("func_8008CCD0");
s32  gBiosToUpper(s32 c) __asm__("func_8008CCE0");
s32  gBiosToLower(s32 c) __asm__("func_8008CCF0");
void gBiosBzero(void *dst, s32 n) __asm__("func_8008CD00");
void gBiosMemcpy(void *dst, void *src, s32 n) __asm__("func_8008CD10");
void gBiosMemset(void *dst, s32 value, s32 n) __asm__("func_8008CD20");
void gBiosExit(s32 code) __asm__("func_8008CC80");

#endif /* FCM_BIOS_MOCK_H */

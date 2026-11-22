/* Shared BIOS vector stand-ins. See bios_mock.h for the contract. */
#include "bios_mock.h"
#include "fcm_report.h"

struct FcmBiosMock fcm_bios;

void fcm_bios_reset(void)
{
    u8 *p = (u8 *)&fcm_bios;
    s32 i;
    for (i = 0; i < (s32)sizeof(fcm_bios); i++) p[i] = 0;
}

/* ------------------------------------------------------------------ services */

/* A0:17 strcmp. Returns 0 when equal, else the difference of the first
 * differing bytes - the sign is what the callers test against 0. */
static s32 bios_strcmp(const u8 *a, const u8 *b)
{
    if (!a || !b)
        fcm_abort("BIOS A0:17 strcmp", "null string argument");
    while (*a && *a == *b) { a++; b++; }
    return (s32)*a - (s32)*b;
}

/* A0:19 strcpy. Returns the destination, as the BIOS leaves it in $v0. */
static s32 bios_strcpy(u8 *dst, const u8 *src)
{
    u8 *start = dst;
    if (!dst || !src)
        fcm_abort("BIOS A0:19 strcpy", "null string argument");
    while ((*dst++ = *src++) != 0) { }
    return (s32)(unsigned long)start;
}

/* A0:15 strcat. Appends src to the terminator of dst, returns dst. */
static s32 bios_strcat(u8 *dst, const u8 *src)
{
    u8 *start = dst;
    if (!dst || !src)
        fcm_abort("BIOS A0:15 strcat", "null string argument");
    while (*dst) dst++;
    while ((*dst++ = *src++) != 0) { }
    return (s32)(unsigned long)start;
}

/* A0:1B strlen. */
static s32 bios_strlen(const u8 *s)
{
    s32 n = 0;
    if (!s)
        fcm_abort("BIOS A0:1B strlen", "null string argument");
    while (s[n]) n++;
    return n;
}

/* A0:1E strchr. Returns a pointer to the first match, or 0. */
static s32 bios_strchr(const u8 *s, s32 c)
{
    if (!s)
        fcm_abort("BIOS A0:1E strchr", "null string argument");
    for (;; s++) {
        if (*s == (u8)c) return (s32)(unsigned long)s;
        if (!*s) return 0;
    }
}

/* A0:1F strrchr. Returns a pointer to the last match, or 0. */
static s32 bios_strrchr(const u8 *s, s32 c)
{
    const u8 *last = 0;
    if (!s)
        fcm_abort("BIOS A0:1F strrchr", "null string argument");
    for (;; s++) {
        if (*s == (u8)c) last = s;
        if (!*s) return (s32)(unsigned long)last;
    }
}

/* A0:2A memcpy. Returns the destination. A negative count would be a caller
 * bug; abort instead of turning it into a huge unsigned loop. */
static s32 bios_memcpy(void *dst, const void *src, s32 n)
{
    u8 *d = dst;
    const u8 *s = src;
    s32 i;
    if (!dst || !src)
        fcm_abort("BIOS A0:2A memcpy", "null pointer argument");
    if (n < 0)
        fcm_abort("BIOS A0:2A memcpy", "negative length");
    for (i = 0; i < n; i++) d[i] = s[i];
    return (s32)(unsigned long)dst;
}

/* A0:2B memset. Returns the destination. */
static s32 bios_memset(void *dst, s32 value, s32 n)
{
    u8 *d = dst;
    s32 i;
    if (!dst)
        fcm_abort("BIOS A0:2B memset", "null pointer argument");
    if (n < 0)
        fcm_abort("BIOS A0:2B memset", "negative length");
    for (i = 0; i < n; i++) d[i] = (u8)value;
    return (s32)(unsigned long)dst;
}

/* A0:28 bzero. Returns nothing meaningful in retail; 0 here. */
static s32 bios_bzero(void *dst, s32 n)
{
    u8 *d = dst;
    s32 i;
    if (!dst)
        fcm_abort("BIOS A0:28 bzero", "null pointer argument");
    if (n < 0)
        fcm_abort("BIOS A0:28 bzero", "negative length");
    for (i = 0; i < n; i++) d[i] = 0;
    return 0;
}

/* A0:25 toupper - ASCII only, everything else unchanged. */
static s32 bios_toupper(s32 c)
{
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}

/* A0:26 tolower - ASCII only, everything else unchanged. */
static s32 bios_tolower(s32 c)
{
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}

/* ---------------------------------------------------------------- dispatcher */

s32 fcm_bios_dispatch(s32 table, s32 service, s32 a0, s32 a1, s32 a2)
{
    fcm_bios.calls++;
    fcm_bios.last_table = table;
    fcm_bios.last_service = service;
    if (fcm_bios.calls <= FCM_BIOS_LOG_MAX)
        fcm_bios.services[fcm_bios.calls - 1] = service;
    else
        fcm_bios.seq_overflow = 1;

    if (table == 0xA0) {
        switch (service) {
        case FCM_BIOS_A0_STRCMP:  return bios_strcmp((const u8 *)a0, (const u8 *)a1);
        case FCM_BIOS_A0_STRCPY:  return bios_strcpy((u8 *)a0, (const u8 *)a1);
        case FCM_BIOS_A0_STRCAT:  return bios_strcat((u8 *)a0, (const u8 *)a1);
        case FCM_BIOS_A0_STRLEN:  return bios_strlen((const u8 *)a0);
        case FCM_BIOS_A0_STRCHR:  return bios_strchr((const u8 *)a0, a1);
        case FCM_BIOS_A0_STRRCHR: return bios_strrchr((const u8 *)a0, a1);
        case FCM_BIOS_A0_MEMCPY:  return bios_memcpy((void *)a0, (const void *)a1, a2);
        case FCM_BIOS_A0_MEMSET:  return bios_memset((void *)a0, a1, a2);
        case FCM_BIOS_A0_BZERO:   return bios_bzero((void *)a0, a1);
        case FCM_BIOS_A0_TOUPPER: return bios_toupper(a0);
        case FCM_BIOS_A0_TOLOWER: return bios_tolower(a0);
        default:
            /* Loudly, not silently: print the vector and the index we were
             * asked for and stop. A unit that needs a service this mock does
             * not implement must be reported, not fed a fake result. */
            fcm_say("FAIL: mock BIOS A0: unsupported service ");
            fcm_say_int(service);
            fcm_say("\n");
            fcm_sys3(4001, 2, 0, 0);
            for (;;) { }
        }
    }

    if (table == 0xB0) {
        switch (service) {
        case FCM_BIOS_B0_EXIT:
            fcm_bios.exit_calls++;
            fcm_bios.exit_code = a0;
            if (fcm_bios.exit_stops) {
                fcm_say("mock BIOS B0:38 exit(");
                fcm_say_int(a0);
                fcm_say(") stopped the run\n");
                fcm_sys3(4001, a0 ? a0 : 2, 0, 0);
                for (;;) { }
            }
            return 0;
        default:
            fcm_say("FAIL: mock BIOS B0: unsupported service ");
            fcm_say_int(service);
            fcm_say("\n");
            fcm_sys3(4001, 2, 0, 0);
            for (;;) { }
        }
    }

    fcm_say("FAIL: mock BIOS: unknown table ");
    fcm_say_hex((u32)table);
    fcm_say("\n");
    fcm_sys3(4001, 2, 0, 0);
    for (;;) { }
}

/* ------------------------------------------------------- the retail labels */

/* Each thunk below is the C twin of a handwritten retail vector call: the
 * service index is fixed (it is an immediate in the retail thunk), the table
 * is implied, and the argument words pass through untouched. */

/* func_8008CCA0 - A0:17 strcmp($a0, $a1) */
s32 gBiosStrcmp(void *a, void *b) __asm__("func_8008CCA0");
s32 gBiosStrcmp(void *a, void *b)
{
    return fcm_bios_dispatch(0xA0, FCM_BIOS_A0_STRCMP,
                             (s32)(unsigned long)a, (s32)(unsigned long)b, 0);
}

/* func_8008CCB0 - A0:19 strcpy($a0, $a1) */
void gBiosStrcpy(void *dst, void *src) __asm__("func_8008CCB0");
void gBiosStrcpy(void *dst, void *src)
{
    fcm_bios_dispatch(0xA0, FCM_BIOS_A0_STRCPY,
                      (s32)(unsigned long)dst, (s32)(unsigned long)src, 0);
}

/* func_8008CC90 - A0:15 strcat($a0, $a1) */
void gBiosStrcat(void *dst, void *src) __asm__("func_8008CC90");
void gBiosStrcat(void *dst, void *src)
{
    fcm_bios_dispatch(0xA0, FCM_BIOS_A0_STRCAT,
                      (s32)(unsigned long)dst, (s32)(unsigned long)src, 0);
}

/* func_8008CCC0 - A0:1B strlen($a0) */
s32 gBiosStrlen(void *s) __asm__("func_8008CCC0");
s32 gBiosStrlen(void *s)
{
    return fcm_bios_dispatch(0xA0, FCM_BIOS_A0_STRLEN, (s32)(unsigned long)s, 0, 0);
}

/* func_8008CCD0 - A0:1E strchr($a0, $a1) */
void *gBiosStrchr(void *s, s32 c) __asm__("func_8008CCD0");
void *gBiosStrchr(void *s, s32 c)
{
    return (void *)(unsigned long)fcm_bios_dispatch(0xA0, FCM_BIOS_A0_STRCHR,
                                                    (s32)(unsigned long)s, c, 0);
}

/* func_8008CCE0 - A0:25 toupper($a0) */
s32 gBiosToUpper(s32 c) __asm__("func_8008CCE0");
s32 gBiosToUpper(s32 c)
{
    return fcm_bios_dispatch(0xA0, FCM_BIOS_A0_TOUPPER, c, 0, 0);
}

/* func_8008CCF0 - A0:26 tolower($a0) */
s32 gBiosToLower(s32 c) __asm__("func_8008CCF0");
s32 gBiosToLower(s32 c)
{
    return fcm_bios_dispatch(0xA0, FCM_BIOS_A0_TOLOWER, c, 0, 0);
}

/* func_8008CD00 - A0:28 bzero($a0, $a1) */
void gBiosBzero(void *dst, s32 n) __asm__("func_8008CD00");
void gBiosBzero(void *dst, s32 n)
{
    fcm_bios_dispatch(0xA0, FCM_BIOS_A0_BZERO, (s32)(unsigned long)dst, n, 0);
}

/* func_8008CD10 - A0:2A memcpy($a0, $a1, $a2) */
void gBiosMemcpy(void *dst, void *src, s32 n) __asm__("func_8008CD10");
void gBiosMemcpy(void *dst, void *src, s32 n)
{
    fcm_bios_dispatch(0xA0, FCM_BIOS_A0_MEMCPY,
                      (s32)(unsigned long)dst, (s32)(unsigned long)src, n);
}

/* func_8008CD20 - A0:2B memset($a0, $a1, $a2) */
void gBiosMemset(void *dst, s32 value, s32 n) __asm__("func_8008CD20");
void gBiosMemset(void *dst, s32 value, s32 n)
{
    fcm_bios_dispatch(0xA0, FCM_BIOS_A0_MEMSET, (s32)(unsigned long)dst, value, n);
}

/* func_8008CC80 - B0:38 exit($a0) */
void gBiosExit(s32 code) __asm__("func_8008CC80");
void gBiosExit(s32 code)
{
    fcm_bios_dispatch(0xB0, FCM_BIOS_B0_EXIT, code, 0, 0);
}

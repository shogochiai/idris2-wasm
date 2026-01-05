/*
 * IC0 stub implementations for Idris2 FFI
 * These wrap the ic0 imports to match Idris2's FFI naming conventions
 */
#include <stdint.h>
#include <emscripten.h>

/* IC0 System API - declared as imports
 * These are provided by the IC runtime at canister execution time
 */
extern void ic0_msg_reply(void) __attribute__((import_module("ic0"), import_name("msg_reply")));
extern void ic0_msg_reply_data_append(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("msg_reply_data_append")));
extern void ic0_debug_print(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("debug_print")));

/* Idris2 FFI wrappers - these match the %foreign declarations in Main.idr */

void ic0_msg_reply_wrapper(void) {
    ic0_msg_reply();
}

void ic0_msg_reply_data_append_wrapper(const char* src, int32_t size) {
    ic0_msg_reply_data_append(src, (uint32_t)size);
}

void ic0_debug_print_wrapper(const char* src, int32_t size) {
    ic0_debug_print(src, (uint32_t)size);
}

/* String helper - Idris2 RefC represents strings as null-terminated C strings */
const char* idris2_getStr(const char* s) {
    return s;
}

/* strlen - provided since we may not link full libc */
int32_t strlen(const char* s) {
    int32_t len = 0;
    while (s[len] != '\0') len++;
    return len;
}

/* Canister entry points - exported for IC to call */

/* Called by IC on canister initialization */
EMSCRIPTEN_KEEPALIVE
void canister_init(void) {
    /* Initialization logic here */
}

/*
 * Canister entry points that call into Idris2 generated code
 * This bridges the IC canister interface with Idris2 RefC runtime
 */
#include <stdint.h>
#include <emscripten.h>

/* IC0 imports - provided by the IC runtime */
extern void ic0_msg_reply(void) __attribute__((import_module("ic0"), import_name("msg_reply")));
extern void ic0_msg_reply_data_append(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("msg_reply_data_append")));
extern void ic0_debug_print(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("debug_print")));
extern uint32_t ic0_msg_arg_data_size(void) __attribute__((import_module("ic0"), import_name("msg_arg_data_size")));
extern void ic0_msg_arg_data_copy(void* dst, uint32_t offset, uint32_t size) __attribute__((import_module("ic0"), import_name("msg_arg_data_copy")));

/* Forward declarations from Idris2 generated code */
/* The RefC runtime initializes on first call */
extern void* __mainExpression_0(void);  /* Idris2 main entry */

/* Helper functions */
static void debug(const char* msg) {
    uint32_t len = 0;
    while (msg[len]) len++;
    ic0_debug_print(msg, len);
}

static void reply_text(const char* msg) {
    uint32_t len = 0;
    while (msg[len]) len++;
    ic0_msg_reply_data_append(msg, len);
    ic0_msg_reply();
}

/* Canister lifecycle methods */

EMSCRIPTEN_KEEPALIVE
void canister_init(void) {
    debug("Idris2 canister: initializing");
    /* Initialize Idris2 runtime by calling main */
    __mainExpression_0();
    debug("Idris2 canister: initialized");
}

/* Query method: greet */
EMSCRIPTEN_KEEPALIVE
void canister_query_greet(void) {
    debug("Idris2 canister: greet query called");
    reply_text("Hello from Idris2 on the Internet Computer!");
}

/* Update method: ping */
EMSCRIPTEN_KEEPALIVE
void canister_update_ping(void) {
    debug("Idris2 canister: ping update called");
    reply_text("pong");
}

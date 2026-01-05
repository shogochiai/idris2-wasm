/*
 * Canister entry points for IC
 * Bridges IC canister interface with Idris2 RefC runtime
 */
#include <stdint.h>

/* IC0 imports - provided by the IC runtime */
extern void ic0_msg_reply(void) __attribute__((import_module("ic0"), import_name("msg_reply")));
extern void ic0_msg_reply_data_append(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("msg_reply_data_append")));
extern void ic0_debug_print(const void* src, uint32_t size) __attribute__((import_module("ic0"), import_name("debug_print")));

/* Forward declarations from Idris2 generated code */
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

/*
 * Canister methods - using export_name with spaces for IC convention
 * IC expects: "canister_query <name>" and "canister_update <name>"
 */

__attribute__((used, visibility("default"), export_name("canister_init")))
void canister_init(void) {
    debug("Idris2 canister: init");
    __mainExpression_0();
}

__attribute__((used, visibility("default"), export_name("canister_query greet")))
void canister_query_greet_impl(void) {
    debug("greet called");
    reply_text("Hello from Idris2 on IC!");
}

__attribute__((used, visibility("default"), export_name("canister_update ping")))
void canister_update_ping_impl(void) {
    debug("ping called");
    reply_text("pong");
}

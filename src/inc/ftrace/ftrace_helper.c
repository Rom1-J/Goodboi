#include "ftrace_helper.h"
#include "../logger/logger.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#if KPROBE_LOOKUP
    static struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name"
    };
#endif

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static int fh_resolve_hook_address(struct ftrace_hook *hook) {
#ifdef KPROBE_LOOKUP
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;

    if (register_kprobe(&kp) != 0) {
        rk_err("failed to register kprobe for kallsyms_lookup_name\n");
        return -EINVAL;
    }

    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
#endif

    hook->address = kallsyms_lookup_name(hook->name);
    if (!hook->address) {
        rk_err("unresolved symbol: %s\n", hook->name);
        return -ENOENT;
    }

#if USE_FENTRY_OFFSET
    *((unsigned long *)hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
    *((unsigned long *)hook->original) = hook->address;
#endif

    return 0;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static void notrace fh_ftrace_thunk(
    unsigned long ip,
    unsigned long parent_ip,
    struct ftrace_ops *ops,
    struct pt_regs *regs
) {
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

#if USE_FENTRY_OFFSET
    regs->ip = (unsigned long)hook->function;
#else
    if (!within_module(parent_ip, THIS_MODULE))
        regs->ip = (unsigned long)hook->function;
#endif
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int fh_install_hook(struct ftrace_hook *hook) {
    int err;

    err = fh_resolve_hook_address(hook);
    if (err)
        return err;

    hook->ops.func = (ftrace_func_t)fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
                    | FTRACE_OPS_FL_RECURSION
                    | FTRACE_OPS_FL_IPMODIFY;

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err) {
        rk_err("ftrace_set_filter_ip() failed: %d\n", err);

        return err;
    }

    err = register_ftrace_function(&hook->ops);
    if (err) {
        rk_err("register_ftrace_function() failed: %d\n", err);
        ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);

        return err;
    }

    return 0;
}

// ////////////////////////////////////////////////////////////////////

void fh_remove_hook(struct ftrace_hook *hook) {
    int err;

    err = unregister_ftrace_function(&hook->ops);
    if (err)
        rk_err("unregister_ftrace_function() failed: %d\n", err);

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
    if (err)
        rk_err("ftrace_set_filter_ip() failed: %d\n", err);
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int fh_install_hooks(struct ftrace_hook *hooks, size_t count) {
    size_t i;
    int err;

    for (i = 0; i < count; i++) {
        err = fh_install_hook(&hooks[i]);
        if (err)
            goto error;
    }

    return 0;

error:
    while (i-- > 0)
        fh_remove_hook(&hooks[i]);
    return err;
}

// ////////////////////////////////////////////////////////////////////

void fh_remove_hooks(struct ftrace_hook *hooks, size_t count) {
    for (size_t i = 0; i < count; i++)
        fh_remove_hook(&hooks[i]);
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "ftrace_helper.h"
#include "../logger/logger.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#if KPROBE_LOOKUP
    static struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name"
    };
#endif

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
static kallsyms_lookup_name_t loaded_kallsyms_lookup_name = NULL;

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static int fh_resolve_hook_address(struct ftrace_hook *hook) {
#ifdef KPROBE_LOOKUP

    if (loaded_kallsyms_lookup_name == NULL) {
        if (register_kprobe(&kp) != 0) {
            rk_err("[fh_resolve_hook_address] register_kprobe() failed to register kprobe for kallsyms_lookup_name\n");
            return -EINVAL;
        }

        loaded_kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
        unregister_kprobe(&kp);
    }
#endif

    hook->address = loaded_kallsyms_lookup_name(hook->name);
    if (!hook->address) {
        rk_err("[fh_resolve_hook_address] unresolved symbol: %s\n", hook->name);
        return -ENOENT;
    }

#if USE_FENTRY_OFFSET
    *(unsigned long *)hook->original = hook->address + MCOUNT_INSN_SIZE;
#else
    *(unsigned long *)hook->original = hook->address;
#endif

    return 0;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static void notrace fh_ftrace_thunk(
    unsigned long ip,
    const unsigned long parent_ip,
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

    if ((err = fh_resolve_hook_address(hook)))
        return err;

    hook->ops.func = (ftrace_func_t)fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
                    | FTRACE_OPS_FL_RECURSION
                    | FTRACE_OPS_FL_IPMODIFY;

    if ((err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0))) {
        rk_err("[fh_install_hook] ftrace_set_filter_ip() failed: %d\n", err);

        return err;
    }

    if ((err = register_ftrace_function(&hook->ops))) {
        rk_err("[fh_install_hook] register_ftrace_function() failed: %d\n", err);
        ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);

        return err;
    }

    return 0;
}

// ////////////////////////////////////////////////////////////////////

void fh_remove_hook(struct ftrace_hook *hook) {
    int err;

    if ((err = unregister_ftrace_function(&hook->ops)))
        rk_err("[fh_remove_hook] unregister_ftrace_function() failed: %d\n", err);

    if ((err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0)))
        rk_err("[fh_remove_hook] ftrace_set_filter_ip() failed: %d\n", err);
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int fh_install_hooks(struct ftrace_hook *hooks, const size_t count) {
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

void fh_remove_hooks(struct ftrace_hook *hooks, const size_t count) {
    for (size_t i = 0; i < count; i++)
        fh_remove_hook(&hooks[i]);
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
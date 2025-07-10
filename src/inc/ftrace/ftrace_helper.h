/*
 * based on https://github.com/xcellerator/linux_kernel_hacking/blob/master/3_RootkitTechniques/3.3_set_root/ftrace_helper.h
 */

#ifndef FTRACE_HELPER_H
#define FTRACE_HELPER_H

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include <linux/ftrace.h>
#include <linux/version.h>

// ////////////////////////////////////////////////////////////////////

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
    #define PTREGS_SYSCALL_STUBS 1
#endif

// ////////////////////////////////////////////////////////////////////

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
    #define KPROBE_LOOKUP 1
    #include <linux/kprobes.h>
#endif

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#ifdef PTREGS_SYSCALL_STUBS
    #define SYSCALL_NAME(name) ("__x64_" name)
#else
    #define SYSCALL_NAME(name) (name)
#endif

#define HOOK(_name, _hook, _orig)   \
{                                   \
    .name = SYSCALL_NAME(_name),    \
    .function = (_hook),            \
    .original = (_orig),            \
}

// ////////////////////////////////////////////////////////////////////

#define USE_FENTRY_OFFSET 0
#if !USE_FENTRY_OFFSET
    #pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

// ////////////////////////////////////////////////////////////////////

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int fh_install_hook(struct ftrace_hook *hook);
void fh_remove_hook(struct ftrace_hook *hook);
int fh_install_hooks(struct ftrace_hook *hooks, size_t count);
void fh_remove_hooks(struct ftrace_hook *hooks, size_t count);

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#endif // FTRACE_HELPER_H

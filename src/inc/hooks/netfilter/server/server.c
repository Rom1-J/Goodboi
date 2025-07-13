#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/tcp.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../logger/logger.h"
#include "server.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

unsigned int hook_nf_server(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    const struct iphdr *iph = ip_hdr(skb);

    if (iph->protocol == IPPROTO_TCP) {
        const struct tcphdr *tcph = tcp_hdr(skb);

        if (ntohs(tcph->dest) == CAPTURE_PORT) {
            rk_info("[hook_nf_client] receive TCP packet on specified port: %02x", *skb->data);
        }
    }

    return NF_ACCEPT;
}
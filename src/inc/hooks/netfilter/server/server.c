#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/tcp.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#include "../../../../logger/logger.h"
#include "server.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

#define MAX_DUMP_BYTES 128

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

static void hex_dump_skb_data(struct sk_buff *skb) {
    if (skb_is_nonlinear(skb)) {
        if (skb_linearize(skb) != 0)
            return;
    }

    const u8 *data = skb->data;
    unsigned int len = skb->len;

    if (len > MAX_DUMP_BYTES)
        len = MAX_DUMP_BYTES;

    rk_info("[hex_dump] Packet (%u bytes):\n", skb->len);

    for (int i = 0; i < len; i++) {
        printk(KERN_CONT "%02x ", data[i]);
        if ((i + 1) % 16 == 0)
            printk(KERN_CONT "\n");
    }

    if (len % 16 != 0)
        printk(KERN_CONT "\n");
}

// ////////////////////////////////////////////////////////////////////

unsigned int hook_nf_server(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    const struct iphdr *iph = ip_hdr(skb);

    if (iph->protocol == IPPROTO_TCP) {
        const struct tcphdr *tcph = tcp_hdr(skb);

        const __be32 src_ip = iph->saddr;
        const __be32 dst_ip = iph->daddr;

        const __be16 src_port = tcph->source;
        const __be16 dest_port = tcph->dest;

        const u32 src = ntohl(src_ip);
        const u32 dst = ntohl(dst_ip);

        if (ntohs(dest_port) == CAPTURE_PORT) {
            rk_info(
                "[hook_nf_client] receive TCP packet on specified:\n"
                "    src.ip: %u.%u.%u.%u\n"
                "    src.port: %d\n"
                "    dst.ip: %u.%u.%u.%u\n"
                "    dst.port: %d\n",
                src >> 24 & 0xFF,
                src >> 16 & 0xFF,
                src >> 8 & 0xFF,
                src & 0xFF,
                ntohs(src_port),
                dst >> 24 & 0xFF,
                dst >> 16 & 0xFF,
                dst >> 8 & 0xFF,
                dst & 0xFF,
                ntohs(dest_port)
            );

            hex_dump_skb_data(skb);
        }
    }

    return NF_ACCEPT;
}
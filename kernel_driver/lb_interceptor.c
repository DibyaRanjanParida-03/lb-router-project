#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/inet.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include "../include/lb_router/lb_netlink.h"

static struct nf_hook_ops nfho;
static __be32 target_ip = 0; 
static struct sock *nl_sk = NULL;

static void nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    struct lb_cmd *cmd;

    nlh = (struct nlmsghdr *)skb->data;
    cmd = (struct lb_cmd *)nlmsg_data(nlh);

    if (cmd->is_healthy) {
        target_ip = cmd->ip_address;
    } else {
        if (target_ip == cmd->ip_address) {
            target_ip = 0; 
        }
    }
}

unsigned int lb_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct tcphdr *tcph;
    int tcplen;

    if (!skb || !target_ip) return NF_ACCEPT;
    
    if (skb_make_writable(skb, skb->len)) return NF_DROP;

    iph = ip_hdr(skb);
    if (!iph) return NF_ACCEPT;

    if (iph->protocol == IPPROTO_TCP) {
        tcph = tcp_hdr(skb);
        if (!tcph) return NF_ACCEPT;

        if (ntohs(tcph->dest) == 80) {
            tcplen = ntohs(iph->tot_len) - iph->ihl * 4;
            
            tcph->check = 0;
            tcph->check = csum_tcpudp_magic(iph->saddr, target_ip, tcplen, IPPROTO_TCP, csum_partial((char *)tcph, tcplen, 0));

            iph->daddr = target_ip;
            ip_send_check(iph);
        }
    }
    return NF_ACCEPT;
}

static int __init lb_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) return -ENOMEM;

    nfho.hook = lb_hook_func;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &nfho);
    return 0;
}

static void __exit lb_exit(void) {
    nf_unregister_net_hook(&init_net, &nfho);
    if (nl_sk) netlink_kernel_release(nl_sk);
}

module_init(lb_init);
module_exit(lb_exit);

MODULE_LICENSE("GPL");

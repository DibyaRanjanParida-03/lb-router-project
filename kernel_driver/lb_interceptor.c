#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/inet.h>

static struct nf_hook_ops nfho;
static __be32 target_ip; 

unsigned int lb_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct tcphdr *tcph;
    int tcplen;

    if (!skb) return NF_ACCEPT;
    
    if (skb_make_writable(skb, skb->len)) return NF_DROP;

    iph = ip_hdr(skb);
    if (!iph) return NF_ACCEPT;

    if (iph->protocol == IPPROTO_TCP) {
        tcph = tcp_hdr(skb);
        if (!tcph) return NF_ACCEPT;

        if (ntohs(tcph->dest) == 80) {
            target_ip = in_aton("192.168.1.10"); 
            
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
    nfho.hook = lb_hook_func;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &nfho);
    return 0;
}

static void __exit lb_exit(void) {
    nf_unregister_net_hook(&init_net, &nfho);
}

module_init(lb_init);
module_exit(lb_exit);

MODULE_LICENSE("GPL");

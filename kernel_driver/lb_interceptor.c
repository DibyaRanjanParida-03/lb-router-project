#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

static struct nf_hook_ops nfho;

unsigned int lb_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct tcphdr *tcph;

    if (!skb) return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (!iph) return NF_ACCEPT;

    if (iph->protocol == IPPROTO_TCP) {
        tcph = tcp_hdr(skb);
        if (!tcph) return NF_ACCEPT;

        if (ntohs(tcph->dest) == 80) {
            printk(KERN_INFO "[LB Data Plane] Intercepted HTTP packet from %pI4\n", &iph->saddr);
        }
    }
    return NF_ACCEPT;
}

static int __init lb_init(void) {
    printk(KERN_INFO "[LB Data Plane] Loading Load Balancer Kernel Module...\n");

    nfho.hook = lb_hook_func;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &nfho);
    
    printk(KERN_INFO "[LB Data Plane] Netfilter hook registered successfully.\n");
    return 0;
}

static void __exit lb_exit(void) {
    nf_unregister_net_hook(&init_net, &nfho);
    printk(KERN_INFO "[LB Data Plane] Module unloaded safely.\n");
}

module_init(lb_init);
module_exit(lb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("System Programming Project");
MODULE_DESCRIPTION("Software-Defined Load Balancer Data Plane");

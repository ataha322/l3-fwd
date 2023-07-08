#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

struct routing_table {
	uint32_t dst_ip;
	uint8_t dst_eth[6];
	int valid;
};

#define BURST_SIZE 32
#define RT_SIZE 16

//looks up rounting table, updates eth header, decremetns
//TTL, updates checksum
//returns outgoing port number
static int resolve_forwarding(struct rte_mbuf* mbuf, struct routing_table* r_tab) {

	//extract ethernet
	struct rte_ether_hdr* eth_hdr;
	eth_hdr = rte_pktmbuf_mtod(mbuf, struct rte_ether_hdr*);

	//extract IP
	struct rte_ipv4_hdr* ip_hdr;
	ip_hdr = (struct rte_ipv4_hdr*)((uint8_t*) eth_hdr + RTE_ETHER_HDR_LEN);

	//check if packet is valid
	if (rte_ipv4_cksum(ip_hdr) != 0) {
		/* printf("resolve_forwarding: checksum failed\n"); */
		return -1;
	}

	int found = 0;
	int i;
	for (i = 0; i < RT_SIZE; i++) {
		//implements EM
		if (r_tab[i].dst_ip == ip_hdr->dst_addr) {
			found = 1;
			break;
		}
	}


	//if no match - use last entry in RT as default gateway
	if (!found) {
		int last_valid = -1;
		for (i = 0; i < RT_SIZE; i++) {
			if (r_tab[i].valid)
				last_valid = i;
		}
		i = last_valid;
	}

	if (i == -1) { 
		return -1;
	}

	//source ethernet is now old destination
	memcpy(&eth_hdr->src_addr, &eth_hdr->dst_addr, RTE_ETHER_ADDR_LEN);
	//new destination found in RT
	memcpy(&eth_hdr->dst_addr, r_tab[i].dst_eth, RTE_ETHER_ADDR_LEN);

	//decrement TTL
	ip_hdr->time_to_live--;

	//recompute checksum
	ip_hdr->hdr_checksum = 0;
	ip_hdr->hdr_checksum = rte_ipv4_cksum(ip_hdr);

	//return port number
	return i;
}

static __rte_noreturn void lcore_main(struct routing_table* r_tab) {

	uint16_t port;

	printf("Core %u is forwarding packets\n", rte_lcore_id());

	while (1) {

		RTE_ETH_FOREACH_DEV(port) {
			/* receive a burst */
			struct rte_mbuf* bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

			if (nb_rx == 0) {
				continue;
			}

			/* for every packet from the burst figure out */
			/* 	destination port and modify the packet, */
			/* 				decrement TTL, recompute checksum */

			int new_port = -1;

			for (int i = 0; i < nb_rx; i++) {
				struct rte_mbuf* mbuf = bufs[i];
				int ret = resolve_forwarding(mbuf, r_tab);
				if (ret != -1)
					new_port = ret;
			}

			uint16_t nb_tx;
			if (new_port != -1) {
				printf("forwarding to port %d\n", new_port);
				/* now, send to a corresponding port */
				nb_tx = rte_eth_tx_burst(new_port, 0, bufs, nb_rx);
			}
			/* free unsent packets */
			if (nb_tx < nb_rx) {
				uint16_t buf;
				for (buf = nb_tx; buf < nb_rx; buf++) {
					rte_pktmbuf_free(bufs[buf]);
				}
			}
		}
	}

}

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#define BURST_SIZE 32

static int forward_322(struct rte_mbuf* mbuf) {

	//extract ethernet
    struct rte_ether_hdr* eth_hdr;
	eth_hdr = rte_pktmbuf_mtod(mbuf, struct rte_ether_hdr*);
	
	//extract IP
	struct rte_ipv4_hdr* ip_hdr;
	ip_hdr = (struct rte_ipv4_hdr*)((uint8_t*) eth_hdr + RTE_ETHER_HDR_LEN);

	printf("forward: destination address: %d\n", ip_hdr->dst_addr);

	return -1;
}

static __rte_noreturn void lcore_main(void) {

	uint16_t port;

/* 	RTE_ETH_FOREACH_DEV(port) */
/* 		if (rte_eth_dev_socket_id(port) >= 0 && */
/* 				rte_eth_dev_socket_id(port) != */
/* 						(int)rte_socket_id()) */
/* 			printf("WARNING, port %u is on remote NUMA node to " */
/* 					"polling thread.\n\tPerformance will " */
/* 					"not be optimal.\n", port); */

	printf("Core %u is forwarding packets\n", rte_lcore_id());

	while (1) {

		RTE_ETH_FOREACH_DEV(port) {
			/* receive a burst */
			struct rte_mbuf* bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

			if (nb_rx == 0) {
				continue;
			}

			/* else, take a sample packet from the burst and figure out */
			/* 	destination port and modify the packet, */
			/* 				decrement TTL, recompute checksum */

			struct rte_mbuf* mbuf = bufs[0];
			forward_322(mbuf);



			/* here, need to call forwarding from forward.c */

			/* now, send to a corresponding pair */
			const uint16_t nb_tx = rte_eth_tx_burst(port ^ 1, 0, bufs, nb_rx);

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

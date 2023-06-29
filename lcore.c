#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#define BURST_SIZE 32

static __rte_noreturn void lcore_main(void) {

	uint16_t port;

	RTE_ETH_FOREACH_DEV(port)
		if (rte_eth_dev_socket_id(port) >= 0 &&
				rte_eth_dev_socket_id(port) !=
						(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
					"polling thread.\n\tPerformance will "
					"not be optimal.\n", port);

	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	printf("Core %u is forwarding packets\n", rte_lcore_id());

	while (1) {

		RTE_ETH_FOREACH_DEV(port) {
			/* receive a burst */
			struct rte_mbuf* bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

			if (nb_rx == 0) {
				continue;
			}

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

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024


static inline int port_init(uint16_t port, struct rte_mempool* mbuf_pool) {
	int retval; //debug variable
	uint16_t i; //iterator
	
	/* sanity */
	if (!rte_eth_dev_is_valid_port(port)) {
		return -1;
	}

	/* port configuration */
	struct rte_eth_conf port_conf;
	memset(&port_conf, 0, sizeof(struct rte_eth_conf));
	/* load port config and write into dev_info */
	struct rte_eth_dev_info dev_info;
	retval = rte_eth_dev_info_get(port, &dev_info);
	if (retval != 0) {
		printf("ERROR: getting port %u info: %s\n", port, strerror(-retval));
		return retval;
	}

	/* check if fast offload is supported, if so then set the fast offload bit to 1 */
	if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE) {
		port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;
	}

	/* set RX and TX ring number to port configuration */
	const uint16_t rx_rings = 1;
	const uint16_t tx_rings = 1;
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0) {
		return retval;
	}
	
	/* adjust number of rx and tx descriptors */
	/* 	take the min{# of descriptors supported by ETH port, */
	/* 				# of descriptors specfied by nb_rxd, nb_txt} */
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0) {
		return retval;
	}

	/* 1 RX queue per ETH port */
	for (i = 0; i < rx_rings; i++) {
		retval = rte_eth_rx_queue_setup(port, i, nb_rxd,
				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0) {
			return retval;
		}
	}

	/* 1 TX queue per ETH port */
	struct rte_eth_txconf txconf = dev_info.default_txconf;
	txconf.offloads = port_conf.txmode.offloads;
	for (i = 0; i < tx_rings; i++) {
		retval = rte_eth_tx_queue_setup(port, i, nb_txd,
				rte_eth_dev_socket_id(port), &txconf);
		if (retval < 0) {
			return retval;
		}
	}

	/* start the port */
	retval = rte_eth_dev_start(port);
	if (retval < 0) {
		return retval;
	}

	/* get the MAC */
	struct rte_ether_addr addr;
	retval = rte_eth_macaddr_get(port, &addr);
	if (retval != 0) {
		return retval;
	}

	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			port, RTE_ETHER_ADDR_BYTES(&addr));

	/* RX promiscuous */
	retval = rte_eth_promiscuous_enable(port);
	if (retval != 0) {
		return retval;
	}
	
	return 0;
}

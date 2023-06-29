#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>

#include "port.c"
#include "lcore.c"

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250

int main(int argc, char** argv) {

	/* initialize EAL */
	int ret = rte_eal_init(argc,argv);
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "ERROR: EAL initialization, invalid arguments\n");
	}

	/* get the number of ports (MUST BE EVEN) */
	unsigned int nb_ports = rte_eth_dev_count_avail();

	/* memory buffer for each port */
	struct rte_mempool* mbuf_pool;
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE,
			0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	/* initialize ports */
	uint16_t portid;
	RTE_ETH_FOREACH_DEV(portid) {
		/* port_init is heavy lifting here */
		if (port_init(portid, mbuf_pool) != 0) {
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n", portid);
		}
	}

	/* main loop that's doing forwarding */
	lcore_main();

	/* clean */
	rte_eal_cleanup();
	
	return 0;
}

#include <inttypes.h>
#include <rte_eal.h>
#include <rte_mbuf.h>
#include <rte_ethdev.h>

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250

int main(int argc, char** argv) {

	int ret = rte_eal_init(argc,argv);
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "ERROR: EAL initialization, invalid arguments\n");
	}

	struct rte_mempool* mbuf_pool;
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE,
			0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	RTE_ETH_FOREACH_DEV(portid) {
		if (port_init(portid, mbuf_pool) != 0) {
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n", portid);
		}
	}

	lcore_main();

	rte_eal_cleanup();
	
	return 0;
}

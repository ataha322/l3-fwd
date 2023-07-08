#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

#include "port.c"
/* #include "lcore.c" */
#include "helpers.c"

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define RT_SIZE 16


void parse_rtable();
void init_table();
struct routing_table r_tab[RT_SIZE];





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

	/* set all table entries to 0 */
	init_table();
	/* load routing table from rtable file */
	parse_rtable();

	/* initialize ports */
	uint16_t portid;
	RTE_ETH_FOREACH_DEV(portid) {
		/* port_init is heavy lifting here */
		if (port_init(portid, mbuf_pool, r_tab) != 0) {
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n", portid);
		}
	}

	/* main loop that's doing forwarding */
	lcore_main(r_tab);

	/* clean */
	rte_eal_cleanup();
	
	return 0;
}















void parse_rtable() {

	FILE* file = fopen("rtable.csv", "r");
	if (file == NULL) {
		printf("Failed to open RT file.\n");
		exit(1);
	}

	char line[128];
	while(fgets(line, sizeof(line), file)) {
		char* address_str;
		char* port_str;
		char* eth_str;
		address_str = strtok(line, ",");
		port_str = strtok(NULL, ",");
		eth_str = strtok(NULL, ",");

		if (address_str == NULL || port_str == NULL || eth_str == NULL) {
			printf("Invalid line format: %s\n", line);
			exit(1);
		}

		uint32_t address = inet_addr(address_str);
		if (address == INADDR_NONE) {
			printf("Invalid IP address: %s\n", address_str);
			exit(1);
		}
		/* address = ntohl(address); */

		uint16_t port = atoi(port_str);

		struct ether_addr eth_addr;
		if (ether_aton_r(eth_str, &eth_addr) == NULL) {
			printf("Invalid MAC address: %s", eth_str);
			exit(1);
		}

		/* uint32_to_str_big_endian(address); */
		/* printf("Port: %" PRIu16 "\n", port); */
		/* printf("----------------------\n"); */

		//fill in routing table
		r_tab[port].valid = 1;
		r_tab[port].dst_ip = address;
		memcpy(r_tab[port].dst_eth, eth_addr.ether_addr_octet, RTE_ETHER_ADDR_LEN);
	}
	
}


void init_table() {
	memset(r_tab, 0, sizeof(struct routing_table));
}

/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2015 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_udp.h>

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32
struct rte_mempool *mbuf_pool;
struct mergeheader{
	struct ether_hdr ether_header;
	struct ipv4_hdr ipv4_header;
	struct udp_hdr udp_header;
	char data[64];
}__attribute__((__packed__));

static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

/* basicfwd.c: Basic DPDK skeleton forwarding example. */

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int
port_init(uint8_t port, struct rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	int retval;
	uint16_t q;

	if (port >= rte_eth_dev_count())
		return -1;

	/* Configure the Ethernet device. */
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
		return retval;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE,
				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
			return retval;
	}

	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE,
				rte_eth_dev_socket_id(port), NULL);
		if (retval < 0)
			return retval;
	}

	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0)
		return retval;

	/* Display the port MAC address. */
	struct ether_addr addr;
	rte_eth_macaddr_get(port, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			(unsigned)port,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	rte_eth_promiscuous_enable(port);

	return 0;
}

/*
 * The lcore main. This is the main thread that does the work, reading from
 * an input port and writing to an output port.
 */
static __attribute__((noreturn)) void
lcore_main(void)
{
	fprintf(stderr,"here9");
	const uint8_t nb_ports = rte_eth_dev_count();
	uint8_t port;
	fprintf(stderr,"here8");
	/*
	 * Check that the port is on the same NUMA node as the polling thread
	 * for best performance.
	 */
	for (port = 0; port < nb_ports; port++)
		if (rte_eth_dev_socket_id(port) > 0 &&
				rte_eth_dev_socket_id(port) !=
						(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
					"polling thread.\n\tPerformance will "
					"not be optimal.\n", port);

	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	/* Run until the application is quit or killed. */
	int jj = 0;
	for (jj = 0;jj<1;jj++) {
		/*
		 * Receive packets on a port and forward them on the paired
		 * port. The mapping is 0 -> 1, 1 -> 0, 2 -> 3, 3 -> 2, etc.
		 */
		//int ii = 0;
		printf("here9\n");
		for (port = 0; port < nb_ports; port++) {

			/* Get burst of RX packets, from first port of pair. */
			//port  = 0;
			printf("here7\n");
			//struct rte_mbuf * mbufs = (struct rte_mbuf*)malloc(sizeof(struct rte_mbuf));
			struct rte_mbuf * mbufs = rte_pktmbuf_alloc(mbuf_pool);
			struct mergeheader * merge_header;
			//merge_header = (struct mergeheader *)malloc(sizeof(struct mergeheader));
			merge_header = rte_pktmbuf_mtod( mbufs, struct mergeheader *);
			rte_pktmbuf_append(mbufs,sizeof(struct mergeheader));
			//filled the ipv4 header
			struct ether_addr des_addr;
			struct ether_addr sour_addr;
			//uint8_t addr_byte[6] = {0x6c,0xa4,0x0a,0x9d,0x7b,0x50};
			uint8_t addr_byte[6] = {0x50,0x7b,0x9d,0x0a,0xa4,0x6c};
			memcpy(des_addr.addr_bytes ,addr_byte,6);
			//uint8_t addr_byte2[6] = {0xdb,0x41,0xa7,0x29,0x0c,0x00};
			uint8_t addr_byte2[6] = {0x00,0x0c,0x29,0xa7,0x41,0xdb};
			//printf("addr_byte 0x%x\n",*(sour_addr.addr_bytes+5));
			memcpy(sour_addr.addr_bytes , addr_byte2,6);
			fprintf(stderr,"here6\n");
			merge_header->ether_header.d_addr = des_addr;
			merge_header->ether_header.s_addr = sour_addr;
			merge_header->ether_header.ether_type = 0x0008;
			// merge_header->ipv4_header.src_addr = 0xc0a8500a;
			// merge_header->ipv4_header.dst_addr = 0xc0a85006;
			merge_header->ipv4_header.src_addr = 0x0a50a8c0;
			merge_header->ipv4_header.dst_addr = 0x0650a8c0;
			merge_header->ipv4_header.version_ihl = 0x45;
			merge_header->ipv4_header.type_of_service = 0;
			merge_header->ipv4_header.total_length = 0x5c00;
			merge_header->ipv4_header.packet_id = 0;
			merge_header->ipv4_header.fragment_offset = 0;
			merge_header->ipv4_header.time_to_live = 64;
			merge_header->ipv4_header.next_proto_id = 17;
			merge_header->udp_header.src_port = 0x6d38;
			merge_header->udp_header.dst_port = 0x6d2e;
			merge_header->udp_header.dgram_len = 0x4800;
			// merge_header->ipv4_header.hdr_checksum = rte_ipv4_cksum(&merge_header->ipv4_header);
			// merge_header->udp_header.dgram_cksum = rte_raw_cksum(&merge_header->udp_header,merge_header->udp_header.dgram_len);
			merge_header->ipv4_header.hdr_checksum = 0x3059;
			merge_header->udp_header.dgram_cksum = 0x2277;
			fprintf(stderr,"here4\n");
			
			// struct mergeheader * merge_header2 = rte_pktmbuf_mtod( mbufs, struct mergeheader *);
			
			// fprintf(stderr,"sour %x\n",(merge_header2->ether_header).ether_type);
			const uint16_t nb_rx = rte_eth_tx_burst(port, 0,
					&mbufs, 1);
			fprintf(stderr,"here5%d\n",nb_rx);
			rte_pktmbuf_free(mbufs)	;


		}
		
	}
	fprintf(stderr, "here3");
}

/*
 * The main function, which does initialization and calls the per-lcore
 * functions.
 */
int
main(int argc, char *argv[])
{
	
	unsigned nb_ports;
	uint8_t portid;

	/* Initialize the Environment Abstraction Layer (EAL). */
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

	argc -= ret;
	argv += ret;

	/* Check that there is an even number of ports to send/receive on. */
	nb_ports = rte_eth_dev_count();
	//rte_exit(EXIT_FAILURE, "Error: %d\n",nb_ports);
	if (nb_ports != 1 )
		rte_exit(EXIT_FAILURE, "Error: number of ports must be 1\n");

	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * 4,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

	/* Initialize all ports. */
	for (portid = 0; portid < nb_ports; portid++)
		if (port_init(portid, mbuf_pool) != 0)
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n",
					portid);

	if (rte_lcore_count() > 1)
		printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");
	printf("here\n");
	/* Call lcore_main on the master core only. */
	lcore_main();
	printf("here2\n");
	return 0;
}

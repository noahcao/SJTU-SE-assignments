# Document Part1+Part2

曹金坤

515260910022

caojinkun@sjtu.edu.cn

## Part 1

* What's the purpose of using hugepage?

  The use of hugepage decreasing the need of page number. It needs less TLB to get a good performance and save the space needed for the translation of virtual address.

* Take examples/helloworld as an example, describe the execution flow of DPDK programs?

  1. Initialize the fundamental runtime environment
  2. Initialize the multi-core runtime environment
  3. Try to load the program onto the logistic core
  4. Boot the thread on each logistic core

* Read the codes of examples/skeleton, describe DPDK APIs related to sending and
  receiving packets.

  1. Initialize port: call the function *port_init* to initialize the ports with set queue number and port index.
  2. Set port: call function *rte_eth_dev_configure* to set the send/receive queue scale
  3. Initialize the queues: call the function *rte_eth_rx_queue_setup* to initialize the queue status, including corresponding memory location, number of descriptor, cache scale and so on.
  4. Send/Receive packets: call respectively the function *rte_eth_rx_burst* and *rte_eth_tx_eth* to send/receive the packet from/to the buffer area. 

* Describe the data structure of ‘rte_mbuf’.

  Header: containing 2 cache lines. The basic and frequently used data is stored in the first cache line, extended data in the second line.

  Next: pointing to the next mbuf, it's designed for some huge/jumbo frame



## Part 2

My implementation for part 2 refers the implementation in skeleton folder. The basicfwd.c file gives a easy thinking on it. I rewrite some parts in this file to construct a mbuf structure and send it.

In the tail of each packet, I add the string content 'dslab2' and it UDP packet. Through WireShark, I cat get the result as shown in below figs. The protocol type, ip address and data content are all verified to be as expected. 

![runtime_res](F:\GoogleDrive\18_19_2\DS\Lab2\assets\runtime_res.png)

![checkres](F:\GoogleDrive\18_19_2\DS\Lab2\assets\checkres.png)
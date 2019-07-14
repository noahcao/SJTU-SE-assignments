#include "ns.h"

extern union Nsipc nsipcbuf;
#define RETRY_MAX 5


void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server (using ipc_send with NSREQ_INPUT as value)
	//	do the above things in a loop
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
    
    int r = 0;
    
    while(true){
        r = sys_page_alloc(0, &nsipcbuf, PTE_U | PTE_P | PTE_W);
        if(r < 0){
            panic("net/input.c input: error: %e\n",r);
        }

        if((r = sys_net_recv((void*)nsipcbuf.pkt.jp_data, 0)) < 0){
            continue;
        }
        cprintf("come here\n");

        if(r < 0){
            panic("net/input.c input error: %e\n",r);
        }

        nsipcbuf.pkt.jp_len = r;
        ipc_send(ns_envid, NSREQ_INPUT, (void*)&nsipcbuf, PTE_U | PTE_P | PTE_W);
        
    }

    /*
    char buf[2048];
	uint32_t len = 2048;

	while(true){
		while ((r = sys_net_recv(buf, len)) < 0)
		{
			if (r != -E_AGAIN)
			{
				panic("sys_net_recv: %e\n", r);
			}
		}
		len = r;
		if ((r = sys_page_alloc(0, &nsipcbuf, PTE_U | PTE_P | PTE_W)) < 0)
		{
			panic("sys_page_alloc: %e\n", r);
		}
		nsipcbuf.pkt.jp_len = len;
		memmove(nsipcbuf.pkt.jp_data, buf, len);
		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_U | PTE_P | PTE_W);
	}*/
}

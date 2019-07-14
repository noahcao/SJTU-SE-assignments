#include "ns.h"

extern union Nsipc nsipcbuf;

#define RETRY_MAX 5

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet request (using ipc_recv)
	//	- send the packet to the device driver (using sys_net_send)
	//	do the above things in a loop、
    while(true){
        // keep reading
        int perm = 0;
        int r = ipc_recv(&ns_envid, &nsipcbuf, &perm);
        
        if(r < 0){
            panic("net/output.c output error: %e", r);
        }

        if((thisenv->env_ipc_from != ns_envid) || 
            (thisenv->env_ipc_value != NSREQ_OUTPUT))
            continue;

        int retry = 0;
        while(retry < RETRY_MAX){
            r = sys_net_send(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);
            if(r == nsipcbuf.pkt.jp_len){
                break;
            }
            if(r == -E_INVAL){
                panic("net/output.c output error: invalid len: %d\n", nsipcbuf.pkt.jp_len);
            }
            retry += 1;
        }
    }
}

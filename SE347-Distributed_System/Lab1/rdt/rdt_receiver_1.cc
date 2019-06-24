/*
 * FILE: rdt_receiver.cc
 * DESCRIPTION: Reliable data transfer receiver.
 * NOTE: This implementation assumes there is no packet loss, corruption, or 
 *       reordering.  You will need to enhance it to deal with all these 
 *       situations.  In this implementation, the packet format is laid out as 
 *       the following:
 *       
 *       |<-  1 byte  ->|<-             the rest            ->|
 *       | payload size |<-             payload             ->|
 *
 *       The first byte of each packet indicates the size of the payload
 *       (excluding this single-byte header)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rdt_struct.h"
#include "rdt_receiver.h"

#define PKT_BUF_SIZE 100
#define CHECKSUM_LEN 2
#define PKT_SIZE_BYTE 4
#define PAYLOAD_SIZE 1
#define HEADER_SIZE (CHECKSUM_LEN + PKT_SIZE_BYTE + PAYLOAD_SIZE)
#define PAYLOAD_MAX_SIZE (RDT_PKTSIZE - HEADER_SIZE)

static struct packet* pkt_buffer;
static struct message* cur_msg;

static int expected_pkt_num;
static int receive_msg_cursor;

short checksum(struct packet *pkt){
    // function to calculate the checksumming value
    unsigned int cksum = 0;
    for(int i = CHECKSUM_LEN; i < RDT_PKTSIZE; i++){
        cksum += pkt->data[i];
    }

    unsigned short cast_cksum = (cksum >> 16) + (cksum & 0xffff);
    return cast_cksum;
}

/* receiver initialization, called once at the very beginning */
void Receiver_Init()
{
    pkt_buffer = (struct packet *)malloc(sizeof(struct packet) * PKT_BUF_SIZE);
    cur_msg = (struct message *)malloc(sizeof(struct message));

    expected_pkt_num = 0;
    receive_msg_cursor = 0;
}

/* receiver finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to use this opportunity to release some 
   memory you allocated in Receiver_init(). */
void Receiver_Final()
{
    free(pkt_buffer);
    free(cur_msg);
}

/* event handler, called when a packet is passed from the lower layer at the 
   receiver */
void Receiver_FromLowerLayer(struct packet *pkt)
{
    short cksum;
    memcpy(&cksum, pkt->data, sizeof(short));

    if(cksum != checksum(pkt)){
        fprintf(stdout, "Receiver get error cksum!\n");
        return;
    }

    int pkt_seq_num = (int)(pkt->data + sizeof(short));

    if(pkt_seq_num == expected_pkt_num){
        struct packet *ack_pkt = pkt;
        while(true){
            int payload_size = (char)(ack_pkt->data + HEADER_SIZE - sizeof(char));
            if(receive_msg_cursor == 0){
                int msg_size = (int)(ack_pkt->data + HEADER_SIZE);
            }
        }
    }
}

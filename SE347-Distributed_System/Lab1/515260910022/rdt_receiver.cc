/*
 * FILE: rdt_receiver.cc
 * DESCRIPTION: Reliable data transfer receiver.
 * NOTE: This implementation assumes there is no packet loss, corruption, or 
 *       reordering.  You will need to enhance it to deal with all these 
 *       situations.  In this implementation, the packet format is laid out as 
 *       the following:
 *       
 *        Ack packet:
 *       |<- 2 byte ->|<- 4 byte ->|<-   the rest   ->|
 *       | check sum  |   pkt seq  |     nothing      |
 *
 *       The first byte of each packet indicates the size of the payload
 *       (excluding this single-byte header)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rdt_struct.h"
#include "rdt_receiver.h"

#define PKT_QUEUE_SIZE 20
#define CHECKSUM_LEN 2
#define PKT_SEQ_LEN 4
#define HEADER_SIZE (CHECKSUM_LEN + PKT_SEQ_LEN + 1)

static struct packet *pkt_buffer;
static struct message *cur_msg;

static int ref_pkt_seq_num;
static int msg_slicer;

static short checksum(struct packet *pkt){
    unsigned int sum = 0;
    for (int i = 2; i < RDT_PKTSIZE; i++){ sum += pkt->data[i]; }

    while ((sum >> 16) > 0){
        sum = (sum >> 16) + (sum & 0xffff);
    }
    return (unsigned short)sum;
}

static void ack(int ack){
    struct packet pkt;
    memcpy(pkt.data + sizeof(short), &ack, sizeof(int));
    short check_sum = checksum(&pkt);
    memcpy(pkt.data, &check_sum, sizeof(short));
    Receiver_ToLowerLayer(&pkt);
}

/* receiver initialization, called once at the very beginning */
void Receiver_Init(){
    pkt_buffer = (struct packet *)malloc(sizeof(struct packet) * PKT_QUEUE_SIZE);
    cur_msg = (struct message *)malloc(sizeof(struct message));
    ref_pkt_seq_num = 0;
    msg_slicer = 0;
}

/* receiver finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to use this opportunity to release some 
   memory you allocated in Receiver_init(). */
void Receiver_Final(){
    free(pkt_buffer);
    free(cur_msg);
}

void slice_msgs(packet *ack_pkt){
    int payload_size = *(char *)(ack_pkt->data + HEADER_SIZE - sizeof(char));
    if (msg_slicer == 0){
        int msg_size = *(int *)(ack_pkt->data + HEADER_SIZE);
        cur_msg->size = msg_size;
        cur_msg->data = (char *)malloc(msg_size);

        memcpy(cur_msg->data,
                ack_pkt->data + HEADER_SIZE + sizeof(int), payload_size - sizeof(int));

        msg_slicer += payload_size - sizeof(int);
    }
    else{
        memcpy(cur_msg->data + msg_slicer, ack_pkt->data + HEADER_SIZE, payload_size);
        msg_slicer += payload_size;
    }
}

/* event handler, called when a packet is passed from the lower layer at the 
   receiver */
void Receiver_FromLowerLayer(struct packet *pkt){
    short cksum;
    memcpy(&cksum, pkt->data, sizeof(short));

    if (cksum != checksum(pkt)) return; // verify the checksum value

    int pkt_num = *(int *)(pkt->data + 2);
    if (pkt_num == ref_pkt_seq_num){
        struct packet *ack_pkt = pkt;
        int seq_num = ref_pkt_seq_num;
        while (seq_num == ref_pkt_seq_num){
            slice_msgs(ack_pkt);
            ack(ref_pkt_seq_num);
            ref_pkt_seq_num++;

            // whether the packet has already been in buffer
            int pkt_index = ref_pkt_seq_num % PKT_QUEUE_SIZE;
            struct packet* target_pkt = &pkt_buffer[pkt_index];
            seq_num = *(int *) (target_pkt->data + CHECKSUM_LEN);
            *ack_pkt = pkt_buffer[ref_pkt_seq_num % PKT_QUEUE_SIZE];

            if (cur_msg->size == msg_slicer){
                // whether this packet is the last packet
                Receiver_ToUpperLayer(cur_msg);
                msg_slicer = 0;
                free(cur_msg->data);
            }
        }
    }
    else{
        if(pkt_num > ref_pkt_seq_num && pkt_num < ref_pkt_seq_num + PKT_QUEUE_SIZE){
            int index = pkt_num % PKT_QUEUE_SIZE;
            memcpy(pkt_buffer[index].data, pkt->data, RDT_PKTSIZE);
        }
        if(pkt_num < ref_pkt_seq_num) ack(ref_pkt_seq_num - 1);
    }
}

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

#define PKT_BUF_SIZE 20
#define HEAD_SIZE 7
#define MAX_PAYLOAD_SIZE (RDT_PKTSIZE - HEAD_SIZE)

static struct packet *pkt_buf;
static struct message *current_msg;

static int expected_pkt_num;
static int receive_msg_cursor;

static short checksum(struct packet *pkt)
{
    unsigned int sum = 0;
    for (int i = 2; i < RDT_PKTSIZE; i++)
    {
        sum += pkt->data[i];
    }

    while ((sum >> 16) > 0)
    {
        sum = (sum >> 16) + (sum & 0xffff);
    }

    return (unsigned short) sum;
}

static int get_pkt_seq(struct packet *pkt)
{
    return *(int *)(pkt->data + sizeof(short));
}

static void Send_ack(int ack)
{
    struct packet pkt;
    memset(pkt.data, 0, RDT_PKTSIZE);
    memcpy(pkt.data + sizeof(short), &ack, sizeof(int));
    short check_sum = checksum(&pkt);
    memcpy(pkt.data, &check_sum, sizeof(short));
    Receiver_ToLowerLayer(&pkt);
}

/* receiver initialization, called once at the very beginning */
void Receiver_Init()
{
    //fprintf(stdout, "At %.2fs: receiver initializing ...\n", GetSimulationTime());
    pkt_buf = (struct packet *)malloc(sizeof(struct packet) * PKT_BUF_SIZE);
    current_msg = (struct message *)malloc(sizeof(struct message));

    // memset(pkt_buf, 0, sizeof(struct packet) * PKT_BUF_SIZE);
    // memset(current_msg, 0, sizeof(struct message));

    expected_pkt_num = 0;
    receive_msg_cursor = 0;
}

/* receiver finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to use this opportunity to release some 
   memory you allocated in Receiver_init(). */
void Receiver_Final()
{
    //fprintf(stdout, "At %.2fs: receiver finalizing ...\n", GetSimulationTime());
    free(pkt_buf);
    free(current_msg);
}

/* event handler, called when a packet is passed from the lower layer at the 
   receiver */
void Receiver_FromLowerLayer(struct packet *pkt)
{
    short receive_checksum;
    memcpy(&receive_checksum, pkt->data, sizeof(short));

    if (receive_checksum != checksum(pkt))
    {
        //fprintf(stdout, "R: Receive packet error!\n");
        return;
    }

    int pkt_num = get_pkt_seq(pkt);
    //fprintf(stdout, "R: receive pkt_num:%d\n", pkt_num);
    //fprintf(stdout, "R: expected pkt_num:%d", expected_pkt_num);
    if (pkt_num == expected_pkt_num)
    {
        struct packet *ack_pkt = pkt;
        while (true)
        {
            // first packet
            int payload_size = *(char *)(ack_pkt->data + HEAD_SIZE - sizeof(char));
            ASSERT(payload_size != 0);
            //fprintf(stdout, "R: payload_size:%d\n", payload_size);
            if (receive_msg_cursor == 0)
            {
                int msg_size = *(int *)(ack_pkt->data + HEAD_SIZE);
                current_msg->size = msg_size;
                current_msg->data = (char *)malloc(msg_size);

                memcpy(current_msg->data,
                       ack_pkt->data + HEAD_SIZE + sizeof(int), payload_size - sizeof(int));

                receive_msg_cursor += payload_size - sizeof(int);
            }
            else
            {
                memcpy(current_msg->data + receive_msg_cursor, ack_pkt->data + HEAD_SIZE, payload_size);
                receive_msg_cursor += payload_size;
            }

            Send_ack(expected_pkt_num);
            expected_pkt_num++;

            // check whether this packet is the last packet
            if (current_msg->size == receive_msg_cursor)
            {
                //fprintf(stdout, "R: last pkt of a message !\n");
                Receiver_ToUpperLayer(current_msg);
                receive_msg_cursor = 0;
                free(current_msg->data);
                // return;
            }

            // check packet in buffer
            if (get_pkt_seq(&pkt_buf[expected_pkt_num % PKT_BUF_SIZE]) != expected_pkt_num)
            {
                break;
            }

            //fprintf(stdout, "R: waiting packet in buffer!\n");
            *ack_pkt = pkt_buf[expected_pkt_num % PKT_BUF_SIZE];
        }
    }
    else if (pkt_num > expected_pkt_num &&
             pkt_num < expected_pkt_num + PKT_BUF_SIZE)
    {
        int index = pkt_num % PKT_BUF_SIZE;
        // pkt_buf[index] = *pkt;
        memcpy(pkt_buf[index].data, pkt->data, RDT_PKTSIZE);
    }
    else if (pkt_num < expected_pkt_num)
    {
        Send_ack(expected_pkt_num - 1);
    }
    //fprintf(stdout, "R: exit receiver_fromlowerlayer\n");
}

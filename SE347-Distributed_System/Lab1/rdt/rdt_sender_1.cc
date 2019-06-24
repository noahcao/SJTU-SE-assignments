/*
 * FILE: rdt_sender.cc
 * DESCRIPTION: Reliable data transfer sender.
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
#include "rdt_sender.h"

#define BUF_SIZE 10000
#define WINDOW_SIZE 20
#define CHECKSUM_LEN 2
#define PKT_SIZE_BYTE 4
#define PAYLOAD_SIZE 1
#define HEADER_SIZE (CHECKSUM_LEN + PKT_SIZE_BYTE + PAYLOAD_SIZE)
#define PAYLOAD_MAX_SIZE (RDT_PKTSIZE - HEADER_SIZE)
#define TIME_OUT 0.3

static struct message* msg_buffer;
static struct packet* slid_window;
static int pkt_map_msg[WINDOW_SIZE];
static int pkt_map_cursor[WINDOW_SIZE];

static int current_msg_index; // the first message to be sent in the buffer
static int latest_msg_index; // the last message to be sent in the buffer

static int current_window_pkt_index; // the first pkt to be sent in the window
static int msg_cursor;

static int latest_seq_num;

/*
short checksum(struct packet *pkt){
    // function to calculate the checksumming value
    unsigned int cksum = 0;
    for(int i = CHECKSUM_LEN; i < RDT_PKTSIZE; i++){
        cksum += pkt->data[i];
    }

    unsigned short cast_cksum = (cksum >> 16) + (cksum & 0xffff);
    return cast_cksum;
}*/

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

    unsigned short result = sum;

    return ~result;
}

/* sender initialization, called once at the very beginning */
void Sender_Init()
{
    // initilize needed structures
    msg_buffer = (struct message*)malloc(sizeof(struct message) * BUF_SIZE);
    slid_window = (struct packet*)malloc(sizeof(struct packet) * WINDOW_SIZE);

    memset(msg_buffer, 0, sizeof(struct message) * BUF_SIZE);
    memset(slid_window, 0, sizeof(struct packet) * WINDOW_SIZE);

    current_msg_index = 0;
    latest_msg_index = 0;
    current_window_pkt_index = 0;
    latest_seq_num = 0;
    msg_cursor = 0;
}

/* sender finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to take this opportunity to release some 
   memory you allocated in Sender_init(). */
void Sender_Final()
{
    free(msg_buffer);
    free(slid_window);
}

struct packet* build_pkt(int seq_num, char size, char *data){
    // build a new packet with given information
    struct packet* pkt = NULL;
    memcpy(pkt->data + CHECKSUM_LEN, &seq_num, PKT_SIZE_BYTE);
    memcpy(pkt->data + (CHECKSUM_LEN + PKT_SIZE_BYTE), &size, PAYLOAD_MAX_SIZE);
    memcpy(pkt->data + HEADER_SIZE, data, size);
    short cksum = checksum(pkt);
    memcpy(pkt->data, &cksum, CHECKSUM_LEN);
    return pkt;
}

void slide_window_send(){
    // send packets with sliding window scheme
    message cur_msg = msg_buffer[current_msg_index % BUF_SIZE];

    while(latest_seq_num < WINDOW_SIZE + current_window_pkt_index && current_msg_index < latest_msg_index){
        int message_size = cur_msg.size;
        struct packet* pkt;
        pkt_map_msg[latest_seq_num % WINDOW_SIZE] = current_msg_index;
        pkt_map_cursor[latest_seq_num % WINDOW_SIZE] = msg_cursor;
        if(message_size - msg_cursor > PAYLOAD_MAX_SIZE){
            pkt = build_pkt(latest_seq_num, PAYLOAD_MAX_SIZE, cur_msg.data + msg_cursor);
            msg_cursor + PAYLOAD_MAX_SIZE;
        }
        else{
            pkt = build_pkt(latest_seq_num, cur_msg.size - msg_cursor, cur_msg.data + msg_cursor);
            current_msg_index += 1;
            msg_cursor = 0;
            if(current_msg_index < latest_seq_num){
                cur_msg = msg_buffer[current_msg_index % BUF_SIZE];
            }
        }
        Sender_ToLowerLayer(pkt);
        latest_seq_num += 1;
    }
}

/* event handler, called when a message is passed from the upper layer at the 
   sender */
void Sender_FromUpperLayer(struct message *msg)
{
    if(latest_msg_index - current_msg_index > BUF_SIZE){
        fprintf(stdout, "Message buffer is too small to hold new message\n");
        return;
    }

    int index = latest_msg_index % BUF_SIZE;
    char *data = (char*)malloc(msg->size);
    
    memcpy(data, msg->data, msg->size);
    msg_buffer[index].size = msg->size;
    msg_buffer[index].data = data;


    latest_msg_index += 1;
    slide_window_send();

    if(!Sender_isTimerSet()) Sender_StartTimer(TIME_OUT);
}

/* event handler, called when a packet is passed from the lower layer at the 
   sender */
void Sender_FromLowerLayer(struct packet *pkt)
{
    short ack_cksum;
    memcpy(&ack_cksum, pkt->data, CHECKSUM_LEN);

    if(ack_cksum != checksum(pkt)){
        fprintf(stdout, "Ack packet checksum error!\n");
        return;
    }

    int ack_seq_num;
    memcpy(&ack_seq_num, pkt->data + CHECKSUM_LEN, sizeof(int));
    if(ack_seq_num >= current_window_pkt_index && ack_seq_num < latest_seq_num){
        current_window_pkt_index = ack_seq_num + 1;
        Sender_StartTimer(TIME_OUT);
        if(current_msg_index < latest_msg_index){
            slide_window_send();
        }

        if(ack_seq_num == latest_seq_num - 1){
            // no new packet to send
            Sender_StopTimer();
        }
    }
}

/* event handler, called when the timer expires */
void Sender_Timeout()
{
    Sender_StartTimer(TIME_OUT);

    latest_seq_num = current_window_pkt_index;
    current_msg_index = pkt_map_msg[current_window_pkt_index % WINDOW_SIZE];
    msg_cursor = pkt_map_cursor[current_window_pkt_index % WINDOW_SIZE];
    slide_window_send();
}

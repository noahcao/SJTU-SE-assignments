/*
 * FILE: rdt_sender.cc
 * DESCRIPTION: Reliable data transfer sender.
 * NOTE: This implementation assumes there is no packet loss, corruption, or 
 *       reordering.  You will need to enhance it to deal with all these 
 *       situations.  In this implementation, the packet format is laid out as 
 *       the following:
 *       
 *      Send packet:
 *       |<- 2 byte ->|<- 4 byte ->|<- 1 byte ->|<-    the rest    ->|
 *       | check sum  |   seq num  |payload size|      payload       |
 * 
 *
 *      Ack packet:
 *       |<- 2 byte ->|<- 4 byte ->|<-   the rest   ->|
 *       | check sum  |   seq num  |     nothing      |
 * 
 *       The first byte of each packet indicates the size of the payload
 *       (excluding this single-byte header)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rdt_struct.h"
#include "rdt_sender.h"

#define WINDOW_SIZE 20
#define TIME_OUT 0.3
#define BUF_SIZE 10000
#define CHECKSUM_LEN 2
#define PKT_SEQ_LEN 4
#define HEADER_SIZE (CHECKSUM_LEN + PKT_SEQ_LEN + 1)
#define MAX_PAYLOAD (RDT_PKTSIZE - HEADER_SIZE)

static struct message* msg_buf;
static struct packet* window;
static int pkt_map_msg[WINDOW_SIZE];
static int pkt_map_cursor[WINDOW_SIZE];

static int window_header_index;
static int latest_seq_num;
static int current_msg_index;
static int latest_msg_index;
static int msg_slicer;

short checksum(struct packet *pkt){
    unsigned int cksum = 0;
    for (int i = CHECKSUM_LEN; i < RDT_PKTSIZE; i++){
        cksum += pkt->data[i];
    }

    while ((cksum >> 4 * sizeof(int)) > 0) {
        cksum = (cksum >> 4 * sizeof(int)) + (cksum & 0xffff);
    }
    return (unsigned short) cksum;
}

/* sender initialization, called once at the very beginning */
void Sender_Init(){
    msg_buf = (struct message *)malloc(sizeof(struct message) * BUF_SIZE);
    window = (struct packet *)malloc(sizeof(struct packet) * WINDOW_SIZE);

    current_msg_index = 0;
    latest_msg_index = 0;
    msg_slicer = 0;
    window_header_index = 0;
    latest_seq_num = 0;
}

/* sender finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to take this opportunity to release some 
   memory you allocated in Sender_init(). */
void Sender_Final(){
    free(msg_buf);
    free(window);
}

void build_pkt(struct packet *pkt, int seq_num, char size, char *data){
    memcpy(pkt->data + CHECKSUM_LEN, &seq_num, CHECKSUM_LEN);
    memcpy(pkt->data + CHECKSUM_LEN + PKT_SEQ_LEN, &size, 1);
    memcpy(pkt->data + HEADER_SIZE, data, size);
    short sum = checksum(pkt);
    memcpy(pkt->data, &sum, CHECKSUM_LEN);
}

/* slide window and send packets in window */
static void slide_window_send(){
    message cur_msg = msg_buf[current_msg_index % BUF_SIZE];
    bool window_is_full = latest_seq_num >= window_header_index + WINDOW_SIZE;
    bool not_last_msg = current_msg_index < latest_msg_index;
    while (!window_is_full && not_last_msg){
        struct packet pkt;
        int pkt_index = latest_seq_num % WINDOW_SIZE;
        pkt_map_msg[pkt_index] = current_msg_index;
        pkt_map_cursor[pkt_index] = msg_slicer;
        if (cur_msg.size - msg_slicer <= MAX_PAYLOAD){
            build_pkt(&pkt, latest_seq_num, cur_msg.size - msg_slicer, cur_msg.data + msg_slicer);
            current_msg_index += 1;
            if (current_msg_index < latest_msg_index) 
                cur_msg = msg_buf[current_msg_index % BUF_SIZE];
            msg_slicer = 0; // reset the cursor to slice message into packets
        }
        else{
            build_pkt(&pkt, latest_seq_num, MAX_PAYLOAD, cur_msg.data + msg_slicer);
            msg_slicer += MAX_PAYLOAD;
        }
        Sender_ToLowerLayer(&pkt);
        latest_seq_num += 1;
        window_is_full = latest_seq_num >= window_header_index + WINDOW_SIZE;
        not_last_msg = current_msg_index < latest_msg_index;
    }
}

/* event handler, called when a message is passed from the upper layer at the 
   sender */
void Sender_FromUpperLayer(struct message *msg){
    if (latest_msg_index - current_msg_index > BUF_SIZE) return;

    int index = latest_msg_index % BUF_SIZE;

    msg_buf[index].size = msg->size + 4;
    char *data = (char *)malloc(msg_buf[index].size);
    memcpy(data, &(msg->size), sizeof(int));
    memcpy(data + sizeof(int), msg->data, msg->size);
    msg_buf[index].data = data;

    latest_msg_index++;
    slide_window_send();

    if (!Sender_isTimerSet()) Sender_StartTimer(TIME_OUT);
}

/* event handler, called when a packet is passed from the lower layer at the 
   sender */
void Sender_FromLowerLayer(struct packet *pkt){
    short ack_checksum;
    memcpy(&ack_checksum, pkt->data, sizeof(short));

    if (ack_checksum != checksum(pkt)) return;

    int ack_pkt_num;
    memcpy(&ack_pkt_num, pkt->data + sizeof(short), sizeof(int));
    
    if (ack_pkt_num >= window_header_index &&
        ack_pkt_num < latest_seq_num){
        window_header_index = ack_pkt_num + 1;
        Sender_StartTimer(TIME_OUT);
        if (current_msg_index < latest_msg_index) slide_window_send();  // keep sending
        if (ack_pkt_num == latest_seq_num - 1) Sender_StopTimer();  // no more message
    }
}

/* event handler, called when the timer expires */
void Sender_Timeout(){
    Sender_StartTimer(TIME_OUT);

    latest_seq_num = window_header_index;
    int pkt_index = window_header_index % WINDOW_SIZE;
    current_msg_index = pkt_map_msg[pkt_index];
    msg_slicer = pkt_map_cursor[pkt_index];
    slide_window_send();
}

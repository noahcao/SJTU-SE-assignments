#include "rte_common.h"
#include "rte_mbuf.h"
#include "rte_meter.h"
#include "rte_red.h"

#include "qos.h"

/**
 * srTCM
 */

struct rte_meter_srtcm_params params[APP_FLOWS_MAX] = {
	{.cir = 20000000000, .cbs = 140000, .ebs = 200000},
	{.cir = 20000000000, .cbs = 68000, .ebs = 100000},
	{.cir = 20000000000, .cbs = 35000, .ebs = 50000},
	{.cir = 20000000000, .cbs = 17500, .ebs = 25000}
};

struct rte_meter_srtcm srtcm[APP_FLOWS_MAX];

int
qos_meter_init(void)
{
	for(int i = 0; i < APP_FLOWS_MAX; i++){
		int flag = rte_meter_srtcm_config(&srtcm[i] , &params[i]);
		if(flag != 0){
			printf("meter init error in srtcm\n");
			return flag;
		}
	}
    return 0;
}

enum qos_color
qos_meter_run(uint32_t flow_id, uint32_t pkt_len, uint64_t time)
{
	return rte_meter_srtcm_color_blind_check(&srtcm[flow_id], time, pkt_len);
	
}


/**
 * WRED
 */


// params for three colors in four flows
struct rte_red_config red_config[4][3];
struct rte_red red[4][3];
int flow_queue[4];
uint64_t last_run_time = -1;

struct rte_red_params red_params[4][3] = {
	{
		{.min_th = 160, .max_th = 240, .maxp_inv = 80, .wq_log2 = 8},
		{.min_th = 1, .max_th = 50, .maxp_inv = 20, .wq_log2 = 8},
		{.min_th = 1, .max_th = 2, .maxp_inv = 1, .wq_log2 = 8}
	},
	{
		{.min_th = 80, .max_th = 120, .maxp_inv = 40, .wq_log2 = 8},
		{.min_th = 1, .max_th = 25, .maxp_inv = 10, .wq_log2 = 8},
		{.min_th = 1, .max_th = 2, .maxp_inv = 1, .wq_log2 = 8}
	},
	{
		{.min_th = 40, .max_th = 60, .maxp_inv = 20, .wq_log2 = 8},
		{.min_th = 1, .max_th = 12, .maxp_inv = 5, .wq_log2 = 8},
		{.min_th = 1, .max_th = 2, .maxp_inv = 1, .wq_log2 = 8}
	},
	{
		{.min_th = 20, .max_th = 30, .maxp_inv = 10, .wq_log2 = 8},
		{.min_th = 1, .max_th = 6, .maxp_inv = 3, .wq_log2 = 8},
		{.min_th = 1, .max_th = 2, .maxp_inv = 1, .wq_log2 = 8}	
	}
};

#define q_size (sizeof(uint32_t)*APP_FLOWS_MAX)

int
qos_dropper_init(void)
{
    /* to do */
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 3; j++){
			rte_red_config_init(&red_config[i][j], red_params[i][j].wq_log2,
	red_params[i][j].min_th, red_params[i][j].max_th, red_params[i][j].maxp_inv);
			rte_red_rt_data_init(&red[i][j]);
		}
	}
	memset(flow_queue, 0, q_size);
	return 0;
}

int
qos_dropper_run(uint32_t flow_id, enum qos_color color, uint64_t time)
{
    /* to do */
	if(time != last_run_time){
		memset(flow_queue, 0, q_size);
		last_run_time = time;
	}

	int queue_flag = rte_red_enqueue(&red_config[flow_id][color], 
		&red[flow_id][color], flow_queue[flow_id], time);

	if(queue_flag != 0) return 1;

	flow_queue[flow_id] += 1;
	return 0;
}

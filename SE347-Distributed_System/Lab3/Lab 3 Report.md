# Lab 3 Report

student id: 515260910022

name: 曹金坤

email: caojinkun@sjtu.edu.cn

## Implementation Details

### Parameter Deduction

There are a handful of parameters worth caring about in this lab. For Meter and Dropper respectively, we list them as set in my implementation together as follows, where I also add comments to help explain them.

```c
struct rte_meter_srtcm_params params[APP_FLOWS_MAX] = {
    // cir is the speed to packet commit, when it's set big enough, it would be the bottleneck of data flow transfer
    // cbs is the permitted size of committed burst
    // ebs is the excess burst size
    // these three parameters combine to adjust the behaviour of meter
	{.cir = 20000000000, .cbs = 140000, .ebs = 200000},
	{.cir = 20000000000, .cbs = 68000, .ebs = 100000},
	{.cir = 20000000000, .cbs = 35000, .ebs = 50000},
	{.cir = 20000000000, .cbs = 17500, .ebs = 25000}
};

struct rte_red_params red_params[4][3] = {
	{	
        // for three color condition of four flows respectively, I set in total 12 groups of parameters to adjust their behaviour
        // min_th is the minimum threshold of packets in the queue
        // max_th is the maximum threshold of packets in the queue
        // maxp_inv is the inverse of packet seting probability maxnimum value
        // wq_log2 is the negated log_2 of queue weight
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
```

First, as no requirements are set for the parameter ``cir''. I set it to be large enough to have no influence on test process.  On the other hand, to catch the bandwidth requirements for different flows, I adjust the remaining parameters specific to each flow. The cbs and ebs are adjusted generally referring to wanted bandwidth proportion.

Second, for the dropper, for the packet with red color, I set the min_th=1 and max_th=2 to filter all red packets, which is wanted by my design pattern. and the packet filtering policy is also strict, though much relaxed than that for red packets, to packets with yellow color. Therefore, the legal queue for red packet and yellow packets would be much shorted than that for green packets. Through such design, we use the color information to apply different passing policy to different packet queue. On the other hand, the packet queue for different flows are also designed to conform to the desired bandwidth proportion.

Finally, the cir would not be a bottleneck and the bandwidth of four flows should be in proportion of about 8:4:2:1. 

### API use

As DPDK has already provide convenient functions that I can use through API, the implementation of this lab becomes much easier. For different functions in **qos.c**, involved function APIs are listed as below:

As introduced above, the parameters for both meter and dropper are defined globally. Then in the implementation of meter, the function **rte_meter_srtcm_config** is called to configure the *rte_meter_srtcm* objects through given meter parameters. Then, **rte_meter_srtcm_color_blind_check** function is called to leverage the given specific *srtcm* object and corresponding time and packet_len information to activate the meter in a blind mode as required.

For the dropper implementation, **rte_red_config_init** is called to initialize the further used *rte_red_config* objects through given parameters. And the maintained *rte_red* and queues are also initialized, the former of which is realized through the function **rte_red_rt_data_init**. Once dropper being running, the main function is implemented through **rte_red_enqueue**. And a variable last_run_time records the last running time.

### Results

Here, with given standard test program, I attach some running results shot for easy check:

![1554629274896](F:\GoogleDrive\18_19_2\DS\Lab3\assets\1554629274896.png)

![1554629295016](F:\GoogleDrive\18_19_2\DS\Lab3\assets\1554629295016.png)

Because the send packet number is partially random, the detailed result changes. But the passed packets for four flows are about in proportion 8:4:2:1 as required.
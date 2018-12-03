module sc_datamem (addr, datain, dataout, we, clock, mem_clk, dmem_clk
   , in_port0, in_port1, dt0, dt1, dt2, dt3, dt4, dt5);
   // in_port0和in_port1是用于计算机操作的两个操作数
 
   input  [31:0]  addr;
   input  [31:0]  datain;
   input          we, clock,mem_clk;
   input [4:0]    in_port0, in_port1;

   output [31:0]  dataout;
   //output [31:0]  out_port0, out_port1, out_port2;
   output         dmem_clk;
   output [6:0] dt0, dt1, dt2, dt3, dt4, dt5;
   
   reg [31:0] out_port0, out_port1, out_port2;

   wire           dmem_clk;    
   wire           write_enable; 
   wire           write_io_reg_enable, write_datamem_enable;
   wire [31:0]    mem_dataout;
   wire [31:0]    io_data;

   assign         write_enable = we & ~clock; 
   assign         write_datamem_enable = write_enable & (~addr[7]);
   assign         write_io_reg_enable = write_enable & (addr[7]);
   assign         dmem_clk = mem_clk & ( ~ clock) ; 

   io_input io_input_regx2(addr, dmem_clk, in_port0, in_port1, io_data,
      dt0, dt1, dt2, dt3); // 得到io_data的数据，在之后的mux2x32中给dataout
   io_output io_output_reg(addr, datain, write_io_reg_enable, dmem_clk, out_port0, out_port1, out_port2, dt4, dt5);  

   mux2x32 mem_io(mem_dataout, io_data, addr[7], dataout);
   // addr[7]和取内存所用的地址无关了，可以作为判断是从io取值还是mem取值的flag位
   lpm_ram_dq_dram  dram(addr[6:2],dmem_clk, datain, write_datamem_enable, mem_dataout);

endmodule 
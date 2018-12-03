/////////////////////////////////////////////////////////////
//                                                         //
// School of Software of SJTU                              //
//                                                         //
/////////////////////////////////////////////////////////////

module sc_computer (resetn,clock,mem_clk,
			pc,inst, aluout, memout, imem_clk, dmem_clk,
            in_port0, in_port1, dt0, dt1, dt2, dt3, dt4, dt5);
   
   input resetn,clock,mem_clk;
   input [4:0]    in_port0, in_port1;

   output [31:0] pc,inst,aluout,memout;
   output        imem_clk,dmem_clk;
   output [6:0]  dt0, dt1, dt2, dt3, dt4, dt5;

   wire   [31:0] data;
   wire          wmem; // all these "wire"s are used to connect or interface the cpu,dmem,imem and so on.
   
   sc_cpu cpu (clock,resetn,inst,memout,pc,wmem,aluout,data);          // CPU module.
   sc_instmem  imem (pc,inst,clock,mem_clk,imem_clk);                  // instruction memory.
   sc_datamem  dmem (aluout, data, memout, wmem, clock,mem_clk, dmem_clk
					in_port0, in_port1, dt0, dt1, dt2, dt3, dt4, dt5); // data memory.
endmodule




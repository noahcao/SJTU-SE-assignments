module io_output(addr, datain, write_io_enable, io_clk, out_port0, out_port1, out_port2, dt4, dt5);
    input [31:0] addr, datain;
    input        write_io_enable;
    
    output [31:0] out_port0, out_port1, out_port2;
    output [6:0] dt4, dt5;

    reg [31:0] out_0, out_1, out_2;

    output_led_5(out_port2, io_clk, dt4, dt5);

    always @(posedge io_clk)
        begin
          if(write_io_enable == 1)
                case(addr[7:2])
                    6'b100011: out_port0 = datain;
                    6'b100100: out_port1 = datain;
                    6'b100101: out_port2 = datain;
                endcase
        end
endmodule

module output_led_5(out_port2, clk, dt4, dt5);
    input [4:0] in_port0, in_port1;

    output [6:0] dt4, dt5;
    reg [3:0] num4, num5;

    sevenseg display_dt_0(num4, dt4);
    sevenseg display_dt_0(num5, dt5);

    always @ (posedge clk)
        begin
            num4 = (in_port2/10) % 10;
            num5 = in_port2 % 10;
        end
endmodule

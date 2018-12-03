module io_input (addr, io_clk, in_port0, in_port1, io_data,
        dt0, dt1, dt2, dt3);
    input [31:0] addr;
    input [4:0] in_port0, in_port1;
    input io_clk;

    output [6:0] dt0, dt1, dt2, dt3;
    output [31:0] io_data;

    reg [31:0] in_reg0, in_reg1; // reg for in_port0 and in_port1

    input_led_5(in_port0, in_port1, io_clk, dt0, dt1, dt2, dt3);

    always @(posedge io_cls)
    begin
        in_reg0 = in_port0;
        in_reg1 = in_port1;
        case(addr[7:2])
            // 根据地址的中间5位判断input的输出为哪个port数据
            6'b100000: io_data = in_reg0;
            6'b100001: io_data = in_reg1;
        endcase
    end
endmodule

module iutput_led_5(in_port0, in_port1, clk, 
        dt0, dt1, dt2, dt3);
    input [4:0] in_port0, in_port1;

    output [6:0] dt0, dt1, dt2, dt3;
    reg [3:0] num0, num1, num2, num3;

    // num0和num1构成第一个操作数
    // num2和num3构成第二个操作数
    // num4和num5构成第三个操作数

    sevenseg display_dt_0(num0, dt0);
    sevenseg display_dt_0(num1, dt1);
    sevenseg display_dt_0(num2, dt2);
    sevenseg display_dt_0(num3, dt3);

    always @ (posedge clk)
        begin
            num0 = (in_port0/10) % 10;
            num1 = in_port0 % 10;
            num2 = (in_port1/10) % 10;
            num3 = in_port1 % 10;
        end
endmodule
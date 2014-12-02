`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date:    18:16:33 04/02/2014
// Design Name:
// Module Name:    goboardinput
// Project Name:
// Target Devices:
// Tool versions:
// Description:
//
// Dependencies:
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////
module goboardinput(
	input wire clk,
	input wire clr,
	input wire ps2_0_clk,
	input wire ps2_0_dat,
	output reg [15:0] RegVal,
	output reg bSet,
	output wire [4:0] pieces_sel_x,
	output wire [4:0] pieces_sel_y
    );

/* PS2_Debug */

reg [4:0] input_sel_x;
reg [4:0] input_sel_y;

assign pieces_sel_x = input_sel_x;
assign pieces_sel_y = input_sel_y;

wire KbdRdy;
wire KbdParityError;
wire [7:0] KbdScanCode;
reg KbdRdy0;

keyboard kbd0(.clk(clk), .clr(clr),
              .PS2KeyboardData(ps2_0_dat), .PS2KeyboardClk(ps2_0_clk),
              .ScanCode(KbdScanCode), .ParityError(KbdParityError), .Ready(KbdRdy));

always @(posedge clk) begin
    if(clr == 1) begin
        KbdRdy0 <= 0;
        RegVal = 0;
		bSet <= 0;
		input_sel_x <= 0;
		input_sel_y <= 0;
    end else begin
        KbdRdy0 <= KbdRdy;
        if(KbdRdy0 == 0 && KbdRdy == 1 && KbdParityError == 0) begin
            RegVal = {RegVal[7:0], KbdScanCode};
			bSet <= 0;
			case (RegVal[15:0])
				16'hF01C :  begin
					input_sel_x <= 0;
					input_sel_y <= 0;
				end
				16'hF032 :begin
					input_sel_x <= 1;
					input_sel_y <= 0;
				end
				16'hF021 :begin
					input_sel_x <= 2;
					input_sel_y <= 0;
				end
				16'hF023 :begin
					input_sel_x <= 3;
					input_sel_y <= 0;
				end
				16'hF024 :begin
					input_sel_x <= 4;
					input_sel_y <= 0;
				end
				16'hF02B :begin
					input_sel_x <= 5;
					input_sel_y <= 0;
				end
				16'hF034 :begin
					input_sel_x <= 6;
					input_sel_y <= 0;
				end
				16'hF033 :begin
					input_sel_x <= 7;
					input_sel_y <= 0;
				end
				16'hF03B :begin
					input_sel_x <= 8;
					input_sel_y <= 0;
				end
				16'hF042 :begin
					input_sel_x <= 9;
					input_sel_y <= 0;
				end
				16'hF04B :begin
					input_sel_x <= 10;
					input_sel_y <= 0;
				end
				16'hF03A :begin
					input_sel_x <= 11;
					input_sel_y <= 0;
				end
				16'hF031 :begin
					input_sel_x <= 12;
					input_sel_y <= 0;
				end
				16'hF044 :begin
					input_sel_x <= 13;
					input_sel_y <= 0;
				end
				16'hF04D :begin
					input_sel_x <= 14;
					input_sel_y <= 0;
				end
				16'hF015 :begin
					input_sel_x <= 15;
					input_sel_y <= 0;
				end
				16'hF02D :begin
					input_sel_x <= 16;
					input_sel_y <= 0;
				end
				16'hF01B :begin
					input_sel_x <= 17;
					input_sel_y <= 0;
				end
				16'hF02C :begin
					input_sel_x <= 18;
					input_sel_y <= 0;
				end
				16'hF045 : input_sel_y <= input_sel_y * 10 + 0;
				16'hF016 : input_sel_y <= input_sel_y * 10 + 1;
				16'hF01E : input_sel_y <= input_sel_y * 10 + 2;
				16'hF026 : input_sel_y <= input_sel_y * 10 + 3;
				16'hF025 : input_sel_y <= input_sel_y * 10 + 4;
				16'hF02E : input_sel_y <= input_sel_y * 10 + 5;
				16'hF036 : input_sel_y <= input_sel_y * 10 + 6;
				16'hF03D : input_sel_y <= input_sel_y * 10 + 7;
				16'hF03E : input_sel_y <= input_sel_y * 10 + 8;
				16'hF046 : input_sel_y <= input_sel_y * 10 + 9;
				16'hF05A : bSet <= 1;
			endcase
        end
    end
end


endmodule

`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date:    15:22:28 01/03/2014
// Design Name:
// Module Name:    keyboard
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
module keyboard(
    input wire clk,
	input wire clr,
    input wire PS2KeyboardData,
    input wire PS2KeyboardClk,
	output wire [7:0] ScanCode,
    output reg ParityError,
	output reg Ready
    );

reg [7:0] PS2KbdDataDeinterference;
reg [7:0] PS2KbdClkDeinterference;

reg PS2KbdData;
reg PS2KbdClk;
reg PS2KbdClk0;
reg [9:0] PS2ShiftReg;
reg [3:0] PS2Count;

wire PS2KbdClkNegedge;
wire [10:0] PS2ScanCode;

assign PS2ScanCode = {PS2KbdData, PS2ShiftReg};
assign PS2KbdClkNegedge = PS2KbdClk0 && ~PS2KbdClk;
assign ScanCode = PS2ShiftReg[7:0];

always @(posedge clk) begin
    if(clr) begin
		PS2KbdDataDeinterference <= 8'b0;
		PS2KbdClkDeinterference <= 8'b0;
	end else begin
		PS2KbdDataDeinterference <= {PS2KbdDataDeinterference[6:0], PS2KeyboardData};
		PS2KbdClkDeinterference <= {PS2KbdClkDeinterference[6:0], PS2KeyboardClk};

		if(PS2KbdDataDeinterference == 8'b1111_1111)
			PS2KbdData <= 1;
		else if(PS2KbdDataDeinterference == 8'b0000_0000)
			PS2KbdData <= 0;

		if(PS2KbdClkDeinterference == 8'b1111_1111)
			PS2KbdClk <= 1;
		else if(PS2KbdClkDeinterference == 8'b0000_0000)
			PS2KbdClk <= 0;
	end
end

always @(posedge clk) begin
	if(clr) begin
		PS2KbdClk0 <= 0;
		PS2Count <= 0;
		PS2ShiftReg <= 0;
		ParityError <= 0;
		Ready <= 0;
	end else begin
		PS2KbdClk0 <= PS2KbdClk;
		if(PS2KbdClkNegedge) begin
		    PS2ShiftReg <= PS2ScanCode[10:1];
			if(PS2Count == 10) begin
			    if( PS2ScanCode[0] == 0 && PS2ScanCode[10] == 1 &&
					PS2ScanCode[9] == ^~PS2ScanCode[8:1] ) begin
					Ready <= 1;
					ParityError <= 0;
					PS2Count <= 0;
				end else begin
				    ParityError <= 1;
					Ready <= 0;
				end
			end else begin
				PS2Count <= PS2Count + 1'b1;
				Ready <= 0;
				ParityError <= 0;
			end
		end
	end
end

endmodule

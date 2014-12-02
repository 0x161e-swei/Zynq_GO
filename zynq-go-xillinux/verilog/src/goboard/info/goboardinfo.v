`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    08:44:10 07/29/2014 
// Design Name: 
// Module Name:    goboardinfo 
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
module goboard_getinfo(
	output wire bInfo,
	input wire 	vga_clk_in,
	input wire	sys_clk_in,
	input wire 	clr,
	input wire  wea,
	input wire [11:0] hc_i,
   input wire [11:0] vc_i,
	//input wire 	[3:0]row,
	//input wire 	[4:0]column,
	input wire	[8:0]write_ram_addr,
	input wire 	[7:0]in_char
	);
	
`include "char_rom.vh"
	 
	parameter top = 12'd24;
	parameter left = 12'd600;
	parameter info_width = 16;	//16 chars for each line
	parameter info_height = 32;	//32 chars for each column
	parameter char_width = 8;
	parameter char_height = 16;

	wire [8:0]vga_ram_addr;
	wire [3:0]client_tx;
	wire [4:0]client_ty;	//client text x/y: the text position of the goboard_info region
	wire [2:0]char_pixel_x;
	wire [3:0]char_pixel_y;
	wire [6:0]char_pixel_addr;
	wire [7:0]vga_char;
	 
	wire [7:0] terma;
	goboard_info_ram goboard_info_ram0(
		.clka(sys_clk_in),
		.wea(wea),
		.addra(write_ram_addr),
		.dina(in_char),
		.clkb(vga_clk_in),
		.douta(terma),
		.web(1'b0),
		.dinb(8'b0),
		.addrb(vga_ram_addr),
		.doutb(vga_char)
		);
	 
	 //***************VGA READ*************//
	 
	 //Calculate the ram address for the write port(write only)
	 //write_ram_addr = column*info_width + row;
	 
	 //Calculate the ram address of the char needed for vga(read only)
	 assign client_tx = (hc_i - left)/char_width;
	 assign client_ty = (vc_i - top)/char_height;
	 assign vga_ram_addr = client_ty*info_width + client_tx;
	 
	 //Calculate the scanning position within the char_dots
	 assign char_pixel_x = (hc_i - left)%char_width;
	 assign char_pixel_y = (vc_i - top)%char_height;
	 assign char_pixel_addr = char_pixel_y*char_height + char_pixel_x;
	 
//	 reg [3:0]client_tx;
//     reg [4:0]client_ty;    //client text x/y: the text position of the goboard_info region
//     reg [2:0]char_pixel_x;
//     reg [3:0]char_pixel_y;
//     reg [6:0]char_pixel_addr;
//     reg [8:0]vga_ram_addr;
     
//     always @(posedge vga_clk_in) begin
//         client_tx <= (hc_i - left)/char_width;
//         client_ty <= (vc_i - top)/char_height;
//         vga_ram_addr <= client_ty*info_width + client_tx;
         
//         char_pixel_x <= (hc_i - left)%char_width;
//         char_pixel_y <= (vc_i - top)%char_height;
//         char_pixel_addr <= char_pixel_y*char_height + char_pixel_x;
//     end
     
	 
	 assign bInfo = char_rom[(vga_char - 8'h20)*char_height+char_pixel_y][char_pixel_x];
	 //assign bInfo = char_rom[(8'h50 - 8'h20)*char_height+char_pixel_y][char_pixel_x];
    
    
    //assign bInfo = bInfo_r;
    
	//if modified, noted with '//'

endmodule

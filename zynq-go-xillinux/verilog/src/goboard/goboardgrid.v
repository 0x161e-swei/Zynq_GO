
module goboardgrid(
	output wire bFill,
    input wire [11:0] hc_i,
    input wire [11:0] vc_i,
    input wire clk,
    input wire clr
);

parameter top   = 12'd24;
parameter left  = 12'd24;

wire [11:0] client_x;
wire [11:0] client_y;

wire bGridX;
wire bGridY;
wire bEnableX;
wire bEnableY;

assign client_x = hc_i - left;
assign client_y = vc_i - top;

assign bFill = bEnableX & bEnableY & (bGridX | bGridY) ;

goboardnet goboardnetX(client_x, bGridX, bEnableX, clk, clr);
goboardnet goboardnetY(client_y, bGridY, bEnableY, clk, clr);

endmodule

module goboardnet(
    input wire [11:0] client_pos,
    output reg bGrid,
	output reg bEnable,
    input wire clk,
    input wire clr);

parameter range  = 12'd28;

reg [11:0] pos_maj;
reg [11:0] pos_min;

always @(posedge clk or posedge clr)
if(clr) begin
	pos_maj <= 0;
	pos_min <= 0;
end else begin
	pos_maj <= client_pos / range;
	pos_min <= client_pos % range;
end

always @(*) begin
	bGrid <= 0;
	bEnable <= 1;

	case ( pos_maj )
	    0:
			case (pos_min)
				0, 1, 2, 3: bGrid <= 1;
			endcase
		17:
			case (pos_min)
				0, range-3, range-2, range-1: bGrid <= 1;
			endcase
		18:
			case (pos_min)
				0: bGrid <= 1;
				default: bEnable <= 0;
			endcase
		2, 8, 14:
			case (pos_min)
				0, range-1: bGrid <= 1;
			endcase
		3, 9, 15:
			case (pos_min)
				0, 1: bGrid <= 1;
			endcase
		1, 4, 5, 6, 7, 10, 11, 12, 13, 16:
			case (pos_min)
				0: bGrid <= 1;
			endcase
		default: bEnable <= 0;
	endcase
end

endmodule

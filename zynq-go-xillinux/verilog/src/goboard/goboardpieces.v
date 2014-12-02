module goboardpieces(
    input wire [11:0] hc_i,
    input wire [11:0] vc_i,
    output reg bChar,
	output reg bTransparence,
    output reg bSel,
	input wire [4:0] addr_i,
	output wire [4:0] addr_o,
	input wire wEnable,
	input wire [37:0] pieces_i,
	output wire [37:0] pieces_o,
	input wire [4:0] sel_x,
	input wire [4:0] sel_y,
    input wire vga_clk_in,
    input wire arst_i,
	input wire sys_clk_in
    );

parameter top       = 12'd24;
parameter left      = 12'd24;
parameter range     = 12'd28;
parameter radius    = 12'd12;
parameter border    = 12'd3;

wire [11:0] client_x_next;
wire [11:0] client_y_next;
wire [11:0] pos_maj_x_next;
wire [11:0] pos_min_x_next;
wire [11:0] pos_maj_y_next;
wire [11:0] pos_min_y_next;

reg [11:0] client_x;
reg [11:0] client_y;
reg [11:0] pos_maj_x;
reg [11:0] pos_min_x;
reg [11:0] pos_maj_y;
reg [11:0] pos_min_y;


assign client_x_next  = hc_i - (left - range / 12'd2);
assign client_y_next  = vc_i - (top - range / 12'd2);
assign pos_maj_x_next = client_x_next / range;
assign pos_min_x_next = client_x_next % range - range / 12'd2;
assign pos_maj_y_next = client_y_next / range;
assign pos_min_y_next = client_y_next % range - range / 12'd2;

assign addr_o = client_y_next[4:0];

piecesmem piecesmem0 (
  .clka(sys_clk_in), // input clka
  .wea(wEnable), // input [0 : 0] wea
  .addra(addr_i), // input [4 : 0] addra
  .dina(pieces_i), // input [37 : 0] dina
  .clkb(vga_clk_in), // input clkb
  .addrb(pos_maj_y_next[4:0]), // input [4 : 0] addrb
  .doutb(pieces_o) // output [37 : 0] doutb
);

always @(posedge vga_clk_in or posedge arst_i)
if(arst_i) begin
	client_x  <= 0;
	client_y  <= 0;
	pos_maj_x <= 0;
	pos_min_x <= 0;
	pos_maj_y <= 0;
	pos_min_y <= 0;
end else begin
	client_x  <= client_x_next;
	client_y  <= client_y_next;
	pos_maj_x <= pos_maj_x_next;
	pos_min_x <= pos_min_x_next;
	pos_maj_y <= pos_maj_y_next;
	pos_min_y <= pos_min_y_next;
end

always @(*) begin
	bChar <= 0;
	bSel <= 0;
	bTransparence <= 1;
	if(client_x >= 0 && client_y >= 0 && pos_maj_x < 19 && pos_maj_y < 19) begin
		if( pieces_o[{pos_maj_x[4:0],1'b0}] ) begin
			if( pos_min_x * pos_min_x + pos_min_y * pos_min_y <=  radius ** 2) begin
				bTransparence <= 0;
				bChar <= 1;
			end
		end else if( pieces_o[{pos_maj_x[4:0],1'b1}] ) begin
			if( pos_min_x * pos_min_x + pos_min_y * pos_min_y <=  radius ** 2 ) begin
				bTransparence <= 0;
				if( pos_min_x * pos_min_x + pos_min_y * pos_min_y >  (radius - border) ** 2 )
					bChar <= 1;
			end
		end else if( pos_maj_x == sel_x && pos_maj_y == sel_y)
			if( pos_min_x * pos_min_x + pos_min_y * pos_min_y <=  radius ** 2 ) begin
				bTransparence <= 0;
				bSel <= 1;
				if(pos_min_x[0] ^ pos_min_y[0]) bChar <= 1;
			end

	end
end

endmodule

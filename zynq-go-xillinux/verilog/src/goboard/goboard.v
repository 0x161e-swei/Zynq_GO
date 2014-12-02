
module goboard(
  vgaRed, vgaGreen, vgaBlue, Hsync, Vsync,

  vga_clk_in, sys_clk_in,

  arst_i, rst_i,

	ps2_0_clk, ps2_0_dat,

	RegVal,

  pieces_sel_x, pieces_sel_y, pieces_sel_set,
  
  wea, ascii, write_ram_addr,
  addr_i, pieces_i, wEnable

);

  output reg vgaRed;
  output reg vgaGreen;
  output reg vgaBlue;
  output reg Hsync;
  output reg Vsync;
  input  wire vga_clk_in;
  input  wire sys_clk_in;
  input  wire rst_i;
  input  wire arst_i;

  input wire ps2_0_clk, ps2_0_dat;
  output wire [15:0] RegVal;

  output wire [4:0] pieces_sel_x;
  output wire [4:0] pieces_sel_y;
  output wire pieces_sel_set;
  input wire [4:0] addr_i;
  input wire [37:0] pieces_i;
  input wire wEnable;
  
  input wire wea;
  input wire [7:0]ascii;
  input wire [8:0]write_ram_addr;

  wire [11:0] hc;
  wire [11:0] vc;
  wire vidon_i;
  wire hsync;
  wire vsync;

  vgabase vgabase0
    (
      .clk(vga_clk_in),
      .clr(arst_i),
      .hsync(hsync),
      .vsync(vsync),
      .hc(hc),
      .vc(vc),
      .vidon(vidon_i)
    );

  wire bFillGrid;

  goboardgrid goboardgrid0
    (
      .bFill(bFillGrid),
      .hc_i(hc),
      .vc_i(vc),
      .clk(vga_clk_in),
      .clr(arst_i)
    );

  wire bFillChar;

  goboardchar goboardchar0
	(
      .bChar(bFillChar),
      .hc_i(hc),
      .vc_i(vc),
      .clk(vga_clk_in),
      .clr(arst_i)
	);

  wire bFillPieces;
  wire bTransparencePieces;
  wire [4:0] addr_o;
  wire [37:0] pieces_o;
  wire bSel;

  wire [4:0] sel_x;
  wire [4:0] sel_y;
  assign sel_x = pieces_sel_x;
  assign sel_y = 5'd19 - pieces_sel_y;

  goboardpieces goboardpieces0
	(
    .bChar(bFillPieces),
	  .bTransparence(bTransparencePieces),
    .hc_i(hc),
    .vc_i(vc),
	  .addr_i(addr_i),
	  .pieces_i(pieces_i),
	  .wEnable(wEnable),
	  .sel_x(sel_x),
	  .sel_y(sel_y),
	  .bSel(bSel),
	  .sys_clk_in(sys_clk_in),
	  .vga_clk_in(vga_clk_in),
    .arst_i(arst_i),
	  .addr_o(addr_o),
	  .pieces_o(pieces_o)
	);

  reg vidon;
  reg hsync0;
  reg vsync0;

  always @(posedge vga_clk_in or posedge arst_i) begin
	if(arst_i) begin
		hsync0 <= 1'b0;
		vsync0 <= 1'b0;
		vidon <= 0;
	end else begin
		hsync0 <= hsync;
		vsync0 <= vsync;
		vidon <= vidon_i;
	end
  end

  wire bFillAll;
  wire bColorSel;

  assign bFillAll = bTransparencePieces & ( bFillGrid | bFillChar ) | bFillPieces;
  assign bColorSel = bSel & bFillPieces;

//  goboardinput(
//  	.clk(sys_clk_in),
//  	.clr(rst_i),
//  	.ps2_0_clk(ps2_0_clk),
//  	.ps2_0_dat(ps2_0_dat),
//  	.RegVal(RegVal),
//  	.bSet(pieces_sel_set),
//  	.pieces_sel_x(pieces_sel_x),
//  	.pieces_sel_y(pieces_sel_y)
//  );
  
  wire bInfo;
//  reg wea;
//  reg [7:0]ascii;
//  reg [8:0]write_ram_addr;
  
//  //reg [3:0]timer;
//  reg [20:0]timer;
//  always @(posedge sys_clk_in) begin
//      timer <= timer + 1;
//  end
      
//  //generate some test chars for goboardinfo
  
//  always @(posedge sys_clk_in) begin
//      //If timer equals to zero, then write a char to the info ram.
//      if(timer == 0) begin
//          //generate write ram addr
//          if( write_ram_addr == 9'h1ff)
//              write_ram_addr <= 0;
//          else
//              write_ram_addr <= write_ram_addr + 1;
          
//          //generate ascii data
//          if( ascii == 8'h7f )
//              ascii <= 8'h20;
//          else                
//              ascii <= ascii + 1;
              
//          wea <= 1;
//      end
//      else
//          wea <= 0;
//  end
  
  	goboard_getinfo goboard_getinfo0(
      .bInfo(bInfo),
      .vga_clk_in(vga_clk_in),
      .sys_clk_in(sys_clk_in),
      .clr(0),
      .wea(wea),
      .hc_i(hc),
      .vc_i(vc),
      .write_ram_addr(write_ram_addr),
      .in_char(ascii)
      );
  

  always @(*) begin /*
    if(arst_i) begin
      Hsync     <= 1'b0;
      Vsync     <= 1'b0;
      vgaRed    <= 0;
      vgaGreen  <= 0;
      vgaBlue   <= 0;
    end else begin */
      Hsync <= hsync0;
      Vsync <= vsync0;
      if(vidon) begin
        if( hc >= 600 && hc <= 600 + 127 && vc <= 599 - 16*4 && vc >= 8+16) begin
            vgaRed    <= ~ bInfo;
            vgaGreen  <= ~ bInfo;
            vgaBlue   <= ~ bInfo;
        end
        else begin
            vgaRed    <= ~ bFillAll;// | bColorSel;
            vgaGreen  <= ~ bFillAll;
            vgaBlue   <= ~ bFillAll;
        end
      end else begin
        vgaRed    <= 0;
        vgaGreen  <= 0;
        vgaBlue   <= 0;
      end
    end

endmodule // goboard


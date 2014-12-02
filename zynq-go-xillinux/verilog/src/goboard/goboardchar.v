
module goboardchar (
    input wire [11:0] hc_i,
    input wire [11:0] vc_i,
    output wire bChar,
    input wire clk,
    input wire clr
    );

    wire bCharX;

    goboardchar_x goboardchar_x0(
        .hc_i   (hc_i),
        .vc_i   (vc_i),
        .bChar  (bCharX),
        .clk    (clk),
        .clr    (clr)
        );

    wire bCharY;

    goboardchar_y goboardchar_y0(
        .hc_i   (hc_i),
        .vc_i   (vc_i),
        .bChar  (bCharY),
        .clk    (clk),
        .clr    (clr)
        );

    assign bChar = bCharX | bCharY;
endmodule

module goboardchar_x (
    input wire [11:0] hc_i,
    input wire [11:0] vc_i,
    output reg bChar,
    input wire clk,
    input wire clr
    );

`include "char_rom_x.vh"

parameter top   = 12'd550;
parameter left  = 12'd20;
parameter seperate  = 12'd20;

reg [11:0] client_x;
reg [11:0] client_y;

reg [11:0] pos_maj_x;
reg [11:0] pos_min_x;
reg [8:0] rom_addr;

always @(posedge clk or posedge clr) begin
    if(clr) begin
        client_x = 0;
        client_y = 0;
        pos_maj_x = 0;
        pos_min_x = 0;
        rom_addr = 0;
    end else begin
        client_x = hc_i - left;
        client_y = vc_i - top;
        pos_maj_x = client_x / (8 + seperate);
        pos_min_x = client_x % (8 + seperate);
        rom_addr[8:0] = {pos_maj_x[4:0], client_y[3:0]};
    end
end

always @(*) begin
    bChar <= 0;
    if(client_x>=0 && client_y>=0 && pos_maj_x<19 && pos_min_x<8 && client_y<16)
        bChar <= char_rom_x[rom_addr][pos_min_x[2:0]];
end

endmodule

module goboardchar_y (
    input wire [11:0] hc_i,
    input wire [11:0] vc_i,
    output reg bChar,
    input wire clk,
    input wire clr
    );

`include "char_rom_y.vh"

parameter top   = 12'd18;
parameter left  = 12'd550;
parameter seperate  = 12'd12;

reg [11:0] client_x;
reg [11:0] client_y;

reg [11:0] pos_maj_y;
reg [11:0] pos_min_y;
reg [7:0] rom_addr;


always @(posedge clk or posedge clr) begin
    if(clr) begin
        client_x = 0;
        client_y = 0;
        pos_maj_y = 0;
        pos_min_y = 0;
        rom_addr = 0;
    end else begin
        client_x = hc_i - left;
        client_y = vc_i - top;
        pos_maj_y = 12'd18 - client_y / (12'd16 + seperate);
        pos_min_y = client_y % (12'd16 + seperate);
        rom_addr[7:0] = {pos_maj_y[4:0] % 4'd10, pos_min_y[3:0]};
    end
end

always @(*) begin
    bChar <= 0;
    if(client_x>=0 && client_y>=0 && client_x<16 && pos_maj_y<19 && pos_min_y<16)
        if(client_x >= 8)
            bChar <= char_rom_y[rom_addr][client_x[3:0] - 8];
        else if(pos_maj_y >= 9)
            bChar <= char_rom_y[pos_min_y[3:0]][client_x[2:0]];
end

endmodule

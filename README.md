Zynq GO
====

Zynq Go runs on three ZYBO boards, with the open source gnugo, running the game of Go under both linux and fpga logic.
The game of go inside the Zynq go is an online game with two clients acting as players and the server acting as a judge.
It provides you with many ways of learning the go or playing go with your friends.

When playing online go, first thing to do is to link the client to server with server address and your username.
After that, you are starting your free trip to explore the go world with our project Zynq go.
By the way, you can quit whenever since you start a game.

Many operations were provided, for instance, you can either set a piece at any position available within the board, or simply pass.
Another way of setting a piece is to turn to server for help, asking the “judge” to come up with a proper move for you.
Actually, the “judge” turns to gnugo for help. And gnugo is a go-intelligence of level 0 to 10, with Monte Carlo method.

The result of your move and the whole game frame would be displayed on a vga output via the server through fpga logic, while some debug message remaining local.


Usage Guide
====
+ Install dependancies: `g++` `make` `gnugo`
+ Hit `make`
+ edit `/etc/hosts` file, add IP address for `go.m` `go.s1` `go.s2`, actually only `go.m` matters.
+ Execute corresponding scripts on server or client side. `client_demo` will always ask the server for suggestions of next step and perform exactly the same step. `client` will recieve input from command line, just from where the script is executed. Attaching a display to HDMI port will provide a CLI. SSH or SERIAL terminal will do, too.

Display Guide(optional)
====
+ Use Vivado 2014.1 to build Xillibus evaluation v1.3 (Choose Verilog) for Zybo. *This tutorial is for evaluation only, please refer to Xillybus Ltd. for licensing details.*
+ Install and configure`Xillinux` for Zybo.
+ *Change MAC address of Zybo board and write it to SPI*, make sure this info will not lost after power loss. Configure IP adresses for each board.
+ Some modifications and expansions has been made to the Vivado project. These files are in the `zynq-go-xillinux` dir. Replace corresponding files in evaluation kit. (If there's something missing, please feel free to contact me.)
+ Add IP cores, reference files provided in `zynq_go_xilinux/verilog/src/ref` dir, directly open may not work, add these IPs manually through wizard.
 1. `blk_mem_piecesmem`data [37 : 0], addr [4:0], always enabled
 2. `clk_wiz_0` CLK_OUT1 40M, CLK_OUT1 50M
 3. `goboard_info_ram` data [7 : 0], addr [8 : 0], always enabled
+ Build the project in Vivado again. Get the bit file. Copy the bit file to the FAT partition of the microSD card, overwrite the existing one.
+ Edit `busOperation.cpp` and change the macro to `#define XILLY_BUS`, that is, remove the letter `n`.
+ Follow the steps in previous section.
+ Attach a monitor to VGA port, a borad will show up.


Known Issues
====

+ Rarely, server/client fails to open socket connection.
+ Some certain combination of server and client, such as x64 server with ARM clients may not work, they often stuck right after opening the socket.
+ Network traffic is not serialized.
+ A certain portion of the code is ugly, though it's working.
+ Serveral types and inaccurate English expressions

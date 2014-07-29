Zynq go description:  


    The Project Zynq go is based on three Zybo boards, with the open source gnugo, running the game of go under both linux and fpga logic.
	The game of go inside the Zynq go is an online game with two clients acting as players and the server acting as a judge. It provides you with many ways of learning the go or playing go with your friends. 
    When playing online go, first thing to do is to link the client to server with server address and your username. After that, you are starting your free trip to explore the go world with our project Zynq go. By the way, you can quit whenever since you start a game. 
    
    Many operations were provided, for instance, you can either set a piece at any position available within the board, or simply pass. Another way of setting a piece is to turn to server for help, asking the “judge” to come up with a proper move for you. Actually, the “judge” turns to gnugo for help. And gnugo is a go-intelligence of level 0 to 10, with Monte Carlo method. 
    
    The result of your move and the whole game frame would be displayed on a vga output via the server through fpga logic, while some debug message remaining local.

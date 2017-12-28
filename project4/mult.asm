// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.

//changes the values of R0 and R1 as side effect
//could use extra registers to avoid changing R0,R1

//set R2 to 0
@R2
M=0

//if R1>0 jump to loop 
@R1
D=M
@LOOP
D; JGE

//if R1<0 switch signs of R0 and R1
@R0
M=-M
@R1
M=-M

//add R0 to sum whilst R1>0
(LOOP)
	//jump to end if R1=0
	@R1
	D=M
	@END
	D;JEQ

	//decrement R1
	@R1
	M=M-1

	//add R0 to R2
	@R0
	D=M
	@R2
	M=D+M

	//return to start of loop
	@LOOP
	0;JEQ

(END)
	//infinite loop
	@END
	0;JEQ
	

	
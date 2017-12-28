// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

//fill byte is 0 for 16 white pixels
//fill byte is -1 for 16 black pixels

//set default fill byte
@fillByte
M=0

(LISTEN)	
	//jump to NOPRESS if no key pressed
	@KBD
	D=M
	@NOPRESS
	D;JEQ

	//If pressed and filled jump to LISTEN
	@fillByte
	D=M
	@LISTEN
	D;JNE

	//set fillByte to -1
	@fillByte
	M=-1
		
(FILL)
	//reset screen pointer
	@8192
	D=A-1;
	@SCREEN
	D=A+D
	@screenPtr
	M=D

	(LOOP)
		//fill next 16 pixels
		@fillByte
		D=M
		@screenPtr
		A=M
		M=D

		//decrement screenPtr
		@screenPtr
		M=M-1

		//if screenPtr!=SCREEN jump to LOOP
		D=M
		@screen
		D=D-A
		@LOOP
		D;JNE

	//return to LISTEN
	@LISTEN
	0;JEQ


(NOPRESS) 
		//If not pressed and not filled jump to LISTEN
		@fillByte
		D=M
		@LISTEN
		D;JEQ

		//If not pressed and filled then unfill
		@fillByte
		M=0;
		@FILL
		0;JEQ

		
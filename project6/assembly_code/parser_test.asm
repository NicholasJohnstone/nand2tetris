//parser test file
	//test comment


//A commands
@2				//0: numeric A command		
 	@2			//1: numeric A command with space padding	
@var				//2: symbolic A command
 @var 				//3: symbolic A command with space padding

//label commands
(LABEL)				//label command
	  (LABEL2)		//label with space padding

//C commands
D				//4: comp only D command
  D  				//5: comp only D command with space padding
D=D+A				//6: dest and comp D command
D	=	D+A		//7: dest and comp D command with space padding
D+A;JGT				//8: comp and jump D command
 D+A ; JGT 			//9: comp and jump D command with space padding
D=D+A;JGT			//10: full D command
 D =	D+A;   JGT		//11: full D command with space padding




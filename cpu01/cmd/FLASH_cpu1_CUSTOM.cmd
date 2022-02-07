MEMORY
{
PAGE 0:
	//-- BOOT ------------------------------------------------------------------------------------------------
	BEGIN       : origin = 0x080000, 	length = 0x000002
	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 only ---------------------------------------------------------------------------------------
   	M0			: origin = 0x000123,   	length = 0x0002DD

   	D0         	: origin = 0x00B000,   	length = 0x000800
//   	D1          : origin = 0x00B800,   	length = 0x000800
   	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 / CLA --------------------------------------------------------------------------------------

// 	LS0        	: origin = 0x008000,   	length = 0x000800
   	LS1        	: origin = 0x008800,   	length = 0x000800

   	LS4_5       : origin = 0x00A000,   	length = 0x001000
   	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 / CPU2 -------------------------------------------------------------------------------------
// 	GS0	        : origin = 0x00C000,   	length = 0x001000
// 	GS1	        : origin = 0x00D000,   	length = 0x001000
// 	GS2	        : origin = 0x00E000,   	length = 0x001000
// 	GS3	        : origin = 0x00F000,   	length = 0x001000
	GS4_7       : origin = 0x010000,   	length = 0x004000
// 	GS4	        : origin = 0x00F000,   	length = 0x001000
// 	GS5	        : origin = 0x00F000,   	length = 0x001000
// 	GS6	        : origin = 0x00F000,   	length = 0x001000
// 	GS7	        : origin = 0x00F000,   	length = 0x001000
// 	GS8	        : origin = 0x00F000,   	length = 0x001000
// 	GS9	        : origin = 0x00F000,   	length = 0x001000
// 	GS10        : origin = 0x00F000,   	length = 0x001000
// 	GS11        : origin = 0x00F000,   	length = 0x001000
// 	GS12        : origin = 0x00F000,   	length = 0x001000
// 	GS13        : origin = 0x00F000,   	length = 0x001000
// 	GS14        : origin = 0x00F000,   	length = 0x001000
// 	GS15        : origin = 0x00F000,   	length = 0x001000

// 	CPU1toCPU2 	: origin = 0x03F800,	length = 0x000400	// msg
// 	CPU2toCPU1 	: origin = 0x03FC00,	length = 0x000400	// msg
	//--------------------------------------------------------------------------------------------------------

	//-- FLASH -----------------------------------------------------------------------------------------------
//	FA          : origin = 0x080002,  	length = 0x001FFE
	FB          : origin = 0x082000,   	length = 0x002000
// 	FC          : origin = 0x084000,   	length = 0x002000
// 	FD          : origin = 0x086000,   	length = 0x002000
 	FD_E        : origin = 0x086000,   	length = 0x004000
// 	FE          : origin = 0x088000,   	length = 0x008000
// 	FF          : origin = 0x090000,   	length = 0x008000
 	FG          : origin = 0x098000,   	length = 0x008000
// 	FH          : origin = 0x0A0000,   	length = 0x008000
// 	FI          : origin = 0x0A8000,   	length = 0x008000
// 	FJ          : origin = 0x0B0000,   	length = 0x008000
// 	FK          : origin = 0x0B8000,   	length = 0x002000
// 	FL          : origin = 0x0BA000,   	length = 0x002000
// 	FM          : origin = 0x0BC000,   	length = 0x002000
// 	FN          : origin = 0x0BE000,   	length = 0x001FF0
	//--------------------------------------------------------------------------------------------------------
	RESET       : origin = 0x3FFFC0,   	length = 0x000002

PAGE 1:
	BOOT_RSVD   : origin = 0x000002,	length = 0x000121	// part of M0, BOOT rom will use this for stack
	M1         	: origin = 0x000400, 	length = 0x0003F8
	LS2    		: origin = 0x009000,   	length = 0x000800
   	LS3    		: origin = 0x009800,   	length = 0x000800
   	CLAtoCPU  	: origin = 0x001480,   	length = 0x000080	// msg
   	CPUtoCLA  	: origin = 0x001500,   	length = 0x000080   // msg

}

SECTIONS
{
	//-- Initialized -----------------------------------------------------------------------------------------
   	codestart        : >  BEGIN,	PAGE = 0, 	ALIGN(8)
   	.reset           : >  RESET 	PAGE = 0,	TYPE = DSECT	// not used
   	.cinit           : >  FB,		PAGE = 0, 	ALIGN(8)		// expected to be in FLASH/ROM
   	.econst          : >  FB, 		PAGE = 0,	ALIGN(8)		// expected to be in FLASH/ROM
   	.switch          : >  FB, 		PAGE = 0,	ALIGN(8)		// depends on the --unified_memory option setting
   	.text            : >  FD_E,		PAGE = 0,	ALIGN(8) 		// expected to be in FLASH/ROM
    .pinit           : >  FB, 		PAGE = 0,	ALIGN(8)		// expected to be in FLASH/ROM
   	//--------------------------------------------------------------------------------------------------------

   	//-- Uninitialized ---------------------------------------------------------------------------------------
   	.ebss            : >  LS2,		PAGE = 1		// must be in RAM
   	.stack           : >  M1,		PAGE = 1  		// must be in low 64K RAM (stack pointer is a 16 bit register)
   	.esysmem         : >  LS3,		PAGE = 1  		// must be in RAM
   	//--------------------------------------------------------------------------------------------------------


    //-- CLA specific sections -------------------------------------------------------------------------------
   	Cla1ToCpuMsgRAM : >  CLAtoCPU,	PAGE = 1  		// must be in RAM within addresses 0x001480 through 0x0014FF
   	CpuToCla1MsgRAM : >  CPUtoCLA,	PAGE = 1  		// must be in RAM within addresses 0x001500 through 0x00157F


 	.scratchpad     : >  LS1,		PAGE = 0  		// must be in RAM within addresses 0x008000 through 0x00AFFF


    Cla1Prog    	: LOAD 	= FG,
                      RUN 	= LS4_5,	// must be in RAM within addresses 0x008000 through 0x00AFFF
                      LOAD_START(_Cla1funcsLoadStart),
                      LOAD_END(_Cla1funcsLoadEnd),
                      RUN_START(_Cla1funcsRunStart),
                      LOAD_SIZE(_Cla1funcsLoadSize),
                      PAGE = 0


   	//--------------------------------------------------------------------------------------------------------

	.TI.ramfunc : {} LOAD 	= FG,
					 RUN 	= GS4_7,
        	         LOAD_START(_RamfuncsLoadStart),
         	         LOAD_SIZE(_RamfuncsLoadSize),
         	         LOAD_END(_RamfuncsLoadEnd),
          	         RUN_START(_RamfuncsRunStart),
          	         RUN_SIZE(_RamfuncsRunSize),
           	         RUN_END(_RamfuncsRunEnd),
					 PAGE = 0,
                     ALIGN(8)

}


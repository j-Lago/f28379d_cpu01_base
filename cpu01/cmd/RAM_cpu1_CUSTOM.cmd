MEMORY
{
	//-- BOOT ------------------------------------------------------------------------------------------------
	BEGIN       : origin = 0x000000,   	length = 0x000002	// BEGIN is used for the "boot to SARAM" bootloader mode
   	BOOT_RSVD   : origin = 0x000002,	length = 0x000121	// part of M0, BOOT rom will use this for stack
	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 only ---------------------------------------------------------------------------------------
   	M0			: origin = 0x000123,   	length = 0x0002DD
   	M1         	: origin = 0x000400, 	length = 0x0003F8
   	D0         	: origin = 0x00B000,   	length = 0x000800
   	D1          : origin = 0x00B800,   	length = 0x000800
   	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 / CLA --------------------------------------------------------------------------------------
   	CLAtoCPU  	: origin = 0x001480,   	length = 0x000080	// msg
   	CPUtoCLA  	: origin = 0x001500,   	length = 0x000080   // msg
 	LS0_1      	: origin = 0x008000,   	length = 0x001000
//	LS0        	: origin = 0x008000,   	length = 0x000800
//	LS1        	: origin = 0x008800,   	length = 0x000800
   	LS2    		: origin = 0x009000,   	length = 0x000800
   	LS3    		: origin = 0x009800,   	length = 0x000800
   	LS4_5       : origin = 0x00A000,   	length = 0x001000
   	//--------------------------------------------------------------------------------------------------------

	//-- RAM CPU1 / CPU2 -------------------------------------------------------------------------------------
// 	GS0	        : origin = 0x00C000,   	length = 0x001000
// 	GS1	        : origin = 0x00D000,   	length = 0x001000
// 	GS2	        : origin = 0x00E000,   	length = 0x001000
// 	GS3	        : origin = 0x00F000,   	length = 0x001000
// 	GS4	        : origin = 0x00F000,   	length = 0x001000
// 	GS5	        : origin = 0x00F000,   	length = 0x001000
// 	GS6	        : origin = 0x00F000,   	length = 0x001000
// 	GS7	        : origin = 0x00F000,   	length = 0x001000
//	GS8	        : origin = 0x00F000,   	length = 0x001000
// 	GS9	        : origin = 0x00F000,   	length = 0x001000
// 	GS10        : origin = 0x00F000,   	length = 0x001000
// 	GS11        : origin = 0x00F000,   	length = 0x001000
// 	GS12        : origin = 0x00F000,   	length = 0x001000
// 	GS13        : origin = 0x00F000,   	length = 0x001000
// 	GS14        : origin = 0x00F000,   	length = 0x001000
// 	GS15        : origin = 0x00F000,   	length = 0x001000
	GS4_7       : origin = 0x010000,   	length = 0x004000
// 	CPU1toCPU2 	: origin = 0x03F800,	length = 0x000400	// msg
// 	CPU2toCPU1 	: origin = 0x03FC00,	length = 0x000400	// msg
	//--------------------------------------------------------------------------------------------------------

	//-- FLASH -----------------------------------------------------------------------------------------------
//	FA          : origin = 0x080002,  	length = 0x001FFE
//  fB          : origin = 0x082000,   	length = 0x002000
// 	FC          : origin = 0x084000,   	length = 0x002000
// 	FD          : origin = 0x086000,   	length = 0x002000
// 	FE          : origin = 0x088000,   	length = 0x008000
// 	FF          : origin = 0x090000,   	length = 0x008000
// 	FG          : origin = 0x098000,   	length = 0x008000
// 	FH          : origin = 0x0A0000,   	length = 0x008000
// 	FI          : origin = 0x0A8000,   	length = 0x008000
// 	FJ          : origin = 0x0B0000,   	length = 0x008000
// 	FK          : origin = 0x0B8000,   	length = 0x002000
// 	FL          : origin = 0x0BA000,   	length = 0x002000
// 	FM          : origin = 0x0BC000,   	length = 0x002000
// 	FN          : origin = 0x0BE000,   	length = 0x001FF0
	//--------------------------------------------------------------------------------------------------------

   	RESET       : origin = 0x3FFFC0,   	length = 0x000002
}

SECTIONS
{
	//-- Initialized -----------------------------------------------------------------------------------------
   	codestart        : >  BEGIN
   	.reset           : >  RESET TYPE = DSECT	// not used
   	.cinit           : >  M0					// expected to be in FLASH/ROM
   	.econst          : >  LS2					// expected to be in FLASH/ROM
   	.switch          : >  M0					// depends on the --unified_memory option setting
   	.text            : >> D0 |
                          D1 |
                          GS4_7					// expected to be in FLASH/ROM
    .pinit           : >  M0					// expected to be in FLASH/ROM
   	//--------------------------------------------------------------------------------------------------------

   	//-- Uninitialized ---------------------------------------------------------------------------------------
   	.ebss            : >  LS0_1		// must be in RAM
   	.stack           : >  M1  		// must be in low 64K RAM (stack pointer is a 16 bit register)
   	.esysmem         : >  LS3		// must be in RAM
   	//--------------------------------------------------------------------------------------------------------


    //-- CLA specific sections -------------------------------------------------------------------------------
// 	.bss_cla		: >  LS1		// must be in RAM within addresses 0x008000 through 0x00AFFF
//  .const_cla	    : >  LS1		// must be in RAM within addresses 0x008000 through 0x00AFFF
    .scratchpad     : >  LS0_1 		// must be in RAM within addresses 0x008000 through 0x00AFFF
   	Cla1Prog        : >  LS4_5		// must be in RAM within addresses 0x008000 through 0x00AFFF
   	Cla1ToCpuMsgRAM : >  CLAtoCPU   // must be in RAM within addresses 0x001480 through 0x0014FF
   	CpuToCla1MsgRAM : >  CPUtoCLA	// must be in RAM within addresses 0x001500 through 0x00157F
   	//--------------------------------------------------------------------------------------------------------

	.TI.ramfunc 	: {} >  M0		// must be in RAM
//	.TI.ramfunc 	: {} load=FLASHx, run=M0, table(BINIT)

}


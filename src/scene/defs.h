#ifndef _NTR_PARSEDEF_H_
#define _NTR_PARSEDEF_H_ 1

/***********
*  MACROS  *
***********/

// Maximum length of a string
#define MAX_STR 	128
// Size of cached output
#define CACHE_SZ	256
// Default length of Scene.instructions field,
// automatically resized, when needed
#define DEF_FD_LEN	64
// Maximum length of Scene.instructions field
// parseFile will fail if the file is longer
#define MAX_FD_LEN	256
// Maximum number of primitive instructions inside
// primitive list
#define MAX_INTR_DEPTH	16

/*********************
*  TYPE DEFINITIONS  *
*********************/

struct Scene {
	// Dynamically allocated list of instruction bytes
	unsigned char *instructions;
	// Length of instructions
	unsigned size;
	// Cached output, allocated by fdInterpr
	char *cache;
};

enum ParseType {
	PT_NONE		= 0x00,
	// Begining of data
	PT_START	= 0xFB,
	// End of data
	PT_END		= 0xFE,

	PT_INSTR_B	= 0x17,
	PT_INSTR_E	= 0xFF,
};

/* Types of instructions
 *	all instructions must start with a PT_INSTR_B(0x17) and end with a PT_INSTR_E(0xFF)
 *	there are 2 types of instructions:
 *	- PRIMITIVE instructions: 0 <= primitive < PI_GENERIC
 *	   * they do NOT take arguments
 *	   * they are ranging from 0x00 to PI_GENERIC(0x15)
 *	   * multiple primitives can be written as a list of primitives
 *		ex: "17 05 06 08 FF", which does NOT mean PI_BOLD(0x05) with arguments 0x06 and 0x08,
 *					but rather, it means: PI_BOLD(05), PI_ITALIC(06), PI_UNDERLN(08)
 *		this is done to reduce the bytes used
 *	- COMPLEX   instructions: PI_GENERIC <= complexs < PI_MAX
 *	   * they take arguments
 *	   * the first complex instruction is PI_GENERIC and the last is the one before PI_MAX
 * further documentation below
 */
enum ParseInstruction {
	/***************************
	*  PRIMITIVE INSTRUCTIONS  *
	***************************/

/* 0x17	0x00						0xFF */
	PI_NEWLN,
/* 0x17	0x01						0xFF */
	PI_RESET,

	/*************
	*  Blinking  *
	*************/

/* 0x17	0x02						0xFF */
	PI_BLNK_SLOW,
/* 0x17	0x03						0xFF */
	/* WARNING: Not not widely supported: */
	PI_BLNK_RAPD,
/* 0x17	0x04						0xFF */
	PI_BLNK_STOP,

	/**********************
	*  Other text styles  *
	**********************/

/* 0x17	0x5						0xFF */
	PI_BOLD,

/* 0x17	0x06						0xFF */
	/* WARNING: Not widely supported. Sometimes treated as inverse or blink */
	PI_ITALIC,
/* 0x17	0x7						0xFF */
	PI_ITALIC_END,

/* 0x17	0x08						0xFF */
	PI_UNDERLN,
/* 0x17	0x09						0xFF */
	// Double underline
	/* WARNING: if ECMA-48 not supported, then it will disable the bold intensity */
	PI_DUNDERLN,
/* 0x17	0x0A						0xFF */
	// Disables both normal and double interline
	PI_UNDERLN_END,

/* 0x17	0x0B						0xFF */
	PI_FAINT,

/* 0x17	0x0C						0xFF */
	PI_STRIKE,
/* 0x17	0x0D						0xFF */
	PI_STRIKE_END,

/* 0x17	0x0E						0xFF */
	// Neither bold nor faint; color changes where intensity is implemented as such
	PI_NORM_INTENS,

/* 0x17	0x0F						0xFF */
	PI_FRAMED,
/* 0x17	0x10						0xFF */
	PI_CIRCLED,
/* 0x17	0x11						0xFF */
	PI_NO_FRAME,

/* 0x17	0x12						0xFF */
	PI_OVERLN,
/* 0x17	0x13						0xFF */
	PI_OVERLN_END,

	/*************************
	*  COMPLEX INSTRUCTIONS  *
	*************************/

/* 0x17	0x14				<text_bytes>	0xFF */
	PI_GENERIC = 0x14,

	/*****************
	*  8 bit colors  *
	*****************/

/* 0x17	0x015	<1B_fg> <1B_bg>		<text_bytes>	0xFF */
	PI_COLOR,
/* 0x17	0x16	<1B_color>				0xFF */
	PI_FG,
/* 0x17	0x17	<1B_color>				0xFF */
	PI_BG,

	/***********************
	*  True(24bit) colors  *
	***********************/

/* 0x17	0x18	fg(<1B_r> <1B_g> <1B_b>) bg(<1B_r> <1B_g> <1B_b>)
 * 					<text_bytes>	0xFF */
	PI_TCOLOR,
/* 0x17 0x19	<1B_r> <1B_g> <1B_b>			0xFF */
	PI_TFG,
/* 0x17 0x1A	<1B_r> <1B_g> <1B_b>			0xFF */
	PI_TBG,

	PI_MAX
};

#endif /* ifndef _NTR_PARSER_H_ */


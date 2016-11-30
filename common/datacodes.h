
/*
 *  AVRcockpit Development System for Racing Simulators
 *
 *  Harald W. Leschner, DK6YF - 2015  H9-Laboratory Ltd. 
 *
 *     hari@h9l.net    or visit    labor.h9l.net
 *
 *
 *  Licensed under the GPL license, see license.txt for details.
 *
 *     All text above must be included in any redistribution.
 *
 */
 
#ifndef _DATACODES_HPP_
#define _DATACODES_HPP_

#define GM_VERSION			"0.5.1"		// GaugeMaster Version number

/* DEFAULTS */
#define TRUE				1
#define FALSE				0


/* DATAGRAM MARKERS */
#define BEGIN_TRANSMISSION	0x02 		// STX
#define END_TRANSMISSION	0x03 		// ETX
#define RECORD_SEPARATOR	0x1e 		// RS


/* DATA */
#define TELEMETRY			0x0001		// not used


/* TYPES */
#define BOOL_T				0x01		// 1 BYTE: TRUE or FALSE
#define CHAR_T				0x02		// 1 BYTE: signed char (-127 .. 127)
#define SHORT_T				0x03		// 2 BYTE: signed short (–32,768 to 32,767)
#define LONG_T				0x04		// 4 BYTE: signed long (–2,147,483,648 to 2,147,483,647)
#define FLOAT_T				0x05		// 4 BYTE: single precision float as IEEE 754 (32 bits)
#define DOUBLE_T			0x06		// 8 BYTE: 64bit (1.7E +/- 308 (15 digits))

#define FOURBYTE_T			0x40 		// 4 BYTE: General purpose


/* WHAT */
/*      Name				Code		   TYPE Simulator 	TYPE I²C-Server */
/* ------------------------------------------------------------------------ */
#define GEAR				0x0001 		// LONG 			CHAR
#define ENGRPM				0x0011		// FLOAT 			FLOAT
#define ENGMAXRPM			0x0012		// FLOAT 			FLOAT
#define ENGWATERTEMP		0x0013		// FLOAT 			FLOAT
#define ENGOILTEMP			0x0014		// FLOAT 			FLOAT
#define ENGOVERHEAT			0x0015		// BOOL 			BOOL
#define FUELTANK			0x0016		// FLOAT 			FLOAT
#define METERSPERSEC		0x0017		// FLOAT 			FLOAT
#define HEADLIGHTS			0x0018		// BOOL 			BOOL
#define HIGHBEAM			0x0019		// BOOL 			BOOL					
#define BLINKER				0x0020 		// FOURBYTE 		frequency[short], mode[char])



#endif //_DATACODES_HPP_
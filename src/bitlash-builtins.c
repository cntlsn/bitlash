/***
	bitlash-builtins.c

	Bitlash is a tiny language interpreter that provides a serial port shell environment
	for bit banging and hardware hacking.

	See the file README for documentation.

	Bitlash lives at: http://bitlash.net
	The author can be reached at: bill@bitlash.net

	Copyright (C) 2008-2011 Bill Roy

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

***/
#include "bitlash.h"

/**********

Built-in Bitlash Scripts

This feature allows you to add built-in scripts at compile time.  

The scripts are stored in flash in the table defined below.  

Add your entries at the end, in pairs: one string ending with \0 for the name,
one string ending with \0 for the script.

NOTE WELL: The table is searched _last_ among all the spaces, so any function defined anywhere
will override the value in the table here.  This means you can easily shoot yourself in the foot, 
like this:

	function high {return 0}
	
... for an endless supply of interesting debugging experiences.  Be careful.

**********/


prog_char builtin_table[] PROGMEM = {

	// The banner must be first.  Add new builtins below.
	"banner\0"	"print \"bitlash here! v2.0 RC4-pre (c) 2011 Bill Roy -type HELP-\",free,\"bytes free\"\0"

	"low\0" 	"return 0\0"
	"high\0" 	"return 1\0"
	"input\0"	"return 0\0"
	"output\0"	"return 1\0"

	"\0\0"			// guards end of table, must be last
};



byte findbuiltin(char *name) {
prog_char *wordlist = builtin_table;

	while (pgm_read_byte(wordlist)) {
		int result = strcmp_P(name, wordlist);
		wordlist += strlen_P(wordlist) + 1;		// skip the name we just tested
		if (!result) {							// got a match:
			sym = s_script_progmem;				// set type to progmem script
			symval = (numvar) wordlist;			// value is starting address of script text
			return 1;
		}
		else wordlist += strlen_P(wordlist) + 1;	// else skip value string and move along
	}
	return 0;
}


//	Banner and copyright notice
//
void displayBanner(void) {
	// print the banner and copyright notice
	// please note the license requires that you maintain this notice
	execscript(SCRIPT_PROGMEM, (numvar) (builtin_table + strlen(builtin_table)+1), 0);
}

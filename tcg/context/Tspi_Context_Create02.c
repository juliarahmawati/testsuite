/*
 *
 *   Copyright (C) International Business Machines  Corp., 2004
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/*
 * NAME
 *	Tspi_Context_Create02.c
 *
 * DESCRIPTION
 *	This test will verify Tspi_Context_Create.
 *	The purpose of this test case is to get TSS_E_BAD_PARAMETER
 *		to be returned. This is easily accomplished by passing
 *		in NULL to Tspi_Context_Create. 
 *
 * ALGORITHM
 *	Setup:	None
 *
 *	Test:	Call Tspi_Context_Create.
 *		Call the Common Errors if not a success
 *		Make sure that it returns the proper return codes
 *	
 *	Cleanup:
 *		Close context
 *		Print error/success message 
 *
 * USAGE:	This test takes no parameters.
 *
 * HISTORY
 *	Author:	Kathy Robertson
 *	Date:	June 2004
 *	Email:	klrobert@us.ibm.com
 *
 * RESTRICTIONS
 *	None.
 */

#include <tss/tss.h>
#include "../common/common.h"

extern int commonErrors(TSS_RESULT result);
int main(int argc, char **argv)
{
	char		*version;

	version = parseArgs( argc, argv );
		// if it is not version 1.1, print error
	if(strcmp(version, "1.1")){
		print_wrongVersion();
	}
	else{
		main_v1_1();
	}
}

int
main_v1_1(void){

	char	*nameOfFunction = "Tspi_Context_Create02";
	
	print_begin_test(nameOfFunction);

	TSS_RESULT result = Tspi_Context_Create(NULL);
	if (result != TSS_E_BAD_PARAMETER) {
		if(!checkNonAPI(result)){
			print_error(nameOfFunction, result);
			print_end_test(nameOfFunction);
			exit(1);
		}
		else{
			print_error_nonapi(nameOfFunction, result);
			print_end_test(nameOfFunction);
			exit(1);
		}
	}
	else{
		print_success(nameOfFunction, result);
		print_end_test(nameOfFunction);
		exit(0);
	}
}

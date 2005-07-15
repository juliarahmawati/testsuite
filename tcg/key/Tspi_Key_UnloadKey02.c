/*
 *
 *   Copyright (C) International Business Machines  Corp., 2004, 2005
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
 *	Tspi_Key_UnloadKey02.c
 *
 * DESCRIPTION
 *	This test will verify Tspi_Key_UnloadKey.
 *
 * ALGORITHM
 *	Setup:
 *		Create Context
 *		Connect Context
 *		Get SRK Handle
 *		Get Key
 *		Load Key
 *
 *	Test:
 *		Call Key_UnloadKey then if it is not a success
 *		Call the Common Errors
 *		Make sure that it returns the proper return codes
 *
 *	Cleanup:
 *		Free memory related to hContext
 *		Close context
 *
 * USAGE
 *      First parameter is --options
 *                         -v or --version
 *      Second parameter is the version of the test case to be run
 *      This test case is currently only implemented for v1.1
 *
 * HISTORY
 *	Megan Schneider, mschnei@us.ibm.com, 6/04.
 *
 * RESTRICTIONS
 *	None.
 */

#include <stdio.h>
#include <trousers/tss.h>
#include "../common/common.h"


int
main( int argc, char **argv )
{
	char		*version;

	version = parseArgs( argc, argv );
		// if it is not version 1.1, print error
	if( strcmp(version, "1.1") )
		print_wrongVersion();
	else
		main_v1_1();
}

int
main_v1_1( void )
{
	char		*function = "Tspi_Key_UnloadKey02";
	TSS_HCONTEXT	hContext;
	TSS_HKEY	hSRK;
        TSS_HKEY        hKey;
	TSS_UUID	SRKUUID			= {0,0,0,0,0,0,0,0,0,0,1};
/*	TSS_UUID	migratableSignUUID	= {1,2,3,4,5,6,7,8,9,10,2};
	TSS_UUID	migratableStorageUUID	= {1,2,3,4,5,6,7,8,9,10,1};
*/	TSS_UUID	uuid;
	BYTE*		migratableSignKeyBlob;
	UINT32		blobLength;
	TSS_RESULT	result;
	UINT32		exitCode;
	TSS_HPOLICY	srkUsagePolicy, keyUsagePolicy, keyMigPolicy;
	TSS_FLAG	initFlags = TSS_KEY_TYPE_SIGNING | TSS_KEY_SIZE_2048  |
				TSS_KEY_VOLATILE | TSS_KEY_NO_AUTHORIZATION |
				TSS_KEY_NOT_MIGRATABLE;

	print_begin_test( function );

		// Create Context
	result = Tspi_Context_Create( &hContext );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_Create", result );
		print_error_exit( function, err_string(result) );
		exit( result );
	}


		//Load Key Blob
	result = Tspi_Key_UnloadKey( 0xffffffff );
	if ( TSS_ERROR_CODE(result) != TSS_E_INVALID_HANDLE )
	{
		if( !(checkNonAPI(result)) )
		{
			print_error( function, result );
			exitCode = 1;
		}
		else
		{
			print_error_nonapi( function, result );
			exitCode = 1;
		}
	}
	else
	{
		print_success( function, result );
		exitCode = 0;
	}

	print_end_test( function );
	Tspi_Context_FreeMemory( hContext, NULL );
	Tspi_Context_Close( hContext );
	exit( exitCode );
}

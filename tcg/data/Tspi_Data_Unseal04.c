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
 *	Tspi_Data_Unseal04.c
 *
 * DESCRIPTION
 *	This test will verify that Tspi_Data_Unseal
 *		returns TSS_SUCCESS.
 *
 * ALGORITHM
 *	Setup:
 *		Create Context
 *		Connect Context
 *		Create Key Object
 *		Create Enc Data
 *		Load Key By UUID
 *		Create PCR Composite
 *		Set PCR Value
 *		Seal Data
 *
 *	Test:
 *		Call Data_Unseal then if it does not succeed
 *		Make sure that it returns the proper return codes
 *		Print results
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
 *      Megan Schneider, mschnei@us.ibm.com, 6/04.
 *      Kent Yoder, shpedoikal@gmail.com, 09/16/04
 *        Created testcase based on Megan's
 *
 * RESTRICTIONS
 *	None.
 */

#include <stdio.h>
#include <tss/tss.h>
#include "../common/common.h"

extern TSS_UUID SRK_UUID;

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
	char		*function = "Tspi_Data_Unseal04";
	TSS_HCONTEXT	hContext;
	TSS_HKEY	hSRK;
	TSS_HKEY	hKey;
	TSS_HPOLICY	hKeyPolicy, hEncUsagePolicy, hSRKPolicy;
	BYTE		rgbDataToSeal[32] = {0,1,3,4,5,6,7,8,9,'A','B','C','D','E','F',0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F'};
	BYTE		rgbPcrValue[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	TSS_HENCDATA	hEncData;
	BYTE		*prgbDataToUnseal;
	TSS_HPCRS	hPcrComposite;
	UINT32		BlobLength;
	UINT32		ulDataLength = 32;
	TSS_UUID	uuid;
	TSS_RESULT	result;
	UINT32		exitCode;
	TSS_FLAGS	initFlags = TSS_KEY_TYPE_STORAGE | TSS_KEY_SIZE_2048  |
				TSS_KEY_VOLATILE | TSS_KEY_AUTHORIZATION |
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

		// Connect to Context
	result = Tspi_Context_Connect( hContext, get_server(GLOBALSERVER) );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_Connect", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		// create hKey
	result = Tspi_Context_CreateObject( hContext,
						TSS_OBJECT_TYPE_RSAKEY,
						initFlags, &hKey );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_CreateObject (hKey)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Context_CreateObject( hContext,
						TSS_OBJECT_TYPE_ENCDATA,
						TSS_ENCDATA_SEAL, &hEncData );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_CreateObject (hEncData)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		// get the use policy for the encrypted data to set its secret
	result = Tspi_GetPolicyObject( hEncData, TSS_POLICY_USAGE, &hEncUsagePolicy );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_GetPolicyObject (hEncData)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Policy_SetSecret( hEncUsagePolicy, TSS_SECRET_MODE_PLAIN, 8, ENCDATA_PWD );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Policy_SetSecret (hEncUsagePolicy)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		//Load Key By UUID
	result = Tspi_Context_LoadKeyByUUID( hContext, TSS_PS_TYPE_SYSTEM,
						SRK_UUID, &hSRK );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_LoadKeyByUUID (hSRK)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		// set the SRK auth data
	result = Tspi_GetPolicyObject( hSRK, TSS_POLICY_USAGE, &hSRKPolicy );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_GetPolicyObject (hSRK)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Policy_SetSecret( hSRKPolicy, TSS_SECRET_MODE_PLAIN, 0, NULL );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Policy_SetSecret (hSRKPolicy)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		// set the new key's authdata
	result = Tspi_GetPolicyObject( hKey, TSS_POLICY_USAGE, &hKeyPolicy );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_GetPolicyObject (hKey)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Policy_SetSecret( hKeyPolicy, TSS_SECRET_MODE_PLAIN, 8, KEY_PWD );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Policy_SetSecret (hKeyPolicy)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		 // create the key
	result = Tspi_Key_CreateKey( hKey, hSRK, 0 );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Key_CreateKey (hKey)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Key_LoadKey( hKey, hSRK );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Key_LoadKey (hKey)", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

#if 0
	result = Tspi_Context_CreateObject( hContext, TSS_OBJECT_TYPE_PCRS,
					TSS_KEY_TSP_SRK, &hPcrComposite );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_CreateObject (hPcrComposite)",
				result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_PcrComposite_SetPcrValue( hPcrComposite, 8, 20,
							rgbPcrValue );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_PcrComposite_SetPcrValue", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}
#endif
		// Data Seal
	result = Tspi_Data_Seal( hEncData, hKey, ulDataLength, rgbDataToSeal, 0);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Data_Seal", result );
		print_error_exit( function, err_string(result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Data_Unseal( hEncData, hKey, NULL, &prgbDataToUnseal );
	if ( result != TSS_E_BAD_PARAMETER )
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

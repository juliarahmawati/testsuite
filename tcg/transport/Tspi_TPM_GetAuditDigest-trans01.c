/*
 *
 *   Copyright (C) International Business Machines  Corp., 2007
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
 *	Tspi_TPM_GetAuditDigest01.c
 *
 * DESCRIPTION
 *	This test will verify that Tspi_TPM_GetAuditDigest
 *		returns TSS_SUCCESS.
 *
 * ALGORITHM
 *	Setup:
 *		Create Context
 *		Connect Context
 *		Get TPM Handle
 *
 *	Test:
 *		Call TPM_GetAuditDigest without a key handle.
 *		Make sure that it returns the proper return codes
 *		Print results
 *		Call TPM_GetAuditDigest with a key handle.
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
 *      This test case is currently only implemented for v1.2
 *
 * HISTORY
 *      Tom Lendacky, toml@us.ibm.com, 6/07.
 *
 * RESTRICTIONS
 *	None.
 */

#include <stdio.h>
#include "common.h"


int
main( int argc, char **argv )
{
	char		version;

	version = parseArgs( argc, argv );
	if (version >= TESTSUITE_TEST_TSS_1_2)
		main_v1_2(version);
	else
		print_NA();
}

int
main_v1_2( char version )
{
	char *			function = "Tspi_TPM_GetAuditDigest-trans01";
	TSS_HCONTEXT		hContext;
	TSS_HKEY		hSRK;
	TSS_HTPM		hTPM;
	TSS_HPOLICY		hTpmPolicy;
	TSS_HKEY		hKey, hSigningKey, hWrappingKey;
	UINT32			auditDigestLen, ordListLen;
	BYTE *			auditDigest;
	UINT32 *		ordList;
	TPM_COUNTER_VALUE	counterValue;
	TSS_RESULT		result;

	print_begin_test( function );

	result = connect_load_all(&hContext, &hSRK, &hTPM);
	if ( result != TSS_SUCCESS )
	{
		print_error( "connect_load_all", (result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Testsuite_Transport_Init(hContext, hSRK, hTPM, TRUE, FALSE, &hWrappingKey,
					  &hSigningKey);
	if (result != TSS_SUCCESS) {
		print_error("Testsuite_Transport_Init", result);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &hTpmPolicy);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_GetPolicyObject", (result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Tspi_Policy_SetSecret(hTpmPolicy, TESTSUITE_OWNER_SECRET_MODE,
					TESTSUITE_OWNER_SECRET_LEN, TESTSUITE_OWNER_SECRET);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Policy_SetSecret", (result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		//Call GetAuditDigest
	result = Tspi_TPM_GetAuditDigest(hTPM, NULL_HKEY, FALSE, &auditDigestLen, &auditDigest,
					 &counterValue, NULL, &ordListLen, &ordList);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_TPM_GetAuditDigest", result );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	} else {
		result = Tspi_Context_FreeMemory(hContext, auditDigest);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Context_FreeMemory ", result);
			Tspi_Context_Close(hContext);
			exit(result);
		}

		result = Tspi_Context_FreeMemory(hContext, (BYTE *)ordList);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Context_FreeMemory ", result);
			Tspi_Context_Close(hContext);
			exit(result);
		}
	}

	result = create_load_key(hContext, TSS_KEY_TYPE_SIGNING, hSRK, &hKey);
	if ( result != TSS_SUCCESS )
	{
		print_error( "create_load_key", (result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

		//Call GetAuditDigest
	result = Tspi_TPM_GetAuditDigest(hTPM, hKey, FALSE, &auditDigestLen, &auditDigest,
					 &counterValue, NULL, NULL, NULL);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_TPM_GetAuditDigest", (result) );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	}

	result = Testsuite_Transport_Final(hContext, hSigningKey);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_TPM_GetAuditDigest", result );
		Tspi_Context_FreeMemory( hContext, NULL );
		Tspi_Context_Close( hContext );
		exit( result );
	} else {
		result = Tspi_Context_FreeMemory(hContext, auditDigest);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Context_FreeMemory ", result);
			Tspi_Context_Close(hContext);
			exit(result);
		}

		print_success( function, result );
	}

	print_end_test( function );
	Tspi_Context_FreeMemory( hContext, NULL );
	Tspi_Context_Close( hContext );
	exit( 0 );
}

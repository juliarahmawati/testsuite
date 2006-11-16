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
 *	Tspi_Key_CreateMigrationBlob01.c
 *
 * DESCRIPTION
 *	This test will verify Tspi_Key_CreateMigrationBlob.
 *	The purpose of this test case is to get TSS_SUCCESS to be
 *		returned. This is accomplished by following the
 *		algorithm described below.
 *
 * ALGORITHM
 *	Setup:
 *		Create Context
 *		Connect Context
 *		Get TPM Object
 *		Create hKey Object
 *		Get SRK Handle
 *		Get Policy Object (srk, tpm)
 *		Set secret (srk, tpm)
 *		Create Key (hkey, parent key, target key)
 *		Load key (parent key, hkey)
 *		Get Pub Key
 *		Set Attrib Data
 *		Authorize Migration Ticket
 *		LoadKeyByUUID
 *
 *	Test:	Call Key_ConvertMigrationBlob then if it is not a success
 *		make sure that it returns the proper return codes
 *
 *	Cleanup:
 *		Free memory associated with the context
 *		Close the hKey object
 *		Close the hParentStorageKey object
 *		Close the hTargetKey object
 *		Close the context.
 *		Print error/success message
 *
 * USAGE:	First parameter is --options
 *			-v or --version
 *		Second Parameter is the version of the test case to be run.
 *		This test case is currently only implemented for 1.1
 *
 * HISTORY
 *	Author:	Kathy Robertson
 *	Date:	June 2004
 *	Email:	klrobert@us.ibm.com
 *
 * RESTRICTIONS
 *	None.
 */
 
#include "common.h"


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

main_v1_1(void){

	char		*nameOfFunction = "Tspi_Key_CreateMigrationBlob01";
	TSS_HCONTEXT	hContext;
	TSS_HKEY	hSRK;
	TSS_HKEY	hKeyToMigrate;
	TSS_HKEY	hMigrationAuthorityKey;
	BYTE		*MigTicket;
	UINT32		TicketLength;
	BYTE		*randomData;
	UINT32		randomLength;
	UINT32		migBlobLength;
	BYTE		*migBlob;
	TSS_RESULT	result;
	TSS_HTPM	hTPM;
	TSS_HPOLICY	hPolicy, tpmUsagePolicy;

	print_begin_test(nameOfFunction);

		//Create Context
	result = Tspi_Context_Create(&hContext);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_Create ", result);
		print_error_exit(nameOfFunction, err_string(result));
		exit(result);
	}
		//Connect Context
	result = Tspi_Context_Connect(hContext, get_server(GLOBALSERVER));
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_Connect", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Get TPM Object
	result = Tspi_Context_GetTpmObject(hContext, &hTPM);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_GetTpmObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
	//Load Key By UUID
	result = Tspi_Context_LoadKeyByUUID(hContext,
			TSS_PS_TYPE_SYSTEM,
			SRK_UUID, &hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID for hSRK", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
#ifndef TESTSUITE_NOAUTH_SRK
		//Get Policy Object
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &hPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(hPolicy, TESTSUITE_SRK_SECRET_MODE,
					TESTSUITE_SRK_SECRET_LEN, TESTSUITE_SRK_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
#endif
		//Get Policy Object
	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &tpmUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(tpmUsagePolicy, TESTSUITE_OWNER_SECRET_MODE,
					TESTSUITE_OWNER_SECRET_LEN, TESTSUITE_OWNER_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Create Object
	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_RSAKEY,
				TSS_KEY_TYPE_BIND|TSS_KEY_SIZE_2048|TSS_KEY_NO_AUTHORIZATION,
				&hMigrationAuthorityKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Create Migrate Authority's key
	result = Tspi_Key_CreateKey(hMigrationAuthorityKey, hSRK, 0);
	if (result != TSS_SUCCESS)
	{
		print_error("Tspi_Key_CreateKey", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Create Object
	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_RSAKEY,
					TSS_KEY_TYPE_SIGNING | TSS_KEY_SIZE_2048  |
					TSS_KEY_NO_AUTHORIZATION | TSS_KEY_MIGRATABLE,
					&hKeyToMigrate);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
	result = Tspi_GetPolicyObject(hKeyToMigrate, TSS_POLICY_MIGRATION,
				      &hPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(hPolicy, TESTSUITE_KEY_SECRET_MODE,
				       TESTSUITE_KEY_SECRET_LEN, TESTSUITE_KEY_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Create Key To Migrate
	result = Tspi_Key_CreateKey(hKeyToMigrate, hSRK, 0);
	if (result != TSS_SUCCESS)
	{
		print_error("Tspi_Key_CreateKey", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hKeyToMigrate);
		exit(result);
	}
		//Authorize Migration Ticket
	result = Tspi_TPM_AuthorizeMigrationTicket(hTPM, hMigrationAuthorityKey,
			TSS_MS_REWRAP, &TicketLength, &MigTicket);
	if (result != TSS_SUCCESS)
	{
		print_error("Tpsi_TPM_AuthorizeMigrationTicket ", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}

		//Create Migration Blob
	result = Tspi_Key_CreateMigrationBlob(hKeyToMigrate, hSRK,
				TicketLength, MigTicket, &randomLength,
				&randomData, &migBlobLength, &migBlob);
	if (result != TSS_SUCCESS) {
		if(!checkNonAPI(result)){
			print_error(nameOfFunction, result);
			print_end_test(nameOfFunction);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_Close(hContext);
			exit(result);
		}
		else{
			print_error_nonapi(nameOfFunction, result);
			print_end_test(nameOfFunction);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_Close(hContext);
			exit(result);
		}
	}
	else{
		result = Tspi_Context_FreeMemory(hContext, migBlob);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Context_FreeMemory ", result);
			print_error_exit(nameOfFunction, err_string(result));
			Tspi_Context_Close(hContext);
			exit(result);
		}
		print_success(nameOfFunction, result);
		print_end_test(nameOfFunction);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(0);
	}
}

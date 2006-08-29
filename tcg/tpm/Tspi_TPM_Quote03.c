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
 *	Tspi_TPM_Quote03.c
 *
 * DESCRIPTION
 *	This test will verify Tspi_TPM_Quote
 *	The point of this test is to get TSS_E_INVALID_HANDLE 
 *		to be returned. This is easily accomplished by 
 *		passing in -1 for the second parameter
 *
 * ALGORITHM
 *	Setup:
 *		Create Context
 *		Connect Context
 *		Create Object and Key for hKey
 *		Get TPM Object
 *		Load Key By UUID for SRK
 *		Get Policy Object for srkUsagePolicy, TPM
 *		Set Secret for srkUsagePolicy, TPM
 *		Create object for the hIdentKey
 *		Get Policy Object for key
 *		Set Secret for key
 *		Create hIdentKey
 *		Create object for the hPcrComposite Key
 *		SelectPcrIndex
 *		Get Random
 *		Set Validation Data
 *
 *	Test:	Call Quote. If this is unsuccessful check for 
 *		type of error, and make sure it returns the proper return code
 * 
 *	Cleanup:
 *		Free memory associated with the context
 *		Close the hIdentKey Object
 *		Close the hKey Object
 *		Close the hPcrComposite Object
 *		Close the context
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

#include <trousers/tss.h>
#include "../common/common.h"



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

	char		*nameOfFunction	= "Tspi_TPM_Quote03";
	TSS_HCONTEXT	hContext;
	TSS_RESULT	result;
	TSS_HKEY	hKey;
	TSS_HTPM	hTPM;
	TSS_HKEY	hSRK;
	TSS_UUID	identityKeyUUID	= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2};
	TSS_VALIDATION	pValidationData;
	TSS_HKEY	hIdentKey;
	TSS_HPCRS	hPcrComposite;
	TSS_FLAG	initFlags;
	initFlags 	= TSS_KEY_TYPE_SIGNING | TSS_KEY_SIZE_2048  |
			TSS_KEY_VOLATILE | TSS_KEY_NO_AUTHORIZATION |
			TSS_KEY_NOT_MIGRATABLE;
	TSS_HPOLICY srkUsagePolicy, keyUsagePolicy, keyMigPolicy;
	BYTE		*data;
	BYTE		well_known_secret[20] = TSS_WELL_KNOWN_SECRET;

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
		//Create hKey
	result = Tspi_Context_CreateObject(hContext,
			TSS_OBJECT_TYPE_RSAKEY,
			initFlags, &hKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Get TPM Object
	result = Tspi_Context_GetTpmObject(hContext, &hTPM);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_GetTpmObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Load Key By UUID
	result = Tspi_Context_LoadKeyByUUID(hContext,
			TSS_PS_TYPE_SYSTEM,
			SRK_UUID, &hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Get Policy Object
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &srkUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(srkUsagePolicy, TESTSUITE_SRK_SECRET_MODE,
					TESTSUITE_SRK_SECRET_LEN, TESTSUITE_SRK_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE,
					&keyUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(keyUsagePolicy,
				TESTSUITE_KEY_SECRET_MODE,
				TESTSUITE_KEY_SECRET_LEN, TESTSUITE_KEY_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret ", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}

		//Create object for the hIdentKey
	result = Tspi_Context_CreateObject(hContext, 
			TSS_OBJECT_TYPE_RSAKEY,
			TSS_KEY_SIZE_2048 |TSS_KEY_TYPE_SIGNING 
			|TSS_KEY_MIGRATABLE, &hIdentKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject for Signing Key", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	result = Tspi_GetPolicyObject(hIdentKey, TSS_POLICY_USAGE,
					&keyUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(keyUsagePolicy,
				TESTSUITE_KEY_SECRET_MODE,
				TESTSUITE_KEY_SECRET_LEN, TESTSUITE_KEY_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret ", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}

		//Create hIdentKey
	result = Tspi_Key_CreateKey(hIdentKey, hSRK, 0);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_CreatehIdentKey", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Create object for the hPcrComposite Key
	result = Tspi_Context_CreateObject(hContext, 
			TSS_OBJECT_TYPE_PCRS, 0,
			&hPcrComposite);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreatehIdentKey", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//SelectPcrIndex
	result = Tspi_PcrComposite_SelectPcrIndex(hPcrComposite, 1);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_PcrComposite_SelectPcrIndex ", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	result = Tspi_TPM_GetRandom(hTPM, 20, &data);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_TPM_GetRandom ", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hKey);
		exit(result);
	}

	pValidationData.ulDataLength = 20;
	pValidationData.rgbExternalData = data;

		//Call TPM Quote
	result = Tspi_TPM_Quote(hTPM, -1, hPcrComposite, &pValidationData);
	if (TSS_ERROR_CODE(result) != TSS_E_INVALID_HANDLE) {
		if(!checkNonAPI(result)){
			print_error(nameOfFunction, result);
			print_end_test(nameOfFunction);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_CloseObject(hContext, hIdentKey);
			Tspi_Context_CloseObject(hContext, hKey);
			Tspi_Context_CloseObject(hContext, hPcrComposite);
			Tspi_Context_Close(hContext);
			exit(result);
		}
		else{
			print_error_nonapi(nameOfFunction, result);
			print_end_test(nameOfFunction);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_CloseObject(hContext, hIdentKey);
			Tspi_Context_CloseObject(hContext, hKey);
			Tspi_Context_CloseObject(hContext, hPcrComposite);
			Tspi_Context_Close(hContext);
			exit(result);
		}
	}
	else{
		print_success(nameOfFunction, result);
		print_end_test(nameOfFunction);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		Tspi_Context_Close(hContext);
		exit(0);
	}
}

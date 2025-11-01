/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file */

#include <sl_api.h>
#include <sl_priv.h>
#ifdef LOOP_BACK_ENABLE
#include <can.h>
#include <mibspi.h>
#endif
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
void SL_SW_Reset(void)
{
    systemREG1->SYSECR = ((uint32)0x2u << 14u);
}


#define CRC_CTRL0 		(*(volatile uint32 *)0xFE000000U)
#define CRC_CTRL2 		(*(volatile uint32 *)0xFE000010U)
#define CRC_STATUS 		(*(volatile uint32 *)0xFE000028U)
#define CRC_SIGREGL1 	(*(volatile uint32 *)0xFE000060U)
#define CRC_SIGREGH1 	(*(volatile uint32 *)0xFE000064U)
#define CRC_SIGREG 		(*(volatile uint64 *)0xFE000060U)
#define CRC_REGL1 		(*(volatile uint32 *)0xFE000068U)
#define CRC_REGH1 		(*(volatile uint32 *)0xFE00006CU)
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
uint64 SL_CRC_Calculate (uint64* startAddr, const uint32 count64)
{
	volatile uint32 count = 0u; /* volatile in order to prevent from being optimised */

	CRC_CTRL0 |= 0x00000001U; /* Reset the CRC Module */
	CRC_CTRL0 &= 0xFFFFFFFEU;
	CRC_CTRL2 |= 0x00000003U; /* Configure to Full CPU Mode */

	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	for (count=0u; count < count64; count++) {
		/*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_2*/
		/*SAFETYMCUSW 93 S MR: 6.1,6.2,10.1,10.2,10.3,10.4 <APPROVED> "LDRA Tool issue" */
		CRC_SIGREG = (uint64)(*startAddr);
		/*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_2*/
		/*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
		startAddr++;
	}
    return(CRC_SIGREG);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
void SL_Clear_nERROR(void)
{
    esmREG->EKR = ESM_NERROR_CLEAR;
    return;
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
void SL_Set_nERROR(void)
{
    esmREG->EKR = ESM_NERROR_SET;
    return;
}

/*Comment_1:
 * "This function will be called by application so not static"*/

/*Comment_2:
 * "Reason - value 0 for start address is valid as it is used for crc calculation"*/

/*Comment_5:
 * "Reason -  FALSE_POSITIVE The rule requires that the expression should be
 * guaranteed to have the same value regardless of the order in which the operands within the expression are evaluated.
 * This usually means no more than one volatile access occuring in the expression.The recommendation that simple
 * assignments should be used is intended to be restricted to simple assignments of the form x = v and not
 * "simple assignments" according to the ISO definition. However, the recommendation does not have to be followed
 * in order to be compliant with the rule. You should implement the rule (the value should be independent of the
 * evaluation order) for compliance purposes and you might choose to issue a warning for non-simple assignments
 * or you might choose to ignore them."*/
 
 
 

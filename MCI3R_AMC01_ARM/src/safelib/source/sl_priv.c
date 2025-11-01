/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file */

#include <sl_types.h>
#include <sl_config.h>
#include <sl_priv.h>


boolean sl_priv_flag_set[TESTTYPE_MAX - TESTTYPE_MIN] = {0};

boolean SL_FLAG_SET(sint32 flag_id) {
    sl_priv_flag_set[flag_id-TESTTYPE_MIN] = TRUE;
    return sl_priv_flag_set[flag_id-TESTTYPE_MIN];
}


void SL_FLAG_CLEAR(sint32 flag_id) {
    sl_priv_flag_set[flag_id-TESTTYPE_MIN] = FALSE;
}


boolean SL_FLAG_GET(sint32 flag_id) {
    return sl_priv_flag_set[flag_id-TESTTYPE_MIN];
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*SAFETYMCUSW 91 S MR: 5.2,5.6,5.7 <APPROVED> Comment_1*/
void _SL_SelfTest_SRAM_RAD (sl_tcramBASE_t* ramBase, SL_SelfTest_Result* rad_stResult)
{
    /** Start with the equality test scheme **/
    ramBase->RAMTEST = (uint32) 0x0000008Au; /* 1->Trigger, 8->Eq., test mode, A->Test enable */
    ramBase->RAMTEST = (uint32) 0x00000100u;
    /* Wait for the test to complete */
    /*SAFETYMCUSW 28 D <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    while (TCRAM_RAMTEST_TST_TRIGGER == (TCRAM_RAMTEST_TST_TRIGGER & ramBase->RAMTEST)) {

	}
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_5*/
    if ((TCRAM_RAMERRSTAT_ADDCMPLG_FAIL == \
        (uint32)(ramBase->RAMERRSTATUS & TCRAM_RAMERRSTAT_ADDCMPLG_FAIL)) ||
    		(TCRAM_RAMERRSTAT_ADDR_DEC_FAIL == \
    				/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_5*/
    				(uint32)(ramBase->RAMERRSTATUS & TCRAM_RAMERRSTAT_ADDR_DEC_FAIL))){
        /* Test failed */
    	/* Clear the error status */
    	ramBase->RAMERRSTATUS |= TCRAM_RAMERRSTAT_ADDCMPLG_FAIL;
    	ramBase->RAMERRSTATUS |= TCRAM_RAMERRSTAT_ADDR_DEC_FAIL;
        *rad_stResult = ST_FAIL;
    } else {
	    ramBase->RAMTEST = (uint32) 0x0000014Au; /* 1->Trigger, 4->InEq., test mode, A->Test enable */
        /* Wait for the test to complete */
        /*SAFETYMCUSW 28 D <APPROVED> Comment_2*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        while (TCRAM_RAMTEST_TST_TRIGGER == (TCRAM_RAMTEST_TST_TRIGGER & ramBase->RAMTEST))
        {

        }
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_4*/
        /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_5*/
        if ((TCRAM_RAMERRSTAT_ADDCMPLG_FAIL == \
            (uint32)(ramBase->RAMERRSTATUS & TCRAM_RAMERRSTAT_ADDCMPLG_FAIL)) &&
        		(TCRAM_RAMERRSTAT_ADDR_DEC_FAIL == \
        				/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_5*/
        				(uint32)(ramBase->RAMERRSTATUS & TCRAM_RAMERRSTAT_ADDR_DEC_FAIL))){
            /* Test failed */
        	/* Clear the error status */
			ramBase->RAMERRSTATUS |= TCRAM_RAMERRSTAT_ADDCMPLG_FAIL;
	    	ramBase->RAMERRSTATUS |= TCRAM_RAMERRSTAT_ADDR_DEC_FAIL;
            *rad_stResult = ST_FAIL;
        } else {
        	ramBase->RAMERRSTATUS |= TCRAM_RAMERRSTAT_ADDR_DEC_FAIL;
            *rad_stResult = ST_PASS;
        }
    }
}
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
void _SL_SelfTest_SRAM_RAD (l2ramwBASE_t * ramBase, SL_SelfTest_Result * rad_stResult)
{
	/* Clear any previous diagnostic errors status bits [22,21,20,19,12,11,10, 4] - required before trigger */
	BIT_SET(ramBase->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);

    /** Start with the equality test scheme **/
	ramBase->DIAG_ECC = (uint32)0xCu; //setting correct ECC value for zeroes (to avoid ecc error)
    ramBase->RAMTEST = (uint32) 0x0000008Au; /* 1->Trigger, 8->Eq., test mode, A->Test enable */
    ramBase->RAMTEST = (uint32) 0x00000100u;
    /* Wait for the test to complete */
	/*SAFETYMCUSW 28 D <APPROVED> Comment_2*/
	while (L2RAM_RAMTEST_TST_TRIGGER == (ramBase->RAMTEST & L2RAM_RAMTEST_TST_TRIGGER)){

	}

	/* Check if the diagnostic error status bits have been set */
	if(((l2ramwREG->RAMERRSTATUS & L2RAM_RAMERRSTATUS_ADDE) == L2RAM_RAMERRSTATUS_ADDE) ||
	   ((l2ramwREG->RAMERRSTATUS & L2RAM_RAMERRSTATUS_ADE) == L2RAM_RAMERRSTATUS_ADE))
	{
        /* Test failed */
    	/* Clear the error status */
    	ramBase->RAMERRSTATUS |= L2RAM_RAMERRSTATUS_ADDE;
    	ramBase->RAMERRSTATUS |= L2RAM_RAMERRSTATUS_ADE;
        *rad_stResult = ST_FAIL;
	}
	else
	{
		/* Clear any previous diagnostic errors status bits [22,21,20,19,12,11,10, 4] - required before trigger */
		BIT_SET(ramBase->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);

		/* Then doing an inequality check in compare */
		ramBase->DIAG_ECC = (uint32)0xCu; //setting correct ECC value for zeroes (to avoid ecc error)
	    ramBase->RAMTEST = (uint32) 0x0000004Au; /* 1->Trigger, 4->InEq., test mode, A->Test enable */
	    ramBase->RAMTEST = (uint32) 0x00000100u;
	    /* Wait for the test to complete */
		/*SAFETYMCUSW 28 D <APPROVED> Comment_2*/
		while (L2RAM_RAMTEST_TST_TRIGGER == (ramBase->RAMTEST & L2RAM_RAMTEST_TST_TRIGGER)){

		}

		/* Check if the diagnostic error status bits have been set */
		if(((l2ramwREG->RAMERRSTATUS & L2RAM_RAMERRSTATUS_ADDE) == L2RAM_RAMERRSTATUS_ADDE) ||
		   ((l2ramwREG->RAMERRSTATUS & L2RAM_RAMERRSTATUS_ADE) == L2RAM_RAMERRSTATUS_ADE))
		{
	        /* Test failed */
	    	/* Clear the error status */
	    	ramBase->RAMERRSTATUS |= L2RAM_RAMERRSTATUS_ADDE;
	    	ramBase->RAMERRSTATUS |= L2RAM_RAMERRSTATUS_ADE;
	        *rad_stResult = ST_FAIL;
		}
		else
		{
            *rad_stResult = ST_PASS;
		}

	}
}
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
/** @fn void SL_SelfTest_adcGetSingleData(adcBASE_t *adc, unsigned group, adcData_t *data)
 *   @brief Get single converted ADC value
 *   @param[in] adc Pointer to ADC module:
 *              - adcREG1: ADC1 module pointer
 *              - adcREG2: ADC2 module pointer
 *              - adcREG3: ADC3 module pointer
 *   @param[in] group Hardware group of ADC module:
 *              - adcGROUP0: ADC event group
 *              - adcGROUP1: ADC group 1
 *              - adcGROUP2: ADC group 2
 *   @param[out] data Pointer to store ADC converted data
 *
 */
boolean _SL_SelfTest_adcGetSingleData(adcBASE_t* adc, uint32 group, SL_ADC_Data* adcData)
{
    boolean retVal = TRUE;
	uint32 buf;
	uint32 temp = 0u;

    if(((adc != adcREG1) && (adc != adcREG2)) || (adcData == NULL)) {
        retVal =  FALSE;
    }
    else
    {
		SL_ADC_Data* ptr = adcData;

		/** -  Get conversion data and channel/pin id */
		buf = adc->GxBUF[group].BUF0;
		/* MISRA-C violation fix - mixing uint16 and uint32 in one statement */
		temp = buf&ADC_VALUE_MASK;
		ptr->value = (uint16)temp;
		ptr->id = ADC_GET_CHANNEL_ID(buf); /* int to unsigned short*/
		/*Clear the flags which are set by writing 1 to those bit fields*/
		adc->GxINTFLG[group] = ADC_GROUP_THR_INT_FLG | ADC_GROUP_CONV_END;

		/**   @note The function adcInit has to be called before this function can be used.\n
		 *           The user is responsible to initialize the message box.
		 */
    }
	return retVal;
}


/** @fn void SL_SelfTest_adcStartConversion_selChn(adcBASE_t *adc, unsigned channel, unsigned fifo_size, unsigned group)
 *   @brief Starts an ADC conversion
 *   @param[in] adc Pointer to ADC module:
 *              - adcREG1: ADC1 module pointer
 *              - adcREG2: ADC2 module pointer
 *   @param[in] channel ADC channel to be selected for conversion
 *   @param[in] fifo_size ADC fifo size to be configured.
 *   @param[in] group Hardware group of ADC module:
 *              - adcGROUP0: ADC event group
 *              - adcGROUP1: ADC group 1
 *              - adcGROUP2: ADC group 2
 *
 *   This function Starts the convesion of the ADC selected group for the selected channel
 *
 */
boolean _SL_SelfTest_adcStartConversion_selChn(adcBASE_t* adc, uint8 channel, uint32 fifo_size, uint32 group)
{
    boolean retVal = TRUE;
    if((adc != adcREG1) && (adc != adcREG2)) {
        retVal =  FALSE;
    }
    else
    {
	/** - Setup FiFo size */
	adc->GxINTCR[group] = fifo_size;
	/** - Start Conversion */
	adc->GxSEL[group] = 1u << channel;
    }
    return retVal;
}


/** @fn boolean SL_SelfTest_adcIsConversionComplete(adcBASE_t *adc, uint32 group)
 *   @brief Checks if Conversion is complete
 *   @param[in] adc Pointer to ADC module:
 *              - adcREG1: ADC1 module pointer
 *              - adcREG2: ADC2 module pointer
 *              - adcREG3: ADC3 module pointer
 *   @param[in] group Hardware group of ADC module:
 *              - adcGROUP0: ADC event group
 *              - adcGROUP1: ADC group 1
 *              - adcGROUP2: ADC group 2
 *   @return The function will return:
 *           - 0: When is not finished
 *           - 8: When conversion is complete
 *
 *   This function checks if conversion is complete.
 *
 */
boolean _SL_SelfTest_adcIsConversionComplete(const adcBASE_t* adc, uint32 group)
{
	uint32 Flags;
	boolean retVal;

	
	
    if((adc != adcREG1) && (adc != adcREG2)) {
        retVal =  FALSE;
    }
    else
    {
		/** - Read conversion flags */
		Flags = adc->GxINTFLG[group] & ADC_GROUP_CONV_END;

		/**   @note The function adcInit has to be called before this function can be used. */

		
		

		if (ADC_GROUP_CONV_END == Flags) {
			retVal=  TRUE;
		} else {
			retVal =  FALSE;
		}
    }
    return retVal;
}
#endif


void _SL_HoldNClear_nError(void)
{
    uint32 tempVal = 0u;
    while (tempVal <= NERROR_HOLD_DELAY) {
        tempVal++;
    }
    esmREG->EKR = 0x5u;

    /*SAFETYMCUSW 28 D <APPROVED> Comment_2*/
    while(TRUE == SL_ESM_nERROR_Active())
    {

    }
    return;
}



#ifdef LOOP_BACK_ENABLE
void _SL_loadDataPattern(uint16 * pptr, uint16 seed_value, uint32 psize)
{
    *pptr = seed_value;
    while((psize--) > 0)
    {
      *pptr = (1u + *pptr++);
    }
}
#endif

uint16  _SL_Approximate_value(uint16 value, uint16 reference, uint8 difference)
{
    uint16 retvalue;
    if((uint16)(value - reference) < difference)
    {
        retvalue =  reference;
    }
    else if((uint16)(reference - value) < difference)
    {
        retvalue =  reference;
    }
    else
    {
        retvalue =  value;
    }
    return retvalue;
}

/*Comment_1:
 * "Name declared in another C name space is deviated"*/

/*Comment_2:
 * "Reason - Design change necessary to remove the infinite while loop"*/

/*Comment_3:
 * "Reason -  FALSE_POSITIVE The rule requires that the expression should be
 * guaranteed to have the same value regardless of the order in which the operands within the expression are evaluated.
 * This usually means no more than one volatile access occuring in the expression.The recommendation that simple
 * assignments should be used is intended to be restricted to simple assignments of the form x = v and not
 * "simple assignments" according to the ISO definition. However, the recommendation does not have to be followed
 * in order to be compliant with the rule. You should implement the rule (the value should be independent of the
 * evaluation order) for compliance purposes and you might choose to issue a warning for non-simple assignments
 * or you might choose to ignore them."*/

/*Comment_4:
 * "Address of volatile variable accessed, not the variable itself"*/

/*Comment_5:
 * "False positive" */
 
 
 
 
 
 
 
 

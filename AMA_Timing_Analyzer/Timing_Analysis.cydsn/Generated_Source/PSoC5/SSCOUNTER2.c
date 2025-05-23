/*******************************************************************************
* File Name: SSCOUNTER2.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "SSCOUNTER2.h"

uint8 SSCOUNTER2_initVar = 0u;


/*******************************************************************************
* Function Name: SSCOUNTER2_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void SSCOUNTER2_Init(void) 
{
        #if (!SSCOUNTER2_UsingFixedFunction && !SSCOUNTER2_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!SSCOUNTER2_UsingFixedFunction && !SSCOUNTER2_ControlRegRemoved) */
        
        #if(!SSCOUNTER2_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 SSCOUNTER2_interruptState;
        #endif /* (!SSCOUNTER2_UsingFixedFunction) */
        
        #if (SSCOUNTER2_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            SSCOUNTER2_CONTROL &= SSCOUNTER2_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                SSCOUNTER2_CONTROL2 &= ((uint8)(~SSCOUNTER2_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                SSCOUNTER2_CONTROL3 &= ((uint8)(~SSCOUNTER2_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (SSCOUNTER2_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                SSCOUNTER2_CONTROL |= SSCOUNTER2_ONESHOT;
            #endif /* (SSCOUNTER2_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            SSCOUNTER2_CONTROL2 |= SSCOUNTER2_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            SSCOUNTER2_RT1 &= ((uint8)(~SSCOUNTER2_RT1_MASK));
            SSCOUNTER2_RT1 |= SSCOUNTER2_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            SSCOUNTER2_RT1 &= ((uint8)(~SSCOUNTER2_SYNCDSI_MASK));
            SSCOUNTER2_RT1 |= SSCOUNTER2_SYNCDSI_EN;

        #else
            #if(!SSCOUNTER2_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = SSCOUNTER2_CONTROL & ((uint8)(~SSCOUNTER2_CTRL_CMPMODE_MASK));
            SSCOUNTER2_CONTROL = ctrl | SSCOUNTER2_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = SSCOUNTER2_CONTROL & ((uint8)(~SSCOUNTER2_CTRL_CAPMODE_MASK));
            
            #if( 0 != SSCOUNTER2_CAPTURE_MODE_CONF)
                SSCOUNTER2_CONTROL = ctrl | SSCOUNTER2_DEFAULT_CAPTURE_MODE;
            #else
                SSCOUNTER2_CONTROL = ctrl;
            #endif /* 0 != SSCOUNTER2_CAPTURE_MODE */ 
            
            #endif /* (!SSCOUNTER2_ControlRegRemoved) */
        #endif /* (SSCOUNTER2_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!SSCOUNTER2_UsingFixedFunction)
            SSCOUNTER2_ClearFIFO();
        #endif /* (!SSCOUNTER2_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        SSCOUNTER2_WritePeriod(SSCOUNTER2_INIT_PERIOD_VALUE);
        #if (!(SSCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
            SSCOUNTER2_WriteCounter(SSCOUNTER2_INIT_COUNTER_VALUE);
        #endif /* (!(SSCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */
        SSCOUNTER2_SetInterruptMode(SSCOUNTER2_INIT_INTERRUPTS_MASK);
        
        #if (!SSCOUNTER2_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)SSCOUNTER2_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            SSCOUNTER2_WriteCompare(SSCOUNTER2_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            SSCOUNTER2_interruptState = CyEnterCriticalSection();
            
            SSCOUNTER2_STATUS_AUX_CTRL |= SSCOUNTER2_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(SSCOUNTER2_interruptState);
            
        #endif /* (!SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void SSCOUNTER2_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (SSCOUNTER2_UsingFixedFunction)
        SSCOUNTER2_GLOBAL_ENABLE |= SSCOUNTER2_BLOCK_EN_MASK;
        SSCOUNTER2_GLOBAL_STBY_ENABLE |= SSCOUNTER2_BLOCK_STBY_EN_MASK;
    #endif /* (SSCOUNTER2_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!SSCOUNTER2_ControlRegRemoved || SSCOUNTER2_UsingFixedFunction)
        SSCOUNTER2_CONTROL |= SSCOUNTER2_CTRL_ENABLE;                
    #endif /* (!SSCOUNTER2_ControlRegRemoved || SSCOUNTER2_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: SSCOUNTER2_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  SSCOUNTER2_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void SSCOUNTER2_Start(void) 
{
    if(SSCOUNTER2_initVar == 0u)
    {
        SSCOUNTER2_Init();
        
        SSCOUNTER2_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    SSCOUNTER2_Enable();        
}


/*******************************************************************************
* Function Name: SSCOUNTER2_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void SSCOUNTER2_Stop(void) 
{
    /* Disable Counter */
    #if(!SSCOUNTER2_ControlRegRemoved || SSCOUNTER2_UsingFixedFunction)
        SSCOUNTER2_CONTROL &= ((uint8)(~SSCOUNTER2_CTRL_ENABLE));        
    #endif /* (!SSCOUNTER2_ControlRegRemoved || SSCOUNTER2_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (SSCOUNTER2_UsingFixedFunction)
        SSCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SSCOUNTER2_BLOCK_EN_MASK));
        SSCOUNTER2_GLOBAL_STBY_ENABLE &= ((uint8)(~SSCOUNTER2_BLOCK_STBY_EN_MASK));
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void SSCOUNTER2_SetInterruptMode(uint8 interruptsMask) 
{
    SSCOUNTER2_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: SSCOUNTER2_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   SSCOUNTER2_ReadStatusRegister(void) 
{
    return SSCOUNTER2_STATUS;
}


#if(!SSCOUNTER2_ControlRegRemoved)
/*******************************************************************************
* Function Name: SSCOUNTER2_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   SSCOUNTER2_ReadControlRegister(void) 
{
    return SSCOUNTER2_CONTROL;
}


/*******************************************************************************
* Function Name: SSCOUNTER2_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    SSCOUNTER2_WriteControlRegister(uint8 control) 
{
    SSCOUNTER2_CONTROL = control;
}

#endif  /* (!SSCOUNTER2_ControlRegRemoved) */


#if (!(SSCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: SSCOUNTER2_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void SSCOUNTER2_WriteCounter(uint16 counter) \
                                   
{
    #if(SSCOUNTER2_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (SSCOUNTER2_GLOBAL_ENABLE & SSCOUNTER2_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        SSCOUNTER2_GLOBAL_ENABLE |= SSCOUNTER2_BLOCK_EN_MASK;
        CY_SET_REG16(SSCOUNTER2_COUNTER_LSB_PTR, (uint16)counter);
        SSCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SSCOUNTER2_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(SSCOUNTER2_COUNTER_LSB_PTR, counter);
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}
#endif /* (!(SSCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: SSCOUNTER2_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 SSCOUNTER2_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(SSCOUNTER2_UsingFixedFunction)
		(void)CY_GET_REG16(SSCOUNTER2_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(SSCOUNTER2_COUNTER_LSB_PTR_8BIT);
	#endif/* (SSCOUNTER2_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(SSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SSCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SSCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 SSCOUNTER2_ReadCapture(void) 
{
    #if(SSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SSCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SSCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void SSCOUNTER2_WritePeriod(uint16 period) 
{
    #if(SSCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SSCOUNTER2_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(SSCOUNTER2_PERIOD_LSB_PTR, period);
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 SSCOUNTER2_ReadPeriod(void) 
{
    #if(SSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SSCOUNTER2_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(SSCOUNTER2_PERIOD_LSB_PTR));
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


#if (!SSCOUNTER2_UsingFixedFunction)
/*******************************************************************************
* Function Name: SSCOUNTER2_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void SSCOUNTER2_WriteCompare(uint16 compare) \
                                   
{
    #if(SSCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SSCOUNTER2_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(SSCOUNTER2_COMPARE_LSB_PTR, compare);
    #endif /* (SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 SSCOUNTER2_ReadCompare(void) 
{
    return (CY_GET_REG16(SSCOUNTER2_COMPARE_LSB_PTR));
}


#if (SSCOUNTER2_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SSCOUNTER2_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void SSCOUNTER2_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    SSCOUNTER2_CONTROL &= ((uint8)(~SSCOUNTER2_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    SSCOUNTER2_CONTROL |= compareMode;
}
#endif  /* (SSCOUNTER2_COMPARE_MODE_SOFTWARE) */


#if (SSCOUNTER2_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SSCOUNTER2_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void SSCOUNTER2_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    SSCOUNTER2_CONTROL &= ((uint8)(~SSCOUNTER2_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    SSCOUNTER2_CONTROL |= ((uint8)((uint8)captureMode << SSCOUNTER2_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (SSCOUNTER2_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: SSCOUNTER2_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void SSCOUNTER2_ClearFIFO(void) 
{

    while(0u != (SSCOUNTER2_ReadStatusRegister() & SSCOUNTER2_STATUS_FIFONEMP))
    {
        (void)SSCOUNTER2_ReadCapture();
    }

}
#endif  /* (!SSCOUNTER2_UsingFixedFunction) */


/* [] END OF FILE */


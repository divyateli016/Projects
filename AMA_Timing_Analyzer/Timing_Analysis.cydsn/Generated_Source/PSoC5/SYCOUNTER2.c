/*******************************************************************************
* File Name: SYCOUNTER2.c  
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

#include "SYCOUNTER2.h"

uint8 SYCOUNTER2_initVar = 0u;


/*******************************************************************************
* Function Name: SYCOUNTER2_Init
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
void SYCOUNTER2_Init(void) 
{
        #if (!SYCOUNTER2_UsingFixedFunction && !SYCOUNTER2_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!SYCOUNTER2_UsingFixedFunction && !SYCOUNTER2_ControlRegRemoved) */
        
        #if(!SYCOUNTER2_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 SYCOUNTER2_interruptState;
        #endif /* (!SYCOUNTER2_UsingFixedFunction) */
        
        #if (SYCOUNTER2_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            SYCOUNTER2_CONTROL &= SYCOUNTER2_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                SYCOUNTER2_CONTROL2 &= ((uint8)(~SYCOUNTER2_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                SYCOUNTER2_CONTROL3 &= ((uint8)(~SYCOUNTER2_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (SYCOUNTER2_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                SYCOUNTER2_CONTROL |= SYCOUNTER2_ONESHOT;
            #endif /* (SYCOUNTER2_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            SYCOUNTER2_CONTROL2 |= SYCOUNTER2_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            SYCOUNTER2_RT1 &= ((uint8)(~SYCOUNTER2_RT1_MASK));
            SYCOUNTER2_RT1 |= SYCOUNTER2_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            SYCOUNTER2_RT1 &= ((uint8)(~SYCOUNTER2_SYNCDSI_MASK));
            SYCOUNTER2_RT1 |= SYCOUNTER2_SYNCDSI_EN;

        #else
            #if(!SYCOUNTER2_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = SYCOUNTER2_CONTROL & ((uint8)(~SYCOUNTER2_CTRL_CMPMODE_MASK));
            SYCOUNTER2_CONTROL = ctrl | SYCOUNTER2_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = SYCOUNTER2_CONTROL & ((uint8)(~SYCOUNTER2_CTRL_CAPMODE_MASK));
            
            #if( 0 != SYCOUNTER2_CAPTURE_MODE_CONF)
                SYCOUNTER2_CONTROL = ctrl | SYCOUNTER2_DEFAULT_CAPTURE_MODE;
            #else
                SYCOUNTER2_CONTROL = ctrl;
            #endif /* 0 != SYCOUNTER2_CAPTURE_MODE */ 
            
            #endif /* (!SYCOUNTER2_ControlRegRemoved) */
        #endif /* (SYCOUNTER2_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!SYCOUNTER2_UsingFixedFunction)
            SYCOUNTER2_ClearFIFO();
        #endif /* (!SYCOUNTER2_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        SYCOUNTER2_WritePeriod(SYCOUNTER2_INIT_PERIOD_VALUE);
        #if (!(SYCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
            SYCOUNTER2_WriteCounter(SYCOUNTER2_INIT_COUNTER_VALUE);
        #endif /* (!(SYCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */
        SYCOUNTER2_SetInterruptMode(SYCOUNTER2_INIT_INTERRUPTS_MASK);
        
        #if (!SYCOUNTER2_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)SYCOUNTER2_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            SYCOUNTER2_WriteCompare(SYCOUNTER2_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            SYCOUNTER2_interruptState = CyEnterCriticalSection();
            
            SYCOUNTER2_STATUS_AUX_CTRL |= SYCOUNTER2_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(SYCOUNTER2_interruptState);
            
        #endif /* (!SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_Enable
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
void SYCOUNTER2_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (SYCOUNTER2_UsingFixedFunction)
        SYCOUNTER2_GLOBAL_ENABLE |= SYCOUNTER2_BLOCK_EN_MASK;
        SYCOUNTER2_GLOBAL_STBY_ENABLE |= SYCOUNTER2_BLOCK_STBY_EN_MASK;
    #endif /* (SYCOUNTER2_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!SYCOUNTER2_ControlRegRemoved || SYCOUNTER2_UsingFixedFunction)
        SYCOUNTER2_CONTROL |= SYCOUNTER2_CTRL_ENABLE;                
    #endif /* (!SYCOUNTER2_ControlRegRemoved || SYCOUNTER2_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: SYCOUNTER2_Start
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
*  SYCOUNTER2_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void SYCOUNTER2_Start(void) 
{
    if(SYCOUNTER2_initVar == 0u)
    {
        SYCOUNTER2_Init();
        
        SYCOUNTER2_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    SYCOUNTER2_Enable();        
}


/*******************************************************************************
* Function Name: SYCOUNTER2_Stop
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
void SYCOUNTER2_Stop(void) 
{
    /* Disable Counter */
    #if(!SYCOUNTER2_ControlRegRemoved || SYCOUNTER2_UsingFixedFunction)
        SYCOUNTER2_CONTROL &= ((uint8)(~SYCOUNTER2_CTRL_ENABLE));        
    #endif /* (!SYCOUNTER2_ControlRegRemoved || SYCOUNTER2_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (SYCOUNTER2_UsingFixedFunction)
        SYCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SYCOUNTER2_BLOCK_EN_MASK));
        SYCOUNTER2_GLOBAL_STBY_ENABLE &= ((uint8)(~SYCOUNTER2_BLOCK_STBY_EN_MASK));
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_SetInterruptMode
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
void SYCOUNTER2_SetInterruptMode(uint8 interruptsMask) 
{
    SYCOUNTER2_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: SYCOUNTER2_ReadStatusRegister
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
uint8   SYCOUNTER2_ReadStatusRegister(void) 
{
    return SYCOUNTER2_STATUS;
}


#if(!SYCOUNTER2_ControlRegRemoved)
/*******************************************************************************
* Function Name: SYCOUNTER2_ReadControlRegister
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
uint8   SYCOUNTER2_ReadControlRegister(void) 
{
    return SYCOUNTER2_CONTROL;
}


/*******************************************************************************
* Function Name: SYCOUNTER2_WriteControlRegister
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
void    SYCOUNTER2_WriteControlRegister(uint8 control) 
{
    SYCOUNTER2_CONTROL = control;
}

#endif  /* (!SYCOUNTER2_ControlRegRemoved) */


#if (!(SYCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: SYCOUNTER2_WriteCounter
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
void SYCOUNTER2_WriteCounter(uint16 counter) \
                                   
{
    #if(SYCOUNTER2_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (SYCOUNTER2_GLOBAL_ENABLE & SYCOUNTER2_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        SYCOUNTER2_GLOBAL_ENABLE |= SYCOUNTER2_BLOCK_EN_MASK;
        CY_SET_REG16(SYCOUNTER2_COUNTER_LSB_PTR, (uint16)counter);
        SYCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SYCOUNTER2_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(SYCOUNTER2_COUNTER_LSB_PTR, counter);
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}
#endif /* (!(SYCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: SYCOUNTER2_ReadCounter
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
uint16 SYCOUNTER2_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(SYCOUNTER2_UsingFixedFunction)
		(void)CY_GET_REG16(SYCOUNTER2_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(SYCOUNTER2_COUNTER_LSB_PTR_8BIT);
	#endif/* (SYCOUNTER2_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(SYCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_ReadCapture
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
uint16 SYCOUNTER2_ReadCapture(void) 
{
    #if(SYCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_WritePeriod
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
void SYCOUNTER2_WritePeriod(uint16 period) 
{
    #if(SYCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SYCOUNTER2_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(SYCOUNTER2_PERIOD_LSB_PTR, period);
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_ReadPeriod
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
uint16 SYCOUNTER2_ReadPeriod(void) 
{
    #if(SYCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYCOUNTER2_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(SYCOUNTER2_PERIOD_LSB_PTR));
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


#if (!SYCOUNTER2_UsingFixedFunction)
/*******************************************************************************
* Function Name: SYCOUNTER2_WriteCompare
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
void SYCOUNTER2_WriteCompare(uint16 compare) \
                                   
{
    #if(SYCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SYCOUNTER2_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(SYCOUNTER2_COMPARE_LSB_PTR, compare);
    #endif /* (SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_ReadCompare
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
uint16 SYCOUNTER2_ReadCompare(void) 
{
    return (CY_GET_REG16(SYCOUNTER2_COMPARE_LSB_PTR));
}


#if (SYCOUNTER2_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYCOUNTER2_SetCompareMode
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
void SYCOUNTER2_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    SYCOUNTER2_CONTROL &= ((uint8)(~SYCOUNTER2_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    SYCOUNTER2_CONTROL |= compareMode;
}
#endif  /* (SYCOUNTER2_COMPARE_MODE_SOFTWARE) */


#if (SYCOUNTER2_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYCOUNTER2_SetCaptureMode
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
void SYCOUNTER2_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    SYCOUNTER2_CONTROL &= ((uint8)(~SYCOUNTER2_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    SYCOUNTER2_CONTROL |= ((uint8)((uint8)captureMode << SYCOUNTER2_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (SYCOUNTER2_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: SYCOUNTER2_ClearFIFO
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
void SYCOUNTER2_ClearFIFO(void) 
{

    while(0u != (SYCOUNTER2_ReadStatusRegister() & SYCOUNTER2_STATUS_FIFONEMP))
    {
        (void)SYCOUNTER2_ReadCapture();
    }

}
#endif  /* (!SYCOUNTER2_UsingFixedFunction) */


/* [] END OF FILE */


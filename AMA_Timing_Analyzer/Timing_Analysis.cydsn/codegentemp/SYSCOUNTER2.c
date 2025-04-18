/*******************************************************************************
* File Name: SYSCOUNTER2.c  
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

#include "SYSCOUNTER2.h"

uint8 SYSCOUNTER2_initVar = 0u;


/*******************************************************************************
* Function Name: SYSCOUNTER2_Init
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
void SYSCOUNTER2_Init(void) 
{
        #if (!SYSCOUNTER2_UsingFixedFunction && !SYSCOUNTER2_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!SYSCOUNTER2_UsingFixedFunction && !SYSCOUNTER2_ControlRegRemoved) */
        
        #if(!SYSCOUNTER2_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 SYSCOUNTER2_interruptState;
        #endif /* (!SYSCOUNTER2_UsingFixedFunction) */
        
        #if (SYSCOUNTER2_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            SYSCOUNTER2_CONTROL &= SYSCOUNTER2_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                SYSCOUNTER2_CONTROL2 &= ((uint8)(~SYSCOUNTER2_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                SYSCOUNTER2_CONTROL3 &= ((uint8)(~SYSCOUNTER2_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (SYSCOUNTER2_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                SYSCOUNTER2_CONTROL |= SYSCOUNTER2_ONESHOT;
            #endif /* (SYSCOUNTER2_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            SYSCOUNTER2_CONTROL2 |= SYSCOUNTER2_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            SYSCOUNTER2_RT1 &= ((uint8)(~SYSCOUNTER2_RT1_MASK));
            SYSCOUNTER2_RT1 |= SYSCOUNTER2_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            SYSCOUNTER2_RT1 &= ((uint8)(~SYSCOUNTER2_SYNCDSI_MASK));
            SYSCOUNTER2_RT1 |= SYSCOUNTER2_SYNCDSI_EN;

        #else
            #if(!SYSCOUNTER2_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = SYSCOUNTER2_CONTROL & ((uint8)(~SYSCOUNTER2_CTRL_CMPMODE_MASK));
            SYSCOUNTER2_CONTROL = ctrl | SYSCOUNTER2_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = SYSCOUNTER2_CONTROL & ((uint8)(~SYSCOUNTER2_CTRL_CAPMODE_MASK));
            
            #if( 0 != SYSCOUNTER2_CAPTURE_MODE_CONF)
                SYSCOUNTER2_CONTROL = ctrl | SYSCOUNTER2_DEFAULT_CAPTURE_MODE;
            #else
                SYSCOUNTER2_CONTROL = ctrl;
            #endif /* 0 != SYSCOUNTER2_CAPTURE_MODE */ 
            
            #endif /* (!SYSCOUNTER2_ControlRegRemoved) */
        #endif /* (SYSCOUNTER2_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!SYSCOUNTER2_UsingFixedFunction)
            SYSCOUNTER2_ClearFIFO();
        #endif /* (!SYSCOUNTER2_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        SYSCOUNTER2_WritePeriod(SYSCOUNTER2_INIT_PERIOD_VALUE);
        #if (!(SYSCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
            SYSCOUNTER2_WriteCounter(SYSCOUNTER2_INIT_COUNTER_VALUE);
        #endif /* (!(SYSCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */
        SYSCOUNTER2_SetInterruptMode(SYSCOUNTER2_INIT_INTERRUPTS_MASK);
        
        #if (!SYSCOUNTER2_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)SYSCOUNTER2_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            SYSCOUNTER2_WriteCompare(SYSCOUNTER2_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            SYSCOUNTER2_interruptState = CyEnterCriticalSection();
            
            SYSCOUNTER2_STATUS_AUX_CTRL |= SYSCOUNTER2_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(SYSCOUNTER2_interruptState);
            
        #endif /* (!SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_Enable
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
void SYSCOUNTER2_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (SYSCOUNTER2_UsingFixedFunction)
        SYSCOUNTER2_GLOBAL_ENABLE |= SYSCOUNTER2_BLOCK_EN_MASK;
        SYSCOUNTER2_GLOBAL_STBY_ENABLE |= SYSCOUNTER2_BLOCK_STBY_EN_MASK;
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!SYSCOUNTER2_ControlRegRemoved || SYSCOUNTER2_UsingFixedFunction)
        SYSCOUNTER2_CONTROL |= SYSCOUNTER2_CTRL_ENABLE;                
    #endif /* (!SYSCOUNTER2_ControlRegRemoved || SYSCOUNTER2_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_Start
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
*  SYSCOUNTER2_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void SYSCOUNTER2_Start(void) 
{
    if(SYSCOUNTER2_initVar == 0u)
    {
        SYSCOUNTER2_Init();
        
        SYSCOUNTER2_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    SYSCOUNTER2_Enable();        
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_Stop
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
void SYSCOUNTER2_Stop(void) 
{
    /* Disable Counter */
    #if(!SYSCOUNTER2_ControlRegRemoved || SYSCOUNTER2_UsingFixedFunction)
        SYSCOUNTER2_CONTROL &= ((uint8)(~SYSCOUNTER2_CTRL_ENABLE));        
    #endif /* (!SYSCOUNTER2_ControlRegRemoved || SYSCOUNTER2_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (SYSCOUNTER2_UsingFixedFunction)
        SYSCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SYSCOUNTER2_BLOCK_EN_MASK));
        SYSCOUNTER2_GLOBAL_STBY_ENABLE &= ((uint8)(~SYSCOUNTER2_BLOCK_STBY_EN_MASK));
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_SetInterruptMode
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
void SYSCOUNTER2_SetInterruptMode(uint8 interruptsMask) 
{
    SYSCOUNTER2_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadStatusRegister
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
uint8   SYSCOUNTER2_ReadStatusRegister(void) 
{
    return SYSCOUNTER2_STATUS;
}


#if(!SYSCOUNTER2_ControlRegRemoved)
/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadControlRegister
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
uint8   SYSCOUNTER2_ReadControlRegister(void) 
{
    return SYSCOUNTER2_CONTROL;
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_WriteControlRegister
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
void    SYSCOUNTER2_WriteControlRegister(uint8 control) 
{
    SYSCOUNTER2_CONTROL = control;
}

#endif  /* (!SYSCOUNTER2_ControlRegRemoved) */


#if (!(SYSCOUNTER2_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: SYSCOUNTER2_WriteCounter
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
void SYSCOUNTER2_WriteCounter(uint16 counter) \
                                   
{
    #if(SYSCOUNTER2_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (SYSCOUNTER2_GLOBAL_ENABLE & SYSCOUNTER2_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        SYSCOUNTER2_GLOBAL_ENABLE |= SYSCOUNTER2_BLOCK_EN_MASK;
        CY_SET_REG16(SYSCOUNTER2_COUNTER_LSB_PTR, (uint16)counter);
        SYSCOUNTER2_GLOBAL_ENABLE &= ((uint8)(~SYSCOUNTER2_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(SYSCOUNTER2_COUNTER_LSB_PTR, counter);
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}
#endif /* (!(SYSCOUNTER2_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadCounter
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
uint16 SYSCOUNTER2_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(SYSCOUNTER2_UsingFixedFunction)
		(void)CY_GET_REG16(SYSCOUNTER2_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(SYSCOUNTER2_COUNTER_LSB_PTR_8BIT);
	#endif/* (SYSCOUNTER2_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(SYSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadCapture
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
uint16 SYSCOUNTER2_ReadCapture(void) 
{
    #if(SYSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER2_STATICCOUNT_LSB_PTR));
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_WritePeriod
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
void SYSCOUNTER2_WritePeriod(uint16 period) 
{
    #if(SYSCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SYSCOUNTER2_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(SYSCOUNTER2_PERIOD_LSB_PTR, period);
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadPeriod
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
uint16 SYSCOUNTER2_ReadPeriod(void) 
{
    #if(SYSCOUNTER2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER2_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER2_PERIOD_LSB_PTR));
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


#if (!SYSCOUNTER2_UsingFixedFunction)
/*******************************************************************************
* Function Name: SYSCOUNTER2_WriteCompare
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
void SYSCOUNTER2_WriteCompare(uint16 compare) \
                                   
{
    #if(SYSCOUNTER2_UsingFixedFunction)
        CY_SET_REG16(SYSCOUNTER2_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(SYSCOUNTER2_COMPARE_LSB_PTR, compare);
    #endif /* (SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_ReadCompare
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
uint16 SYSCOUNTER2_ReadCompare(void) 
{
    return (CY_GET_REG16(SYSCOUNTER2_COMPARE_LSB_PTR));
}


#if (SYSCOUNTER2_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYSCOUNTER2_SetCompareMode
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
void SYSCOUNTER2_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    SYSCOUNTER2_CONTROL &= ((uint8)(~SYSCOUNTER2_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    SYSCOUNTER2_CONTROL |= compareMode;
}
#endif  /* (SYSCOUNTER2_COMPARE_MODE_SOFTWARE) */


#if (SYSCOUNTER2_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYSCOUNTER2_SetCaptureMode
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
void SYSCOUNTER2_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    SYSCOUNTER2_CONTROL &= ((uint8)(~SYSCOUNTER2_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    SYSCOUNTER2_CONTROL |= ((uint8)((uint8)captureMode << SYSCOUNTER2_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (SYSCOUNTER2_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: SYSCOUNTER2_ClearFIFO
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
void SYSCOUNTER2_ClearFIFO(void) 
{

    while(0u != (SYSCOUNTER2_ReadStatusRegister() & SYSCOUNTER2_STATUS_FIFONEMP))
    {
        (void)SYSCOUNTER2_ReadCapture();
    }

}
#endif  /* (!SYSCOUNTER2_UsingFixedFunction) */


/* [] END OF FILE */


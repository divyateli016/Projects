/*******************************************************************************
* File Name: SYSCOUNTER1.c  
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

#include "SYSCOUNTER1.h"

uint8 SYSCOUNTER1_initVar = 0u;


/*******************************************************************************
* Function Name: SYSCOUNTER1_Init
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
void SYSCOUNTER1_Init(void) 
{
        #if (!SYSCOUNTER1_UsingFixedFunction && !SYSCOUNTER1_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!SYSCOUNTER1_UsingFixedFunction && !SYSCOUNTER1_ControlRegRemoved) */
        
        #if(!SYSCOUNTER1_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 SYSCOUNTER1_interruptState;
        #endif /* (!SYSCOUNTER1_UsingFixedFunction) */
        
        #if (SYSCOUNTER1_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            SYSCOUNTER1_CONTROL &= SYSCOUNTER1_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                SYSCOUNTER1_CONTROL2 &= ((uint8)(~SYSCOUNTER1_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                SYSCOUNTER1_CONTROL3 &= ((uint8)(~SYSCOUNTER1_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (SYSCOUNTER1_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                SYSCOUNTER1_CONTROL |= SYSCOUNTER1_ONESHOT;
            #endif /* (SYSCOUNTER1_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            SYSCOUNTER1_CONTROL2 |= SYSCOUNTER1_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            SYSCOUNTER1_RT1 &= ((uint8)(~SYSCOUNTER1_RT1_MASK));
            SYSCOUNTER1_RT1 |= SYSCOUNTER1_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            SYSCOUNTER1_RT1 &= ((uint8)(~SYSCOUNTER1_SYNCDSI_MASK));
            SYSCOUNTER1_RT1 |= SYSCOUNTER1_SYNCDSI_EN;

        #else
            #if(!SYSCOUNTER1_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = SYSCOUNTER1_CONTROL & ((uint8)(~SYSCOUNTER1_CTRL_CMPMODE_MASK));
            SYSCOUNTER1_CONTROL = ctrl | SYSCOUNTER1_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = SYSCOUNTER1_CONTROL & ((uint8)(~SYSCOUNTER1_CTRL_CAPMODE_MASK));
            
            #if( 0 != SYSCOUNTER1_CAPTURE_MODE_CONF)
                SYSCOUNTER1_CONTROL = ctrl | SYSCOUNTER1_DEFAULT_CAPTURE_MODE;
            #else
                SYSCOUNTER1_CONTROL = ctrl;
            #endif /* 0 != SYSCOUNTER1_CAPTURE_MODE */ 
            
            #endif /* (!SYSCOUNTER1_ControlRegRemoved) */
        #endif /* (SYSCOUNTER1_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!SYSCOUNTER1_UsingFixedFunction)
            SYSCOUNTER1_ClearFIFO();
        #endif /* (!SYSCOUNTER1_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        SYSCOUNTER1_WritePeriod(SYSCOUNTER1_INIT_PERIOD_VALUE);
        #if (!(SYSCOUNTER1_UsingFixedFunction && (CY_PSOC5A)))
            SYSCOUNTER1_WriteCounter(SYSCOUNTER1_INIT_COUNTER_VALUE);
        #endif /* (!(SYSCOUNTER1_UsingFixedFunction && (CY_PSOC5A))) */
        SYSCOUNTER1_SetInterruptMode(SYSCOUNTER1_INIT_INTERRUPTS_MASK);
        
        #if (!SYSCOUNTER1_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)SYSCOUNTER1_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            SYSCOUNTER1_WriteCompare(SYSCOUNTER1_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            SYSCOUNTER1_interruptState = CyEnterCriticalSection();
            
            SYSCOUNTER1_STATUS_AUX_CTRL |= SYSCOUNTER1_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(SYSCOUNTER1_interruptState);
            
        #endif /* (!SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_Enable
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
void SYSCOUNTER1_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (SYSCOUNTER1_UsingFixedFunction)
        SYSCOUNTER1_GLOBAL_ENABLE |= SYSCOUNTER1_BLOCK_EN_MASK;
        SYSCOUNTER1_GLOBAL_STBY_ENABLE |= SYSCOUNTER1_BLOCK_STBY_EN_MASK;
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!SYSCOUNTER1_ControlRegRemoved || SYSCOUNTER1_UsingFixedFunction)
        SYSCOUNTER1_CONTROL |= SYSCOUNTER1_CTRL_ENABLE;                
    #endif /* (!SYSCOUNTER1_ControlRegRemoved || SYSCOUNTER1_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_Start
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
*  SYSCOUNTER1_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void SYSCOUNTER1_Start(void) 
{
    if(SYSCOUNTER1_initVar == 0u)
    {
        SYSCOUNTER1_Init();
        
        SYSCOUNTER1_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    SYSCOUNTER1_Enable();        
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_Stop
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
void SYSCOUNTER1_Stop(void) 
{
    /* Disable Counter */
    #if(!SYSCOUNTER1_ControlRegRemoved || SYSCOUNTER1_UsingFixedFunction)
        SYSCOUNTER1_CONTROL &= ((uint8)(~SYSCOUNTER1_CTRL_ENABLE));        
    #endif /* (!SYSCOUNTER1_ControlRegRemoved || SYSCOUNTER1_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (SYSCOUNTER1_UsingFixedFunction)
        SYSCOUNTER1_GLOBAL_ENABLE &= ((uint8)(~SYSCOUNTER1_BLOCK_EN_MASK));
        SYSCOUNTER1_GLOBAL_STBY_ENABLE &= ((uint8)(~SYSCOUNTER1_BLOCK_STBY_EN_MASK));
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_SetInterruptMode
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
void SYSCOUNTER1_SetInterruptMode(uint8 interruptsMask) 
{
    SYSCOUNTER1_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadStatusRegister
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
uint8   SYSCOUNTER1_ReadStatusRegister(void) 
{
    return SYSCOUNTER1_STATUS;
}


#if(!SYSCOUNTER1_ControlRegRemoved)
/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadControlRegister
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
uint8   SYSCOUNTER1_ReadControlRegister(void) 
{
    return SYSCOUNTER1_CONTROL;
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_WriteControlRegister
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
void    SYSCOUNTER1_WriteControlRegister(uint8 control) 
{
    SYSCOUNTER1_CONTROL = control;
}

#endif  /* (!SYSCOUNTER1_ControlRegRemoved) */


#if (!(SYSCOUNTER1_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: SYSCOUNTER1_WriteCounter
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
void SYSCOUNTER1_WriteCounter(uint16 counter) \
                                   
{
    #if(SYSCOUNTER1_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (SYSCOUNTER1_GLOBAL_ENABLE & SYSCOUNTER1_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        SYSCOUNTER1_GLOBAL_ENABLE |= SYSCOUNTER1_BLOCK_EN_MASK;
        CY_SET_REG16(SYSCOUNTER1_COUNTER_LSB_PTR, (uint16)counter);
        SYSCOUNTER1_GLOBAL_ENABLE &= ((uint8)(~SYSCOUNTER1_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(SYSCOUNTER1_COUNTER_LSB_PTR, counter);
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}
#endif /* (!(SYSCOUNTER1_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadCounter
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
uint16 SYSCOUNTER1_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(SYSCOUNTER1_UsingFixedFunction)
		(void)CY_GET_REG16(SYSCOUNTER1_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(SYSCOUNTER1_COUNTER_LSB_PTR_8BIT);
	#endif/* (SYSCOUNTER1_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(SYSCOUNTER1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER1_STATICCOUNT_LSB_PTR));
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadCapture
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
uint16 SYSCOUNTER1_ReadCapture(void) 
{
    #if(SYSCOUNTER1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER1_STATICCOUNT_LSB_PTR));
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_WritePeriod
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
void SYSCOUNTER1_WritePeriod(uint16 period) 
{
    #if(SYSCOUNTER1_UsingFixedFunction)
        CY_SET_REG16(SYSCOUNTER1_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(SYSCOUNTER1_PERIOD_LSB_PTR, period);
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadPeriod
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
uint16 SYSCOUNTER1_ReadPeriod(void) 
{
    #if(SYSCOUNTER1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(SYSCOUNTER1_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(SYSCOUNTER1_PERIOD_LSB_PTR));
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


#if (!SYSCOUNTER1_UsingFixedFunction)
/*******************************************************************************
* Function Name: SYSCOUNTER1_WriteCompare
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
void SYSCOUNTER1_WriteCompare(uint16 compare) \
                                   
{
    #if(SYSCOUNTER1_UsingFixedFunction)
        CY_SET_REG16(SYSCOUNTER1_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(SYSCOUNTER1_COMPARE_LSB_PTR, compare);
    #endif /* (SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_ReadCompare
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
uint16 SYSCOUNTER1_ReadCompare(void) 
{
    return (CY_GET_REG16(SYSCOUNTER1_COMPARE_LSB_PTR));
}


#if (SYSCOUNTER1_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYSCOUNTER1_SetCompareMode
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
void SYSCOUNTER1_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    SYSCOUNTER1_CONTROL &= ((uint8)(~SYSCOUNTER1_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    SYSCOUNTER1_CONTROL |= compareMode;
}
#endif  /* (SYSCOUNTER1_COMPARE_MODE_SOFTWARE) */


#if (SYSCOUNTER1_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: SYSCOUNTER1_SetCaptureMode
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
void SYSCOUNTER1_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    SYSCOUNTER1_CONTROL &= ((uint8)(~SYSCOUNTER1_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    SYSCOUNTER1_CONTROL |= ((uint8)((uint8)captureMode << SYSCOUNTER1_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (SYSCOUNTER1_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: SYSCOUNTER1_ClearFIFO
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
void SYSCOUNTER1_ClearFIFO(void) 
{

    while(0u != (SYSCOUNTER1_ReadStatusRegister() & SYSCOUNTER1_STATUS_FIFONEMP))
    {
        (void)SYSCOUNTER1_ReadCapture();
    }

}
#endif  /* (!SYSCOUNTER1_UsingFixedFunction) */


/* [] END OF FILE */


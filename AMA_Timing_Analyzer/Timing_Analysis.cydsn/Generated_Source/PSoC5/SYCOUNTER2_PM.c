/*******************************************************************************
* File Name: SYCOUNTER2_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
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

static SYCOUNTER2_backupStruct SYCOUNTER2_backup;


/*******************************************************************************
* Function Name: SYCOUNTER2_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  SYCOUNTER2_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void SYCOUNTER2_SaveConfig(void) 
{
    #if (!SYCOUNTER2_UsingFixedFunction)

        SYCOUNTER2_backup.CounterUdb = SYCOUNTER2_ReadCounter();

        #if(!SYCOUNTER2_ControlRegRemoved)
            SYCOUNTER2_backup.CounterControlRegister = SYCOUNTER2_ReadControlRegister();
        #endif /* (!SYCOUNTER2_ControlRegRemoved) */

    #endif /* (!SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  SYCOUNTER2_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SYCOUNTER2_RestoreConfig(void) 
{      
    #if (!SYCOUNTER2_UsingFixedFunction)

       SYCOUNTER2_WriteCounter(SYCOUNTER2_backup.CounterUdb);

        #if(!SYCOUNTER2_ControlRegRemoved)
            SYCOUNTER2_WriteControlRegister(SYCOUNTER2_backup.CounterControlRegister);
        #endif /* (!SYCOUNTER2_ControlRegRemoved) */

    #endif /* (!SYCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYCOUNTER2_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  SYCOUNTER2_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void SYCOUNTER2_Sleep(void) 
{
    #if(!SYCOUNTER2_ControlRegRemoved)
        /* Save Counter's enable state */
        if(SYCOUNTER2_CTRL_ENABLE == (SYCOUNTER2_CONTROL & SYCOUNTER2_CTRL_ENABLE))
        {
            /* Counter is enabled */
            SYCOUNTER2_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            SYCOUNTER2_backup.CounterEnableState = 0u;
        }
    #else
        SYCOUNTER2_backup.CounterEnableState = 1u;
        if(SYCOUNTER2_backup.CounterEnableState != 0u)
        {
            SYCOUNTER2_backup.CounterEnableState = 0u;
        }
    #endif /* (!SYCOUNTER2_ControlRegRemoved) */
    
    SYCOUNTER2_Stop();
    SYCOUNTER2_SaveConfig();
}


/*******************************************************************************
* Function Name: SYCOUNTER2_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  SYCOUNTER2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SYCOUNTER2_Wakeup(void) 
{
    SYCOUNTER2_RestoreConfig();
    #if(!SYCOUNTER2_ControlRegRemoved)
        if(SYCOUNTER2_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            SYCOUNTER2_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!SYCOUNTER2_ControlRegRemoved) */
    
}


/* [] END OF FILE */

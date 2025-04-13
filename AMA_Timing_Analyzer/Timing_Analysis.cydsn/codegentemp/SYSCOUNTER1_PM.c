/*******************************************************************************
* File Name: SYSCOUNTER1_PM.c  
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

#include "SYSCOUNTER1.h"

static SYSCOUNTER1_backupStruct SYSCOUNTER1_backup;


/*******************************************************************************
* Function Name: SYSCOUNTER1_SaveConfig
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
*  SYSCOUNTER1_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void SYSCOUNTER1_SaveConfig(void) 
{
    #if (!SYSCOUNTER1_UsingFixedFunction)

        SYSCOUNTER1_backup.CounterUdb = SYSCOUNTER1_ReadCounter();

        #if(!SYSCOUNTER1_ControlRegRemoved)
            SYSCOUNTER1_backup.CounterControlRegister = SYSCOUNTER1_ReadControlRegister();
        #endif /* (!SYSCOUNTER1_ControlRegRemoved) */

    #endif /* (!SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_RestoreConfig
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
*  SYSCOUNTER1_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SYSCOUNTER1_RestoreConfig(void) 
{      
    #if (!SYSCOUNTER1_UsingFixedFunction)

       SYSCOUNTER1_WriteCounter(SYSCOUNTER1_backup.CounterUdb);

        #if(!SYSCOUNTER1_ControlRegRemoved)
            SYSCOUNTER1_WriteControlRegister(SYSCOUNTER1_backup.CounterControlRegister);
        #endif /* (!SYSCOUNTER1_ControlRegRemoved) */

    #endif /* (!SYSCOUNTER1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_Sleep
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
*  SYSCOUNTER1_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void SYSCOUNTER1_Sleep(void) 
{
    #if(!SYSCOUNTER1_ControlRegRemoved)
        /* Save Counter's enable state */
        if(SYSCOUNTER1_CTRL_ENABLE == (SYSCOUNTER1_CONTROL & SYSCOUNTER1_CTRL_ENABLE))
        {
            /* Counter is enabled */
            SYSCOUNTER1_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            SYSCOUNTER1_backup.CounterEnableState = 0u;
        }
    #else
        SYSCOUNTER1_backup.CounterEnableState = 1u;
        if(SYSCOUNTER1_backup.CounterEnableState != 0u)
        {
            SYSCOUNTER1_backup.CounterEnableState = 0u;
        }
    #endif /* (!SYSCOUNTER1_ControlRegRemoved) */
    
    SYSCOUNTER1_Stop();
    SYSCOUNTER1_SaveConfig();
}


/*******************************************************************************
* Function Name: SYSCOUNTER1_Wakeup
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
*  SYSCOUNTER1_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SYSCOUNTER1_Wakeup(void) 
{
    SYSCOUNTER1_RestoreConfig();
    #if(!SYSCOUNTER1_ControlRegRemoved)
        if(SYSCOUNTER1_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            SYSCOUNTER1_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!SYSCOUNTER1_ControlRegRemoved) */
    
}


/* [] END OF FILE */

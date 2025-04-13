/*******************************************************************************
* File Name: SYSCOUNTER2_PM.c  
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

#include "SYSCOUNTER2.h"

static SYSCOUNTER2_backupStruct SYSCOUNTER2_backup;


/*******************************************************************************
* Function Name: SYSCOUNTER2_SaveConfig
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
*  SYSCOUNTER2_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void SYSCOUNTER2_SaveConfig(void) 
{
    #if (!SYSCOUNTER2_UsingFixedFunction)

        SYSCOUNTER2_backup.CounterUdb = SYSCOUNTER2_ReadCounter();

        #if(!SYSCOUNTER2_ControlRegRemoved)
            SYSCOUNTER2_backup.CounterControlRegister = SYSCOUNTER2_ReadControlRegister();
        #endif /* (!SYSCOUNTER2_ControlRegRemoved) */

    #endif /* (!SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_RestoreConfig
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
*  SYSCOUNTER2_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SYSCOUNTER2_RestoreConfig(void) 
{      
    #if (!SYSCOUNTER2_UsingFixedFunction)

       SYSCOUNTER2_WriteCounter(SYSCOUNTER2_backup.CounterUdb);

        #if(!SYSCOUNTER2_ControlRegRemoved)
            SYSCOUNTER2_WriteControlRegister(SYSCOUNTER2_backup.CounterControlRegister);
        #endif /* (!SYSCOUNTER2_ControlRegRemoved) */

    #endif /* (!SYSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_Sleep
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
*  SYSCOUNTER2_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void SYSCOUNTER2_Sleep(void) 
{
    #if(!SYSCOUNTER2_ControlRegRemoved)
        /* Save Counter's enable state */
        if(SYSCOUNTER2_CTRL_ENABLE == (SYSCOUNTER2_CONTROL & SYSCOUNTER2_CTRL_ENABLE))
        {
            /* Counter is enabled */
            SYSCOUNTER2_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            SYSCOUNTER2_backup.CounterEnableState = 0u;
        }
    #else
        SYSCOUNTER2_backup.CounterEnableState = 1u;
        if(SYSCOUNTER2_backup.CounterEnableState != 0u)
        {
            SYSCOUNTER2_backup.CounterEnableState = 0u;
        }
    #endif /* (!SYSCOUNTER2_ControlRegRemoved) */
    
    SYSCOUNTER2_Stop();
    SYSCOUNTER2_SaveConfig();
}


/*******************************************************************************
* Function Name: SYSCOUNTER2_Wakeup
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
*  SYSCOUNTER2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SYSCOUNTER2_Wakeup(void) 
{
    SYSCOUNTER2_RestoreConfig();
    #if(!SYSCOUNTER2_ControlRegRemoved)
        if(SYSCOUNTER2_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            SYSCOUNTER2_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!SYSCOUNTER2_ControlRegRemoved) */
    
}


/* [] END OF FILE */

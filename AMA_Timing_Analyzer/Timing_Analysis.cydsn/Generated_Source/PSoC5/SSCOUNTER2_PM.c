/*******************************************************************************
* File Name: SSCOUNTER2_PM.c  
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

#include "SSCOUNTER2.h"

static SSCOUNTER2_backupStruct SSCOUNTER2_backup;


/*******************************************************************************
* Function Name: SSCOUNTER2_SaveConfig
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
*  SSCOUNTER2_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void SSCOUNTER2_SaveConfig(void) 
{
    #if (!SSCOUNTER2_UsingFixedFunction)

        SSCOUNTER2_backup.CounterUdb = SSCOUNTER2_ReadCounter();

        #if(!SSCOUNTER2_ControlRegRemoved)
            SSCOUNTER2_backup.CounterControlRegister = SSCOUNTER2_ReadControlRegister();
        #endif /* (!SSCOUNTER2_ControlRegRemoved) */

    #endif /* (!SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_RestoreConfig
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
*  SSCOUNTER2_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SSCOUNTER2_RestoreConfig(void) 
{      
    #if (!SSCOUNTER2_UsingFixedFunction)

       SSCOUNTER2_WriteCounter(SSCOUNTER2_backup.CounterUdb);

        #if(!SSCOUNTER2_ControlRegRemoved)
            SSCOUNTER2_WriteControlRegister(SSCOUNTER2_backup.CounterControlRegister);
        #endif /* (!SSCOUNTER2_ControlRegRemoved) */

    #endif /* (!SSCOUNTER2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SSCOUNTER2_Sleep
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
*  SSCOUNTER2_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void SSCOUNTER2_Sleep(void) 
{
    #if(!SSCOUNTER2_ControlRegRemoved)
        /* Save Counter's enable state */
        if(SSCOUNTER2_CTRL_ENABLE == (SSCOUNTER2_CONTROL & SSCOUNTER2_CTRL_ENABLE))
        {
            /* Counter is enabled */
            SSCOUNTER2_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            SSCOUNTER2_backup.CounterEnableState = 0u;
        }
    #else
        SSCOUNTER2_backup.CounterEnableState = 1u;
        if(SSCOUNTER2_backup.CounterEnableState != 0u)
        {
            SSCOUNTER2_backup.CounterEnableState = 0u;
        }
    #endif /* (!SSCOUNTER2_ControlRegRemoved) */
    
    SSCOUNTER2_Stop();
    SSCOUNTER2_SaveConfig();
}


/*******************************************************************************
* Function Name: SSCOUNTER2_Wakeup
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
*  SSCOUNTER2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SSCOUNTER2_Wakeup(void) 
{
    SSCOUNTER2_RestoreConfig();
    #if(!SSCOUNTER2_ControlRegRemoved)
        if(SSCOUNTER2_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            SSCOUNTER2_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!SSCOUNTER2_ControlRegRemoved) */
    
}


/* [] END OF FILE */

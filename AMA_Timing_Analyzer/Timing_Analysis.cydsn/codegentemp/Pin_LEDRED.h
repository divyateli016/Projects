/*******************************************************************************
* File Name: Pin_LEDRED.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Pin_LEDRED_H) /* Pins Pin_LEDRED_H */
#define CY_PINS_Pin_LEDRED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Pin_LEDRED_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Pin_LEDRED__PORT == 15 && ((Pin_LEDRED__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Pin_LEDRED_Write(uint8 value);
void    Pin_LEDRED_SetDriveMode(uint8 mode);
uint8   Pin_LEDRED_ReadDataReg(void);
uint8   Pin_LEDRED_Read(void);
void    Pin_LEDRED_SetInterruptMode(uint16 position, uint16 mode);
uint8   Pin_LEDRED_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Pin_LEDRED_SetDriveMode() function.
     *  @{
     */
        #define Pin_LEDRED_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Pin_LEDRED_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Pin_LEDRED_DM_RES_UP          PIN_DM_RES_UP
        #define Pin_LEDRED_DM_RES_DWN         PIN_DM_RES_DWN
        #define Pin_LEDRED_DM_OD_LO           PIN_DM_OD_LO
        #define Pin_LEDRED_DM_OD_HI           PIN_DM_OD_HI
        #define Pin_LEDRED_DM_STRONG          PIN_DM_STRONG
        #define Pin_LEDRED_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Pin_LEDRED_MASK               Pin_LEDRED__MASK
#define Pin_LEDRED_SHIFT              Pin_LEDRED__SHIFT
#define Pin_LEDRED_WIDTH              1u

/* Interrupt constants */
#if defined(Pin_LEDRED__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Pin_LEDRED_SetInterruptMode() function.
     *  @{
     */
        #define Pin_LEDRED_INTR_NONE      (uint16)(0x0000u)
        #define Pin_LEDRED_INTR_RISING    (uint16)(0x0001u)
        #define Pin_LEDRED_INTR_FALLING   (uint16)(0x0002u)
        #define Pin_LEDRED_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Pin_LEDRED_INTR_MASK      (0x01u) 
#endif /* (Pin_LEDRED__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Pin_LEDRED_PS                     (* (reg8 *) Pin_LEDRED__PS)
/* Data Register */
#define Pin_LEDRED_DR                     (* (reg8 *) Pin_LEDRED__DR)
/* Port Number */
#define Pin_LEDRED_PRT_NUM                (* (reg8 *) Pin_LEDRED__PRT) 
/* Connect to Analog Globals */                                                  
#define Pin_LEDRED_AG                     (* (reg8 *) Pin_LEDRED__AG)                       
/* Analog MUX bux enable */
#define Pin_LEDRED_AMUX                   (* (reg8 *) Pin_LEDRED__AMUX) 
/* Bidirectional Enable */                                                        
#define Pin_LEDRED_BIE                    (* (reg8 *) Pin_LEDRED__BIE)
/* Bit-mask for Aliased Register Access */
#define Pin_LEDRED_BIT_MASK               (* (reg8 *) Pin_LEDRED__BIT_MASK)
/* Bypass Enable */
#define Pin_LEDRED_BYP                    (* (reg8 *) Pin_LEDRED__BYP)
/* Port wide control signals */                                                   
#define Pin_LEDRED_CTL                    (* (reg8 *) Pin_LEDRED__CTL)
/* Drive Modes */
#define Pin_LEDRED_DM0                    (* (reg8 *) Pin_LEDRED__DM0) 
#define Pin_LEDRED_DM1                    (* (reg8 *) Pin_LEDRED__DM1)
#define Pin_LEDRED_DM2                    (* (reg8 *) Pin_LEDRED__DM2) 
/* Input Buffer Disable Override */
#define Pin_LEDRED_INP_DIS                (* (reg8 *) Pin_LEDRED__INP_DIS)
/* LCD Common or Segment Drive */
#define Pin_LEDRED_LCD_COM_SEG            (* (reg8 *) Pin_LEDRED__LCD_COM_SEG)
/* Enable Segment LCD */
#define Pin_LEDRED_LCD_EN                 (* (reg8 *) Pin_LEDRED__LCD_EN)
/* Slew Rate Control */
#define Pin_LEDRED_SLW                    (* (reg8 *) Pin_LEDRED__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Pin_LEDRED_PRTDSI__CAPS_SEL       (* (reg8 *) Pin_LEDRED__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Pin_LEDRED_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Pin_LEDRED__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Pin_LEDRED_PRTDSI__OE_SEL0        (* (reg8 *) Pin_LEDRED__PRTDSI__OE_SEL0) 
#define Pin_LEDRED_PRTDSI__OE_SEL1        (* (reg8 *) Pin_LEDRED__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Pin_LEDRED_PRTDSI__OUT_SEL0       (* (reg8 *) Pin_LEDRED__PRTDSI__OUT_SEL0) 
#define Pin_LEDRED_PRTDSI__OUT_SEL1       (* (reg8 *) Pin_LEDRED__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Pin_LEDRED_PRTDSI__SYNC_OUT       (* (reg8 *) Pin_LEDRED__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Pin_LEDRED__SIO_CFG)
    #define Pin_LEDRED_SIO_HYST_EN        (* (reg8 *) Pin_LEDRED__SIO_HYST_EN)
    #define Pin_LEDRED_SIO_REG_HIFREQ     (* (reg8 *) Pin_LEDRED__SIO_REG_HIFREQ)
    #define Pin_LEDRED_SIO_CFG            (* (reg8 *) Pin_LEDRED__SIO_CFG)
    #define Pin_LEDRED_SIO_DIFF           (* (reg8 *) Pin_LEDRED__SIO_DIFF)
#endif /* (Pin_LEDRED__SIO_CFG) */

/* Interrupt Registers */
#if defined(Pin_LEDRED__INTSTAT)
    #define Pin_LEDRED_INTSTAT            (* (reg8 *) Pin_LEDRED__INTSTAT)
    #define Pin_LEDRED_SNAP               (* (reg8 *) Pin_LEDRED__SNAP)
    
	#define Pin_LEDRED_0_INTTYPE_REG 		(* (reg8 *) Pin_LEDRED__0__INTTYPE)
#endif /* (Pin_LEDRED__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Pin_LEDRED_H */


/* [] END OF FILE */

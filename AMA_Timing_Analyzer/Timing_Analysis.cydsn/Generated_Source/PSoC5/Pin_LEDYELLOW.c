/*******************************************************************************
* File Name: Pin_LEDYELLOW.c  
* Version 2.20
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Pin_LEDYELLOW.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 Pin_LEDYELLOW__PORT == 15 && ((Pin_LEDYELLOW__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: Pin_LEDYELLOW_Write
****************************************************************************//**
*
* \brief Writes the value to the physical port (data output register), masking
*  and shifting the bits appropriately. 
*
* The data output register controls the signal applied to the physical pin in 
* conjunction with the drive mode parameter. This function avoids changing 
* other bits in the port by using the appropriate method (read-modify-write or
* bit banding).
*
* <b>Note</b> This function should not be used on a hardware digital output pin 
* as it is driven by the hardware signal attached to it.
*
* \param value
*  Value to write to the component instance.
*
* \return 
*  None 
*
* \sideeffect
*  If you use read-modify-write operations that are not atomic; the Interrupt 
*  Service Routines (ISR) can cause corruption of this function. An ISR that 
*  interrupts this function and performs writes to the Pins component data 
*  register can cause corrupted port data. To avoid this issue, you should 
*  either use the Per-Pin APIs (primary method) or disable interrupts around 
*  this function.
*
* \funcusage
*  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_Write
*******************************************************************************/
void Pin_LEDYELLOW_Write(uint8 value)
{
    uint8 staticBits = (Pin_LEDYELLOW_DR & (uint8)(~Pin_LEDYELLOW_MASK));
    Pin_LEDYELLOW_DR = staticBits | ((uint8)(value << Pin_LEDYELLOW_SHIFT) & Pin_LEDYELLOW_MASK);
}


/*******************************************************************************
* Function Name: Pin_LEDYELLOW_SetDriveMode
****************************************************************************//**
*
* \brief Sets the drive mode for each of the Pins component's pins.
* 
* <b>Note</b> This affects all pins in the Pins component instance. Use the
* Per-Pin APIs if you wish to control individual pin's drive modes.
*
* \param mode
*  Mode for the selected signals. Valid options are documented in 
*  \ref driveMode.
*
* \return
*  None
*
* \sideeffect
*  If you use read-modify-write operations that are not atomic, the ISR can
*  cause corruption of this function. An ISR that interrupts this function 
*  and performs writes to the Pins component Drive Mode registers can cause 
*  corrupted port data. To avoid this issue, you should either use the Per-Pin
*  APIs (primary method) or disable interrupts around this function.
*
* \funcusage
*  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_SetDriveMode
*******************************************************************************/
void Pin_LEDYELLOW_SetDriveMode(uint8 mode)
{
	CyPins_SetPinDriveMode(Pin_LEDYELLOW_0, mode);
}


/*******************************************************************************
* Function Name: Pin_LEDYELLOW_Read
****************************************************************************//**
*
* \brief Reads the associated physical port (pin status register) and masks 
*  the required bits according to the width and bit position of the component
*  instance. 
*
* The pin's status register returns the current logic level present on the 
* physical pin.
*
* \return 
*  The current value for the pins in the component as a right justified number.
*
* \funcusage
*  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_Read  
*******************************************************************************/
uint8 Pin_LEDYELLOW_Read(void)
{
    return (Pin_LEDYELLOW_PS & Pin_LEDYELLOW_MASK) >> Pin_LEDYELLOW_SHIFT;
}


/*******************************************************************************
* Function Name: Pin_LEDYELLOW_ReadDataReg
****************************************************************************//**
*
* \brief Reads the associated physical port's data output register and masks 
*  the correct bits according to the width and bit position of the component 
*  instance. 
*
* The data output register controls the signal applied to the physical pin in 
* conjunction with the drive mode parameter. This is not the same as the 
* preferred Pin_LEDYELLOW_Read() API because the 
* Pin_LEDYELLOW_ReadDataReg() reads the data register instead of the status 
* register. For output pins this is a useful function to determine the value 
* just written to the pin.
*
* \return 
*  The current value of the data register masked and shifted into a right 
*  justified number for the component instance.
*
* \funcusage
*  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_ReadDataReg 
*******************************************************************************/
uint8 Pin_LEDYELLOW_ReadDataReg(void)
{
    return (Pin_LEDYELLOW_DR & Pin_LEDYELLOW_MASK) >> Pin_LEDYELLOW_SHIFT;
}


/* If interrupt is connected for this Pins component */ 
#if defined(Pin_LEDYELLOW_INTSTAT) 

    /*******************************************************************************
    * Function Name: Pin_LEDYELLOW_SetInterruptMode
    ****************************************************************************//**
    *
    * \brief Configures the interrupt mode for each of the Pins component's
    *  pins. Alternatively you may set the interrupt mode for all the pins
    *  specified in the Pins component.
    *
    *  <b>Note</b> The interrupt is port-wide and therefore any enabled pin
    *  interrupt may trigger it.
    *
    * \param position
    *  The pin position as listed in the Pins component. You may OR these to be 
    *  able to configure the interrupt mode of multiple pins within a Pins 
    *  component. Or you may use Pin_LEDYELLOW_INTR_ALL to configure the
    *  interrupt mode of all the pins in the Pins component.       
    *  - Pin_LEDYELLOW_0_INTR       (First pin in the list)
    *  - Pin_LEDYELLOW_1_INTR       (Second pin in the list)
    *  - ...
    *  - Pin_LEDYELLOW_INTR_ALL     (All pins in Pins component)
    *
    * \param mode
    *  Interrupt mode for the selected pins. Valid options are documented in
    *  \ref intrMode.
    *
    * \return 
    *  None
    *  
    * \sideeffect
    *  It is recommended that the interrupt be disabled before calling this 
    *  function to avoid unintended interrupt requests. Note that the interrupt
    *  type is port wide, and therefore will trigger for any enabled pin on the 
    *  port.
    *
    * \funcusage
    *  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_SetInterruptMode
    *******************************************************************************/
    void Pin_LEDYELLOW_SetInterruptMode(uint16 position, uint16 mode)
    {
		if((position & Pin_LEDYELLOW_0_INTR) != 0u) 
		{ 
			 Pin_LEDYELLOW_0_INTTYPE_REG = (uint8)mode; 
		}
    }
    
    
    /*******************************************************************************
    * Function Name: Pin_LEDYELLOW_ClearInterrupt
    ****************************************************************************//**
    *
    * \brief Clears any active interrupts attached with the component and returns 
    *  the value of the interrupt status register allowing determination of which
    *  pins generated an interrupt event.
    *
    * \return 
    *  The right-shifted current value of the interrupt status register. Each pin 
    *  has one bit set if it generated an interrupt event. For example, bit 0 is 
    *  for pin 0 and bit 1 is for pin 1 of the Pins component.
    *  
    * \sideeffect
    *  Clears all bits of the physical port's interrupt status register, not just
    *  those associated with the Pins component.
    *
    * \funcusage
    *  \snippet Pin_LEDYELLOW_SUT.c usage_Pin_LEDYELLOW_ClearInterrupt
    *******************************************************************************/
    uint8 Pin_LEDYELLOW_ClearInterrupt(void)
    {
        return (Pin_LEDYELLOW_INTSTAT & Pin_LEDYELLOW_MASK) >> Pin_LEDYELLOW_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */

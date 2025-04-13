
#ifndef Analyzer_H
#define Analyzer_H

//Defining the number for output pins 
#define RED_LED 1
#define GREEN_LED 2
#define YELLOW_LED 3
    
#include "stdint.h"
#include "string.h"
    
 extern volatile uint32_t timer;

//Enum to know the status of Analyzer
typedef enum 
{
    STATUS_RUNNING,
    STATUS_PAUSED,
    STATUS_STOPPED,
    STATUS_RESUME
}AnalyzerStatus_t;

//Enum for mode of the Analyzer
typedef enum
{
   DWT_Cycle_Counter,
   DWT_Cycle_Counter_Output_Pin,
   SYSTICK,
   SYSTICK_Output_Pin,
   Output_Pin_only 
}AnalyzerMode;

//Definig  the members of each Analyzer mode
typedef struct 
{
    uint8_t id;
    uint16_t pin;
    char name[20];
    AnalyzerMode Mode;//Enum as a member of struct to access the Modes of timing analyzer
   AnalyzerStatus_t Status;//Enum as a member of structure to check the status of Analyzers;
    uint32_t StopCount;
    uint32_t StartCount;// Is for providing cPU Cycles
    uint32_t PauseCycle;//This hold the value of the counter when the analyzer is Paused
    uint32_t totalCycle;//This is to calculate the the elapsed time between start of the counter and when it is paused
    void (*outputPinFunc)(void);

    
}Analyzer_t;

//Function declaration to initialize the Data watch point and trace enable 
void timing_Analyzer_init(void); //This function defines the DWT initialization by enabling the bit maks of the control registers

void SysTick_Timing_Analyzer_init(uint32_t);//Function definition for initialializing the systick timer

void Time_Analyzer_Create(Analyzer_t* me,uint8_t id,AnalyzerMode Mode,char *name);//This is to create the function timer by passing the IDs

void Analyzer_Start(Analyzer_t *analyzer);//This function will take the pointer to structure as variable

void Analyzer_Stop(Analyzer_t *analyzer);

void Analyzer_Pause(Analyzer_t *analyzer);

void Analyzer_Resume(Analyzer_t *analyzer);

void Analyzer_printStatus(Analyzer_t *analyzer);

void UART_LOG_PutString(const char *str);// this is to print the status of timing analyzer using UART

void REDLED_START();
void GREEN_LED_START();
void YELLOWLED_START();

void REDLED_STOP();
void GREEN_LED_STOP();
void YELLOWLED_STOP();

void SysTick_Handler(void);

#endif


/* [] END OF FILE */

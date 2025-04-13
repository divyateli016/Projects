/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include "project.h"
#include "Analyzer.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

/*This function definition is to initialze the DWT Counter and SYSTIC Counter

*/
//Extra Variables initialiyation
volatile uint32_t timer  = 0;
volatile uint32_t isPaused  = 0;


void timing_Analyzer_init()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;//This to enable the Tracebility mask
    
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;// This is to enable the Cyclic counter enable mas
    
    DWT->CYCCNT = 0;// restetting the timer
    
    CySysTickInit();
    CySysTickSetCallback(0,SysTick_Handler);
    CySysTickSetReload(24000-1);
    
}
/* this function definition is to assing the reload value to provide the tickTime of 1 milli second 
* @param reloadValue is the value calculated based on our CPU Frequency to get the desires systick time.
* if the clock frequency id 24 MHZ to get the 1 ms of tick time we need reloadValue of 24,000
* 26 MHZ means 26,000,000 cycles/second


*/


/* this function is to initialize the structure members
*@param uint16_t id : is an input represnts the id of the analyzer
*@param Analyzer_t* analyzer is the strcure we are passing to the function as a constructor to initialize the structure variables
*@AnalyzerMode Mode this is the enum represeting the Mode of timing analysis such as DWT or systick declared in the enum
*param void(*outputPin)(void) this is an function pointer points to the pin operations sunch as LED green red and white
*/

void Time_Analyzer_Create(Analyzer_t* me,uint8_t id,AnalyzerMode Mode,char* name)
{
    me->id = id;
    me->Mode = Mode;
    me->Status = STATUS_STOPPED;
    me->StopCount = 0;
    me->StartCount = 0;
    me->PauseCycle = 0;
    me->totalCycle = 0;
   strncpy(me->name, name, sizeof(me->name)-1);// StrnCpy gives the length of the stringa nd itwill not copy the null character
   me->name[sizeof(me->name)-1] = '\0';//assigning the last character as null character
}
/*
This function definition starts the timing analyzer for DWY_CYCLE ONLY by starting the DWT CYCLE COUNTER
*/
void Analyzer_Start(Analyzer_t *me)
{
    char StrMsg[50];
   // char Buffer[50];// Print the status of analyzer
    if(me->Status == STATUS_STOPPED&&(me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
    {
        
        me->StartCount = DWT->CYCCNT;// The currentely counted CPU Cycles are stoored in the CycleCount
        utoa(me->StartCount,StrMsg,10);
        UART_LOG_PutString(StrMsg);
        UART_LOG_PutString("\r\n");
        
        if(me->id == RED_LED)
        REDLED_START(1);
        else if(me->id == GREEN_LED)
        GREEN_LED_START();
        else if(me->id == YELLOW_LED)
        YELLOWLED_START();
        
        me->totalCycle = 0;// resetting the total cycle 
        me->Status = STATUS_RUNNING;
        //sprintf(StrMsg,"Analyzer %s is Running",me->name);
        //UART_LOG_PutString(StrMsg);
    }
    else if(me->Status == STATUS_STOPPED&&(me->Mode == SYSTICK || me->Mode == SYSTICK_Output_Pin))
    {
        CySysTickEnable();//Enabling the counter and it starts counting 
        me->totalCycle = 0;
        isPaused = 0;
        me->StartCount = timer;
        
        utoa(me->StartCount,StrMsg,10);
        UART_LOG_PutString(StrMsg);
        UART_LOG_PutString("\r\n");
        
        if(me->id == RED_LED)
        REDLED_START(1);
        else if(me->id == GREEN_LED)
        GREEN_LED_START();
        else if(me->id == YELLOW_LED)
        YELLOWLED_START();
        
        me->Status = STATUS_RUNNING;
    }
    
}

/*
this function is provides the definition for pausing the counter and reading the cycle count time to calculate the elapsed time

*/

void Analyzer_Pause(Analyzer_t *me)
{
    char StrMsg[50];// Variable to print the count in UART using utoa function
    if(me->Status == STATUS_RUNNING && (me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
    {
        me->PauseCycle = DWT->CYCCNT;//Reading the counter value when it is pasued and assigning it to the pausecycle variable;
        utoa(me->PauseCycle,StrMsg,10);
        UART_LOG_PutString(StrMsg);
        UART_LOG_PutString("\r\n");
        me->totalCycle = me->totalCycle +(me->PauseCycle - me->StartCount);// This is to calculate the elapsed time in zerms of number of cycles
        me->Status = STATUS_PAUSED;
       // utoa(me->totalCycle,StrMsg,10);
        //UART_LOG_PutString(StrMsg);
        //UART_LOG_PutString("\r\n");
        
    }
    else if(me->Status == STATUS_RUNNING &&(me->Mode == SYSTICK || me->Mode == SYSTICK_Output_Pin))
    {
    char strMsg[20];
    me->PauseCycle = timer;
    isPaused = 1;
    me->totalCycle = me->PauseCycle - me->StartCount;
    //sprintf(strMsg,"Elapsed time when paused is %u\r\n",me->totalCycle);
    //UART_LOG_PutString(strMsg);
    CySysTickStop();
    me->Status = STATUS_PAUSED;
    //Pin_REDLED_Write(0);
    }
}

//Function definition to resum the analyzer
void Analyzer_Resume(Analyzer_t *me)
{
    char strmsg[20];
    if(me->Status == STATUS_PAUSED &&(me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
    {
        me->StartCount = DWT->CYCCNT;//reading the counter value after resuming 
        utoa(me->StartCount,strmsg,10);
        UART_LOG_PutString(strmsg);
        UART_LOG_PutString("\r\n");
        /*
        me->totalCycle = me->totalCycle +(me->PauseCycle - me->StartCount);
        utoa(me->totalCycle,strmsg,10);
        UART_LOG_PutString(strmsg);
        UART_LOG_PutString("\r\n");
        */
        me->Status = STATUS_RUNNING;
    }
    else if(me->Status == STATUS_PAUSED &&(me->Mode == SYSTICK || me->Mode == SYSTICK_Output_Pin))
    {
        //Enabling the counter and it starts counting 
        CySysTickEnable();
        me->StartCount = timer;
        isPaused = 0;
        me->Status = STATUS_RUNNING;
        //Printing Resume timing
        utoa(me->StartCount,strmsg,10);
        UART_LOG_PutString(strmsg);
        UART_LOG_PutString("\r\n");
    }
    
    
}
/*
void Analyzer_Stop(Analyzer_t *me)
{
    
    char StrMsg[20];
    if(me->Status == STATUS_RUNNING || me->Status == STATUS_PAUSED)
    {
        if(me->Status == STATUS_RUNNING && (me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
        {
         me->StopCount = DWT->CYCCNT;// The currentely counted CPU Cycles are stoored in the CycleCount
   
         utoa(me->StopCount,StrMsg,10);
         UART_LOG_PutString(StrMsg);
        UART_LOG_PutString("\r\n");
        
        me->totalCycle = me->totalCycle+(me->StopCount - me->StartCount);
        me->Status = STATUS_STOPPED;
        
        utoa(me->totalCycle,StrMsg,10);
        UART_LOG_PutString(StrMsg);
        UART_LOG_PutString("\r\n");
        
        if(me->id == RED_LED)
        REDLED_STOP();
        else if(me->id == GREEN_LED)
        GREEN_LED_STOP();
        else if(me->id == YELLOW_LED)
        YELLOWLED_STOP();
        }
        else if(me->Status == STATUS_RUNNING && (me->Mode == SYSTICK || me->Mode == SYSTICK_Output_Pin))
        {
            me->PauseCycle= timer;
            if(isPaused ==1)
            {
                me->totalCycle = me->PauseCycle;
            }
            else
            {
                me->totalCycle = me->totalCycle + (me->PauseCycle-me->StartCount);
            }
            utoa(me->totalCycle,StrMsg,10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
            CySysTickStop();
        }
        else if(me->Status == STATUS_PAUSED || (me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
        {
            me->StopCount = DWT->CYCCNT;// The currentely counted CPU Cycles are stoored in the CycleCount
            utoa(me->StartCount,StrMsg,10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
            
            me->totalCycle = me->totalCycle+(me->PauseCycle - me->StopCount);
            me->Status = STATUS_STOPPED;
            
            utoa(me->totalCycle,StrMsg,10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
        }
    }
}
*/

void Analyzer_Stop(Analyzer_t *me)
{
    char StrMsg[50];  // Increased buffer size for safety

    if (me->Status == STATUS_RUNNING || me->Status == STATUS_PAUSED)
    {
        // Handle DWT Cycle Counter Mode
        if ((me->Mode == DWT_Cycle_Counter || me->Mode == DWT_Cycle_Counter_Output_Pin))
        {
            me->StopCount = DWT->CYCCNT;  // Store current cycle count

            // Print stop count
            utoa(me->StopCount, StrMsg, 10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
            
            if (me->id == RED_LED)
                REDLED_STOP();
            else if (me->id == GREEN_LED)
                GREEN_LED_STOP();
            else if (me->id == YELLOW_LED)
                YELLOWLED_STOP();

            // Update total cycles
            if (me->Status == STATUS_RUNNING)
            {
                me->totalCycle += (me->StopCount - me->StartCount);
            }
            else  // STATUS_PAUSED
            {
                me->totalCycle += (me->PauseCycle - me->StartCount);
            }

            // Print total cycles
            utoa(me->totalCycle, StrMsg, 10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");

            // Stop the LED if used
//            if (me->id == RED_LED)
//                REDLED_STOP();
//            else if (me->id == GREEN_LED)
//                GREEN_LED_STOP();
//            else if (me->id == YELLOW_LED)
//                YELLOWLED_STOP();

            // Update status
            me->Status = STATUS_STOPPED;
        }
        // Handle SysTick Mode
        else if (me->Mode == SYSTICK || me->Mode == SYSTICK_Output_Pin)
        {
            me->PauseCycle = timer;  // Assume 'timer' holds the current SysTick value
            
            utoa(me->PauseCycle, StrMsg, 10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
            
            // Calculate total cycles
            if (isPaused == 1)
            {
                me->totalCycle = me->PauseCycle;
            }
            else
            {
                me->totalCycle += (me->PauseCycle - me->StartCount);
            }

            // Print total cycles
            utoa(me->totalCycle, StrMsg, 10);
            UART_LOG_PutString(StrMsg);
            UART_LOG_PutString("\r\n");
            
            

            // Stop SysTick
            CySysTickStop();

             //Stop the LED if used
            if (me->id == RED_LED)
                REDLED_STOP();
            else if (me->id == GREEN_LED)
                GREEN_LED_STOP();
            else if (me->id == YELLOW_LED)
                YELLOWLED_STOP();

            // Update status
            me->Status = STATUS_STOPPED;
        }
    }
  
}
// Tp print the status of Analyzers
void Analyzer_printStatus(Analyzer_t *me)
{
    char statusMsg[50];
    if(me->Status == STATUS_RUNNING)
    {
        if(me->Status == STATUS_PAUSED)
        sprintf(statusMsg,"Analyzer is Paused");
        else
        sprintf(statusMsg,"Analyzer is Running");
    }
    else
    {
        sprintf(statusMsg,"Analyzer is Stopped");
    }
    
    UART_LOG_PutString(statusMsg);

}


void test_cycle_counter() 
{
    DWT->CYCCNT = 0; // Reset counter
    for (volatile int i = 0; i < 100000; i++) {
        // Just a busy loop
    }
    uint32_t count = DWT->CYCCNT;
    char buffer[20];
    utoa(count, buffer, 10);
    UART_LOG_PutString(buffer);
    UART_LOG_PutString("\r\n");
}
void REDLED_START()
{
    Pin_LEDRED_Write(1);
}
void GREEN_LED_START()
{
    Pin_LEDGREEN_Write(1);
}
void YELLOWLED_START()
{
    Pin_LEDYELLOW_Write(1);
}
void REDLED_STOP()
{
    Pin_LEDRED_Write(0);
}
void GREEN_LED_STOP()
{
    Pin_LEDGREEN_Write(0);
}
void YELLOWLED_STOP()
{
    Pin_LEDYELLOW_Write(0);
}

//Systick handle which increments every specified time 
void SysTick_Handler()
{
     
    timer++;
}



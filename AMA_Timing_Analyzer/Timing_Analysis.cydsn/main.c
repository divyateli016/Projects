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
#include "project.h"
#include "Analyzer.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

//Declaration of interrupts
//CY_ISR_PROTO(isr_1msecTick);
//CY_ISR_PROTO(isr_2SecTick);
//
 Analyzer_t TimingAnalyzer;
 Analyzer_t TimingAnalyzer1;


int main(void)
{
    UART_LOG_Start();
    timing_Analyzer_init();//Initializing the DWT CYCLE COUNTER
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /**
    
    isr_1msecTick_StartEx(isr_1msecTick);
    isr_2SecTick_StartEx(isr_2SecTick);
    
    CLOCK1_Start();
    CLOCK2_Start();
    
     SYSCOUNTER1_Start();
     SYSCOUNTER2_Start();
    
    **/
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    char StrMsg[50];
    volatile uint32_t sum = 0;
    volatile unsigned product = 1;
    volatile float quotient = 1;
    volatile float fsum = 0;
    volatile int mul = 1;
    volatile float result=0.0f;
    
//    char strMsg[60];
//    float add = 0.0f;
  
   
    
    CyDelay(5000);// just to start the Logic Analyzer
    
    //Calling the function to initialize the DWT counter
    
   // Time_Analyzer_Create(&TimingAnalyzer,0,SYSTICK,"Analyzer1");
    
     //Time_Analyzer_Create(&TimingAnalyzer,0,DWT_Cycle_Counter,"Analyzer1");
    
    //Time_Analyzer_Create(&TimingAnalyzer,0,SYSTICK,"Analyzer1");//Since no output pins are use poinetr function is assigned to null pointer
    
    //Time_Analyzer_Create(&TimingAnalyzer,RED_LED,SYSTICK_Output_Pin,"Analyzer1"); 
    
    Time_Analyzer_Create(&TimingAnalyzer,RED_LED,DWT_Cycle_Counter_Output_Pin,"Analyzer2");
    
  // test_cycle_counter();
  //Analyzer_Start(&TimingAnalyzer);
  //Analyzer_printStatus(&TimingAnalyzer);

/* This Function is to add the delay between Start and stop of timing analyzer*/
/*This example has executed for DWT Cycle Counter
   CyDelay(5000);

   Analyzer_Pause(&TimingAnalyzer);

   CyDelay(2000);

   Analyzer_Resume(&TimingAnalyzer);
   
  CyDelay(2000);

   Analyzer_Stop(&TimingAnalyzer);
Analyzer_printStatus(&TimingAnalyzer);
*/
/*For SYSTICK COUNTER TO CHECK START PAUSE RESUME and  STOP 

   
    CyDelay(5000);
    
    Analyzer_Pause(&TimingAnalyzer);
    CyDelay(2000);
    
    Analyzer_Resume(&TimingAnalyzer);
    CyDelay(2000);
    
    Analyzer_Stop(&TimingAnalyzer);
*/

/*Analyzer Stzart and stop 
 CyDelay(5000);
 Analyzer_Stop(&TimingAnalyzer);
*/
 /*For integer addition
    Analyzer_Start(&TimingAnalyzer);
    
    for(int i=0;i<1000;i++)
    {
    sum = sum+i;
    }
    //CyDelay(2000);
   Analyzer_Stop(&TimingAnalyzer);
    */
    
    /*For integer multiplication   
    
    Analyzer_Start(&TimingAnalyzer);
    
    for(int i=1;i<=1000;i++)
    {
      product = product *i;
    }
     //CyDelay(2000);
     Analyzer_Stop(&TimingAnalyzer);
*/
    
    //Division
    /*
    Analyzer_Start(&TimingAnalyzer);

    for(int i = 1; i <= 1000; i++)
    {
        quotient /=(float) i;
    }
     sprintf(StrMsg,"Division is %0.6f\r\n",quotient);
     UART_LOG_PutString(StrMsg);
    Analyzer_Stop(&TimingAnalyzer);
    */
    
    /*
    Analyzer_Start(&TimingAnalyzer);
   for (int i = 0; i < 1000; i++) 
    {
       fsum = fsum + (float)i;
    }
   sprintf(StrMsg,"Addition is %0.6f\r\n",fsum);
    UART_LOG_PutString(StrMsg);
    Analyzer_Stop(&TimingAnalyzer);*/
         
//Floating point multiplication
    /*Analyzer_Start(&TimingAnalyzer);
  
    for (int i = 1; i <= 33; i++) 
    {
        mul *= (float)i;
    }
   sprintf(StrMsg,"multiplication is %0.6f\r\n",(float)mul);
     UART_LOG_PutString(StrMsg);
   // Analyzer_Stop(&TimingAnalyzer);*/
    
    
//    float quot = 1000.0f;
//    Analyzer_Start(&TimingAnalyzer);
//    for (int i = 1; i <= 1000; i++) 
//    {
//        quot /= (float)i;
//    }
//    Analyzer_Stop(&TimingAnalyzer);
    
    
    
//    for (int i = 0; i < 1000; i++) 
//    {
//       result =  sqrt((float)i);
//    }
//    Analyzer_Start(&TimingAnalyzer);
    
    /*
    Analyzer_Start(&TimingAnalyzer);
    Analyzer_Stop(&TimingAnalyzer);
     for (int i = 0; i < 1000; i++) 
    {
        sin(i);

    }
    CyDelay(500);
    Analyzer_Stop(&TimingAnalyzer);
    Analyzer_printStatus(&TimingAnalyzer);
    
    */ 
    
    Analyzer_Start(&TimingAnalyzer);
    Analyzer_Stop(&TimingAnalyzer);

    for (int i = 0; i < 1000; i++)
    {
        sinf((float)i);
    }
   Analyzer_Stop(&TimingAnalyzer);
   Analyzer_printStatus(&TimingAnalyzer);
    
    
    for(;;)
    {
        /* Place your application code here. */
        
    }
    
}
/*
CY_ISR(isr_1msecTick)
{
   
    Pin_LEDRED_Write(~Pin_LEDRED_Read());
   // Analyzer_Start(&TimingAnalyzer);
    isr_1msecTick_ClearPending();
    //Analyzer_Stop(&TimingAnalyzer);
}

CY_ISR(isr_2SecTick)
    {
       // Pin_REDLED_Write(~Pin_REDLED_Read());//This is to check if interrupt is triggering at correct time 
        Pin_LEDGREEN_Write(1);
         Analyzer_Start(&TimingAnalyzer1);
        CyDelay(1000);
       Analyzer_Stop(&TimingAnalyzer1);
        Pin_LEDGREEN_Write(0);
  
    }

*/

/* [] END OF FILE */

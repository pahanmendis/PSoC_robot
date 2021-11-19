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
#include <stdio.h>
int speed = 190;
int PWM_Slave = 0;
int count = 0;
double kp = 0;

int left_angle = 999;
int right_angle = 875;

uint16 US_count = 0;
float distance_measured = 0;
int obstacle = 0;

int color_array[4] = {3,2,3,1}; // green,blue,green,red
int color_index = 0;

int compare_ready;
int mode = 1;
int count_1 = 0;
int count_2 = 0;
int count_3 = 0;
int correct = 0;
int patch_no = 0;
    
void Going_Straight (int PWM_Master,int PWM_Slave,int count);
void Stop();
void Left_Turn(int PWM_Master,int PWM_Slave,int count);
void Right_Turn(int PWM_Master,int PWM_Slave,int count);
void Going_Straight_Forever (int PWM_Master,int PWM_Slave);
int Color_Sense(int color);
void Flick_Left();
void Flick_Right();

CY_ISR(Pin_ISR_Handler)
{
    Pin_1_ClearInterrupt();
//    Going_Straight (speed,speed,1000);
//    CyDelay(100);
//    Left_Turn(speed,speed,3700);
//    CyDelay(100);
//    Going_Straight (speed,speed,6500);
//    CyDelay(100);
//    Right_Turn(speed,speed,3900);
//    CyDelay(100);
//    Going_Straight (speed,speed,7500);
//    CyDelay(100);
//    Right_Turn(speed,speed,3900);
//    CyDelay(100);
    Going_Straight_Forever (speed,speed);
//    CyDelay(100);
}

CY_ISR(IR_ISR_Handler)
{
   
    UART_1_PutString("Puck Detected\n");
    Pin_2_ClearInterrupt();
    Stop();
    Going_Straight(speed,speed,10); // adjust until puck is under color sensor
    correct = Color_Sense(color_array[color_index]);
    if (color_index == 3) 
    {
        color_index = 0;
    }
    else 
    {
        color_index += 1;
    }
    if (correct == 1)
    {
        CyDelay(1000);
        Flick_Left();
        CyDelay(1000);
        Flick_Right();
    }
    CyDelay(500);  
}

CY_ISR(ISR_Handler_1)
{
    // UART_1_PutString("Interrupt\n");
    PWM_Color_ReadStatusRegister();
    compare_ready = 1;
}

CY_ISR(Timer_US_ISR_Handler)
{
    Timer_US_ReadStatusRegister();
    US_count = Timer_US_ReadCounter();
    distance_measured = (65535-US_count)/58; //distance mesured in cm
    
    if (distance_measured < 10)
    {
        UART_1_PutString("Obstacle Detected\n");
        obstacle = 1;
        Stop();
        CyDelay(2000);
        // Turn
        Left_Turn(speed,speed,300);
        // Go straight
        Going_Straight (speed,speed,500);
        // Turn
        Left_Turn(speed,speed,300);
    }
    else if (distance_measured >= 10)
    {
        obstacle = 0;
    }
}

void Going_Straight (int PWM_Master,int PWM_Slave,int count)
{
    kp = 0.01;
    int encoder1,encoder2;
    char string_1[20];
    char string_2[20];
    Motor_1_IN_1_Write(0); // The shaft in Motor_1 rotates in clock wise direction
    Motor_1_IN_2_Write(1);
    PWM_Master_WriteCompare(PWM_Master);
    
    Motor_2_IN_3_Write(0); // The shaft in Motor_2 rotates in clock wise direction
    Motor_2_IN_4_Write(1);
    PWM_Slave_WriteCompare(PWM_Slave);
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    while(1)
    {
        encoder1 = QuadDec_1_GetCounter();
        encoder2 = QuadDec_2_GetCounter();
        
        PWM_Slave = (encoder1-encoder2)*kp+PWM_Master;
        
        Motor_1_IN_1_Write(0); // The shaft in Motor_1 rotates in clock wise direction
        Motor_1_IN_2_Write(1);
        PWM_Master_WriteCompare(PWM_Master);
    
        Motor_2_IN_3_Write(0); // The shaft in Motor_2 rotates in clock wise direction
        Motor_2_IN_4_Write(1);
        PWM_Slave_WriteCompare(PWM_Slave);
        
        sprintf(string_1, "%s %d\n", "1 -", encoder1);
        UART_1_PutString(string_1);
        CyDelay(100);
        
        sprintf(string_2, "%s %d\n", "2 -",encoder2);
        UART_1_PutString(string_2);
        CyDelay(100);
        if (encoder1 >= count || encoder2 >= count)
        {
            Stop();
            return;
        }
    }
}

void Going_Straight_Forever (int PWM_Master,int PWM_Slave)
{
    kp = 0.01;
    int encoder1,encoder2;
    char string_1[20];
    char string_2[20];
    Motor_1_IN_1_Write(0); // The shaft in Motor_1 rotates in clock wise direction
    Motor_1_IN_2_Write(1);
    PWM_Master_WriteCompare(PWM_Master);
    
    Motor_2_IN_3_Write(0); // The shaft in Motor_2 rotates in clock wise direction
    Motor_2_IN_4_Write(1);
    PWM_Slave_WriteCompare(PWM_Slave);
    
    while(1)
    {
        encoder1 = QuadDec_1_GetCounter();
        encoder2 = QuadDec_2_GetCounter();
        
   //     PWM_Slave = (encoder1-encoder2)*kp+PWM_Master;
        
        Motor_1_IN_1_Write(0); // The shaft in Motor_1 rotates in clock wise direction
        Motor_1_IN_2_Write(1);
        PWM_Master_WriteCompare(PWM_Master);
    
        Motor_2_IN_3_Write(0); // The shaft in Motor_2 rotates in clock wise direction
        Motor_2_IN_4_Write(1);
        PWM_Slave_WriteCompare(PWM_Slave);
        
        sprintf(string_1, "%s %d\n", "1 -", encoder1);
        UART_1_PutString(string_1);
        CyDelay(100);
        
        sprintf(string_2, "%s %d\n", "2 -",encoder2);
        UART_1_PutString(string_2);
        CyDelay(100);
        
        while(Echo_Read() == 0)
        {
            Trigger_Write(1);
            CyDelayUs(10);
            Trigger_Write(0);
        }
        CyDelay(100);
    }
}

void Stop()
{
    Motor_1_IN_1_Write(1); // The shaft in Motor_1 rotates in clock wise direction
    Motor_1_IN_2_Write(0);
    PWM_Master_WriteCompare(speed);

    Motor_2_IN_3_Write(1); // The shaft in Motor_2 rotates in clock wise direction
    Motor_2_IN_4_Write(0);
    PWM_Slave_WriteCompare(speed);
    
    CyDelay(100);
    
    Motor_1_IN_1_Write(0); // The shaft in Motor_1 stops
    Motor_1_IN_2_Write(0);
    
    Motor_2_IN_3_Write(0); // The shaft in Motor_2 stops
    Motor_2_IN_4_Write(0);
}

void Left_Turn(int PWM_Master,int PWM_Slave,int count)
{
    int encoder1;
    int encoder2;
    char string_1[20];
    char string_2[20];
    Motor_1_IN_1_Write(1); // The shaft in Motor_1 rotates in anticlock wise direction
    Motor_1_IN_2_Write(0);
    PWM_Master_WriteCompare(PWM_Master);
    
    Motor_2_IN_3_Write(0); // The shaft in Motor_2 rotates in clock wise direction
    Motor_2_IN_4_Write(1);
    PWM_Slave_WriteCompare(PWM_Slave);
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    while(1)
    {
        encoder1 = QuadDec_1_GetCounter();
        encoder2 = QuadDec_2_GetCounter();
        sprintf(string_1, "%s %d\n", "1 -", encoder1);
        UART_1_PutString(string_1);
        CyDelay(100);
        
        sprintf(string_2, "%s %d\n", "2 -",encoder2);
        UART_1_PutString(string_2);
        CyDelay(100);
        if (encoder2 >= count || encoder1 <= -1*count)
        {
            Stop();
            return;
        }
    }
    
    return;
}

void Right_Turn(int PWM_Master,int PWM_Slave,int count)
{
    int encoder1,encoder2;
    char string_1[20];
    char string_2[20];
    Motor_1_IN_1_Write(0); // The shaft in Motor_1 rotates in anticlock wise direction
    Motor_1_IN_2_Write(1);
    PWM_Master_WriteCompare(PWM_Master);
    
    Motor_2_IN_3_Write(1); // The shaft in Motor_2 rotates in clock wise direction
    Motor_2_IN_4_Write(0);
    PWM_Slave_WriteCompare(PWM_Slave);
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    for(;;)
    {
        encoder1 = QuadDec_1_GetCounter();
        encoder2 = QuadDec_2_GetCounter();
        sprintf(string_1, "%s %d\n", "1 -", encoder1);
        UART_1_PutString(string_1);
        CyDelay(100);
        
        sprintf(string_2, "%s %d\n", "2 -",encoder2);
        UART_1_PutString(string_2);
        CyDelay(100);
        if (encoder1 >= count || encoder2 <= -1*count)
        {
            Stop();
            return;
        }
    }
    
    return;
}

// parameter color - input the color of the square(red -> 1, blue -> 2, green -> 3
// Senses the color from the color sensor and returns 1 if the color is matched, 0 if not
int Color_Sense(int color)
{
    char string_2[20];
    LED_Write(1); //Turning On the LED
    S0_Write(1); // S0 and S1 are both set to 1 to get an output
    S1_Write(1); //frequency scaling of 100%
    CyDelay(20);
    
    compare_ready = 0;
   //Detecting the colour of the Puck
    
   //RED Mode
    if (mode == 1)
    {
        S2_Write(0); //fliter pins for RED
        S3_Write(0); // Here we have activated filter RED 
        CyDelay(20);
        Control_Reg_1_Write(1);
        CyDelay(1);
        Control_Reg_1_Write(0);
        while (compare_ready == 0 ) //Resetting both the modules
        {
            //UART_1_PutString("In loop1\n");
            //Waiting for the compare value
        }
        count_1 = Counter_Color_ReadCapture();
        sprintf(string_2, "red: %d\n", count_1); //Reading the capture value
        UART_1_PutString(string_2);
        mode = 2;           // In the end it will go to mode 2
        compare_ready = 0; //reseting the compare ready value
     }
    
    
    //BLUE Mode
    if (mode == 2)
    {
        S2_Write(0); //fliter pins for BLUE
        S3_Write(1); // Here we have activated filter BLUE 
        CyDelay(20);
        Control_Reg_1_Write(1);
        CyDelay(1);
        Control_Reg_1_Write(0);
        while (compare_ready == 0 ) //Resetting both the modules
        {
            //UART_1_PutString("In loop2\n");
            //Waiting for the compare value
        }
        count_2 = Counter_Color_ReadCapture();
        sprintf(string_2, "blue: %d\n", count_2); //Reading the capture value
        UART_1_PutString(string_2);
        mode = 3;           // In the end it will go to mode 2
        compare_ready = 0; //reseting the compare ready value
     }
    
    
    //GREEN Mode
    if (mode == 3)
    {
        S2_Write(1); //fliter pins for GREEN
        S3_Write(1); // Here we have activated filter GREEN 
        CyDelay(20);
        Control_Reg_1_Write(1);
        CyDelay(1);
        Control_Reg_1_Write(0);
        while (compare_ready == 0 ) //Resetting both the modules
        {
            //UART_1_PutString("In loop3\n");
            //Waiting for the compare value
        }
        count_3 = Counter_Color_ReadCapture();
        sprintf(string_2, "Green: %d\n\n", count_3); //Reading the capture value
        UART_1_PutString(string_2);
        mode = 1;   // In the end it will go to mode 2
        compare_ready = 0; //reseting the compare ready value
     }
    
    LED_Write(0);
    if (((count_1 > count_2 && count_1 > count_3) && color == 1) || ((count_2 > count_1 && count_2 > count_3) && color == 2) || ((count_3 > count_1 && count_3 > count_2) && color == 3)) // red -> 1, blue -> 2, green -> 3
        return 1;
        
    else return 0;
}

void Flick_Left()
{
    PWM_Dragging_WriteCompare(left_angle);
}

void Flick_Right()
{
    PWM_Dragging_WriteCompare(right_angle);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    Timer_US_Start();
    PWM_Master_Start();
    PWM_Slave_Start();
    PWM_Color_Start();
    PWM_Dragging_Start(); //start PWM
    Counter_Color_Start();
   
    QuadDec_1_Start();
    QuadDec_2_Start();
    
    UART_1_Start();
    
    isr_1_StartEx(Pin_ISR_Handler);
    isr_2_StartEx(ISR_Handler_1);
    isr_4_StartEx(IR_ISR_Handler);
    isr_5_StartEx(Timer_US_ISR_Handler);
    compare_ready = 0;

    for(;;)
    {
        /* Place your application code here. */
        
    }
}

/* [] END OF FILE */

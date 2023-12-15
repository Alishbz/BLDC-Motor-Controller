/****************************************************************************************
* ALI SAHBAZ
*
*
* Gateway Firmware v1.0
*
* Date          : 22.11.2022
* By            : Ali Sahbaz
* e-mail        : ali_sahbaz@outlook.com
*/
#include "main.h"
#include "bldc_driver_user_interface.h"
#include "bldc_master.h"
#include "bldc_hal_sensor_driver.h"
#include "bldc_x_definations.h"
#include "bldc_torque_control.h"

extern unsigned char UART_Receive_Byte[2];
extern unsigned char UART_Receive_Byte2[2];
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_tim1_ch2;
extern DMA_HandleTypeDef hdma_tim1_ch3;
extern PCD_HandleTypeDef hpcd_USB_FS;
unsigned int array_lengthx(unsigned char *arr);

static bldc_motor_str *motor1;


#define MCU_TIMER_AHB_FREQ            (32000000) // AHB1 PRESCALLER
#define COMMON_PRESCALER              (32-1)     // htim1.Init.Prescaler

unsigned int motor1_freq = 0;

/** INIT PERIODS **/
static uint32_t ch1_period = 100 - 1;
static uint32_t ch2_period = 100 - 1;
static uint32_t ch3_period = 100 - 1;
static unsigned char ch1_duty = 50;
static unsigned char ch2_duty = 50;
static unsigned char ch3_duty = 50;

uint32_t period_calculator(uint32_t wanted_khz){
   return (uint32_t)((((double)MCU_TIMER_AHB_FREQ/(double)(COMMON_PRESCALER+1))/(double)(wanted_khz))-1.0);
}

/** duty_cycle_value must be 0 to 99 : 99 -> 1 , 0 -> 0**/
void ch1_duty_update( unsigned char duty_cycle_value ){
  if(duty_cycle_value > 99){duty_cycle_value = 99;}
  ch1_duty = duty_cycle_value;
  TIM1->CCR1 = (uint32_t)( (double)(ch1_duty) * (double)((double)(ch1_period+1)/(100.0)));
 // bldc_driver_duty_feedback(motor1 , duty_cycle_value);
}

/** duty_cycle_value must be 0 to 99 : 99 -> 1 , 0 -> 0**/
void ch2_duty_update( unsigned char duty_cycle_value ){
  if(duty_cycle_value > 99){duty_cycle_value = 99;}
  ch2_duty = duty_cycle_value;
  TIM1->CCR2 = (uint32_t)( (double)(ch2_duty) * (double)((double)(ch2_period+1)/(100.0)));
 // bldc_driver_duty_feedback(motor1 , duty_cycle_value);
}

/** duty_cycle_value must be 0 to 99 : 99 -> 1 , 0 -> 0**/
void ch3_duty_update( unsigned char duty_cycle_value ){
  if(duty_cycle_value > 99){duty_cycle_value = 99;}
  ch3_duty = duty_cycle_value;
  TIM1->CCR3 = (uint32_t)( (double)(ch3_duty) * (double)((double)(ch3_period+1)/(100.0)));
 // bldc_driver_duty_feedback(motor1 , duty_cycle_value);
}

void ch1_freq_update( uint32_t khz ){
  motor1_freq = khz;
  ch1_period = period_calculator(khz);
  TIM1->ARR = ch1_period;
  /** bütün kanallarin ayni anda frekansi degistiginden **/
  ch2_period = ch1_period;
  ch3_period = ch1_period;
  /** eger frekans degisirse dutyde ona göre istenen duruma gelmeli **/
  ch1_duty_update(ch1_duty);
//  bldc_driver_freq_feedback(motor1 , khz);
}

void ch2_freq_update( uint32_t khz ){
  motor1_freq = khz;
  ch2_period = period_calculator(khz);
  TIM1->ARR = ch2_period;
  /** bütün kanallarin ayni anda frekansi degistiginden **/
  ch1_period = ch2_period;
  ch3_period = ch2_period;
  /** eger frekans degisirse dutyde ona göre istenen duruma gelmeli **/
  ch2_duty_update(ch2_duty);
 // bldc_driver_freq_feedback(motor1 , khz);
}

void ch3_freq_update( uint32_t khz ){
  motor1_freq = khz;
  ch3_period = period_calculator(khz);
  TIM1->ARR = ch3_period;
  /** bütün kanallarin ayni anda frekansi degistiginden **/
  ch2_period = ch3_period;
  ch1_period = ch3_period;
  /** eger frekans degisirse dutyde ona göre istenen duruma gelmeli **/
  ch2_duty_update(ch3_duty);
//  bldc_driver_freq_feedback(motor1 , khz);
}



void ch1_high_pwm_start( void ){
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void ch1_low_pwm_start( void ){
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void ch2_high_pwm_start( void ){
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void ch2_low_pwm_start( void ){
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void ch3_high_pwm_start( void ){
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void ch3_low_pwm_start( void ){
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void ch1_high_pwm_stop( void ){
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void ch1_low_pwm_stop( void ){
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
}

void ch2_high_pwm_stop( void ){
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}

void ch2_low_pwm_stop( void ){
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

void ch3_high_pwm_stop( void ){
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

void ch3_low_pwm_stop( void ){
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
}

void uart_sender(unsigned char* data , unsigned short length){
  if(data==NULL || length < 1)
  { 
    return;  
  }
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)data, (uint16_t)length,1000);// HAL_UART_Transmit(&huart1, (uint8_t *)data, (uint16_t)length,1000);
}

void read_and_update_all_hal_sensors_stade( void );

void prnt(unsigned char* data) {
#ifndef IS_DEBUG_PRINT_ACTIVE 
#endif
    unsigned int lng = array_lengthx(data);
    uart_sender(data,lng);

}

unsigned int duty_getter(void) { 
    return (unsigned int)motor1->set_duty_cycle;
}


unsigned char duty_getter2(void) { 
    return (unsigned char)motor1->set_duty_cycle;
}

void duty_setter(unsigned int sp) {
  bldc_driver_direct_duty_set(motor1 , (unsigned char)sp);
}
void duty_setter2(unsigned char sp) {
  bldc_driver_direct_duty_set(motor1 , (unsigned char)sp);
}
unsigned int freq_getter(void) { 
    return (unsigned int)motor1->set_freq;
}

void freq_setter(unsigned int sp) {
  bldc_driver_direct_freq_set( motor1 , sp); 
}

smooth_fp_str     smooth_duty_fp = {
    prnt,
    duty_setter,
    duty_getter
};

smooth_fp_str     smooth_freq_fp = {
    prnt,
    freq_setter,
    freq_getter
};

unsigned int get_rpm(void) {
    return (unsigned int)motor1->rpm;
}

torque_fp_str     tork_fpx = {
    prnt,
    duty_setter2,
    duty_getter2,
    get_rpm
};

bldc_motor_config_fp_str my_motor_driver_functions = 
{
  ch1_freq_update,
  ch2_freq_update,
  ch3_freq_update,
  
  ch1_duty_update,
  ch2_duty_update,
  ch3_duty_update,
  
  ch1_high_pwm_start,
  ch2_high_pwm_start,
  ch3_high_pwm_start,
  ch1_low_pwm_start,
  ch2_low_pwm_start,
  ch3_low_pwm_start,
  
  ch1_high_pwm_stop,
  ch2_high_pwm_stop,
  ch3_high_pwm_stop,
  ch1_low_pwm_stop,
  ch2_low_pwm_stop,
  ch3_low_pwm_stop,
  
  uart_sender,
  
  read_and_update_all_hal_sensors_stade 
}; 


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
       //bldc_driver_cli_receive_byte(motor1 , UART_Receive_Byte[0]);  
       
       HAL_UART_Receive_IT(&huart1,(unsigned char*)UART_Receive_Byte,1);
    }
    else if (huart->Instance == LPUART1)
    {
       bldc_driver_cli_receive_byte(motor1 , UART_Receive_Byte2[0]);  
       
       HAL_UART_Receive_IT(&hlpuart1,(unsigned char*)UART_Receive_Byte2,1);
    }
}

static hal_stade_types hal_sens_1_stade = HAL_RESET;
static hal_stade_types hal_sens_2_stade = HAL_RESET;
static hal_stade_types hal_sens_3_stade = HAL_RESET;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_10){
    
    hal_sens_1_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_10);
                                        
    bldc_driver_hal_sensor_stade_update(motor1 , 
                                      hal_sens_1_stade , 
                                      hal_sens_2_stade , 
                                      hal_sens_3_stade ); 
  }
  else if(GPIO_Pin == GPIO_PIN_11){
    
    hal_sens_2_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_11);
    
    bldc_driver_hal_sensor_stade_update(motor1 , 
                                      hal_sens_1_stade , 
                                      hal_sens_2_stade , 
                                      hal_sens_3_stade ); 
  }
  else if(GPIO_Pin == GPIO_PIN_12){
    
    hal_sens_3_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_12);
    
    bldc_driver_hal_sensor_stade_update(motor1 , 
                                      hal_sens_1_stade , 
                                      hal_sens_2_stade , 
                                      hal_sens_3_stade ); 
  }
}

void read_and_update_all_hal_sensors_stade( void ){
  hal_sens_1_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_10);
  hal_sens_2_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_11);
  hal_sens_3_stade = (hal_stade_types)HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_12);
  
  bldc_driver_hal_sensor_stade_update(motor1 , 
                                      hal_sens_1_stade , 
                                      hal_sens_2_stade , 
                                      hal_sens_3_stade ); 
}


void bldc_user_systick(void){
  
  bldc_driver_1_ms_timer_update(motor1);

}

void bldc_user_init(void){
  
  
  motor1 = bldc_driver_motor_create( &my_motor_driver_functions , 
                                     BLDC_MODE_HAL_SENSORS , 
                                     &smooth_duty_fp , 
                                     &smooth_freq_fp , 
                                     &tork_fpx); 

}

void bldc_user_loop(void){
  
  bldc_driver_task(motor1);
  
}




unsigned int array_lengthx(unsigned char *arr){
  int i=0;
  while (*arr) { i++;arr++;}
  return i;
}


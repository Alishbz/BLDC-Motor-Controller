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
#ifndef BLDC_MASTER_H
#define BLDC_MASTER_H

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "math.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "bldc_comm_interface.h"
#include "bldc_x_definations.h"
#include "bldc_smooth_transition.h"
#include "bldc_torque_control.h"

#define container_of(ptr, type, member)       ((type *)((uintptr_t)(ptr) - offsetof(type, member)))
    
typedef enum{
  BLDC_RIGHT_DIRECTION=0,
  BLDC_LEFT_DIRECTION
}direction_stades;

/** function out tpyes **/
typedef enum{
  SUCCESS_Q=0,
  ERROR_Q,
  TIMEOUT_Q, 
  TRUE_Q,
  FALSE_Q
}bldc_driver_out;

/** driver mode tpyes **/
typedef enum{
  BLDC_MODE_EMPTY=0,
  BLDC_MODE_BEMF,
  BLDC_MODE_FOC,
  BLDC_MODE_HAL_SENSORS
}bldc_driver_modes;

/** hal_stade tpyes **/
typedef enum{
  HAL_RESET=0,
  HAL_SET
}hal_stade_types;

/** function poninters prototypes **/
typedef void (*set_ch1_pwm_freq)(unsigned int pwm_freq_khz);
typedef void (*set_ch2_pwm_freq)(unsigned int pwm_freq_khz);
typedef void (*set_ch3_pwm_freq)(unsigned int pwm_freq_khz);
typedef void (*set_ch1_pwm_duty_cycle)(unsigned char pwm_duty_cycle_percent);
typedef void (*set_ch2_pwm_duty_cycle)(unsigned char pwm_duty_cycle_percent);
typedef void (*set_ch3_pwm_duty_cycle)(unsigned char pwm_duty_cycle_percent);
typedef void (*cli_comm_data_send)(unsigned char* data, unsigned short data_length);
typedef void (*ch1_high_start)(void);
typedef void (*ch2_high_start)(void);
typedef void (*ch3_high_start)(void);
typedef void (*ch1_low_start)(void);
typedef void (*ch2_low_start)(void);
typedef void (*ch3_low_start)(void);
typedef void (*ch1_high_stop)(void);
typedef void (*ch2_high_stop)(void);
typedef void (*ch3_high_stop)(void);
typedef void (*ch1_low_stop)(void);
typedef void (*ch2_low_stop)(void);
typedef void (*ch3_low_stop)(void);
typedef void (*read_hal_sensors_order_fp)(void);

typedef struct{

  set_ch1_pwm_freq channel_1_pwm_frequency_set; 
  set_ch2_pwm_freq channel_2_pwm_frequency_set; 
  set_ch3_pwm_freq channel_3_pwm_frequency_set; 
  
  set_ch1_pwm_duty_cycle channel_1_pwm_duty_cycle_set; 
  set_ch2_pwm_duty_cycle channel_2_pwm_duty_cycle_set; 
  set_ch3_pwm_duty_cycle channel_3_pwm_duty_cycle_set; 
  
  ch1_high_start channel_1_high_pwm_start;
  ch2_high_start channel_2_high_pwm_start;
  ch3_high_start channel_3_high_pwm_start;
  ch1_high_start channel_1_low_pwm_start;
  ch2_high_start channel_2_low_pwm_start;
  ch3_high_start channel_3_low_pwm_start;
  
  ch1_high_start channel_1_high_pwm_stop;
  ch2_high_start channel_2_high_pwm_stop;
  ch3_high_start channel_3_high_pwm_stop;
  ch1_high_start channel_1_low_pwm_stop;
  ch2_high_start channel_2_low_pwm_stop;
  ch3_high_start channel_3_low_pwm_stop;
    
  /** command responce sender fp **/
  cli_comm_data_send cli_send;
  
  /** read hall sensors and call -> "bldc_driver_hal_sensor_stade_update" **/
  read_hal_sensors_order_fp read_hall_sensors;
  
}bldc_motor_config_fp_str;

typedef struct{
  unsigned char is_init;
  unsigned int  adc_value;
  unsigned int  timeout_us_low;
  unsigned int  timeout_us_high;
}back_emf_str;

typedef struct{
  hal_stade_types stade;
  unsigned char is_init;
  unsigned int  timeout_us_low;
  unsigned int  timeout_us_high;
}hal_sensor_str;

typedef struct{
   unsigned char  is_update;
   unsigned char  is_soft_start_done;
   unsigned int  freq_value;
   unsigned int  timeout_us_low;
   unsigned int  timeout_us_high;
}soft_start_str;

/** private motor driver struct **/
typedef struct bldc_motor_str{
  
  bldc_driver_modes mode;
    
  unsigned char is_init;
  unsigned int  motor_timer_ms;
    
  back_emf_str  BEMF_CH1;
  back_emf_str  BEMF_CH2;
  back_emf_str  BEMF_CH3;
    
  hal_sensor_str HAL_SENSOR_1;
  hal_sensor_str HAL_SENSOR_2;
  hal_sensor_str HAL_SENSOR_3;
  
  /** command line interface queue **/
  bldc_queue_str queue_cli;
  
  /** functions declarations **/
  bldc_motor_config_fp_str *init_functions;
  
  unsigned char is_motor_start;
  
  unsigned char set_duty_cycle;
  
  unsigned int set_freq;
  
  direction_stades direction;
  
  /*** SMOOTH MOVE SPEED **/
  smooth_str soft_duty;
  smooth_fp_str *soft_duty_fp;
  
  /*** SMOOTH MOVE FREQ **/
  smooth_str soft_freq;
  smooth_fp_str *soft_freq_fp;
  
  unsigned int rpm;
  unsigned int rpm_timeout;
  unsigned int rpm_counter;
  unsigned int rpm_counter_holder;
  unsigned char rpm_init;
  
  double speed_km_h;
  
  
  
  
  torque_str torque;
  
}bldc_motor_str;




/** public functions **/
bldc_motor_str* bldc_driver_motor_create(const bldc_motor_config_fp_str* user_fp_s , bldc_driver_modes mode_select,smooth_fp_str *soft_speed_fp,smooth_fp_str *soft_freq_fp , torque_fp_str*tork_fp);
bldc_driver_out bldc_driver_task(bldc_motor_str* me );  /** in super loop **/
void bldc_driver_1_ms_timer_update(bldc_motor_str* me);  /** in 1ms timer interrupt **/
void bldc_driver_cli_receive_byte(bldc_motor_str* me , unsigned char receive_byte); /** in uart receive interrupt **/

//void bldc_driver_duty_feedback(bldc_motor_str* me, unsigned char duty);
//void bldc_driver_freq_feedback(bldc_motor_str* me , unsigned int freq);

void bldc_driver_motor_start(bldc_motor_str* me,unsigned char speed, unsigned int sampling_ms , unsigned int completion_ms);
void bldc_driver_motor_stop(bldc_motor_str* me);
  
void bldc_driver_direct_duty_set(bldc_motor_str* me , unsigned char speed);
void bldc_driver_soft_duty_set(bldc_motor_str* me , unsigned char speed, unsigned int sampling_ms , unsigned int completion_ms);
  
void bldc_driver_direct_freq_set(bldc_motor_str* me , unsigned int freq);
void bldc_driver_soft_freq_set(bldc_motor_str* me , unsigned int freq, unsigned int sampling_ms , unsigned int completion_ms);
  
unsigned char bldc_driver_is_motor_runing(bldc_motor_str* me);
void bldc_driver_motor_brake(bldc_motor_str* me);
void bldc_driver_motor_unbrake(bldc_motor_str* me);

void bldc_driver_set_direction(bldc_motor_str* me , direction_stades dir);




//tork
void bldc_driver_tork_start(bldc_motor_str* me);
void bldc_driver_tork_stop(bldc_motor_str* me);
void bldc_driver_tork_set(bldc_motor_str* me , unsigned int target_rpm);
#endif  





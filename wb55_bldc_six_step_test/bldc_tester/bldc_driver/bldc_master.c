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
#include "bldc_master.h"
#include "bldc_hal_sensor_driver.h"
#include "bldc_bemf_driver.h"
#include "bldc_foc_driver.h"
#include "bldc_torque_control.h"

#define DEBUG_SEND(dt)           (bldc_queue_cli_send(&me->queue_cli,(unsigned char*)dt))



static void _bldc_driver_init(bldc_motor_str* me , 
                              const bldc_motor_config_fp_str* user_fp_s ,  
                              bldc_driver_modes mode_select,
                              smooth_fp_str *soft_speed_fp,
                              smooth_fp_str *soft_freq_fp,
                              torque_fp_str*tork_fp){

  /** first we must be init fp struct **/
  me->init_functions = (bldc_motor_config_fp_str*)user_fp_s;

  me->mode = mode_select;
    
  me->set_freq=FREQ_MIN_VALUE;
    
  me->set_duty_cycle = DUTY_MIN_VALUE;
  
  me->direction = BLDC_RIGHT_DIRECTION; 
  
  me->is_init = 1; 
  me->motor_timer_ms = 0; 
      
  me->is_motor_start = 0;
  
  me->BEMF_CH1.is_init = 1;
  me->BEMF_CH1.adc_value = 0;
  me->BEMF_CH1.timeout_us_low = 0;
  me->BEMF_CH1.timeout_us_high = 0;
  
  me->BEMF_CH2.is_init = 1;
  me->BEMF_CH2.adc_value = 0;
  me->BEMF_CH2.timeout_us_low = 0;
  me->BEMF_CH2.timeout_us_high = 0;
  
  me->BEMF_CH3.is_init = 1;
  me->BEMF_CH3.adc_value = 0;
  me->BEMF_CH3.timeout_us_low = 0;
  me->BEMF_CH3.timeout_us_high = 0;
  
  me->HAL_SENSOR_1.is_init = 1;
  me->HAL_SENSOR_1.stade = HAL_RESET;
  me->HAL_SENSOR_1.timeout_us_low = 0;
  me->HAL_SENSOR_1.timeout_us_high = 0;
  
  me->HAL_SENSOR_2.is_init = 1;
  me->HAL_SENSOR_2.stade = HAL_RESET;
  me->HAL_SENSOR_2.timeout_us_low = 0;
  me->HAL_SENSOR_2.timeout_us_high = 0;
  
  me->HAL_SENSOR_3.is_init = 1;
  me->HAL_SENSOR_3.stade = HAL_RESET;
  me->HAL_SENSOR_3.timeout_us_low = 0;
  me->HAL_SENSOR_3.timeout_us_high = 0;
  
  me->rpm = 0;
  me->rpm_timeout = 0;
  me->rpm_counter = 0;
  me->speed_km_h = 0.0; 
  me->rpm_init = 1;  
  
  me->rpm_counter_holder = 0;
  
  bldc_queue_create(&me->queue_cli , me->init_functions->cli_send);
  
  smooth_config_str speed_cfg;
  speed_cfg.completion_time_ms = 2000;
  speed_cfg.sampling_time_ms =  50;
  speed_cfg.posiible_max_value = DUTY_MAX_VALUE;
  speed_cfg.posiible_min_value = DUTY_MIN_VALUE;

  me->soft_duty_fp = soft_speed_fp;
  
  bldc_smooth_create(&me->soft_duty,  
                     soft_speed_fp, 
                     &speed_cfg,
                     SMOOTH_WAIT_OPERATION_DONE);
  
  smooth_config_str freq_cfg;
  freq_cfg.completion_time_ms = 2000;
  freq_cfg.sampling_time_ms =  50; 
  freq_cfg.posiible_max_value = FREQ_MAX_VALUE;
  freq_cfg.posiible_min_value = FREQ_MIN_VALUE;
  me->soft_freq_fp = soft_freq_fp;
  
  bldc_smooth_create(&me->soft_freq,  
                     soft_freq_fp, 
                     &freq_cfg,
                     SMOOTH_WAIT_OPERATION_DONE);
  
  bldc_smooth_target_value(&me->soft_duty, 0);
  bldc_smooth_target_value(&me->soft_freq, 0);
  
  DEBUG_SEND("Welcome to AH_Bldc_Driver Software....\n");
  DEBUG_SEND("Company: Saykal Elektronik\n");
  DEBUG_SEND("Date: 03.02.2023\n");
  DEBUG_SEND("Driver version: v2.13\n");
  DEBUG_SEND("This Software/Hardware created by Ali and Hüseyin ..\n\n\n");
  DEBUG_SEND("Bldc Motor object created, init done\n");
  
  /** set init pwm's and stop pwm's **/
  bldc_driver_motor_stop(me);
  
  if(me->mode == BLDC_MODE_HAL_SENSORS){
     bldc_hal_sensor_driver_init(me);
  }
  
  
    torque_config_str cfg;
    cfg.sampling_freq_ms = 10; 
    cfg.work_type = TORQUE_LOAD_PROTECT;
    cfg.target_value = 0;
    cfg.sum_error = 0;
    cfg.pre_error = 0;
    cfg.integral = 0;
    cfg.MAX_RPM = (float)11000;  // bunu degistirebilir yap
    cfg.MIN_RPM = 0;
    cfg.MAX_PWM = (float)100;
    cfg.MIN_MIN = 0;
    cfg.Kp = 0.6;
    cfg.Ki = 0.1;
    cfg.Kd = 0.1;
    bldc_torque_control_create(&me->torque,tork_fp, &cfg);
}

void bldc_driver_tork_start(bldc_motor_str* me){
  bldc_torque_control_start(&me->torque);
}

void bldc_driver_tork_stop(bldc_motor_str* me){
   bldc_torque_control_stop(&me->torque);
}

void bldc_driver_tork_set(bldc_motor_str* me , unsigned int target_rpm){
  torque_config_str cfg;
    cfg.sampling_freq_ms = 50; 
    cfg.work_type = TORQUE_LOAD_PROTECT;
    cfg.target_value = target_rpm;
    cfg.sum_error = 0;
    cfg.pre_error = 0;
    cfg.integral = 0;
    cfg.MAX_RPM = (float)11000;  // bunu degistirebilir yap
    cfg.MIN_RPM = 0;
    cfg.MAX_PWM = (float)100;
    cfg.MIN_MIN = 0;
    cfg.Kp = 0.6;
    cfg.Ki = 0.1;
    cfg.Kd = 0.01;
    bldc_torque_control_config_changed(&me->torque, &cfg);
}


bldc_motor_str* bldc_driver_motor_create(const bldc_motor_config_fp_str* user_fp_s, 
                                         bldc_driver_modes mode_select,
                                         smooth_fp_str *soft_speed_fp,
                                         smooth_fp_str *soft_freq_fp, torque_fp_str*tork_fp){  
  bldc_motor_str *me = (bldc_motor_str*)malloc(sizeof(bldc_motor_str));
  if(me != NULL && user_fp_s!=NULL ){
    _bldc_driver_init(me , user_fp_s , mode_select ,soft_speed_fp,soft_freq_fp,tork_fp);
  }
  else{
    DEBUG_SEND("Bldc Motor object not created ERROR \n");
  }
  return me;
}

 
bldc_driver_out bldc_driver_task(bldc_motor_str* me ){
 
  bldc_driver_out out_func = SUCCESS_Q;
  
  bldc_queue_process_task(&me->queue_cli);
  
  bldc_smooth_process(&me->soft_duty);
  
  bldc_smooth_process(&me->soft_freq);
  
  bldc_torque_control_super_task(&me->torque);
  
  switch (me->mode){
    case BLDC_MODE_BEMF:{
       out_func = bldc_bef_driver_task(me);
      break;
    }
     case BLDC_MODE_FOC:{
       out_func = bldc_foc_driver_task(me); 
      break;
    }
     case BLDC_MODE_HAL_SENSORS:{
       
       /** is motor stoped check **/
       /*if(me->motor_timer_ms > 500 && me->rpm == 0 && me->is_motor_start == 1) {
         me->is_motor_start = 0;
         me->set_duty_cycle = DUTY_MIN_VALUE;
         me->set_freq = FREQ_MIN_VALUE;
         me->motor_timer_ms = 0;   // attantion motor timer cleared
       }*/
       
       out_func = bldc_hal_sensor_driver_task(me);
      break;
    } 
      case BLDC_MODE_EMPTY:
      break;
  }

  return out_func;
}


void bldc_driver_1_ms_timer_update(bldc_motor_str* me){
 
  me->motor_timer_ms++;
  
  bldc_queue_1_ms_timer_update(&me->queue_cli);
  
  if(me->mode == BLDC_MODE_HAL_SENSORS){
    bldc_hal_sensor_driver_1_ms_timer_update( me);
  }
  
  bldc_smooth_1_ms_timer_update(&me->soft_duty);
  
  bldc_smooth_1_ms_timer_update(&me->soft_freq);
  
  bldc_torque_control_1_ms_timer_task(&me->torque);
}

/** command line interface receive function **/
void bldc_driver_cli_receive_byte(bldc_motor_str* me , unsigned char receive_byte){
 
  bldc_queue_data_write(&me->queue_cli , receive_byte);

}


void bldc_driver_motor_start(bldc_motor_str* me , unsigned char speed, unsigned int sampling_ms , unsigned int completion_ms){  
  if(me->mode == BLDC_MODE_HAL_SENSORS){
    
    /** if motor is stoped **/
    if(me->rpm == 0 && speed > 0){
      
     
      
      me->is_motor_start = 1; 
      me->motor_timer_ms = 0;  // attantion motor driver timer cleared
      
      me->set_duty_cycle = DUTY_TARGET_VALUE;  
      me->set_freq = FREQ_MIN_VALUE + 7000;  
      
      me->init_functions->read_hall_sensors();
       
      bldc_driver_soft_duty_set(me , speed, sampling_ms , completion_ms ); 
      bldc_driver_soft_freq_set(me , FREQ_TARGET_VALUE, sampling_ms , completion_ms );
        
     // me->init_functions->read_hall_sensors();
      
      DEBUG_SEND("Debug: HAL motor start \n");
    }
    else{
      DEBUG_SEND("Debug: HAL motor start error\n");
    }
    
  }
}

void bldc_driver_motor_stop(bldc_motor_str* me){  // buna gerek var mi ?
  if(me->mode == BLDC_MODE_HAL_SENSORS){
    
    /** if motor is working **/
    if(me->rpm){
      smooth_config_str speed_cfg;
      speed_cfg.completion_time_ms = 2000;
      speed_cfg.sampling_time_ms = 100;
      speed_cfg.posiible_max_value = DUTY_MAX_VALUE;
      speed_cfg.posiible_min_value = DUTY_MIN_VALUE;

      bldc_smooth_config_update(&me->soft_duty, &speed_cfg);
      bldc_smooth_target_value(&me->soft_duty, 0);
        
    }
    
    DEBUG_SEND("Debug: HAL motor stop \n");
  }  
}

/** freq 6k - 34k **/
void bldc_driver_direct_freq_set(bldc_motor_str* me , unsigned int freq){
  if(freq>FREQ_MAX_VALUE){return; }
  me->set_freq = freq;
  me->init_functions->channel_1_pwm_frequency_set(freq);
  me->init_functions->channel_2_pwm_frequency_set(freq);
  me->init_functions->channel_3_pwm_frequency_set(freq);
  bldc_smooth_set_process_done_signal(&me->soft_freq); 
}

/** freq 6k - 34k **/
void bldc_driver_soft_freq_set(bldc_motor_str* me , unsigned int freq , unsigned int sampling_ms , unsigned int completion_ms){  
  if(freq>FREQ_MAX_VALUE){return;}
  me->set_freq = freq; 
  smooth_config_str freq_cfg;
  freq_cfg.completion_time_ms = completion_ms; 
  freq_cfg.sampling_time_ms = sampling_ms;
  freq_cfg.posiible_max_value = FREQ_MAX_VALUE;
  freq_cfg.posiible_min_value = FREQ_MIN_VALUE;
  bldc_smooth_config_update(&me->soft_freq, &freq_cfg);
  bldc_smooth_target_value(&me->soft_freq, freq);
}

/** speed 0 - 100 **/
void bldc_driver_direct_duty_set(bldc_motor_str* me , unsigned char speed){
  if(speed>DUTY_MAX_VALUE){return;}
  bldc_smooth_set_process_done_signal(&me->soft_duty); 
  me->set_duty_cycle = speed;   
  me->init_functions->channel_1_pwm_duty_cycle_set(speed);
  me->init_functions->channel_2_pwm_duty_cycle_set(speed);
  me->init_functions->channel_3_pwm_duty_cycle_set(speed);
}

/** speed 0 - 100 **/
void bldc_driver_soft_duty_set(bldc_motor_str* me , unsigned char speed, unsigned int sampling_ms , unsigned int completion_ms){  
  if(speed>DUTY_MAX_VALUE){return;}
  smooth_config_str speed_cfg;
  speed_cfg.completion_time_ms = completion_ms;
  speed_cfg.sampling_time_ms = sampling_ms;
  speed_cfg.posiible_max_value = DUTY_MAX_VALUE;
  speed_cfg.posiible_min_value = DUTY_MIN_VALUE;
  bldc_smooth_config_update(&me->soft_duty, &speed_cfg);
  bldc_smooth_target_value(&me->soft_duty, speed);
}

unsigned char bldc_driver_is_motor_runing(bldc_motor_str* me){
  if(me->rpm){return 1;}
  return 0;
}
 
void bldc_driver_set_direction(bldc_motor_str* me , direction_stades dir){
  me->direction = dir;
}

void bldc_driver_motor_brake(bldc_motor_str* me){
  me->init_functions->channel_1_pwm_duty_cycle_set(0);
  me->init_functions->channel_1_high_pwm_start();
  me->init_functions->channel_1_low_pwm_start();
  me->init_functions->channel_2_pwm_duty_cycle_set(0);
  me->init_functions->channel_2_high_pwm_start();
  me->init_functions->channel_2_low_pwm_start();
  me->init_functions->channel_3_pwm_duty_cycle_set(0);
  me->init_functions->channel_3_high_pwm_start();
  me->init_functions->channel_3_low_pwm_start(); 
  me->is_motor_start = 0;
  me->set_duty_cycle = 0;
  me->set_freq = FREQ_MIN_VALUE;
}

void bldc_driver_motor_unbrake(bldc_motor_str* me){
  me->init_functions->channel_1_pwm_duty_cycle_set(0);
  me->init_functions->channel_1_high_pwm_stop();
  me->init_functions->channel_1_low_pwm_stop();
  me->init_functions->channel_2_pwm_duty_cycle_set(0);
  me->init_functions->channel_2_high_pwm_stop();
  me->init_functions->channel_2_low_pwm_stop();
  me->init_functions->channel_3_pwm_duty_cycle_set(0);
  me->init_functions->channel_3_high_pwm_stop();
  me->init_functions->channel_3_low_pwm_stop(); 
  me->is_motor_start = 0;
  me->set_duty_cycle = 0;
  me->set_freq = FREQ_MIN_VALUE;
}
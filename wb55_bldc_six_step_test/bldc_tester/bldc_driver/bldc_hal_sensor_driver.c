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
#include "bldc_hal_sensor_driver.h"    

#define DEBUG_SEND(dt)           (bldc_queue_cli_send(&me->queue_cli,(unsigned char*)dt))

#define PWM_OUT_ONE_VAL   0 

#define RPM_SAMPLE_MS   (1000) 

#define MY_PI           (3.14159265359)
#define MY_R            (100)            // TEKERLEGIN YARIÇAPINI GIRERSEN HIZ ÇIKAR

static void comminication_table(bldc_motor_str* me , unsigned char logic_input , unsigned char duty_cycle , direction_stades direction);

static void rpm_calculator(bldc_motor_str* me){
  
  if(me->rpm_init == 0){
    me->rpm_timeout++;
  }
  
  if(me->rpm_counter == 0 && me->rpm_timeout > 300 && me->rpm_init == 0){
    me->rpm_counter_holder = 0;
    me->rpm_counter = 0;
    me->rpm_timeout = 0;
    me->rpm = 0;
    me->rpm_init = 1;
  }
  else if(me->rpm_counter > 0){ 
  
   me->rpm_counter_holder = me->rpm_counter;
   me->rpm_init = 0;
   
   if(me->rpm_timeout > (RPM_SAMPLE_MS-1)){
     me->rpm = (unsigned int)((double)((double)((double)me->rpm_counter/6.0) *(double)(60000.0/(double)RPM_SAMPLE_MS))) ;
     me->speed_km_h = (3/25)*MY_PI*MY_R*( (double) me->rpm); 
     me->rpm_timeout = 0;
     me->rpm_counter = 0;
   }
  }
}

void bldc_hal_sensor_driver_1_ms_timer_update(bldc_motor_str* me){

  rpm_calculator(me);
    
}

void bldc_hal_sensor_driver_init(bldc_motor_str* me) {
  
  DEBUG_SEND("Hal sensor driver mode active \n");
}



bldc_driver_out bldc_hal_sensor_driver_task(bldc_motor_str* me){
 
  bldc_driver_out out_func = SUCCESS_Q;
  
  

  return out_func;
}
  



/**
1 -> A
2 -> B
3 -> C
**/
void bldc_driver_hal_sensor_stade_update(bldc_motor_str* me , hal_stade_types sensor_1_stade , hal_stade_types sensor_2_stade, hal_stade_types sensor_3_stade)
{
  me->HAL_SENSOR_1.stade = sensor_1_stade; // ha
  me->HAL_SENSOR_2.stade = sensor_2_stade; // hb
  me->HAL_SENSOR_3.stade = sensor_3_stade; // hc
  
  if(me->is_motor_start == 1){
    
        comminication_table( me , 
                            (me->HAL_SENSOR_1.stade << 2)|(me->HAL_SENSOR_2.stade << 1)|(me->HAL_SENSOR_3.stade << 0),
                             me->set_duty_cycle ,  
                             me->direction); 
        
        me->rpm_counter++;
  }
}





































/** @REF: MP6532 Datasheet, page: 12 , table2 - https://www.monolithicpower.com/en/documentview/productdocument/index/version/2/document_type/Datasheet/lang/en/sku/MP6532/document_id/1845 ***/
static void comminication_table(bldc_motor_str* me , unsigned char logic_input , unsigned char duty_cycle , direction_stades direction){
  
  if(duty_cycle > 100){return;}
  
  if(direction == BLDC_RIGHT_DIRECTION){
      switch (logic_input){
        case 0b00000101:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_stop();
            me->init_functions->channel_2_low_pwm_stop();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start();
          break;
        }
        case 0b00000100:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_stop();
            me->init_functions->channel_1_low_pwm_stop();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start();
          break;
        }    
        case 0b00000110:{
            me->init_functions->channel_1_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_stop();
            me->init_functions->channel_3_low_pwm_stop(); 
          break;
        }    
        case 0b00000010:{
            me->init_functions->channel_1_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_stop();
            me->init_functions->channel_2_low_pwm_stop();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start(); 
          break;
        }    
        case 0b00000011:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_stop();
            me->init_functions->channel_1_low_pwm_stop();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start(); 
          break;
        }    
        case 0b00000001:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_stop();
            me->init_functions->channel_3_low_pwm_stop();
          break;
        }
      }
  }
  else if(direction == BLDC_LEFT_DIRECTION){
      switch (logic_input){
        case 0b00000101:{
            me->init_functions->channel_1_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_stop();
            me->init_functions->channel_2_low_pwm_stop();
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start(); 
          break;
        }
        case 0b00000100:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_stop();
            me->init_functions->channel_1_low_pwm_stop();
            me->init_functions->channel_2_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start(); 
          break;
        }    
        case 0b00000110:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
            me->init_functions->channel_2_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_stop();
            me->init_functions->channel_3_low_pwm_stop();
          break;
        }    
        case 0b00000010:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_stop();
            me->init_functions->channel_2_low_pwm_stop();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start();
          break;
        }    
        case 0b00000011:{
            me->init_functions->channel_1_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_1_high_pwm_stop();
            me->init_functions->channel_1_low_pwm_stop();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_3_high_pwm_start();
            me->init_functions->channel_3_low_pwm_start();
          break;
        }    
        case 0b00000001:{
            me->init_functions->channel_1_pwm_duty_cycle_set(PWM_OUT_ONE_VAL);
            me->init_functions->channel_1_high_pwm_start();
            me->init_functions->channel_1_low_pwm_start();
             
            me->init_functions->channel_2_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_2_high_pwm_start();
            me->init_functions->channel_2_low_pwm_start();
            
            me->init_functions->channel_3_pwm_duty_cycle_set(duty_cycle);
            me->init_functions->channel_3_high_pwm_stop();
            me->init_functions->channel_3_low_pwm_stop(); 
          break;
        }
      }
  }
 
}

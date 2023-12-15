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
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bldc_torque_control.h"

#define DEBUG_SEND(dt)           (me->fps->debug_send( (unsigned char*)dt ) )

#define GOTO_STEP(stepx)         do{me->task_timer = 0; me->task = stepx; }while(0);

#define RPM_TO_PWM(rpm)          (unsigned char)( rpm * me->config.MAX_PWM /  me->config.MAX_RPM )

static float pid_calculator(torque_config_str * me, float actual_position);

void bldc_torque_control_super_task(torque_str* me) {

    if (me->is_work == 0) { GOTO_STEP(TORQUE_INIT_CASE); return; }

    switch (me->task)
    {
        case TORQUE_INIT_CASE: {
            // clear params

            /** work type check **/
            if (me->config.work_type == TORQUE_LOAD_PROTECT) { GOTO_STEP(TORQUE_LOAD_PROTECT_CASE); }
            else { GOTO_STEP(TORQUE_LOAD_REALEASE_CASE); }
            break;
        }

        case TORQUE_LOAD_PROTECT_CASE: {
          
            if (me->task_timer > me->config.sampling_freq_ms) {
                float new_rpm = pid_calculator(&me->config, (float)me->fps->get_rpm());
                me->fps->set_duty(RPM_TO_PWM(new_rpm));
                GOTO_STEP(TORQUE_LOAD_PROTECT_CASE);
            }

            break;
        }

        case TORQUE_LOAD_REALEASE_CASE: {

            break;
        }

        default:break;
    }
}

static float pid_calculator(torque_config_str *me, float actual_position) {
    float error = 0; 
    float derivative; 
    float output; 

    error = (float)me->target_value - actual_position; 

    me->sum_error += error;

    if (me->sum_error > (float)(me->MAX_RPM / me->Ki)) {
        me->sum_error = (float)(me->MAX_RPM / me->Ki);
    }

    derivative = (error - me->pre_error)* me->Kd;

    output = (me->Kp*error) + (me->Ki* me->sum_error) + derivative;
    
    if(output > me->MAX_RPM) {
        output = me->MAX_RPM;
    } else if(output < me->MIN_RPM) {
        output = me->MIN_RPM;
    } 
     
    me->pre_error = error;

    return output;
}




void bldc_torque_control_start(torque_str* me) {
    me->is_work = 1;
}

void bldc_torque_control_stop(torque_str* me) {
    me->is_work=0;
}



void bldc_torque_control_config_changed(torque_str* me, torque_config_str* new_config) {
    me->config.sampling_freq_ms = new_config->sampling_freq_ms;
    me->config.work_type = new_config->work_type;
    me->config.target_value = new_config->target_value;


    me->config.sum_error = new_config->sum_error;
    me->config.pre_error = new_config->pre_error;
    me->config.integral = new_config->integral;

    me->config.MAX_RPM = new_config->MAX_RPM;
    me->config.MIN_RPM = new_config->MIN_RPM;
    me->config.MAX_PWM = new_config->MAX_PWM;
    me->config.MIN_MIN = new_config->MIN_MIN;

    me->config.Kp = new_config->Kp;
    me->config.Ki = new_config->Ki;
    me->config.Kd = new_config->Kd;
    
    GOTO_STEP(TORQUE_INIT_CASE);  // CONFÝG DEÐÝÞÝRSE ÝNÝT STEPE GO
}


void bldc_torque_control_create(torque_str*me, torque_fp_str* fp, torque_config_str* new_config){
  
        me->is_init = 1;
        me->is_work = 0;
        me->fps = fp;
        me->task = TORQUE_INIT_CASE;
        me->task_timer = 0;
        bldc_torque_control_config_changed(me, new_config);

}

void bldc_torque_control_1_ms_timer_task(torque_str* me) {
    me->task_timer++;
}

 

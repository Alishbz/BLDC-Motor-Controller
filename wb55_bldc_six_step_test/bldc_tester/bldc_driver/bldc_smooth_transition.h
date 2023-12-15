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
#ifndef BLDC_SMOOTH_TRANSITION_H
#define BLDC_SMOOTH_TRANSITION_H

typedef unsigned int smooth_main_type;   // ulasilmak istenen degerin tipi

typedef enum{
  SMOOTH_EMPTY_CASE=0,
  SMOOTH_INIT_CASE,
  SMOOTH_WAIT_CASE,
  SMOOTH_CALCULATION_CASE,
  SMOOTH_INCREASE_CASE,
  SMOOTH_DECREASE_CASE,
  SMOOTH_LAST_UPDATE_CASE
}smooth_steps;

typedef enum{   // bunu ekle
  SMOOTH_PID_DISABLE,
  SMOOTH_PID_ENABLE,
  SMOOTH_PI_ENABLE,
  SMOOTH_PD_ENABLE,
  SMOOTH_P_ENABLE
}smooth_pid_type;

typedef struct{
  smooth_pid_type stade;
  unsigned short sampling_freq_ms;
  double max_sum_error;
  double minimum_percent;
  double kp;
  double ki;
  double kd;
  double factor;
}smooth_pid_str;

typedef enum{   // bunu ekle
  SMOOTH_KALMAN_DISABLE,
  SMOOTH_KALMAN_ENABLE
}smooth_kalman_type;

typedef struct{
  smooth_kalman_type stade;
  double Q;
  double R;
}smooth_kalman_str;

/** after  set_value_fp -> set_value smooth process wait the **/
typedef enum{
  SMOOTH_NOT_WAIT_OPERATION_DONE,
  SMOOTH_WAIT_OPERATION_DONE 
}smooth_waiting_type;

typedef void (*debug_sender_fp)(unsigned char* data);
typedef void (*set_value_fp)(smooth_main_type value);
typedef smooth_main_type (*get_value_fp)(void);

typedef struct{
  debug_sender_fp debug_send;
  set_value_fp set_value;
  get_value_fp get_value;
}smooth_fp_str;

typedef struct{
  unsigned int sampling_time_ms;     // ulasilmak istenen degeri örnekleme araligi
  unsigned int completion_time_ms;   // ulasilmak istenen degere kaç ms de ulasilmak isteniyor
  smooth_main_type posiible_max_value;
  smooth_main_type posiible_min_value;
}smooth_config_str;

typedef struct{
  
  unsigned char is_init;
  
  smooth_main_type target_value;
  smooth_main_type current_value;
  
  smooth_main_type increase_value;
  smooth_main_type decrease_value;
  
  unsigned short reach_max_value;  
  unsigned short reach_counter;  
  
  unsigned int step_timer;
  smooth_steps step;
  
  smooth_config_str config;
  smooth_fp_str *init_functions;
  
  smooth_waiting_type is_wait;
  smooth_waiting_type wait_signal;
  
  //smooth_kalman_str *kalman_config;
  //smooth_pid_str *pid_config;
  
}smooth_str;

void bldc_smooth_create(smooth_str* me, smooth_fp_str* fps , 
                        smooth_config_str *config , 
                        smooth_waiting_type wait_signal);
void bldc_smooth_1_ms_timer_update(smooth_str* me);
void bldc_smooth_process(smooth_str* me);
void bldc_smooth_config_update(smooth_str* me , const smooth_config_str*config);

void bldc_smooth_target_value(smooth_str* me , smooth_main_type terget_value);

/** if you use SMOOTH_WAIT_OPERATION_DONE then you must give that signal **/
void bldc_smooth_set_process_done_signal(smooth_str* me);

#endif  





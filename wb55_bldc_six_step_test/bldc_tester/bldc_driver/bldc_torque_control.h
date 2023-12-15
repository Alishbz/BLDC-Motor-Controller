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
#ifndef BLDC_TORQUE_CONTROL_H
#define BLDC_TORQUE_CONTROL_H

// ulasilmak istenen degerin tipleri
typedef unsigned char torque_duty_type;   
typedef unsigned int  torque_rpm_type;

typedef enum { 
	TORQUE_LOAD_PROTECT,  // YÜK ALTINDA rpm ' i korur
	TORQUE_LOAD_REALEASE  // Yük altýnda rpm ' i düþürür
}torque_work_config;

typedef enum {
	TORQUE_INIT_CASE= 0, 
	TORQUE_LOAD_PROTECT_CASE,  // YÜK ALTINDA rpm ' i korur
	TORQUE_LOAD_REALEASE_CASE  // Yük altýnda rpm ' i düþürür
}torque_tasks_steps;

typedef void (*tdebug_sender_fp)(unsigned char* data);
typedef void (*set_duty_fp)(torque_duty_type value);
typedef torque_duty_type(*get_duty_fp)(void);
typedef torque_rpm_type(*get_rpm_fp)(void);

typedef struct {
	tdebug_sender_fp debug_send;
	set_duty_fp     set_duty;
	get_duty_fp     get_duty;
	get_rpm_fp      get_rpm;
}torque_fp_str;

typedef struct {
	unsigned int sampling_freq_ms;
	torque_work_config work_type;

	torque_rpm_type target_value;

	// DEÐÝÞKEN
    float sum_error;
	float pre_error;
    float integral;

	//statics
	float MAX_RPM;
	float MIN_RPM;
	float MAX_PWM;
	float MIN_MIN;
	float Kp;
	float Ki;
	float Kd;

}torque_config_str;

typedef struct {

	/** is object created **/
	unsigned char is_init;

	/** is object work **/
	unsigned char is_work;

	/** stade machine manager **/
	torque_tasks_steps task;
	unsigned int task_timer;

	torque_config_str config;

	/** function pointers **/
	torque_fp_str* fps;

}torque_str;


void bldc_torque_control_create(torque_str*me, torque_fp_str* fp, torque_config_str* new_config);
void bldc_torque_control_1_ms_timer_task(torque_str*me);
void bldc_torque_control_super_task(torque_str* me);
void bldc_torque_control_config_changed(torque_str* me , torque_config_str*new_config);

void bldc_torque_control_start(torque_str* me);
void bldc_torque_control_stop(torque_str* me);

#endif  





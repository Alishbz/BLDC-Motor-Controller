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
#ifndef BLDC_HAL_SENSOR_DRIVER_H
#define BLDC_HAL_SENSOR_DRIVER_H

#include "bldc_master.h"

bldc_driver_out bldc_hal_sensor_driver_task(bldc_motor_str* me);
void bldc_hal_sensor_driver_init(bldc_motor_str* me); 

/** if use hal sensor, than call in interrupt this hal sensor update function**/
void bldc_driver_hal_sensor_stade_update(bldc_motor_str* me , hal_stade_types sensor_1_stade , hal_stade_types sensor_2_stade, hal_stade_types sensor_3_stade); /** in hall change interrupt **/

void bldc_hal_sensor_driver_1_ms_timer_update(bldc_motor_str* me); 

#endif  





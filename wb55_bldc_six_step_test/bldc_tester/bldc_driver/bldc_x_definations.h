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
#ifndef BLDC_X_DEFINATIONS_H
#define BLDC_X_DEFINATIONS_H

/**************************************************************/
/***************** soft start config inits ********************/
/**************************************************************/

#define DUTY_MAX_VALUE      99
#define DUTY_MIN_VALUE      0

#define FREQ_MAX_VALUE      34000
#define FREQ_MIN_VALUE      9000

#define INSTANCE_SAMPLING_TIME      100
#define INSTANCE_COMPLETION_TIME    2000


/** can be chanced **/
#define FREQ_TARGET_VALUE      32000
#define DUTY_TARGET_VALUE      14


/**************************************************************/



/** PRINTF'S FOR TEST AND DDEVELOPING **/
#define IS_DEBUG_PRINT_ACTIVE   1 

#endif  


 


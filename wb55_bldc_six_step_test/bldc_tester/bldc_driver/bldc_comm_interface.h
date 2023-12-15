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
#ifndef BLDC_COMM_INTERFACE_H
#define BLDC_COMM_INTERFACE_H

#include <stdbool.h>

typedef unsigned char  mychar;

typedef void (*comm_data_send)(unsigned char* data, unsigned short data_length);

#define BLDC_QUEUE_BUFFER_LENGTH  0x1FF

typedef struct{
  
    mychar data[BLDC_QUEUE_BUFFER_LENGTH+1]; 
    
    unsigned short read_ptr;
    
    unsigned short write_ptr;
    
    unsigned int main_timer_ms;
    
    unsigned int receive_timer_ms;
    
    comm_data_send send_data;
    
}bldc_queue_str;


/** private test **/
void bldc_queue_cli_send(bldc_queue_str * me, unsigned char*data);  

/** main functions **/
void bldc_queue_create(bldc_queue_str * me , comm_data_send sender_function); 
void bldc_queue_data_write(bldc_queue_str * me , mychar data );  
void bldc_queue_process_task(bldc_queue_str * me);  
void bldc_queue_1_ms_timer_update(bldc_queue_str * me);  

#endif  





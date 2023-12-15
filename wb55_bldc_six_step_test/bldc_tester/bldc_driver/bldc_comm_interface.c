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
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "bldc_comm_interface.h"
#include "bldc_master.h"

/** it could be change by baudrate **/
#define PARSE_MESSAGES_SPACE_TIMEOUT            10


static void parser(bldc_queue_str * me , unsigned char * data , unsigned short data_length){

  char *hold;
  
 if(strstr((char*)(data), "motor brake"))
  {
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_motor_brake(str); // fren  
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
 
 else if(strstr((char*)(data), "motor unbrake"))
  { 
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_motor_unbrake(str); // fren  
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
 
  else if(strstr((char*)(data), "soft duty:"))
  {
    hold = strstr((char*)(data), "soft duty:");
    if(hold==NULL){me->send_data((unsigned char*)"duty error\n" , 11);  return;}
    
    int cx = 0;
    unsigned short duty = (unsigned short)atoi(hold+10);
    
    /** find comma **/
    cx = 0;  do{if( (hold[cx]) == ',' ){break;}cx++; }while(cx<30);
    
    unsigned short sampling_time = (unsigned short)atoi(hold+cx+1);
    
    /** find comma **/
    cx++;
    do{if( (hold[cx]) == ',' ){break;}cx++; }while(cx<30);
    
    unsigned short comp_time = (unsigned short)atoi(hold+cx+1);
      
    if( duty < DUTY_MAX_VALUE && sampling_time>0 && comp_time>0){
      
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );  
      
      /*** motor already run ***/
      if(bldc_driver_is_motor_runing(str)){ 
        bldc_driver_soft_duty_set(str , duty, sampling_time , comp_time );
        if(duty == 0){
          bldc_driver_soft_freq_set(str , FREQ_MIN_VALUE , sampling_time , comp_time );
        }
      }
      
      /*** motor stoped ***/
      else{
        bldc_driver_motor_start(str , duty, sampling_time , comp_time);
      }
  
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);}
  }
  
   else if(strstr((char*)(data), "chs freq:")){
    hold = strstr((char*)(data), "chs freq:");
    if(hold==NULL){me->send_data((unsigned char*)"freqs error\n" , 12);  return;}
    unsigned int freq = (unsigned int)atoi(hold+9);
    
    if( freq < FREQ_MAX_VALUE){  
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      
      bldc_driver_direct_freq_set(str,freq);  
      
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
    
  else if(strstr((char*)(data), "direction left"))
  { 
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_set_direction(str , BLDC_LEFT_DIRECTION); 
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
 
  else if(strstr((char*)(data), "direction right"))
  { 
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_set_direction(str , BLDC_RIGHT_DIRECTION); 
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  
   else if(strstr((char*)(data), "tork start"))
  { 
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_tork_start(str ); 
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  
  else if(strstr((char*)(data), "tork stop"))
  { 
    bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
    
    bldc_driver_tork_stop(str ); 
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "tork val:"))
  { 
    hold = strstr((char*)(data), "tork val:");
    if(hold==NULL){me->send_data((unsigned char*)"torks error\n" , 12);  return;}
    unsigned int tork = (unsigned int)atoi(hold+9);
    
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      
      bldc_driver_tork_set(str,tork);  
      
      me->send_data((unsigned char*)"ok\n" , 3); 
    
    me->send_data((unsigned char*)"ok\n" , 3);
  }
  
  
  
  
  
  
  
 
 
 
 
 
 
 
 
 
 // gerek yok assagisina
  
// ***********************  for tester *******************************************************************

  else if(strstr((char*)(data), "motor start"))
  {
   // bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );  
    
    //bldc_driver_motor_start(str);
      
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "set speed:"))// gerek yok
  {
    hold = strstr((char*)(data), "set speed:");
    if(hold==NULL){me->send_data((unsigned char*)"speed error\n" , 12);  return;}
    
    unsigned short speed = (unsigned short)atoi(hold+10);
      
    if( speed < DUTY_MAX_VALUE){
      
     // bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );  
      
      //bldc_driver_soft_duty_set(str , speed);  // gerek yok
      
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);}
  }
  else if(strstr((char*)(data), "chd freq:")){
    hold = strstr((char*)(data), "chd freq:");
    if(hold==NULL){me->send_data((unsigned char*)"freqs error\n" , 12);  return;}
    unsigned int freq = (unsigned int)atoi(hold+9);
    
    if( freq < FREQ_MAX_VALUE){  
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      
      bldc_driver_direct_freq_set(str,freq);
      
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "chs duty:")){
    hold = strstr((char*)(data), "chs duty:");
    if(hold==NULL){me->send_data((unsigned char*)"dutys error\n" , 12);  return;}
    unsigned short duty = (unsigned short)atoi(hold+9);
    
    if( duty < DUTY_MAX_VALUE){ 
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );  
      
      // test
      //str->init_functions->channel_1_pwm_duty_cycle_set(duty);
      //str->init_functions->channel_2_pwm_duty_cycle_set(duty);
      //str->init_functions->channel_3_pwm_duty_cycle_set(duty);
      
      bldc_driver_direct_duty_set(str , duty);
      
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }    
  
  else if(strstr((char*)(data), "ch1 duty:")){
    hold = strstr((char*)(data), "ch1 duty:");
    if(hold==NULL){me->send_data((unsigned char*)"duty1 error\n" , 12);  return;}
    unsigned short duty = (unsigned short)atoi(hold+9);
    
    if( duty < DUTY_MAX_VALUE){ 
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_pwm_duty_cycle_set(duty);
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch2 duty:")){
    hold = strstr((char*)(data), "ch2 duty:");
    if(hold==NULL){me->send_data((unsigned char*)"duty2 error\n" , 12);  return;}
    unsigned short duty = (unsigned short)atoi(hold+9);
    
    if( duty < DUTY_MAX_VALUE){ 
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_2_pwm_duty_cycle_set(duty);
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
   else if(strstr((char*)(data), "ch3 duty:")){
    hold = strstr((char*)(data), "ch3 duty:");
    if(hold==NULL){me->send_data((unsigned char*)"duty3 error\n" , 12);  return;}
    unsigned short duty = (unsigned short)atoi(hold+9);
    
    if( duty < DUTY_MAX_VALUE){ 
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_3_pwm_duty_cycle_set(duty);
      me->send_data((unsigned char*)"ok\n" , 3); 
    }
    else{ me->send_data((unsigned char*)"value error\n" , 12);} 
    me->send_data((unsigned char*)"ok\n" , 3); 
  }
  
  else if(strstr((char*)(data), "all high chs start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_high_pwm_start();
      str->init_functions->channel_2_high_pwm_start();
      str->init_functions->channel_3_high_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "all high chs stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_high_pwm_stop();
      str->init_functions->channel_2_high_pwm_stop();
      str->init_functions->channel_3_high_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "all low chs start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_low_pwm_start();
      str->init_functions->channel_2_low_pwm_start();
      str->init_functions->channel_3_low_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "all low chs stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_low_pwm_stop();
      str->init_functions->channel_2_low_pwm_stop();
      str->init_functions->channel_3_low_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch1 high start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_high_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch1 high stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_high_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch1 low start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_low_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch1 low stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_1_low_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch2 high start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_2_high_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch2 high stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_2_high_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch2 low start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_2_low_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch2 low stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_2_low_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch3 high start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_3_high_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch3 high stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_3_high_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch3 low start")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_3_low_pwm_start();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  else if(strstr((char*)(data), "ch3 low stop")){
      bldc_motor_str *str = container_of( me, bldc_motor_str, queue_cli );    
      str->init_functions->channel_3_low_pwm_stop();
      me->send_data((unsigned char*)"ok\n" , 3); 
  }
  
// ***********************  for tester *****************************************************************  
  
  else{
    me->send_data((unsigned char*)"undefined\n" , 10); 
  }
  
}













/******************************************************************************************************************/
/******************************************************************************************************************/
/************************ COMM DRIVER PART *************************************************************************/
/******************************************************************************************************************/

static void check_write_ptr(bldc_queue_str * me){

  if(me->write_ptr > BLDC_QUEUE_BUFFER_LENGTH){
     me->write_ptr = 0;
  }
}

static void check_read_ptr(bldc_queue_str * me){

  if(me->read_ptr > BLDC_QUEUE_BUFFER_LENGTH){
     me->read_ptr = 0;
  }
}

static bool queue_is_empty(bldc_queue_str * me){
  if( me->read_ptr == me->write_ptr ){
     return true;
  }
  return false;
}

static mychar queue_data_read(bldc_queue_str * me ){
  mychar hold_data = 0;
  if(!queue_is_empty(me)){
    hold_data = me->data[ me->read_ptr++ ];
    check_read_ptr(me);
  }
  return hold_data;
}

static unsigned int array_length(unsigned char *arr){
  int i=0;
  while (*arr) { i++;arr++;}
  return i;
}

void bldc_queue_create(bldc_queue_str * me, comm_data_send sender_function){  
  
  me->send_data = sender_function;
  
  me->main_timer_ms = 0;
  me->receive_timer_ms = 0;
  
  me->write_ptr = 0;
  
  me->data[me->write_ptr] = 0;
    
  me->write_ptr++;
  
  while(me->write_ptr){
    
    me->data[me->write_ptr++] = 0;
    
    check_write_ptr(me);
  }
  
  me->read_ptr = 0;
  
  me->write_ptr = 0;
  
}

void bldc_queue_data_write(bldc_queue_str * me , mychar data ){
  me->receive_timer_ms = 0;
  me->data[ me->write_ptr++ ] = data;
  check_write_ptr(me);
}


void bldc_queue_process_task(bldc_queue_str * me){

  unsigned char hold_buffer[(BLDC_QUEUE_BUFFER_LENGTH+1)/2] ;
  unsigned short hold_index = 0;
  
  if( queue_is_empty(me) == false && me->receive_timer_ms > PARSE_MESSAGES_SPACE_TIMEOUT ){
  
    while(!queue_is_empty(me)  ){
      hold_buffer[hold_index++] = queue_data_read(me);
      if(hold_index > (BLDC_QUEUE_BUFFER_LENGTH/2)){ break; }
    }
    
    parser(me, (unsigned char*)hold_buffer , hold_index );
    
  }
}

void bldc_queue_1_ms_timer_update(bldc_queue_str * me){
  me->main_timer_ms++;
  me->receive_timer_ms++;
}



void bldc_queue_cli_send(bldc_queue_str * me, unsigned char*data){
#ifdef IS_DEBUG_PRINT_ACTIVE
  me->send_data((unsigned char*)data , array_length(data)); 
#endif
}










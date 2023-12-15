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
#include "bldc_smooth_transition.h"

/** DIKKAT BU DEGERE HIÇ ULASILMAMALI VE BU DEGER "smooth_main_type" BAGIMLI **/
#define TARGET_CLEAR_VALUE           9999 

#define DEBUG_SEND(dt)           (me->init_functions->debug_send( (unsigned char*)dt ) )

#define GOTO_STEP(stepx)           do{me->step_timer = 0; me->step = stepx; }while(0);

#define MAX_WAIT_TIMEOUT           3000

void bldc_smooth_create(smooth_str* me, smooth_fp_str* fps, smooth_config_str* config, smooth_waiting_type wait_signal) {

    if (me != NULL && fps != NULL && config != NULL) {

        me->reach_max_value = 0;
        me->reach_counter = 0;
        me->decrease_value = 0;
        me->increase_value = 0;
        me->current_value = 0;
        me->target_value = TARGET_CLEAR_VALUE;
        me->init_functions = fps;
        me->step_timer = 0;
        me->is_wait = wait_signal;
        me->is_wait = SMOOTH_NOT_WAIT_OPERATION_DONE;
        me->step = SMOOTH_INIT_CASE;

        bldc_smooth_config_update(me, config);

        me->is_init = 1;
    }
    else { /** ERROR **/ }

}


static void bldc_smooth_process_clear(smooth_str* me) {
    me->reach_max_value = 0;
    me->reach_counter = 0;
    me->decrease_value = 0;
    me->increase_value = 0;
    me->step_timer = 0;
}


void bldc_smooth_1_ms_timer_update(smooth_str* me) {

    if ( me == NULL || me->is_init == 0) { return; }

    me->step_timer++;
}

void bldc_smooth_process(smooth_str* me) {

    if (me->is_init == 0) { return; }

    if (me->target_value == TARGET_CLEAR_VALUE) { GOTO_STEP(SMOOTH_INIT_CASE); return; }

    switch (me->step) {

    case SMOOTH_INIT_CASE: {
        
        if (me->config.completion_time_ms < me->config.sampling_time_ms) {
            DEBUG_SEND("SMOOTH: config times ERROR\n");
            me->init_functions->set_value(me->target_value);
            me->target_value = TARGET_CLEAR_VALUE;
            GOTO_STEP(SMOOTH_INIT_CASE);
            break;
        }
        bldc_smooth_process_clear(me);
        
        me->reach_max_value = (unsigned short)((double)((double)me->config.completion_time_ms / (double)me->config.sampling_time_ms));
       
        GOTO_STEP(SMOOTH_CALCULATION_CASE);
        break;
    }

    case SMOOTH_CALCULATION_CASE: {
        me->current_value = me->init_functions->get_value();
        if (me->reach_max_value < 2 || me->reach_counter == me->reach_max_value - 1) {
            GOTO_STEP(SMOOTH_LAST_UPDATE_CASE);
            if (me->reach_counter == me->reach_max_value - 1) {
                DEBUG_SEND("SMOOTH: last value updating\n");
            }
            else {
                DEBUG_SEND("SMOOTH: reach value is too small, last value updating\n");
            }
            break;
        }

        if (me->target_value > me->current_value) {
            me->increase_value = (smooth_main_type)((double)((double)(me->target_value - me->current_value) / (double)( (me->config.completion_time_ms - (me->reach_counter * me->config.sampling_time_ms)) / me->config.sampling_time_ms)));
            if (me->increase_value == 0) {
                DEBUG_SEND("SMOOTH: increase_value zero, last value updating\n");
                GOTO_STEP(SMOOTH_LAST_UPDATE_CASE);
                break;
            }
            GOTO_STEP(SMOOTH_INCREASE_CASE);
        }
        else {
            me->decrease_value = (smooth_main_type)((double)((double)(me->current_value - me->target_value) / (double)((me->config.completion_time_ms - (me->reach_counter * me->config.sampling_time_ms)) / me->config.sampling_time_ms)));
            if (me->decrease_value == 0) {
                DEBUG_SEND("SMOOTH: decrease_value zero, last value updating\n");
                GOTO_STEP(SMOOTH_LAST_UPDATE_CASE);
                break;
            }
            GOTO_STEP(SMOOTH_DECREASE_CASE);
        }
        break;
    }

    case SMOOTH_INCREASE_CASE: {
        if (me->step_timer > (me->config.sampling_time_ms - 1)) {

            me->current_value += me->increase_value;

            if (me->current_value >= me->config.posiible_max_value || me->current_value >= me->target_value) {
                GOTO_STEP(SMOOTH_LAST_UPDATE_CASE);
                break;
            }

            me->init_functions->set_value(me->current_value);

            me->reach_counter++;

            if (me->is_wait == SMOOTH_WAIT_OPERATION_DONE) {

                GOTO_STEP(SMOOTH_WAIT_CASE);
            }
            else {
                GOTO_STEP(SMOOTH_CALCULATION_CASE);
            }
        }
        break;
    }

    case SMOOTH_DECREASE_CASE: {
        if (me->step_timer > (me->config.sampling_time_ms - 1)) {

            me->current_value -= me->decrease_value;

            if (me->current_value <= me->config.posiible_min_value || me->current_value <= me->target_value) {
                GOTO_STEP(SMOOTH_LAST_UPDATE_CASE);
                break;
            }


            me->init_functions->set_value(me->current_value);

            me->reach_counter++;

            if (me->is_wait == SMOOTH_WAIT_OPERATION_DONE) {

                GOTO_STEP(SMOOTH_WAIT_CASE);
            }
            else {
                GOTO_STEP(SMOOTH_CALCULATION_CASE);
            }
        }
        break;
    }

    case SMOOTH_WAIT_CASE: {
        if (me->is_wait == SMOOTH_WAIT_OPERATION_DONE) {
            GOTO_STEP(SMOOTH_CALCULATION_CASE);
            me->is_wait = SMOOTH_NOT_WAIT_OPERATION_DONE;
        }
        else if (me->step_timer > MAX_WAIT_TIMEOUT) {
            DEBUG_SEND("SMOOTH: wait flag is not come ERROR\n");
            GOTO_STEP(SMOOTH_CALCULATION_CASE);
            me->is_wait = SMOOTH_NOT_WAIT_OPERATION_DONE;
        }
        break;
    }

    case SMOOTH_LAST_UPDATE_CASE: {
        if (me->step_timer > (me->config.sampling_time_ms - 1)) {
            me->init_functions->set_value(me->target_value);
            me->target_value = TARGET_CLEAR_VALUE;
            GOTO_STEP(SMOOTH_INIT_CASE);
            DEBUG_SEND("SMOOTH: last value done, goto init\n");
        }
        break;
    }

    case SMOOTH_EMPTY_CASE: {break; }
    }

}

void bldc_smooth_config_update(smooth_str* me, const smooth_config_str* config) {
    me->config.sampling_time_ms = config->sampling_time_ms;
    me->config.completion_time_ms = config->completion_time_ms;
    me->config.posiible_max_value = config->posiible_max_value;
    me->config.posiible_min_value = config->posiible_min_value;
}

void bldc_smooth_target_value(smooth_str* me, smooth_main_type terget_value) {
    if (me->target_value) {
        DEBUG_SEND("SMOOTH: my work is not done but date came, clear process\n");
        GOTO_STEP(SMOOTH_INIT_CASE); /// ********************* ?? eger deger daha update olmadan yeni istek geldiyse stade machine i temizle
    }
    if (terget_value > me->config.posiible_max_value) { DEBUG_SEND("SMOOTH: target value exceeded the max limit\n");  return; }
    if (terget_value < me->config.posiible_min_value) { DEBUG_SEND("SMOOTH: target value exceeded the min limit\n");  return; }
    me->target_value = terget_value;
}

void bldc_smooth_set_process_done_signal(smooth_str* me) {
    me->is_wait = SMOOTH_WAIT_OPERATION_DONE;
}




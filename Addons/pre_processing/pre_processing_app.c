/**
 ******************************************************************************
 * @file    pre_processing_app.c
 * @author  STMicroelectronics
 * @version 1.0.0
 * @date    June 29, 2022
 *
 * @brief File generated with Handlebars.
 *
 ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
 ******************************************************************************
 */

#include "sys_functions.h"
#include "arm_math.h"
#include "params.h"
#include "pre_processing_app.h"
#include <stdlib.h>

float32_t data_0[INPUT_BUFFER_SIZE];
float32_t data_1[INPUT_BUFFER_SIZE];

void pre_processing_init_mcu(pre_processing_data_t* pre_processing_data, float32_t sensor_odr) {
    /* Axis selection. */
    pre_processing_data->axis_selection_axis = AXIS_SELECTION_AXIS;

    /* MFCC. */
    arm_status mfcc_status;
    arm_rfft_fast_init_f32(&((*pre_processing_data).mfcc_handler), INPUT_BUFFER_SIZE);
    mfcc_status = arm_dct4_init_f32(
        &((*pre_processing_data).mfcc_dct4f32),
        &((*pre_processing_data).mfcc_rfftf32),
        &((*pre_processing_data).mfcc_cfftradix4f32),
        MFCC_TRIANGULAR_FILTERS_BANK_SIZE,
        MFCC_TRIANGULAR_FILTERS_BANK_SIZE / 2.0,
        0.125
    );
    if (mfcc_status != ARM_MATH_SUCCESS) {
        sys_error_handler();
    }
    pre_processing_data->mfcc_triangular_filters_scale = MFCC_TRIANGULAR_FILTERS_SCALE;
    pre_processing_data->mfcc_signal_windowing = MFCC_SIGNAL_WINDOWING;
	triangular_filters_init(INPUT_BUFFER_SIZE,
                            MFCC_TRIANGULAR_FILTERS_BANK_SIZE,
                            MFCC_TRIANGULAR_FILTERS_BANK_FRACTION,
                            sensor_odr,
                            pre_processing_data->mfcc_triangular_filters_scale,
                            (*pre_processing_data).mfcc_bin);
	(*pre_processing_data).mfcc_multipliers = (float32_t*) SysAlloc(INPUT_BUFFER_SIZE * sizeof(float32_t));
    multipliers_init((*pre_processing_data).mfcc_multipliers,
                        INPUT_BUFFER_SIZE,
                        (*pre_processing_data).mfcc_signal_windowing);
}

void pre_processing_process_mcu(tridimensional_data_t* data_in,
                                uint32_t data_in_size,
                                float32_t* data_out,
                                uint32_t data_out_size,
                                pre_processing_data_t* pre_processing_data) {
    /* Axis selection. */
    axis_selection(data_in, data_in_size, data_0, data_in_size, pre_processing_data->axis_selection_axis);

    /* Mean removal. */
    mean_removal(data_0, data_in_size, data_1, data_in_size);

    /* MFCC. */
    mfcc(data_1, data_in_size, data_out, data_out_size,
        ((*pre_processing_data).mfcc_bin),
        &((*pre_processing_data).mfcc_dct4f32),
        &((*pre_processing_data).mfcc_handler),
        (*pre_processing_data).mfcc_signal_windowing,
        (*pre_processing_data).mfcc_multipliers);
}

void pre_processing_free(pre_processing_data_t* pre_processing_data) {
    /* MFCC. */
    SysFree((*pre_processing_data).mfcc_multipliers);
}

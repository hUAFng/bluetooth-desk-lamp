/**
 ******************************************************************************
 * @file    pre_processing_app.h
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

#ifndef PRE_PROCESSING_APP_H_
#define PRE_PROCESSING_APP_H_

/* Includes ------------------------------------------------------------------*/

#include "pre_processing_core.h"

/* Exported Types ------------------------------------------------------------*/

typedef struct{
    /* Axis selection. */
    axis_t axis_selection_axis;

    /* Signal normalization. */
    float32_t signal_normalization_peak_to_peak;
    float32_t signal_normalization_offset;

    /* MFCC. */
    arm_status mfcc_status;
    arm_rfft_fast_instance_f32 mfcc_handler;
    arm_dct4_instance_f32 mfcc_dct4f32;
    arm_rfft_instance_f32 mfcc_rfftf32;
    arm_cfft_radix4_instance_f32 mfcc_cfftradix4f32;
    triangular_filters_scale_t mfcc_triangular_filters_scale;
    signal_windowing_t mfcc_signal_windowing;
    uint32_t mfcc_bin[MFCC_TRIANGULAR_FILTERS_BANK_SIZE + 2];
    float32_t* mfcc_multipliers;
} pre_processing_data_t;

/* Exported Functions --------------------------------------------------------*/

void pre_processing_init_mcu(pre_processing_data_t* pre_processing_data, float32_t sensor_odr);
void pre_processing_process_mcu(tridimensional_data_t* data_in ,
                                uint32_t data_in_size ,
                                float32_t * data_out,
                                uint32_t data_out_size,
                                pre_processing_data_t * pre_processing_data);
void pre_processing_free(pre_processing_data_t* pre_processing_data);

#endif /* PRE_PROCESSING_APP_H_ */

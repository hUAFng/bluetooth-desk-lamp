/**
 ******************************************************************************
 * @file    params.h
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

#ifndef PARAMS_H_
#define PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Sensors. */

/* Pre-Processing. */
/* Axis selection. */
#define AXIS_SELECTION_AXIS (X)
/* Signal normalization. */
#define SIGNAL_NORMALIZATION_PEAK_TO_PEAK (2.0)
#define SIGNAL_NORMALIZATION_OFFSET (0.0)
/* MFCC. */
#define MFCC_TRIANGULAR_FILTERS_SCALE (TRIANGULAR_FILTERS_SCALE_HZ)
#define MFCC_SIGNAL_WINDOWING (HANNING)

/* AI-Processing. */
#define NETWORK_NAME ("network")

/* Post-Processing. */

#ifdef __cplusplus
}
#endif

#endif /* PARAMS_H_ */

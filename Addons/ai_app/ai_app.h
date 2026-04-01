/**
 ******************************************************************************
 * @file    ai_app.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version V2.0.0
 * @date    31-March-2022
 *
 * @brief   AI entry function definitions
 *
 *********************************************************************************
   * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
 *********************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AI_APP_H
#define __AI_APP_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ai_platform.h"
#include <stdio.h>

int32_t ai_init(void);
int32_t aiProcess(float *, float[2]);

#ifdef __cplusplus
}
#endif
#endif /* __AI_APP_ */

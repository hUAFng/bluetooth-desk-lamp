/**
  ******************************************************************************
  * @file    sys_functions.h
  * @author  STMicroelectronics
  * @version 1.0.0
  * @date    June 29, 2022
  *
  * @brief File generated with Handlebars.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2022 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "sys_functions.h"
#include "stdint.h"

void sys_error_handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  //__disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void *SysAlloc(size_t nSize) {
  void *pcMemory = (void *) calloc(nSize,sizeof(uint8_t));
  return pcMemory;
}

void SysFree(void *pvData) {
  free(pvData);
}

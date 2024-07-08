/**
 ******************************************************************************
 * @file    usbd_cud.h
 * @author  MCD Application Team
 * @brief   Header for the usbd_cud.c file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CUD_H
#define __USBD_CUD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_ioreq.h"
#include "usbd_msc.h"
#include "usbd_dfu.h"
#include "usbd_composite.h"

/** @addtogroup USBD_CUD_BOT
 * @{
 */

/** @defgroup USBD_CUD
 * @brief This file is the Header file for usbd_cud.c
 * @{
 */

/** @defgroup USBD_BOT_Exported_Defines
 * @{
 */
/* CUD Class Config */
#ifndef CUD_MEDIA_PACKET
#define CUD_MEDIA_PACKET 512U
#endif /* CUD_MEDIA_PACKET */

#define CUD_MAX_FS_PACKET       0x40U
#define CUD_MAX_HS_PACKET       0x200U

#define CUD_EPIN_ADDR           0x81U
#define CUD_EPOUT_ADDR          0x01U

/* Structure for CUD process */
extern USBD_ClassTypeDef USBD_CUD;

uint8_t USBD_CUD_Register(USBD_HandleTypeDef *pdev);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CUD_H */
/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

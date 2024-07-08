/**
 * @file usbd_composite.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-07-07
 * @last modified 2024-07-07
 *
 * @copyright Copyright (c) 2024 Liu Yuanlin Personal.
 *
 */
#ifndef USBD_COMPOSITE_H
#define USBD_COMPOSITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "usbd_desc.h"

//这些是对象的编号不用改
#define USBD_DFU_CLASS_ID      0x00U
#define USBD_MSC_CLASS_ID      0x01U
#define USBD_CDC_CLASS_ID      0x02U
#define USBD_DFU_USERDATA_ID   0x00U
#define USBD_MSC_USERDATA_ID   0x01U
#define USBD_CDC_USERDATA_ID   0x02U

#define ENBALE_DUF_CFGDESC 1

#define MSC_CDC_CFG_IDX 0x01
#define DFU_CFG_IDX     0x02

#define USBD_INTERFACE_NUM     0x03U
#define USBD_MSC_INTERFACE_NUM 0x00U
#define USBD_CDC_INTERFACE_NUM 0x01U

#define USB_CUD_CONFIG_DESC_SIZ (9 + 8 + (USB_MSC_CONFIG_DESC_SIZ - 9) + (USB_CDC_CONFIG_DESC_SIZ - 9))
#ifdef __cplusplus
}
#endif
#endif //! USBD_COMPOSITE_H

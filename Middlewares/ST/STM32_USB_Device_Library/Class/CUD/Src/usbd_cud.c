/**
 ******************************************************************************
 * @file    usbd_cud.c
 * @author  MCD Application Team
 * @brief   This file provides all the CUD core functions.
 *
 * @verbatim
 *
 *          ===================================================================
 *                                CUD Class  Description
 *          ===================================================================
 *           This module manages the CUD class V1.0 following the "Universal
 *           Serial Bus Mass Storage Class (CUD) Bulk-Only Transport (BOT) Version 1.0
 *           Sep. 31, 1999".
 *           This driver implements the following aspects of the specification:
 *             - Bulk-Only Transport protocol
 *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
 *
 *  @endverbatim
 *
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

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_sd.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cud.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
 * @{
 */

/** @defgroup CUD_CORE
 * @brief Mass storage core module
 * @{
 */

/** @defgroup CUD_CORE_Private_TypesDefinitions
 * @{
 */
/**
 * @}
 */

/** @defgroup CUD_CORE_Private_Defines
 * @{
 */

/**
 * @}
 */

/** @defgroup CUD_CORE_Private_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup CUD_CORE_Private_FunctionPrototypes
 * @{
 */
uint8_t USBD_CUD_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CUD_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CUD_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_CUD_EP0_TxReady(USBD_HandleTypeDef *pdev)
{
    return USBD_DFU_EP0_TxReady(pdev);
}
static uint8_t USBD_CUD_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    uint8_t res = 0;
    res         = USBD_CDC_EP0_RxReady(pdev);
    res         = USBD_DFU_EP0_RxReady(pdev);
    return res;
}
uint8_t USBD_CUD_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_CUD_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CUD_SOF(USBD_HandleTypeDef *pdev)
{
    return USBD_DFU_SOF(pdev);
}
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
static uint8_t *USBD_CUD_GetUsrStringDesc(USBD_HandleTypeDef *pdev,
                                          uint8_t index, uint16_t *length)
{
    return USBD_DFU_GetUsrStringDesc(pdev, index, length);
}
#endif
uint8_t *USBD_CUD_GetHSCfgDesc(uint16_t *length);
uint8_t *USBD_CUD_GetFSCfgDesc(uint16_t *length);
uint8_t *USBD_CUD_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_CUD_GetDeviceQualifierDescriptor(uint16_t *length);

/**
 * @}
 */

/** @defgroup CUD_CORE_Private_Variables
 * @{
 */

USBD_ClassTypeDef USBD_CUD =
    {
        USBD_CUD_Init,
        USBD_CUD_DeInit,
        USBD_CUD_Setup,
        USBD_CUD_EP0_TxReady, /*EP0_TxSent*/
        USBD_CUD_EP0_RxReady, /*EP0_RxReady*/
        USBD_CUD_DataIn,
        USBD_CUD_DataOut,
        USBD_CUD_SOF, /*SOF */
        NULL,
        NULL,
        USBD_CUD_GetHSCfgDesc,
        USBD_CUD_GetFSCfgDesc,
        USBD_CUD_GetOtherSpeedCfgDesc,
        USBD_CUD_GetDeviceQualifierDescriptor,
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
        USBD_CUD_GetUsrStringDesc
#endif
};

#define ENBALE_DUF_CFGDESC 1
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN static uint8_t USBD_CUD_CfgHSDesc[USB_CUD_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,                        /* bLength: Configuration Descriptor size */
        USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
        USB_CUD_CONFIG_DESC_SIZ,

        0x00,
        USBD_INTERFACE_NUM, /* bNumInterfaces: 1 interface */
        0x01,               /* bConfigurationValue: */
        0x04,               /* iConfiguration: */
#if (USBD_SELF_POWERED == 1U)
        0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
        0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
        USBD_MAX_POWER, /* MaxPower 200 mA */

#if ENBALE_DUF_CFGDESC
        /**********  Descriptor of DFU interface 0 Alternate setting 0 **************/
        USBD_DFU_IF_DESC(0U), /* This interface is mandatory for all devices */

#if (USBD_DFU_MAX_ITF_NUM > 1U)
        /**********  Descriptor of DFU interface 0 Alternate setting 1 **************/
        USBD_DFU_IF_DESC(1),
#endif /* (USBD_DFU_MAX_ITF_NUM > 1) */

#if (USBD_DFU_MAX_ITF_NUM > 2U)
        /**********  Descriptor of DFU interface 0 Alternate setting 2 **************/
        USBD_DFU_IF_DESC(2),
#endif /* (USBD_DFU_MAX_ITF_NUM > 2) */

#if (USBD_DFU_MAX_ITF_NUM > 3U)
        /**********  Descriptor of DFU interface 0 Alternate setting 3 **************/
        USBD_DFU_IF_DESC(3),
#endif /* (USBD_DFU_MAX_ITF_NUM > 3) */

#if (USBD_DFU_MAX_ITF_NUM > 4U)
        /**********  Descriptor of DFU interface 0 Alternate setting 4 **************/
        USBD_DFU_IF_DESC(4),
#endif /* (USBD_DFU_MAX_ITF_NUM > 4) */

#if (USBD_DFU_MAX_ITF_NUM > 5U)
        /**********  Descriptor of DFU interface 0 Alternate setting 5 **************/
        USBD_DFU_IF_DESC(5),
#endif /* (USBD_DFU_MAX_ITF_NUM > 5) */

#if (USBD_DFU_MAX_ITF_NUM > 6U)
#error "ERROR: usbd_dfu_core.c: Modify the file to support more descriptors!"
#endif /* (USBD_DFU_MAX_ITF_NUM > 6) */

        /******************** DFU Functional Descriptor********************/
        0x09,                /* blength = 9 Bytes */
        DFU_DESCRIPTOR_TYPE, /* DFU Functional Descriptor */
        0x0B,                /* bmAttribute:
                                bitCanDnload             = 1      (bit 0)
                                bitCanUpload             = 1      (bit 1)
                                bitManifestationTolerant = 0      (bit 2)
                                bitWillDetach            = 1      (bit 3)
                                Reserved                          (bit4-6)
                                bitAcceleratedST         = 0      (bit 7) */
        0xFF,                /* DetachTimeOut= 255 ms*/
        0x00,
        /* WARNING: In DMA mode the multiple MPS packets feature is still not supported
         ==> In this case, when using DMA USBD_DFU_XFER_SIZE should be set to 64 in usbd_conf.h */
        TRANSFER_SIZE_BYTES(USBD_DFU_XFER_SIZE), /* TransferSize = 1024 Byte */
        0x1A,                                    /* bcdDFUVersion */
        0x01,
        /***********************************************************/
        /* 9*/
#endif
        /********************  Mass Storage interface ********************/
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        USBD_MSC_INTERFACE_NUM,  /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints 该接口使用的端点数，不包括端点0 */
        0x08,                    /* bInterfaceClass: CUD Class */
        0x06,                    /* bInterfaceSubClass : SCSI transparent */
        0x50,                    /* nInterfaceProtocol */
        0x05,                    /* iInterface: */
        /********************  Mass Storage Endpoints ********************/
        0x07,          /* Endpoint descriptor length = 7 */
        0x05,          /* Endpoint descriptor type */
        CUD_EPIN_ADDR, /* Endpoint address (IN, address 1) USB设备的端点地址: 0x81 => 1000 00001(Bit7 表示方向 1/0 => In/Out ，Bit3~0表示端点号)*/
        0x02,          /* Bulk endpoint type 端点属性:(Bit1-0: 00控制，01同步，02批量，03中断) */
        LOBYTE(CUD_MAX_HS_PACKET),
        HIBYTE(CUD_MAX_HS_PACKET),
        0x00, /* Polling interval in milliseconds */

        0x07,           /* Endpoint descriptor length = 7 */
        0x05,           /* Endpoint descriptor type */
        CUD_EPOUT_ADDR, /* Endpoint address (OUT, address 1) */
        0x02,           /* Bulk endpoint type */
        LOBYTE(CUD_MAX_HS_PACKET),
        HIBYTE(CUD_MAX_HS_PACKET),
        0x00, /* Polling interval in milliseconds */

        /*-------------------------- CDC ------------------------*/

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        USBD_CDC_INTERFACE_NUM,  /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x01,                    /* bNumEndpoints: One endpoints used */
        0x02,                    /* bInterfaceClass: Communication Interface Class */
        0x02,                    /* bInterfaceSubClass: Abstract Control Model */
        0x01,                    /* bInterfaceProtocol: Common AT commands */
        0x00,                    /* iInterface: */

        /* Header Functional Descriptor */
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /* Call Management Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /* ACM Functional Descriptor */
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /* Union Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /* Endpoint 2 Descriptor */
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC_CMD_EP,                  /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_HS_BINTERVAL, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /* Data class interface descriptor */
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE,       /* bDescriptorType: */
        USBD_CDC_INTERFACE_NUM + 0x01, /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */

        /* Endpoint OUT Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_OUT_EP,                          /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
        0x00, /* bInterval: ignore for Bulk transfer */

        /* Endpoint IN Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_IN_EP,                           /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
        0x00 /* bInterval: ignore for Bulk transfer */
};

/* USB Mass storage device Configuration Descriptor */
/* All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN static uint8_t USBD_CUD_CfgFSDesc[USB_CUD_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,                        /* bLength: Configuration Descriptor size */
        USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
        USB_CUD_CONFIG_DESC_SIZ,

        0x00,
        USBD_INTERFACE_NUM, /* bNumInterfaces: 1 interface */
        0x01,               /* bConfigurationValue: */
        0x04,               /* iConfiguration: */
#if (USBD_SELF_POWERED == 1U)
        0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
        0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
        USBD_MAX_POWER, /* MaxPower 100 mA */
#if ENBALE_DUF_CFGDESC
        /**********  Descriptor of DFU interface 0 Alternate setting 0 **************/
        USBD_DFU_IF_DESC(0U), /* This interface is mandatory for all devices */

#if (USBD_DFU_MAX_ITF_NUM > 1U)
        /**********  Descriptor of DFU interface 0 Alternate setting 1 **************/
        USBD_DFU_IF_DESC(1),
#endif /* (USBD_DFU_MAX_ITF_NUM > 1) */

#if (USBD_DFU_MAX_ITF_NUM > 2U)
        /**********  Descriptor of DFU interface 0 Alternate setting 2 **************/
        USBD_DFU_IF_DESC(2),
#endif /* (USBD_DFU_MAX_ITF_NUM > 2) */

#if (USBD_DFU_MAX_ITF_NUM > 3U)
        /**********  Descriptor of DFU interface 0 Alternate setting 3 **************/
        USBD_DFU_IF_DESC(3),
#endif /* (USBD_DFU_MAX_ITF_NUM > 3) */

#if (USBD_DFU_MAX_ITF_NUM > 4U)
        /**********  Descriptor of DFU interface 0 Alternate setting 4 **************/
        USBD_DFU_IF_DESC(4),
#endif /* (USBD_DFU_MAX_ITF_NUM > 4) */

#if (USBD_DFU_MAX_ITF_NUM > 5U)
        /**********  Descriptor of DFU interface 0 Alternate setting 5 **************/
        USBD_DFU_IF_DESC(5),
#endif /* (USBD_DFU_MAX_ITF_NUM > 5) */

#if (USBD_DFU_MAX_ITF_NUM > 6U)
#error "ERROR: usbd_dfu_core.c: Modify the file to support more descriptors!"
#endif /* (USBD_DFU_MAX_ITF_NUM > 6) */

        /******************** DFU Functional Descriptor********************/
        0x09,                /* blength = 9 Bytes */
        DFU_DESCRIPTOR_TYPE, /* DFU Functional Descriptor */
        0x0B,                /* bmAttribute:
                                bitCanDnload             = 1      (bit 0)
                                bitCanUpload             = 1      (bit 1)
                                bitManifestationTolerant = 0      (bit 2)
                                bitWillDetach            = 1      (bit 3)
                                Reserved                          (bit4-6)
                                bitAcceleratedST         = 0      (bit 7) */
        0xFF,                /* DetachTimeOut= 255 ms*/
        0x00,
        /* WARNING: In DMA mode the multiple MPS packets feature is still not supported
         ==> In this case, when using DMA USBD_DFU_XFER_SIZE should be set to 64 in usbd_conf.h */
        TRANSFER_SIZE_BYTES(USBD_DFU_XFER_SIZE), /* TransferSize = 1024 Byte */
        0x1A,                                    /* bcdDFUVersion */
        0x01,
        /***********************************************************/
        /* 9*/
#endif
        /********************  Mass Storage interface ********************/
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        USBD_MSC_INTERFACE_NUM,  /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints*/
        0x08,                    /* bInterfaceClass: CUD Class */
        0x06,                    /* bInterfaceSubClass : SCSI transparent*/
        0x50,                    /* nInterfaceProtocol */
        0x05,                    /* iInterface: */
        /********************  Mass Storage Endpoints ********************/
        0x07,          /* Endpoint descriptor length = 7 */
        0x05,          /* Endpoint descriptor type */
        CUD_EPIN_ADDR, /* Endpoint address (IN, address 1) */
        0x02,          /* Bulk endpoint type */
        LOBYTE(CUD_MAX_FS_PACKET),
        HIBYTE(CUD_MAX_FS_PACKET),
        0x00, /* Polling interval in milliseconds */

        0x07,           /* Endpoint descriptor length = 7 */
        0x05,           /* Endpoint descriptor type */
        CUD_EPOUT_ADDR, /* Endpoint address (OUT, address 1) */
        0x02,           /* Bulk endpoint type */
        LOBYTE(CUD_MAX_FS_PACKET),
        HIBYTE(CUD_MAX_FS_PACKET),
        0x00, /* Polling interval in milliseconds */

        /*-------------------------- CDC ------------------------*/

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        USBD_CDC_INTERFACE_NUM,  /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x01,                    /* bNumEndpoints: One endpoints used */
        0x02,                    /* bInterfaceClass: Communication Interface Class */
        0x02,                    /* bInterfaceSubClass: Abstract Control Model */
        0x01,                    /* bInterfaceProtocol: Common AT commands */
        0x00,                    /* iInterface: */

        /* Header Functional Descriptor */
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /* Call Management Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /* ACM Functional Descriptor */
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /* Union Functional Descriptor */
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /* Endpoint 2 Descriptor */
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC_CMD_EP,                  /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_HS_BINTERVAL, /* bInterval: */
        /*---------------------------------------------------------------------------*/

        /* Data class interface descriptor */
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE,       /* bDescriptorType: */
        USBD_CDC_INTERFACE_NUM + 0x01, /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */

        /* Endpoint OUT Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_OUT_EP,                          /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00, /* bInterval: ignore for Bulk transfer */

        /* Endpoint IN Descriptor */
        0x07,                                /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
        CDC_IN_EP,                           /* bEndpointAddress */
        0x02,                                /* bmAttributes: Bulk */
        LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
        0x00 /* bInterval: ignore for Bulk transfer */
};

__ALIGN_BEGIN static uint8_t USBD_CUD_OtherSpeedCfgDesc[USB_CUD_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09, /* bLength: Configuration Descriptor size */
        USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
        USB_CUD_CONFIG_DESC_SIZ,

        0x00,
        USBD_INTERFACE_NUM, /* bNumInterfaces: 1 interface */
        0x01,               /* bConfigurationValue: */
        0x04,               /* iConfiguration: */
#if (USBD_SELF_POWERED == 1U)
        0xC0, /* bmAttributes: Bus Powered according to user configuration */
#else
        0x80, /* bmAttributes: Bus Powered according to user configuration */
#endif
        USBD_MAX_POWER, /* MaxPower 100 mA */
#if ENBALE_DUF_CFGDESC
        /**********  Descriptor of DFU interface 0 Alternate setting 0 **************/
        USBD_DFU_IF_DESC(0U), /* This interface is mandatory for all devices */

#if (USBD_DFU_MAX_ITF_NUM > 1U)
        /**********  Descriptor of DFU interface 0 Alternate setting 1 **************/
        USBD_DFU_IF_DESC(1),
#endif /* (USBD_DFU_MAX_ITF_NUM > 1) */

#if (USBD_DFU_MAX_ITF_NUM > 2U)
        /**********  Descriptor of DFU interface 0 Alternate setting 2 **************/
        USBD_DFU_IF_DESC(2),
#endif /* (USBD_DFU_MAX_ITF_NUM > 2) */

#if (USBD_DFU_MAX_ITF_NUM > 3U)
        /**********  Descriptor of DFU interface 0 Alternate setting 3 **************/
        USBD_DFU_IF_DESC(3),
#endif /* (USBD_DFU_MAX_ITF_NUM > 3) */

#if (USBD_DFU_MAX_ITF_NUM > 4U)
        /**********  Descriptor of DFU interface 0 Alternate setting 4 **************/
        USBD_DFU_IF_DESC(4),
#endif /* (USBD_DFU_MAX_ITF_NUM > 4) */

#if (USBD_DFU_MAX_ITF_NUM > 5U)
        /**********  Descriptor of DFU interface 0 Alternate setting 5 **************/
        USBD_DFU_IF_DESC(5),
#endif /* (USBD_DFU_MAX_ITF_NUM > 5) */

#if (USBD_DFU_MAX_ITF_NUM > 6U)
#error "ERROR: usbd_dfu_core.c: Modify the file to support more descriptors!"
#endif /* (USBD_DFU_MAX_ITF_NUM > 6) */

        /******************** DFU Functional Descriptor********************/
        0x09,                /* blength = 9 Bytes */
        DFU_DESCRIPTOR_TYPE, /* DFU Functional Descriptor */
        0x0B,                /* bmAttribute:
                                bitCanDnload             = 1      (bit 0)
                                bitCanUpload             = 1      (bit 1)
                                bitManifestationTolerant = 0      (bit 2)
                                bitWillDetach            = 1      (bit 3)
                                Reserved                          (bit4-6)
                                bitAcceleratedST         = 0      (bit 7) */
        0xFF,                /* DetachTimeOut= 255 ms*/
        0x00,
        /* WARNING: In DMA mode the multiple MPS packets feature is still not supported
         ==> In this case, when using DMA USBD_DFU_XFER_SIZE should be set to 64 in usbd_conf.h */
        TRANSFER_SIZE_BYTES(USBD_DFU_XFER_SIZE), /* TransferSize = 1024 Byte */
        0x1A,                                    /* bcdDFUVersion */
        0x01,
        /***********************************************************/
        /* 9*/
#endif
        /********************  Mass Storage interface ********************/
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
        USBD_MSC_INTERFACE_NUM,  /* bInterfaceNumber: Number of Interface */
        0x00,                    /* bAlternateSetting: Alternate setting */
        0x02,                    /* bNumEndpoints */
        0x08,                    /* bInterfaceClass: CUD Class */
        0x06,                    /* bInterfaceSubClass : SCSI transparent command set */
        0x50,                    /* nInterfaceProtocol */
        0x05,                    /* iInterface: */
        /********************  Mass Storage Endpoints ********************/
        0x07,          /* Endpoint descriptor length = 7 */
        0x05,          /* Endpoint descriptor type */
        CUD_EPIN_ADDR, /* Endpoint address (IN, address 1) */
        0x02,          /* Bulk endpoint type */
        0x40,
        0x00,
        0x00, /* Polling interval in milliseconds */

        0x07,           /* Endpoint descriptor length = 7 */
        0x05,           /* Endpoint descriptor type */
        CUD_EPOUT_ADDR, /* Endpoint address (OUT, address 1) */
        0x02,           /* Bulk endpoint type */
        0x40,
        0x00,
        0x00, /* Polling interval in milliseconds */

        /*-------------------------- CDC ------------------------*/

        /* Interface Descriptor */
        0x09,                    /* bLength: Interface Descriptor size */
        USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        USBD_CDC_INTERFACE_NUM, /* bInterfaceNumber: Number of Interface */
        0x00,                   /* bAlternateSetting: Alternate setting */
        0x01,                   /* bNumEndpoints: One endpoints used */
        0x02,                   /* bInterfaceClass: Communication Interface Class */
        0x02,                   /* bInterfaceSubClass: Abstract Control Model */
        0x01,                   /* bInterfaceProtocol: Common AT commands */
        0x00,                   /* iInterface: */

        /* Header Functional Descriptor */
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /*Call Management Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /*Endpoint 2 Descriptor*/
        0x07,                        /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
        CDC_CMD_EP,                  /* bEndpointAddress */
        0x03,                        /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SIZE),
        CDC_FS_BINTERVAL, /* bInterval: */

        /*---------------------------------------------------------------------------*/

        /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_INTERFACE,       /* bDescriptorType: */
        USBD_CDC_INTERFACE_NUM + 0x01, /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */

        /*Endpoint OUT Descriptor*/
        0x07,                   /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
        CDC_OUT_EP,             /* bEndpointAddress */
        0x02,                   /* bmAttributes: Bulk */
        0x40,                   /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */

        /*Endpoint IN Descriptor*/
        0x07,                   /* bLength: Endpoint Descriptor size */
        USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
        CDC_IN_EP,              /* bEndpointAddress */
        0x02,                   /* bmAttributes: Bulk */
        0x40,                   /* wMaxPacketSize: */
        0x00,
        0x00 /* bInterval */
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
    {
        USB_LEN_DEV_QUALIFIER_DESC,
        USB_DESC_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        CUD_MAX_FS_PACKET,
        0x01,
        0x00,
};

/**
 * @}
 */

/** @defgroup CUD_CORE_Private_Functions
 * @{
 */

/**
 * @brief  USBD_CUD_Init
 *         Initialize  the mass storage configuration
 * @param  pdev: device instance
 * @param  cfgidx: configuration index
 * @retval status
 */
uint8_t USBD_CUD_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t res = 0;
    res         = USBD_DFU_Init(pdev, cfgidx);
    res         = USBD_MSC_Init(pdev, cfgidx);
    res         = USBD_CDC_Init(pdev, cfgidx);
    return res;
}

/**
 * @brief  USBD_CUD_DeInit
 *         DeInitialize  the mass storage configuration
 * @param  pdev: device instance
 * @param  cfgidx: configuration index
 * @retval status
 */
uint8_t USBD_CUD_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t res = 0;
    res         = USBD_DFU_DeInit(pdev, cfgidx);
    res         = USBD_MSC_DeInit(pdev, cfgidx);
    res         = USBD_CDC_DeInit(pdev, cfgidx);
    return res;
}
/**
 * @brief  USBD_CUD_Setup
 *         Handle the CUD specific requests
 * @param  pdev: device instance
 * @param  req: USB request
 * @retval status
 */
uint8_t USBD_CUD_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint8_t res = 0;
    switch (req->wIndex) {
        case USBD_MSC_INTERFACE_NUM:
            res = USBD_MSC_Setup(pdev, req);
            break;
        case USBD_DFU_INTERFACE_NUM:
            res = USBD_DFU_Setup(pdev, req);
            break;
        case USBD_CDC_INTERFACE_NUM:
            res = USBD_CDC_Setup(pdev, req);
            break;
        case USBD_CDC_INTERFACE_NUM + 0x01:
            res = USBD_CDC_Setup(pdev, req);
            break;
        default:
            // 未识别的接口请求
            USBD_CtlError(pdev, req);
            break;
    }
    return res;
}

/**
 * @brief  USBD_CUD_DataIn
 *         handle data IN Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
uint8_t USBD_CUD_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    if (epnum == CUD_EPIN_ADDR || epnum == CUD_EPOUT_ADDR) {
        return USBD_MSC_DataIn(pdev, epnum);
    } else if (epnum == CDC_IN_EP || epnum == CDC_OUT_EP || epnum == CDC_CMD_EP) {
        return USBD_CDC_DataIn(pdev, epnum);
    }

    return USBD_OK;
}

/**
 * @brief  USBD_CUD_DataOut
 *         handle data OUT Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
uint8_t USBD_CUD_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    if (epnum == CUD_EPIN_ADDR || epnum == CUD_EPOUT_ADDR) {
        return USBD_MSC_DataOut(pdev, epnum);
    } else if (epnum == CDC_IN_EP || epnum == CDC_OUT_EP || epnum == CDC_CMD_EP) {
        return USBD_CDC_DataOut(pdev, epnum);
    }
    return USBD_OK;
}

/**
 * @brief  USBD_CUD_GetHSCfgDesc
 *         return configuration descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
uint8_t *USBD_CUD_GetHSCfgDesc(uint16_t *length)
{
    *length = (uint16_t)sizeof(USBD_CUD_CfgHSDesc);

    return USBD_CUD_CfgHSDesc;
}

/**
 * @brief  USBD_CUD_GetFSCfgDesc
 *         return configuration descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
uint8_t *USBD_CUD_GetFSCfgDesc(uint16_t *length)
{
    *length = (uint16_t)sizeof(USBD_CUD_CfgFSDesc);

    return USBD_CUD_CfgFSDesc;
}

/**
 * @brief  USBD_CUD_GetOtherSpeedCfgDesc
 *         return other speed configuration descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
uint8_t *USBD_CUD_GetOtherSpeedCfgDesc(uint16_t *length)
{
    *length = (uint16_t)sizeof(USBD_CUD_OtherSpeedCfgDesc);

    return USBD_CUD_OtherSpeedCfgDesc;
}
/**
 * @brief  DeviceQualifierDescriptor
 *         return Device Qualifier descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
uint8_t *USBD_CUD_GetDeviceQualifierDescriptor(uint16_t *length)
{
    *length = (uint16_t)sizeof(USBD_CUD_DeviceQualifierDesc);

    return USBD_CUD_DeviceQualifierDesc;
}

#include "usbd_storage_if.h"
#include "usbd_dfu_flash.h"
/**
 * @brief  USBD_CUD_Register
 * @param  fops: storage callback
 * @retval status
 */
uint8_t USBD_CUD_Register(USBD_HandleTypeDef *pdev)
{
    pdev->pUserDatas[USBD_MSC_USERDATA_ID] = &USBD_Storage_Interface_fops_FS;
    pdev->pUserDatas[USBD_DFU_USERDATA_ID] = &USBD_DFU_Flash_fops;
    return USBD_OK;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

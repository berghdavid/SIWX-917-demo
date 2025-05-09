/*******************************************************************************
* @file  rsi_error.h
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>

#ifndef __RSI_ERROR_H__
#define __RSI_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum errnoCode {
  RSI_FAIL           = -1,
  RSI_OK             = 0,
  INVALID_PARAMETERS = 1,

  /*USART error codes*/
  ERROR_USART_BASE          = 0x100,
  ERROR_USART_CALLBACK_ERR  = ERROR_USART_BASE + 1,
  ERROR_USART_NOT_SUPPORTED = ERROR_USART_CALLBACK_ERR + 1,

  /* GPDMA error codes */
  ERROR_GPDMA_BASE              = 0x200,
  ERROR_GPDMA_INVALIDCHNLNUM    = ERROR_GPDMA_BASE + 1,
  ERROR_GPDMA_FLW_CTRL          = ERROR_GPDMA_INVALIDCHNLNUM + 1,
  ERROR_GPDMA_BURST             = ERROR_GPDMA_FLW_CTRL + 1,
  ERROR_GPDMA_SRC_ADDR          = ERROR_GPDMA_BURST + 1,
  ERROR_GPDMA_DST_ADDR          = ERROR_GPDMA_SRC_ADDR + 1,
  NOERR_GPDMA_FLAG_SET          = ERROR_GPDMA_DST_ADDR + 1,
  ERROR_GPDMA_INVALID_EVENT     = NOERR_GPDMA_FLAG_SET + 1,
  ERROR_GPDMA_INVALID_XFERMODE  = ERROR_GPDMA_INVALID_EVENT + 1,
  ERROR_GPDMA_INVALID_TRANS_LEN = ERROR_GPDMA_INVALID_XFERMODE + 1,
  ERROR_GPDMA_INVALID_ARG       = ERROR_GPDMA_INVALID_TRANS_LEN + 1,
  ERROR_GPDMA_CHNL_BUSY         = ERROR_GPDMA_INVALID_ARG + 1,
  ERROR_GPDMA_NOT_ALIGNMENT     = ERROR_GPDMA_CHNL_BUSY + 1,
  ERROR_GPDMA_QUEUE_EMPTY       = ERROR_GPDMA_NOT_ALIGNMENT + 1,
  ERROR_GPDMA_GENERAL           = ERROR_GPDMA_QUEUE_EMPTY + 1,

  /* UDMA error codes */
  ERROR_UDMA_BASE              = 0x300,
  ERROR_UDMA_INVALIDCHNLNUM    = ERROR_UDMA_BASE + 1,
  ERROR_UDMA_CTRL_BASE_INVALID = ERROR_UDMA_INVALIDCHNLNUM + 1,
  ERROR_UDMA_INVALID_XFERMODE  = ERROR_UDMA_CTRL_BASE_INVALID + 1,
  ERROR_UDMA_INVALID_TRANS_LEN = ERROR_UDMA_INVALID_XFERMODE + 1,
  ERROR_UDMA_INVALID_ARG       = ERROR_UDMA_INVALID_TRANS_LEN + 1,
  ERROR_UDMA_SRC_ADDR          = ERROR_UDMA_INVALID_ARG + 1,
  ERROR_UDMA_DST_ADDR          = ERROR_UDMA_SRC_ADDR + 1,
  ERROR_UDMA_CHNL_BUSY         = ERROR_UDMA_DST_ADDR + 1,

  /* I2C error codes */
  ERROR_I2C_BASE                = 0x400,
  ERROR_I2C_INVALID_ARG         = ERROR_I2C_BASE + 1,
  ERROR_I2CS_UNKNOWN            = ERROR_I2C_INVALID_ARG + 1,
  ERROR_I2C_SPIKE_LOGIC         = ERROR_I2CS_UNKNOWN + 1,
  ERROR_I2C_IGNORE_GC_OR_START  = ERROR_I2C_SPIKE_LOGIC + 1,
  ERROR_I2C_STATUS_FLAG_NOT_SET = ERROR_I2C_IGNORE_GC_OR_START + 1,
  ERROR_I2C_BUSY_FLAG           = ERROR_I2C_STATUS_FLAG_NOT_SET + 1,
  ERROR_I2C_MST_BUSY_FLAG       = ERROR_I2C_BUSY_FLAG + 1,
  ERROR_I2C_SLV_BUSY_FLAG       = ERROR_I2C_MST_BUSY_FLAG + 1,
  ERROR_I2C_SLV_DIS_WHILE_BUSY  = ERROR_I2C_SLV_BUSY_FLAG + 1,
  ERROR_I2C_MST_XFER_ABORT      = ERROR_I2C_SLV_DIS_WHILE_BUSY + 1,
  ERROR_I2C_MST_TX_CMD_BLOCK    = ERROR_I2C_MST_XFER_ABORT + 1,
  ERROR_I2C_SLV_RX_DATA_LOST    = ERROR_I2C_MST_TX_CMD_BLOCK + 1,
  ERROR_I2C_NO_TX_DATA          = ERROR_I2C_SLV_RX_DATA_LOST + 1,
  ERROR_I2C_NO_INTR_FLAG        = ERROR_I2C_NO_TX_DATA + 1,
  ERROR_I2C_ERROR_FLAG_NONE     = ERROR_I2C_NO_INTR_FLAG + 1,
  ERROR_I2C_INVALID_CB          = ERROR_I2C_ERROR_FLAG_NONE + 1,
  ERROR_I2C_INVALID_POINTER     = ERROR_I2C_INVALID_CB + 1,
  ERROR_I2C_GENERAL_FAILURE     = ERROR_I2C_INVALID_POINTER + 1,
  ERROR_I2C_TXABORT             = ERROR_I2C_GENERAL_FAILURE + 1,
  ERROR_I2C_SCL_STUCK_ATLOW     = ERROR_I2C_TXABORT + 1,
  ERROR_I2C_MST_ON_HOLD         = ERROR_I2C_SCL_STUCK_ATLOW + 1,
  ERROR_I2C_BUFFER_OVERFLOW     = ERROR_I2C_MST_ON_HOLD + 1,
  ERROR_I2C_BUFFER_UNDERFLOW    = ERROR_I2C_BUFFER_OVERFLOW + 1,

  /* I2S error codes */
  ERROR_I2S_BASE           = 0x500,
  ERROR_I2S_INVALID_ARG    = ERROR_I2S_BASE + 1,
  ERROR_I2S_INVALID_RES    = ERROR_I2S_INVALID_ARG + 1,
  ERROR_I2S_INVALID_LENGTH = ERROR_I2S_INVALID_RES + 1,
  ERROR_I2S_BUSY           = ERROR_I2S_INVALID_LENGTH + 1,
  ERROR_I2S_TXOVERRUN      = ERROR_I2S_BUSY + 1,
  ERROR_I2S_RXOVERRUN      = ERROR_I2S_TXOVERRUN + 1,
  ERROR_I2S_TXCOMPLETE     = ERROR_I2S_RXOVERRUN + 1,
  ERROR_I2S_RXCOMPLETE     = ERROR_I2S_TXCOMPLETE + 1,

  /* UART error codes */
  ERROR_UART_BASE        = 0x600,
  ERROR_UART_INVALID_ARG = ERROR_UART_BASE + 1,
  ERROR_UART_INVALID_RES = ERROR_UART_INVALID_ARG + 1,

  /* PWM error codes */
  ERROR_PWM_BASE            = 0x700,
  ERROR_PWM_INVALID_CHNLNUM = ERROR_PWM_BASE + 1,
  ERROR_PWM_INVALID_PWMOUT  = ERROR_PWM_INVALID_CHNLNUM + 1,
  ERROR_PWM_NO_INTR         = ERROR_PWM_INVALID_PWMOUT + 1,
  ERROR_PWM_INVALID_ARG     = ERROR_PWM_NO_INTR + 1,

  /* Timers error codes */
  ERROR_TIMER_BASE        = 0x800,
  ERROR_INVAL_TIMER_NUM   = ERROR_TIMER_BASE + 1,
  ERROR_INVAL_TIMER_MODE  = ERROR_INVAL_TIMER_NUM + 1,
  ERROR_INVAL_TIMERTYPE   = ERROR_INVAL_TIMER_MODE + 1,
  ERROR_INVAL_COUNTER_DIR = ERROR_INVAL_TIMERTYPE + 1,

  /* SCT error codes */
  ERROR_CT_BASE                = 0x900,
  ERROR_CT_INVALID_COUNTER_NUM = ERROR_CT_BASE + 1,
  ERROR_CT_INVALID_ARG         = ERROR_CT_INVALID_COUNTER_NUM + 1,

  /* EFUSE ERROR CODES */
  ERROR_EFUSE_BASE                       = 0xA00,
  ERROR_EFUSE_INVALID_WRITE_ADDRESS      = ERROR_EFUSE_BASE + 1,
  ERROR_EFUSE_INVALID_WRITE_BIT_POSITION = ERROR_EFUSE_INVALID_WRITE_ADDRESS + 1,
  ERROR_EFUSE_INVALID_PARAMETERS         = ERROR_EFUSE_INVALID_WRITE_BIT_POSITION + 1,

  /* CCI ERROR CODES */
  ERROR_CCI_BASE_ADDRESS = 0xB00,
  ERROR_CCI_INIT_FAIL    = ERROR_CCI_BASE_ADDRESS + 1,
  ERROR_CCI_ADDRESS_ERR  = ERROR_CCI_INIT_FAIL + 1,

  /* QEI ERROR CODES */
  ERROR_QEI_BASE                   = 0xC00,
  ERROR_INVALID_WRITE_ADDRESS      = ERROR_QEI_BASE + 1,
  ERROR_INVALID_WRITE_BIT_POSITION = ERROR_INVALID_WRITE_ADDRESS + 1,
  ERROR_INVALID_PARAMETERS         = ERROR_INVALID_WRITE_BIT_POSITION + 1,

  /* SDIO ERROR CODES */
  ERROR_SSDIO_BASE_ADDRESS  = 0xD00, /*!< SDIO Error base address */
  ERROR_SSDIO_INIT_FAIL     = ERROR_SSDIO_BASE_ADDRESS + 1,
  ERROR_SSDIO_ADDRESS_ERR   = ERROR_SSDIO_INIT_FAIL + 1,
  ERROR_SSDIO_INVALID_FN    = ERROR_SSDIO_ADDRESS_ERR + 1,
  ERROR_SSDIO_INVALID_PARAM = ERROR_SSDIO_INVALID_FN + 1,

  /* SPI ERROR CODES*/
  ERROR_SSPI_BASE_ADDRESS = 0xE00,
  ERROR_SSPI_INIT_FAIL    = ERROR_SSPI_BASE_ADDRESS + 1,
  ERROR_SSPI_ADDRESS_ERR  = ERROR_SSPI_INIT_FAIL + 1,
  ERROR_SSPI_CB_ERROR     = ERROR_SSPI_ADDRESS_ERR + 1,

  /* ETHERNET ERROR CODES */
  ERROR_ETH_BASE_ADDRESS = 0xF00,
  ERROR_ETH_INIT_FAIL    = ERROR_ETH_BASE_ADDRESS + 1,
  ERROR_ETH_PARAM        = ERROR_ETH_INIT_FAIL + 1,
  ERROR_ETH_NULL         = ERROR_ETH_PARAM + 1,
  ERR_DMA_NOT_ALIGNMENT  = ERROR_ETH_NULL + 1,
  ERROR_ETH_CALLBACK_ERR = ERR_DMA_NOT_ALIGNMENT + 1,

  /*CAN ERROR CODES*/
  ERROR_CAN_BASE                      = 0x1000,
  ERROR_CAN_INVALID_PARAMETERS        = ERROR_CAN_BASE + 1,
  ERROR_CAN_INVALID_TIMING_PARAMETERS = ERROR_CAN_INVALID_PARAMETERS + 1,
  ERROR_CAN_OPERATION_IN_PROGRESS     = ERROR_CAN_INVALID_TIMING_PARAMETERS + 1,

  /*GSPI ERROR CODES*/
  ERROR_GSPI_BASE           = 0x1100,
  ERROR_GSPI_INVALID_ARG    = ERROR_GSPI_BASE + 1,
  ERROR_GSPI_INVALID_LENGTH = ERROR_GSPI_BASE + 2,
  ERROR_GSPI_BUSY           = ERROR_GSPI_BASE + 3,
  ERROR_GSPI_READ_DONE      = ERROR_GSPI_BASE + 4,
  ERROR_GSPI_IDLE           = ERROR_GSPI_BASE + 5,
  ERROR_GSPI_TX_DONE        = ERROR_GSPI_BASE + 6,

  /*SSI ERROR CODES*/
  ERROR_SSI_BASE        = 0x1200,
  ERROR_SSI_INVALID_ARG = ERROR_SSI_BASE + 1,
  ERROR_SSI_BUSY        = ERROR_SSI_BASE + 2,
  ERROR_SSI_IDLE        = ERROR_SSI_BASE + 3,
  TRANSFER_COMPLETE     = ERROR_SSI_BASE + 4,
  READ_COMPLETED        = ERROR_SSI_BASE + 5,

  /*SSI ERROR CODES*/
  ERROR_CRC_BASE        = 0x1300,
  ERROR_CRC_INVALID_ARG = ERROR_CRC_BASE + 1,

  /*SSI ERROR CODES*/
  ERROR_RNG_BASE        = 0x1400,
  ERROR_RNG_INVALID_ARG = ERROR_RNG_BASE + 1,

  /*NPSS ERROR CODES*/
  ERROR_BOD_BASE               = 0x1500,
  ERROR_PS_BASE                = ERROR_BOD_BASE + 1,
  ERROR_BOD_INVALID_PARAMETERS = ERROR_PS_BASE + 1,
  ERROR_PS_INVALID_PARAMETERS  = ERROR_BOD_INVALID_PARAMETERS + 1,
  ERROR_PS_INVALID_STATE       = ERROR_PS_INVALID_PARAMETERS + 1,

  /*TIME PERIOD*/
  ERROR_TIMEPERIOD_BASE               = 0x1600,
  ERROR_TIME_PERIOD_PARAMETERS        = ERROR_TIMEPERIOD_BASE + 1,
  ERROR_TIME_PERIOD_RC_CALIB_NOT_DONE = ERROR_TIME_PERIOD_PARAMETERS + 1,
  ERROR_CAL_INVALID_PARAMETERS        = ERROR_TIME_PERIOD_RC_CALIB_NOT_DONE + 1,

  /*M4SS CLOCKS */
  ERROR_M4SS_CLK_BASE           = 0x1700,
  ERROR_CLOCK_NOT_ENABLED       = ERROR_M4SS_CLK_BASE + 1,
  ERROR_INVALID_INPUT_FREQUENCY = ERROR_CLOCK_NOT_ENABLED + 1,

  /*ULPSS CLOCKS */
  ERROR_ULPCLK_BASE               = 1800,
  ERROR_ULPCLK_INVALID_PARAMETERS = ERROR_ULPCLK_BASE + 1,

  ERROR_SIO_BASE       = 0x1900,
  ERROR_SIO_I2C_NO_ACK = ERROR_SIO_BASE + 1,

  /* ULPSS FIM */
  ERROR_FIM_BASE               = 0x2000,
  ERROR_FIM_MATRIX_INVALID_ARG = ERROR_FIM_BASE + 1,

  /* AUX ADC */
  ERROR_NO_MULTI_CHNL_ENABLE = 0x2100,
  NO_MODE_SET                = ERROR_NO_MULTI_CHNL_ENABLE + 1,
  ERROR_PING_PONG_ADDR_MATCH = ERROR_NO_MULTI_CHNL_ENABLE + 2,
  ERROR_ADC_INVALID_ARG      = ERROR_NO_MULTI_CHNL_ENABLE + 3,
  INVALID_SAMPLING_RATE      = ERROR_NO_MULTI_CHNL_ENABLE + 4,
  INVALID_AUX_REF_VOLTAGE    = ERROR_NO_MULTI_CHNL_ENABLE + 5,
  INVALID_SAMPLE_LENGTH      = ERROR_NO_MULTI_CHNL_ENABLE + 6,
  INVALID_ADC_CHANNEL_ENABLE = ERROR_NO_MULTI_CHNL_ENABLE + 6,

  /* AUX DAC */
  ERROR_NO_PAD_SEL = 0x2200,
  ERROR_FREQ_VAL   = ERROR_NO_PAD_SEL + 1,

  /*SDMEM*/
  ERROR_ACCESS_RIGHTS        = 0x2300,
  ERROR_ADDR_ALIGHMENGT      = ERROR_ACCESS_RIGHTS + 1,
  ERROR_SMIH                 = ERROR_ADDR_ALIGHMENGT + 1,
  ERROR_INAVLID_MODE         = ERROR_SMIH + 1,
  ERROR_OPERATION_INPROGRESS = ERROR_INAVLID_MODE + 1,
  ERROR_NOT_READY            = ERROR_OPERATION_INPROGRESS + 1,
  ERROR_UNINITIALIZED        = ERROR_NOT_READY + 1,
  ERROR_BUFFER_FULL          = ERROR_UNINITIALIZED + 1,
  ERROR_TIMEOUT              = ERROR_BUFFER_FULL + 1,
  CARD_NOT_READY_OP          = ERROR_TIMEOUT + 1,
  CARD_TYPE_MEMCARD          = CARD_NOT_READY_OP + 1,
} rsi_error_t;

#ifdef __cplusplus
}
#endif

#endif // __RSI_ERROR_H__

/*END OF FILE */

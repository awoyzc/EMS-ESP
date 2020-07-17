/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * ESP32 UART port by @ArwedL and improved by @MichaelDvP. See https://github.com/proddy/EMS-ESP/issues/380
 */
#if defined(ESP32)

#ifndef EMSESP_EMSUART_H
#define EMSESP_EMSUART_H

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "freertos/queue.h"
#include <driver/uart.h>
#include <driver/timer.h>

#define EMS_MAXBUFFERSIZE 33 // max size of the buffer. EMS packets are max 32 bytes, plus extra for BRK

#define EMSUART_UART UART_NUM_2 // on the ESP32 we're using UART2
#define EMS_UART UART2          // for intr setting
#define EMSUART_BAUD 9600       // uart baud rate for the EMS circuit

#define EMS_TXMODE_DEFAULT 1
#define EMS_TXMODE_EMSPLUS 2
#define EMS_TXMODE_HT3 3
#define EMS_TXMODE_NEW 4 // for michael's testing

// LEGACY
#define EMSUART_TX_BIT_TIME 104                        // bit time @9600 baud
#define EMSUART_TX_WAIT_BRK (EMSUART_TX_BIT_TIME * 11) // 1144

// EMS 1.0
#define EMSUART_TX_BUSY_WAIT (EMSUART_TX_BIT_TIME / 8)                       // 13
#define EMSUART_TX_TIMEOUT (32 * EMSUART_TX_BIT_TIME / EMSUART_TX_BUSY_WAIT) // 256

// HT3/Junkers - Time to send one Byte (8 Bits, 1 Start Bit, 1 Stop Bit) plus 7 bit delay. The -8 is for lag compensation.
// since we use a faster processor the lag is negligible
#define EMSUART_TX_WAIT_HT3 (EMSUART_TX_BIT_TIME * 17) // 1768

// EMS+ - Time to send one Byte (8 Bits, 1 Start Bit, 1 Stop Bit) and delay of another Bytetime.
#define EMSUART_TX_WAIT_PLUS (EMSUART_TX_BIT_TIME * 20) // 2080


// customize the GPIO pins for RX and TX here
#ifdef WEMOS_D1_32
#define EMSUART_RXPIN 23 // 17 is UART2 RX. Use 23 for D7 on a Wemos D1-32 mini for backwards compatabilty
#define EMSUART_TXPIN 5  // 16 is UART2 TX. Use 5 for D8 on a Wemos D1-32 mini for backwards compatabilty
#else
#define EMSUART_RXPIN 17 // 17 is UART2 RX. Use 23 for D7 on a Wemos D1-32 mini for backwards compatabilty
#define EMSUART_TXPIN 16 // 16 is UART2 TX. Use 5 for D8 on a Wemos D1-32 mini for backwards compatabilty
#endif

namespace emsesp {

#define EMS_TX_STATUS_OK 1
#define EMS_TX_STATUS_ERR 0

class EMSuart {
  public:
    EMSuart()  = default;
    ~EMSuart() = default;

    static void     start(const uint8_t tx_mode);
    static void     send_poll(const uint8_t data);
    static void     stop();
    static void     restart();
    static uint16_t transmit(const uint8_t * buf, const uint8_t len);

  private:
    static void           emsuart_recvTask(void * para);
    static void IRAM_ATTR emsuart_rx_intr_handler(void * para);
    static void IRAM_ATTR emsuart_tx_timer_intr_handler();
};

} // namespace emsesp

#endif
#endif

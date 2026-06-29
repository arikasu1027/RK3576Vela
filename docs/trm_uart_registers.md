## Chapter 33 UART

## 33.1 Overview

The Universal Asynchronous Receiver/Transmitter (UART) is used for serial communication with a peripheral, modem (data carrier equipment, DCE) or data set. Data is written from a master (CPU) over the APB bus to the UART and it is converted to serial form and transmitted to the destination device. Serial data is also received by the UART and stored for the master (CPU) to read back.

UART Controller supports the following features:

 Support 12 independent UART controller: UART0-UART11

 All contain two 64 Bytes FIFOs for data receive and transmit

 UART1-UART11 support auto flow-control

 UART1-UART11 support RS485 function

 Support bit rates 115.2Kbps, 460.8Kbps, 921.6Kbps, 1.5Mbps, 3Mbps, 4Mbps, 8Mbps

● Support programmable baud rates, even with non-integer clock divider

 Standard asynchronous communication bits (start, stop and parity)

 Support interrupt-based or DMA-based mode

 Support 5-8 bits width transfer

## 33.2 Block Diagram

This section provides a description about the functions and behavior under various conditions. The UART Controller comprises with:

 AMBA APB interface

 FIFO controllers

 Register block

 Modem synchronization block and baud clock generation block

 Serial receiver and serial transmitter

![](images/0228c4459f46aab6dba9405ae28644a93fa64afcdee41c7f566cf9fd41c6b424.jpg)  
Fig. 33-1 UART Architecture

## APB INTERFACE

The host processor accesses data, control, and status information on the UART through the APB interface. The UART supports APB data bus widths of 8, 16, and 32 bits.

## Register block

Be responsible for the main UART functionality including control, status and interrupt generation.

## Modem Synchronization block

Synchronizes the modem input signal.

## FIFO block

Be responsible for FIFO control and storage (when using internal RAM) or signaling to control external RAM (when used).

## Baud Clock Generator

Generates the transmitter and receiver baud clock along with the output reference clock signal (baudout\_n).

## Serial Transmitter

Converts the parallel data, written to the UART, into serial form and adds all additional bits, as specified by the control register, for transmission.

## Serial Receiver

Converts the serial data character (as specified by the control register) received to parallel form. Parity error detection, framing error detection and line break detection is carried out in this block.

## 33.3 Function Description

## UART Serial Protocol

Because the serial communication is asynchronous, additional bits (start and stop) are added to the serial data to indicate the beginning and end. An additional parity bit may be added to the serial character. This bit appears after the last data bit and before the stop bit(s) in the character structure to perform simple error checking on the received data, as shown in Figure.

![](images/916d77b09ec75240cf9b2945c5a36ae8458af9e2bfce4d1dcf10ded36939c7a7.jpg)  
Fig. 33-2 UART Serial protocol

## Baud Clock

The baud rate is controlled by the serial clock (sclk or pclk in a single clock implementation) and the Divisor Latch Register (DLH and DLL). As the exact number of baud clocks that each bit was transmitted for is known, calculating the mid-point for sampling is not difficult, that is every 16 baud clocks after the mid-point sample of the start bit.

![](images/b29a76d7449e82d4c57d7e22f77afc1079173ab1bb3bcfa0390c95f5d4df1f97.jpg)

## FIFO Support

## 1. NONE FIFO MODE

If FIFO support is not selected, then no FIFOs are implemented and only a single receive data byte and transmit data byte can be stored at a time in the RBR and THR.

## 2. FIFO MODE

The FIFO depth of is 64bytes. The FIFO mode of all the UART is enabled by register FCR[0].

## Interrupts

The following interrupt types can be enabled with the IER register.

 Receiver Error

 Receiver Data Available

 Character Timeout (in FIFO mode only)

Transmitter Holding Register Empty at/below threshold (in Programmable THRE Interrupt mode)

##  Modem Status

## DMA Support

The UART supports DMA signaling with the use of two output signals (dma\_tx\_req\_n and dma\_rx\_req\_n) to indicate when data is ready to be read or when the transmit FIFO is empty.

The dma\_tx\_req\_n signal is asserted under the following conditions:

 When the Transmitter Holding Register is empty in non-FIFO mode.

 When the transmitter FIFO is empty in FIFO mode with Programmable THRE interrupt mode disabled.

When the transmitter FIFO is at, or below the programmed threshold with Programmable THRE interrupt mode enabled.

The dma\_rx\_req\_n signal is asserted under the following conditions:

When there is a single character available in the Receive Buffer Register in non-FIFO mode.

 When the Receiver FIFO is at or above the programmed trigger level in FIFO mode.

## Auto Flow Control

The UART can be configured to have a 16750-compatible Auto RTS and Auto CTS serial data flow control mode available. If FIFOs are not implemented, then this mode cannot be selected. When Auto Flow Control mode has been selected, it can be enabled with the Modem Control Register (MCR[5]). Following figure shows a block diagram of the Auto Flow Control functionality.

![](images/b76c92567399e3d8918cd33098e06598a2af48a13de208dd1227d1aaae5d18fe.jpg)  
Fig. 33-4 UART Auto flow control block diagram

Auto RTS – Becomes active when the following occurs:

 Auto Flow Control is selected during configuration

 FIFOs are implemented

 RTS (MCR[1] bit and MCR[5]bit are both set)

 FIFOs are enabled (FCR[0]) bit is set)

 SIR mode is disabled (MCR[6] bit is not set)

T = Receiver FIFO Threshold Value

![](images/f22241c4db7dbabd1265b6d9f96d53fc39a8437304bb63c4772fe1a05da9f6fd.jpg)  
Fig. 33-5 UART AUTO RTS TIMING

Auto CTS – becomes active when the following occurs:

 Auto Flow Control is selected during configuration

 FIFOs are implemented

 AFCE (MCR[5] bit is set)

 FIFOs are enabled through FIFO Control Register FCR[0] bit

 SIR mode is disabled (MCR[6] bit is not set)

![](images/f613ccf9385b380d7186a8429a21bddb9a71a43e1fa269800eae3e31bfa3218a.jpg)  
Fig. 33-6 UART AUTO CTS TIMING

## RS485 Mode

The RS485 standard supports serial communication over a twisted pair configuration. The difference between the RS232 and RS485 standards is its use of a balanced line for transmission. This usage is also known as the differential format that sends the same signal on two separate lines with phase delay and then compares the signals at the end, subtracts any noise, and adds them to regain signal strength. This process allows the RS485 standard to be viable over significantly longer distances than its short range RS232 counterpart.

UART supports hardware mode and software mode.

Hardware mode, DE and RE are mutually exclusive. By default 'de' and 're' will be disabled. When at user mode, user should programs the DE to active the 'de', while at auto mode, DE will assert when transfer starts. Once the TX FIFO becomes empty and the stop bit be sampled, 're' signal gets enabled and 'de' signal will be disabled, automatically. In this mode, hardware will consider the de-inactive timing and turn around timing, which are programmed in the DET and TAT register, respectively. When 'de' is valid, it is useless to configure RE, and vice versa.

Software mode, transmit and receive can happen simultaneously. The user can enable DE, RE at any point of time. Turn around timing as programmed in the TAT register and de-inactive timing as programmed in the DET are not applicable in this mode.

## 33.4 Register Description

## 33.4.1 Internal Address Mapping

Slave address can be divided into different length for different usage, which is shown as follows.

Operational Base

<table><tr><td>Name</td><td>Base Address</td></tr><tr><td>UART0</td><td>0x2AD40000</td></tr><tr><td>UART1</td><td>0x27310000</td></tr><tr><td>UART2</td><td>0x2AD50000</td></tr><tr><td>UART3</td><td>0x2AD60000</td></tr><tr><td>UART4</td><td>0x2AD70000</td></tr><tr><td>UART5</td><td>0x2AD80000</td></tr><tr><td>UART6</td><td>0x2AD90000</td></tr><tr><td>UART7</td><td>0x2ADA0000</td></tr><tr><td>UART8</td><td>0x2ADB0000</td></tr><tr><td>UART9</td><td>0x2ADC0000</td></tr><tr><td>UART10</td><td>0x2AFC0000</td></tr><tr><td>UART11</td><td>0x2AFD0000</td></tr></table>

33.4.2 Registers Summary

<table><tr><td>Name</td><td>Offset</td><td>Size</td><td>Reset Value</td><td>Description</td></tr><tr><td>UART_RBR</td><td>0x0000</td><td>W</td><td>0x00000000</td><td>Receive Buffer Register</td></tr><tr><td>UART_DLL</td><td>0x0000</td><td>W</td><td>0x00000000</td><td>Divisor Latch Low</td></tr><tr><td>UART_THR</td><td>0x0000</td><td>W</td><td>0x00000000</td><td>Transmit Buffer Register</td></tr><tr><td>UART_DLH</td><td>0x0004</td><td>W</td><td>0x00000000</td><td>Divisor Latch High</td></tr><tr><td>UART_IER</td><td>0x0004</td><td>W</td><td>0x00000000</td><td>Interrupt Enable Register</td></tr><tr><td>UART_FCR</td><td>0x0008</td><td>W</td><td>0x00000000</td><td>FIFO Enable</td></tr><tr><td>UART_IIR</td><td>0x0008</td><td>W</td><td>0x00000001</td><td>Interrupt Identity Register</td></tr><tr><td>UART_LCR</td><td>0x000C</td><td>W</td><td>0x00000000</td><td>Line Control Register</td></tr><tr><td>UART_MCR</td><td>0x0010</td><td>W</td><td>0x00000000</td><td>Modem Control Register</td></tr><tr><td>UART_LSR</td><td>0x0014</td><td>W</td><td>0x00000060</td><td>Line Status Register</td></tr><tr><td>UART_MSR</td><td>0x0018</td><td>W</td><td>0x00000000</td><td>Modem Status Register</td></tr><tr><td>UART_SCR</td><td>0x001C</td><td>W</td><td>0x00000000</td><td>Scratchpad Register</td></tr><tr><td>UART_SRBR</td><td>0x0030</td><td>W</td><td>0x00000000</td><td>Shadow Receive Buffer Register</td></tr><tr><td>UART_STHR</td><td>0x0030</td><td>W</td><td>0x00000000</td><td>Shadow Transmit Holding Register</td></tr><tr><td>UART_FAR</td><td>0x0070</td><td>W</td><td>0x00000000</td><td>FIFO Access Register</td></tr><tr><td>UART_TFR</td><td>0x0074</td><td>W</td><td>0x00000000</td><td>Transmit FIFO Read</td></tr><tr><td>UART_RFW</td><td>0x0078</td><td>W</td><td>0x00000000</td><td>Receive FIFO write</td></tr><tr><td>UART_USR</td><td>0x007C</td><td>W</td><td>0x00000006</td><td>UART Status Register</td></tr><tr><td>UART_TFL</td><td>0x0080</td><td>W</td><td>0x00000000</td><td>Transmit FIFO level</td></tr><tr><td>UART_RFL</td><td>0x0084</td><td>W</td><td>0x00000000</td><td>Receive FIFO level</td></tr><tr><td>UART_SRR</td><td>0x0088</td><td>W</td><td>0x00000000</td><td>Software Reset Register</td></tr><tr><td>UART_SRTS</td><td>0x008C</td><td>W</td><td>0x00000000</td><td>Shadow Request to Send</td></tr><tr><td>UART_SBCR</td><td>0x0090</td><td>W</td><td>0x00000000</td><td>Shadow Break Control Register</td></tr><tr><td>UART_SDMAM</td><td>0x0094</td><td>W</td><td>0x00000000</td><td>Shadow DMA Mode</td></tr><tr><td>UART_SFE</td><td>0x0098</td><td>W</td><td>0x00000000</td><td>Shadow FIFO enable</td></tr><tr><td>UART_SRT</td><td>0x009C</td><td>W</td><td>0x00000000</td><td>Shadow RCVR Trigger</td></tr><tr><td>UART_STET</td><td>0x00A0</td><td>W</td><td>0x00000000</td><td>Shadow TX Empty Trigger</td></tr><tr><td>UART_HTX</td><td>0x00A4</td><td>W</td><td>0x00000000</td><td>Halt TX</td></tr><tr><td>UART_DMASA</td><td>0x00A8</td><td>W</td><td>0x00000000</td><td>DMA Software Acknowledge</td></tr><tr><td>UART_TCR</td><td>0x00AC</td><td>W</td><td>0x00000000</td><td>Transceiver Control Register</td></tr><tr><td>UART_DE</td><td>0x00B0</td><td>W</td><td>0x00000000</td><td>Driver Output Enable Register</td></tr><tr><td>UART_RE</td><td>0x00B4</td><td>W</td><td>0x00000000</td><td>Receiver Output Enable Register</td></tr><tr><td>UART_DET</td><td>0x00B8</td><td>W</td><td>0x00000000</td><td>Driver Output Enable Timing Register</td></tr><tr><td>UART_TAT</td><td>0x00BC</td><td>W</td><td>0x00000000</td><td>Turn Around Timing Register</td></tr><tr><td>UART_CPR</td><td>0x00F4</td><td>W</td><td>0x00043FF2</td><td>Component Parameter Register</td></tr><tr><td>UART_UCV</td><td>0x00F8</td><td>W</td><td>0x3430322A</td><td>UART Component Version</td></tr><tr><td>UART_CTR</td><td>0x00FC</td><td>W</td><td>0x44570110</td><td>Component Type Register</td></tr></table>

Notes:Size:B- Byte (8 bits) access, HW- Half WORD (16 bits) access, W-WORD (32 bits) access, DW-Double WORD (64 bits) access

33.4.3 Detail Registers Description UART\_RBR

Address: Operational Base + offset (0x0000)

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>RO</td><td>0x00</td><td>data_inputData byte received on the serial input port (sin) in UART mode, or the serial infrared input (sir_in) in infrared mode. The data in this register is valid only if the Data Ready (DR) bit in the Line Status Register (LCR) is set. If in non-FIFO mode (FIFO_MODE == NONE) or FIFOs are disabled (FCR[0] set to zero), the data in the RBR must be read before the next data arrives, otherwise it is overwritten, resulting in an over-run error.If in FIFO mode (FIFO_MODE != NONE) and FIFOs are enabled (FCR[0] set to one), this register accesses the head of the receive FIFO. If the receive FIFO is full and this register is not read before the next data character arrives, then the data already in the FIFO is preserved, but any incoming data are lost and an over-run error occurs.</td></tr></table>

## UART\_DLL

Address: Operational Base + offset (0x0000)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>RW</td><td>0x00</td><td>baud_rate_divisor_ILower 8 bits of a 16-bit, read/write, Divisor Latch register that contains the baud rate divisor for the UART. This register may only be accessed when the DLAB bit (LCR[7]) is set and the UART is not busy (USR[0] is zero). The output baud rate is equal to the serial clock (pclk if one clock design, sclk if two clock design (CLOCK_MODE == Enabled)) frequency divided by sixteen times the value of the baud rate divisor, as follows: baud rate = (serial clock freq) / (16 * divisor).Note that with the Divisor Latch Registers (DLL and DLH) set to zero, the baud clock is disabled and no serial communications occur. Also, once the DLL is set, at least 8 clock cycles of the slowest UART clock should be allowed to pass before transmitting or receiving data.</td></tr></table>

## UART\_THR

Address: Operational Base + offset (0x0000)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>WO</td><td>0x00</td><td>data_outputData to be transmitted on the serial output port (sout) in UART mode or the serial infrared output (sir_out_n) in infrared mode.Data should only be written to the THR when the THR Empty (THRE) bit (LSR[5]) is set. If in non-FIFO mode or FIFOs are disabled (FCR[0] = 0) and THRE is set, writing a single character to the THR clears the THRE. Any additional writes to the THR before the THRE is set again causes the THR data to be overwritten.If in FIFO mode and FIFOs are enabled (FCR[0] = 1) and THRE is set, 64 characters of data may be written to the THR before the FIFO is full.. Any attempt to write data when the FIFO is full results in the write data being lost.</td></tr></table>

## UART\_DLH

Address: Operational Base + offset (0x0004)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>RW</td><td>0x00</td><td>baud_rate_divisor_hUpper 8-bits of a 16-bit, read/write, Divisor Latch register that contains the baud rate divisor for the UART. This register may only be accessed when the DLAB bit (LCR[7]) is set and the UART is not busy (USR[0] is zero). The output baud rate is equal to the serial clock (pclk if one clock design, sclk if two clock design CLOCK_MODE == Enabled)) frequency divided by sixteen times the value of the baud rate divisor, as follows: baud rate = (serial clock freq) / (16 * divisor).Note that with the Divisor Latch Registers (DLL and DLH) set to zero, the baud clock is disabled and no serial communications occur. Also, once the DLH is set, at least 8 clock cycles of the slowest UART clock should be allowed to pass before transmitting or receiving data.</td></tr></table>

## UART\_IER

Address: Operational Base + offset (0x0004)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7</td><td>RW</td><td>0x0</td><td>prog_thre_int_enProgrammable THRE Interrupt Mode Enable that can be written to only when THRE_MODE_USER == Enabled, always readable. This is used to enable/disable the generation of THRE Interrupt.1&#x27;b0: Disabled1&#x27;b1: Enabled</td></tr><tr><td>6:4</td><td>RO</td><td>0x0</td><td>reserved</td></tr><tr><td>3</td><td>RW</td><td>0x0</td><td>modem_status_int_enEnable Modem Status Interrupt. This is used to enable/disable the generation of Modem Status Interrupt. This is the fourth highest priority interrupt.1&#x27;b0: Disabled1&#x27;b1: Enabled</td></tr><tr><td>2</td><td>RW</td><td>0x0</td><td>receive_line_status_int_enEnable Receiver Line Status Interrupt. This is used to enable/disable the generation of Receiver Line Status Interrupt. This is the highest priority interrupt.1&#x27;b0: Disabled1&#x27;b1: Enabled</td></tr><tr><td>1</td><td>RW</td><td>0x0</td><td>trans_hold_empty_int_enEnable Transmit Holding Register Empty Interrupt. This is used to enable/disable the generation of Transmitter Holding Register Empty Interrupt. This is the third highest priority interrupt.1&#x27;b0: Disabled1&#x27;b1: Enabled</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>receive_data_available_int_enEnable Received Data Available Interrupt. This is used to enable/disable the generation of Received Data Available Interrupt and the Character Timeout Interrupt (if in FIFO mode and FIFOs enabled). These are the second highest priority interrupts.1&#x27;b0: Disabled1&#x27;b1: Enabled</td></tr></table>

## UART\_FCR

Address: Operational Base + offset (0x0008)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:6</td><td>WO</td><td>0x0</td><td>rcvr_triggerat which the Received Data Available Interrupt is generated. In auto flow control mode it is used to determine when the rts_n signal is de-asserted. It also determines when the dma_rx_req_n signal is asserted in certain modes of operation. For details on DMA support, refer to &quot;DMA Support&quot;. The following trigger levels are supported:2&#x27;b00: 1 character in the FIFO2&#x27;b01: FIFO 1/4 full2&#x27;b10: FIFO 1/2 full2&#x27;b11: FIFO 2 less than full</td></tr><tr><td>5:4</td><td>WO</td><td>0x0</td><td>tx_empty_triggerTX Empty Trigger. Writes have no effect when THRE_MODE_USER == Disabled. This is used to select the empty threshold level at which the THRE Interrupts are generated when the mode is active. It also determines when the dma_tx_req_n signal is asserted when in certain modes of operation. For details on DMA support, refer to &quot;DMA Support&quot;. The following trigger levels are supported:2&#x27;b00: FIFO empty2&#x27;b01: 2 characters in the FIFO2&#x27;b10: FIFO 1/4 full2&#x27;b11: FIFO 1/2 full</td></tr><tr><td>3</td><td>WO</td><td>0x0</td><td>dma_modeDMA Mode. This determines the DMA signalling mode used for the dma_tx_req_n and dma_rx_req_n output signals when additional DMA handshaking signals are not selected (DMA_EXTRA == No). For details on DMA support, refer to DMA Support.1&#x27;b0: Mode 01&#x27;b1: Mode 1</td></tr><tr><td>2</td><td>WO</td><td>0x0</td><td>xmit_fifo_resetXMIT FIFO Reset. This resets the control portion of the transmit FIFO and treats the FIFO as empty. This also de-asserts the DMA TX request and single signals when additional DMA handshaking signals are selected (DMA_EXTRA == YES). Note that this bit is &#x27;self-clearing&#x27;. It is not necessary to clear this bit.</td></tr><tr><td>1</td><td>WO</td><td>0x0</td><td>rcvr_fifo_resetRCVR FIFO Reset. This resets the control portion of the receive FIFO and treats the FIFO as empty. This also de-asserts the DMA RX request and single signals when additional DMA handshaking signals are selected (DMA_EXTRA == YES). Note that this bit is &#x27;self-clearing&#x27;. It is not necessary to clear this bit.</td></tr><tr><td>0</td><td>WO</td><td>0x0</td><td>fifo_enFIFO Enable. This enables/disables the transmit (XMIT) and receive (RCVR) FIFOs. Whenever the value of this bit is changed both the XMIT and RCVR controller portion of FIFOs is reset.</td></tr></table>

## UART\_IIR

Address: Operational Base + offset (0x0008)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>7:6</td><td>RO</td><td>0x0</td><td>fifos_enFIFOs Enabled. This is used to indicate whether the FIFOs are enabled or disabled.2&#x27;b00: Disabled2&#x27;b11: Enabled</td></tr><tr><td>5:4</td><td>RO</td><td>0x0</td><td>reserved</td></tr><tr><td>3:0</td><td>RO</td><td>0x1</td><td>int_idInterrupt ID. This indicates the highest priority pending interrupt which can be one of the following types:4&#x27;b0000: Modem status4&#x27;b0001: No interrupt pending4&#x27;b0010: THR empty4&#x27;b0100: Received data available4&#x27;b0110: Receiver line status4&#x27;b0111: Busy detect4&#x27;b1100: Character timeoutThe interrupt priorities are split into four levels that are detailed in Table X.Bit 3 indicates an interrupt can only occur when the FIFOs are enabled and used to distinguish a Character Timeout condition interrupt.</td></tr></table>

UART\_LCR  
Address: Operational Base + offset (0x000C)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7</td><td>RW</td><td>0x0</td><td>div_lat_accessDivisor Latch Access Bit. Writeable only when UART is not busy (USR[0] is zero), always readable. This bit is used to enable reading and writing of the Divisor Latch register (DLL and DLH) to set the baud rate of the UART. This bit must be cleared after initial baud rate setup in order to access other registers.</td></tr><tr><td>6</td><td>RW</td><td>0x0</td><td>break_ctrlBreak Control Bit.This is used to cause a break condition to be transmitted to the receiving device. If set to one the serial output is forced to the spacing (logic 0) state. When not in Loopback Mode, as determined by MCR[4], the sout line is forced low until the Break bit is cleared. If SIR_MODE == Enabled and active (MCR[6] set to one) the sir_out_n line is continuously pulsed. When in Loopback Mode, the break condition is internally looped back to the receiver and the sir_out_n line is forced low.</td></tr><tr><td>5</td><td>RW</td><td>0x0</td><td>stick_parityIf UART_16550_COMPATIBLE = NO, then writeable only when UART is not busy (USR[0] is 0); otherwise always writable and always readable. This bit is used to force parity value. When PEN, EPS and Stick Parity are set to 1, the parity bit is transmitted and checked as logic 0. If PEN and Stick Parity are set to 1 and EPS is a logic 0, then parity bit is transmitted and checked as a logic 1. If this bit is set to 0, Stick Parity is disabled.</td></tr><tr><td>4</td><td>RW</td><td>0x0</td><td>even_parity_selEven Parity Select. Writeable only when UART is not busy (USR[0] is zero), always readable. This is used to select between even and odd parity, when parity is enabled (PEN set to one). If set to one, an even number of logic 1s is transmitted or checked. If set to zero, an odd number of logic 1s is transmitted or checked.</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>3</td><td>RW</td><td>0x0</td><td>parity_enParity Enable. Writeable only when UART is not busy (USR[0] is zero), always readable. This bit is used to enable and disable parity generation and detection in transmitted and received serial character respectively.1&#x27;b0: Parity disabled1&#x27;b1: Parity enabled</td></tr><tr><td>2</td><td>RW</td><td>0x0</td><td>stop_bits_numNumber of stop bits. Writeable only when UART is not busy (USR[0] is zero), always readable. This is used to select the number of stop bits per character that the peripheral transmits and receives. If set to zero, one stop bit is transmitted in the serial data. If set to one and the data bits are set to 5 (LCR[1:0] set to zero) one and a half stop bits is transmitted. Otherwise, two stop bits are transmitted. Note that regardless of the number of stop bits selected, the receiver checks only the first stop bit.1&#x27;b0: 1 stop bit.1&#x27;b1: 1.5 stop bits when DLS (LCR[1:0]) is zero, else 2 stop bit.</td></tr><tr><td>1:0</td><td>RW</td><td>0x0</td><td>data_length_selData Length Select. Writeable only when UART is not busy (USR[0] is zero), always readable. This is used to select the number of data bits per character that the peripheral transmits and receives. The number of bit that may be selected areas follows:2&#x27;b00: 5 bits2&#x27;b01: 6 bits2&#x27;b10: 7 bits2&#x27;b11: 8 bits</td></tr></table>

## UART\_MCR

Address: Operational Base + offset (0x0010)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:7</td><td>RO</td><td>0x0000000</td><td>reserved</td></tr><tr><td>6</td><td>RW</td><td>0x0</td><td>sir_mode_enSIR Mode Enable. Writeable only when SIR_MODE == Enabled, always readable. This is used to enable/disable the IrDA SIR Mode features as described in "IrDA 1.0 SIR Protocol".1'b0: IrDA SIR Mode disabled1'b1: IrDA SIR Mode enabled</td></tr><tr><td>5</td><td>RW</td><td>0x0</td><td>auto_flow_ctrl_enAuto Flow Control Enable. Writeable only when AFCE_MODE == Enabled, always readable. When FIFOs are enabled and the Auto Flow Control Enable (AFCE) bit is set, Auto Flow Control features are enabled as described in "Auto Flow Control".1'b0: Auto Flow Control Mode disabled1'b1: Auto Flow Control Mode enabled</td></tr><tr><td>4</td><td>RW</td><td>0x0</td><td>loopbackLoopBack Bit. This is used to put the UART into a diagnostic mode for test purposes. If operating in UART mode (SIR_MODE != Enabled or not active, MCR[6] set to zero), data on the sout line is held high, while serial data output is looped back to the sin line, internally. In this mode all the interrupts are fully functional. Also, in loopback mode, the modem control inputs (dsr_n, cts_n, ri_n, dcd_n) are disconnected and the modem control outputs (dtr_n, rts_n, out1_n, out2_n) are looped back to the inputs, internally. If operating in infrared mode (SIR_MODE == Enabled AND active, MCR[6] set to one), data on the sir_out_n line is held low, while serial data output is inverted and looped back to the sir_in line.</td></tr><tr><td>3</td><td>RW</td><td>0x0</td><td>out2OUT2. This is used to directly control the user-designated Output2 (out2_n) output. The value written to this location is inverted and driven out on out2_n, that is:1'b0: Out2_n de-asserted (logic 1)1'b1: Out2_n asserted (logic 0)Note that in Loopback mode (MCR[4] set to one), the out2_n output is held inactive high while the value of this location is internally looped back to an input.</td></tr><tr><td>2</td><td>RW</td><td>0x0</td><td>out1OUT1. This is used to directly control the user-designated Output1 (out1_n) output. The value written to this location is inverted and driven out on out1_n, that is:1'b0: Out1_n de-asserted (logic 1)1'b1: Out1_n asserted (logic 0)Note that in Loopback mode (MCR[4] set to one), the out1_n output is held inactive high while the value of this location is internally looped back to an input.</td></tr><tr><td>1</td><td>RW</td><td>0x0</td><td>req_to_sendRequest to Send. This is used to directly control the Request to Send (rts_n) output. The Request To Send (rts_n) output is used to inform the modem or data set that the UART is ready to exchange data. When Auto RTS Flow Control is not enabled (MCR[5] set to zero), the rts_n signal is set low by programming MCR[1] (RTS) to a high.In Auto Flow Control, AFCE_MODE == Enabled and active (MCR[5] set to one) and FIFOs enable (FCR[0] set to one), the rts_n output is controlled in the same way, but is also gated with the receiver FIFO threshold trigger (rts_n is inactive high when above the threshold). The rts_n signal is de-asserted when MCR[1] is set low. Note that in Loopback mode (MCR[4] set to one), the rts_n output is held inactive high while the value of this location is internally looped back to an input.</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>data_terminal_readyData Terminal Ready. This is used to directly control the Data Terminal Ready (dtr_n) output. The value written to this location is inverted and driven out on dtr_n, that is:1'b0: dtr_n de-asserted (logic 1)1'b1: dtr_n asserted (logic 0)The Data Terminal Ready output is used to inform the modem or data set that the UART is ready to establish communications. Note that in Loopback mode (MCR[4] set to one), the dtr_n output is held inactive</td></tr></table>

UART\_LSR  
Address: Operational Base + offset (0x0014)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7</td><td>RO</td><td>0x0</td><td>receiver_fifo_errorReceiver FIFO Error bit. This bit is only relevant whenFIFO_MODE != NONE AND FIFOs are enabled (FCR[0] set to one). This is used to indicate if there is at least one parity error, framing error, or break indication in the FIFO.1&#x27;b0: No error in RX FIFO1&#x27;b1: Error in RX FIFOThis bit is cleared when the LSR is read and the character with the error is at the top of the receiver FIFO and there are no subsequent errors in the FIFO.</td></tr><tr><td>6</td><td>RO</td><td>0x1</td><td>trans_emptyTransmitter Empty bit. If in FIFO mode (FIFO_MODE != NONE) and FIFOs enabled (FCR[0] set to one), this bit is set whenever the Transmitter Shift Register and the FIFO are both empty. If in non-FIFO mode or FIFOs are disabled, this bit is set whenever the Transmitter Holding Register and the Transmitter Shift Register are both empty.</td></tr><tr><td>5</td><td>RO</td><td>0x1</td><td>trans_hold_reg_emptyTransmit Holding Register Empty bit. If THRE_MODE_USER == Disabled or THRE mode is disabled (IER[7] set to zero) and regardless of FIFO&#x27;s being implemented/enabled or not, this bit indicates that the THR or TX FIFO is empty.This bit is set whenever data is transferred from the THR or TX FIFO to the transmitter shift register and no new data has been written to the THR or TX FIFO. This also causes a THRE Interrupt to occur, if the THRE Interrupt is enabled. If THRE_MODE_USER == Enabled AND FIFO_MODE != NONE and both modes are active (IER[7] set to one and FCR[0] set to one respectively), the functionality is switched to indicate the transmitter FIFO is full, and no longer controls THRE interrupts, which are then controlled by the FCR[5:4] threshold setting.</td></tr><tr><td>4</td><td>RO</td><td>0x0</td><td>break_intBreak Interrupt bit. This is used to indicate the detection of a break sequence on the serial input data. If in UART mode (SIR_MODE == Disabled), it is set whenever the serial input, sin, is held in a logic &#x27;0&#x27; state for longer than the sum of start time + data bits + parity + stop bits. If in infrared mode (SIR_MODE == Enabled), it is set whenever the serial input, sir_in, is continuously pulsed to logic &#x27;0&#x27; for longer than the sum of start time + data bits + parity + stop bits. A break condition on serial input causes one and only one character, consisting of all zeros, to be received by the UART. In the FIFO mode, the character associated with the break condition is carried through the FIFO and is revealed when the character is at the top of the FIFO.Reading the LSR clears the BI bit. In the non-FIFO mode, the BI indication occurs immediately and persists until the LSR is read.</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>3</td><td>RO</td><td>0x0</td><td>framing_errorFraming Error bit. This is used to indicate the occurrence of a framing error in the receiver. A framing error occurs when the receiver does not detect a valid STOP bit in the received data. In the FIFO mode, since the framing error is associated with a character received, it is revealed when the character with the framing error is at the top of the FIFO. When a framing error occurs, the UART tries to resynchronize. It does this by assuming that the error was due to the start bit of the next character and then continues receiving the other bit i.e. data, and/or parity and stop. It should be noted that the Framing Error (FE) bit (LSR[3]) is set if a break interrupt has occurred, as indicated by Break Interrupt (BI) bit (LSR[4]).1&#x27;b0: No framing error1&#x27;b1: Framing errorReading the LSR clears the FE bit.</td></tr><tr><td>2</td><td>RO</td><td>0x0</td><td>parity_errorParity Error bit. This is used to indicate the occurrence of a parity error in the receiver if the Parity Enable (PEN) bit (LCR[3]) is set. In the FIFO mode, since the parity error is associated with a character received, it is revealed when the character with the parity error arrives at the top of the FIFO. It should be noted that the Parity Error (PE) bit (LSR[2]) is set if a break interrupt has occurred, as indicated by Break Interrupt (BI) bit (LSR[4]).1&#x27;b0: No parity error1&#x27;b1: Parity errorReading the LSR clears the PE bit.</td></tr><tr><td>1</td><td>RO</td><td>0x0</td><td>overrun_errorOverrun error bit. This is used to indicate the occurrence of an overrun error. This occurs if a new data character was received before the previous data was read. In the non-FIFO mode, the OE bit is set when a new character arrives in the receiver before the previous character was read from the RBR. When this happens, the data in the RBR is overwritten. In the FIFO mode, an overrun error occurs when the FIFO is full and a new character arrives at the receiver. The data in the FIFO is retained and the data in the receive shift register is lost.1&#x27;b0: No overrun error1&#x27;b1: Overrun errorReading the LSR clears the OE bit.</td></tr><tr><td>0</td><td>RO</td><td>0x0</td><td>data_readyData Ready bit. This is used to indicate that the receiver contains at least one character in the RBR or the receiver FIFO.1&#x27;b0: No data ready1&#x27;b1: Data readyThis bit is cleared when the RBR is read in non-FIFO mode, or when the receiver FIFO is empty, in FIFO mode.</td></tr></table>

UART\_MSR  
Address: Operational Base + offset (0x0018)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>7</td><td>RO</td><td>0x0</td><td>data_carrior_detectData Carrier Detect. This is used to indicate the current state of the modem control line dcd_n. This bit is the complement of dcd_n. When the Data Carrier Detect input (dcd_n) is asserted it is an indication that the carrier has been detected by the modem or data set.1&#x27;b0: dcd_n input is de-asserted (logic 1)1&#x27;b1: dcd_n input is asserted (logic 0)In Loopback Mode (MCR[4] set to one), DCD is the same as MCR[3] (Out2).</td></tr><tr><td>6</td><td>RO</td><td>0x0</td><td>ring_indicatorRing Indicator. This is used to indicate the current state of the modem control line ri_n. This bit is the complement of ri_n. When the Ring Indicator input (ri_n) is asserted it is an indication that a telephone ringing signal has been received by the modem or data set.1&#x27;b0: ri_n input is de-asserted (logic 1)1&#x27;b1: ri_n input is asserted (logic 0)In Loopback Mode (MCR[4] set to one), RI is the same as MCR[2] (Out1).</td></tr><tr><td>5</td><td>RO</td><td>0x0</td><td>data_set_readyData Set Ready. This is used to indicate the current state of the modem control line dsr_n. This bit is the complement of dsr_n.When the Data Set Ready input (dsr_n) is asserted it is an indication that the modem or data set is ready to establish communications with the UART.1&#x27;b0: dsr_n input is de-asserted (logic 1)1&#x27;b1: dsr_n input is asserted (logic 0)In Loopback Mode (MCR[4] set to one), DSR is the same as MCR[0] (DTR).</td></tr><tr><td>4</td><td>RO</td><td>0x0</td><td>clear_to_sendClear to Send. This is used to indicate the current state of the modem control line cts_n. This bit is the complement of cts_n.When the Clear to Send input (cts_n) is asserted it is an indication that the modem or data set is ready to exchange data with the UART.1&#x27;b0: cts_n input is de-asserted (logic 1)1&#x27;b1: cts_n input is asserted (logic 0)In Loopback Mode (MCR[4] = 1), CTS is the same as MCR[1] (RTS).</td></tr><tr><td>3</td><td>RO</td><td>0x0</td><td>delta_data_carrier_detectDelta Data Carrier Detect. This is used to indicate that the modem control line dcd_n has changed since the last time the MSR was read.1&#x27;b0: No change on dcd_n since last read of MSR1&#x27;b1: Change on dcd_n since last read of MSRReading the MSR clears the DDCD bit. In Loopback Mode (MCR[4] = 1), DDCD reflects changes on MCR[3] (Out2).Note, if the DDCD bit is not set and the dcd_n signal is asserted (low) and a reset occurs (software or otherwise), then the DDCD bit is set when the reset is removed if the dcd_n signal remains asserted.</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>2</td><td>RO</td><td>0x0</td><td>trailing_edge_ring_indicatorTrailing Edge of Ring Indicator. This is used to indicate that a change on the input ri_n (from an active-low to an inactive-high state) has occurred since the last time the MSR was read.1&#x27;b0: No change on ri_n since last read of MSR1&#x27;b1: Change on ri_n since last read of MSRReading the MSR clears the TERI bit. In Loopback Mode (MCR[4] = 1), TERIreflects when MCR[2] (Out1) has changed state from a high to a low.</td></tr><tr><td>1</td><td>RO</td><td>0x0</td><td>delta_data_set_readyDelta Data Set Ready. This is used to indicate that the modem control line dsr_n has changed since the last time the MSR was read.1&#x27;b0: No change on dsr_n since last read of MSR1&#x27;b1: Change on dsr_n since last read of MSRReading the MSR clears the DDSR bit. In Loopback Mode (MCR[4] = 1), DDSR reflects changes on MCR[0] (DTR).Note, if the DDSR bit is not set and the dsr_n signal is asserted (low) and a reset occurs (software or otherwise), then the DDSR bit is set when the reset is removed if the dsr_n signal remains asserted.</td></tr><tr><td>0</td><td>RO</td><td>0x0</td><td>delta_clear_to_sendDelta Clear to Send. This is used to indicate that the modem control line cts_n has changed since the last time the MSR was read.1&#x27;b0: No change on ctsdsr_n since last read of MSR1&#x27;b1: Change on ctsdsr_n since last read of MSR Reading the MSR clears the DCTS bit. In Loopback Mode (MCR[4] = 1), DCTS reflects changes on MCR[1] (RTS). Note, if the DCTS bit is not set and the cts_n signal is asserted (low) and a reset occurs (software or otherwise), then the DCTS bit is set when the reset is removed if the cts_n signal remains asserted.</td></tr></table>

## UART\_SCR

Address: Operational Base + offset (0x001C)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>RW</td><td>0x00</td><td>temp_store_spaceScratchpad register.This register is for programmers to use as a temporary storage space. It has no defined purpose in the UART.</td></tr></table>

## UART\_SRBR

Address: Operational Base + offset (0x0030)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>7:0</td><td>RO</td><td>0x00</td><td>shadow_rbrThis is a shadow register for the RBR and has been allocated sixteen 32-bit locations (0x30-0x6c) so as to accommodate burst accesses from the master. This register contains the data byte received on the serial input port (sin) in UART mode or the serial infrared input (sir_in) in infrared mode. The data in this register is valid only if the Data Ready (DR) bit in the Line status Register (LSR) is set. If in non-FIFO mode (FIFO_MODE == NONE) or FIFOs are disabled (FCR[0] set to zero), the data in the RBR must be read before the next data arrives, otherwise it is overwritten, resulting in an overrun error. If in FIFO mode (FIFO_MODE != NONE) and FIFOs are enabled (FCR[0] set to one), this register accesses the head of the receive FIFO. If the receive FIFO is full and this register is not read before the next data character arrives, then the data already in the FIFO are preserved, but any incoming data is lost. An overrun error also occurs.</td></tr></table>

## UART\_STHR

Address: Operational Base + offset (0x0030)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>WO</td><td>0x00</td><td>shadow_thrThis is a shadow register for the THR and has been allocated sixteen 32-bit locations(0x30-0x6c) so as to accommodate burst accesses from the master. This register contains data to be transmitted on the serial output port (sout) in UART mode or the serial infrared output (sir_out_n) in infrared mode. Data should only be written to the THR when the THR Empty (THRE) bit (LSR[5]) is set. If in non-FIFO mode or FIFOs are disabled (FCR[0] set to zero) and THRE is set, writing a single character to the THR clears the THRE. Any additional writes to the THR before the THRE is set again causes the THR data to be overwritten. If in FIFO mode and FIFOs are enabled (FCR[0] set to one) and THRE is set, x number of characters of data may be written to the THR before the FIFO is full. The number x (default=16) is determined by the value of FIFO Depth that you set during configuration. Any attempt to write data when the FIFO is full results in the write data being lost.</td></tr></table>

## UART\_FAR

Address: Operational Base + offset (0x0070)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>fifo_access_test_enWrites have no effect when FIFO_ACCESS == No, always readable. This register is use to enable a FIFO access mode for testing, so that the receive FIFO can be written by the master and the transmit FIFO can be read by the master when FIFOs are implemented and enabled. When FIFOs are not implemented or not enabled it allows the RBR to be written by the master and the THR to be read by the master.1&#x27;b0: FIFO access mode disabled1&#x27;b1: FIFO access mode enabledNote, that when the FIFO access mode is enabled/disabled, the control portion of the receive FIFO and transmit FIFO is reset and the FIFOs are treated as empty.</td></tr></table>

## UART\_TFR

Address: Operational Base + offset (0x0074)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:8</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>7:0</td><td>RO</td><td>0x00</td><td>trans_fifo_readTransmit FIFO Read. These bits are only valid when FIFO access mode is enabled (FAR[0] is set to one). When FIFOs are implemented and enabled, reading this register gives the data at the top of the transmit FIFO. Each consecutive read pops the transmit FIFO and gives the next data value that is currently at the top of the FIFO.When FIFOs are not implemented or not enabled, reading this register gives the data in the THR.</td></tr></table>

## UART\_RFW

Address: Operational Base + offset (0x0078)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:10</td><td>RO</td><td>0x000000</td><td>reserved</td></tr><tr><td>9</td><td>WO</td><td>0x0</td><td>receive_fifo_framing_errorReceive FIFO Framing Error. These bits are only valid when FIFO access mode is enabled (FAR[0] is set to one). When FIFOs are implemented and enabled, this bit is used to write framing error detection information to the receive FIFO. When FIFOs are not implemented or not enabled, this bit is used to write framing error detection information to the RBR.</td></tr><tr><td>8</td><td>WO</td><td>0x0</td><td>receive_fifo_parity_errorReceive FIFO Parity Error. These bits are only valid when FIFO access mode is enabled (FAR[0] is set to one). When FIFOs are implemented and enabled, this bit is used to write parity error detection information to the receive FIFO. When FIFOs are not implemented or not enabled, this bit is used to write parity error detection information to the RBR.</td></tr><tr><td>7:0</td><td>WO</td><td>0x00</td><td>receive_fifo_writeReceive FIFO Write Data. These bits are only valid when FIFO access mode is enabled (FAR[0] is set to one). When FIFOs are implemented and enabled, the data that is written to the RFWD is pushed into the receive FIFO. Each consecutive write pushes the new data to the next write location in the receive FIFO. When FIFOs are not implemented or not enabled, the data that is written to the RFWD is pushed into the RBR.</td></tr></table>

## UART\_USR

Address: Operational Base + offset (0x007C)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:5</td><td>RO</td><td>0x0000000</td><td>reserved</td></tr><tr><td>4</td><td>RO</td><td>0x0</td><td>receive_fifo_fullReceive FIFO Full. This bit is only valid when FIFO_STAT == YES.This is used to indicate that the receive FIFO is completely full.1'b0: Receive FIFO not full1'b1: Receive FIFO FullThis bit is cleared when the RX FIFO is no longer full.</td></tr><tr><td>3</td><td>RO</td><td>0x0</td><td>receive_fifo_not_emptyReceive FIFO Not Empty. This bit is only valid when FIFO_STAT == YES. This is used to indicate that the receive FIFO contains one or more entries.1'b0: Receive FIFO is empty1'b1: Receive FIFO is not emptyThis bit is cleared when the RX FIFO is empty.</td></tr><tr><td>2</td><td>RO</td><td>0x1</td><td>trasn_fifo_emptyTransmit FIFO Empty. This bit is only valid when FIFO_STAT == YES. This is used to indicate that the transmit FIFO is completely empty.1'b0: Transmit FIFO is not empty1'b1: Transmit FIFO is emptyThis bit is cleared when the TX FIFO is no longer empty.</td></tr><tr><td>1</td><td>RO</td><td>0x1</td><td>trans_fifo_not_fullTransmit FIFO Not Full. This bit is only valid when FIFO_STAT == YES. This is used to indicate that the transmit FIFO in not full.1'b0: Transmit FIFO is full1'b1: Transmit FIFO is not fullThis bit is cleared when the TX FIFO is full.</td></tr><tr><td>0</td><td>RO</td><td>0x0</td><td>uart_busyUART Busy. This is indicates that a serial transfer is in progress, when cleared indicates that the UART is idle or inactive.1'b0: UART is idle or inactive1'b1: UART is busy (actively transferring data)Note that it is possible for the UART Busy bit to be cleared even though a new character may have been sent from another device. That is, if the UART has no data in THR and RBR and there is no transmission in progress and a start bit of a new character has just reached the UART. This is due to the fact that a valid start is not seen until the middle of the bit period and this duration is dependent on the baud divisor that has been programmed. If a second system clock has been implemented (CLOCK_MODE == Enabled), the assertion of this bit is also delayed by several cycles of the slower clock.</td></tr></table>

## UART\_TFL

Address: Operational Base + offset (0x0080)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:6</td><td>RO</td><td>0x0000000</td><td>reserved</td></tr><tr><td>5:0</td><td>RO</td><td>0x00</td><td>trans_fifo_levelTransmit FIFO Level. This is indicates the number of data entries in the transmit FIFO.</td></tr></table>

## UART\_RFL

Address: Operational Base + offset (0x0084)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:6</td><td>RO</td><td>0x0000000</td><td>reserved</td></tr><tr><td>5:0</td><td>RO</td><td>0x00</td><td>receive_fifo_levelReceive FIFO Level. This is indicates the number of data entries in the receive FIFO.</td></tr></table>

## UART\_SRR

Address: Operational Base + offset (0x0088)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:3</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>2</td><td>WO</td><td>0x0</td><td>xmit_fifo_resetXMIT FIFO Reset. This is a shadow register for the XMIT FIFO Reset bit (FCR[2]). This can be used to remove the burden on software having to store previously written FCR values (which are pretty static) just to reset the transmit FIFO. This resets the control portion of the transmit FIFO and treats the FIFO as empty. This also de-asserts the DMA TX request and single signals when additional DMA handshaking signals are selected (DMA_EXTRA == YES). Note that this bit is &#x27;self-clearing&#x27;. It is not necessary to clear this bit.</td></tr><tr><td>1</td><td>WO</td><td>0x0</td><td>rcvr_fifo_resetRCVR FIFO Reset. This is a shadow register for the RCVR FIFO Reset bit (FCR[1]). This can be used to remove the burden on software having to store previously written FCR values (which are pretty static) just to reset the receive FIFO This resets the control portion of the receive FIFO and treats the FIFO as empty. This also de-asserts the DMA RX request and single signals when additional DMA handshaking signals are selected (DMA_EXTRA == YES).Note that this bit is &#x27;self-clearing&#x27;. It is not necessary to clear this bit.</td></tr><tr><td>0</td><td>WO</td><td>0x0</td><td>uart_resetUART Reset. This asynchronously resets the UART and synchronously removes the reset assertion. For a two clock implementation both pclk and sclk domains are reset.</td></tr></table>

## UART\_SRTS

Address: Operational Base + offset (0x008C)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>shadow_req_to_sendShadow Request to Send. This is a shadow register for the RTS bit (MCR[1]), this can be used to remove the burden of having to performing a read-modify-write on the MCR. This is used to directly control the Request to Send (rts_n) output. The Request To Send (rts_n) output is used to inform the modem or data set that the UART is ready to exchange data. When Auto RTS Flow Control is not enabled (MCR[5] = 0), the rts_n signal is set low by programming MCR[1] (RTS) to a high. In Auto Flow Control, AFCE_MODE == Enabled and active (MCR[5] = 1) and FIFOs enable (FCR[0] = 1), the rts_n output is controlled in the same way, but is also gated with the receiver FIFO threshold trigger (rts_n is inactive high when above the threshold).Note that in Loopback mode (MCR[4] = 1), the rts_n output is held inactive-high while the value of this location is internally looped back to an input.</td></tr></table>

UART\_SBCR  
Address: Operational Base + offset (0x0090)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>shadow_break_ctrlShadow Break Control Bit. This is a shadow register for the Break bit (LCR[6]), this can be used to remove the burden of having to performing a read modify write on the LCR. This is used to cause a break condition to be transmitted to the receiving device. If set to one the serial output is forced to the spacing (logic 0) state. When not in Loopback Mode, as determined by MCR[4], the sout line is forced low until the Break bit is cleared. If SIR_MODE == Enabled and active (MCR[6] = 1) the sir_out_n line is continuously pulsed. When in Loopback Mode, the break condition is internally looped back to the receiver.</td></tr></table>

## UART\_SDMAM

Address: Operational Base + offset (0x0094)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>shadow_dma_modeShadow DMA Mode. This is a shadow register for the DMA mode bit (FCR[3]). This can be used to remove the burden of having to store the previously written value to the FCR in memory and having to mask this value so that only the DMA Mode bit gets updated. This determines the DMA signalling mode used for the dma_tx_req_n and dma_rx_req_n output signals when additional DMA handshaking signals are not selected (DMA_EXTRA == NO).1&#x27;b0: Mode 01&#x27;b1: Mode 1</td></tr></table>

## UART\_SFE

Address: Operational Base + offset (0x0098)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>shadow_fifo_enShadow FIFO Enable. This is a shadow register for the FIFO enable bit (FCR[0]). This can be used to remove the burden of having to store the previously written value to the FCR in memory and having to mask this value so that only the FIFO enable bit gets updated.This enables/disables the transmit (XMIT) and receive (RCVR) FIFOs. If this bit is set to zero (disabled) after being enabled then both the XMIT and RCVR controller portion of FIFOs are reset.</td></tr></table>

## UART\_SRT

Address: Operational Base + offset (0x009C)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:2</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>1:0</td><td>RW</td><td>0x0</td><td>shadow_rcvr_triggerShadow RCVR Trigger. This is a shadow register for the RCVR trigger bits (FCR[7:6]). This can be used to remove the burden of having to store the previously written value to the FCR in memory and having to mask this value so that only the RCVR trigger bit gets updated.This is used to select the trigger level in the receiver FIFO at which the Received Data Available Interrupt is generated. It also determines when the dma_rx_req_n signal is asserted when DMA Mode (FCR[3]) = 1. The following trigger levels are supported:2'b00: 1 character in the FIFO2'b01: FIFO 1/4 full2'b10: FIFO 1/2 full2'b11: FIFO 2 less than full</td></tr></table>

## UART\_STET

Address: Operational Base + offset (0x00A0)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:2</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>1:0</td><td>RW</td><td>0x0</td><td>shadow_tx_empty_triggerShadow TX Empty Trigger. This is a shadow register for the TX empty trigger bits (FCR[5:4]). This can be used to remove the burden of having to store the previously written value to the FCR in memory and having to mask this value so that only the TX empty trigger bit gets updated.This is used to select the empty threshold level at which the THRE Interrupts are generated when the mode is active. The following trigger levels are supported:2&#x27;b00: FIFO empty2&#x27;b01: 2 characters in the FIFO2&#x27;b10: FIFO 1/4 full2&#x27;b11: FIFO 1/2 full</td></tr></table>

## UART\_HTX

Address: Operational Base + offset (0x00A4)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>halt_tx_enThis register is use to halt transmissions for testing, so that the transmit FIFO can be filled by the master when FIFOs are implemented and enabled.1&#x27;b0: Halt TX disabled1&#x27;b1: Halt TX enabledNote, if FIFOs are implemented and not enabled, the setting of the halt TX register has no effect on operation.</td></tr></table>

## UART\_DMASA

<table><tr><td colspan="4">Address: Operational Base + offset (0x00A8)</td></tr><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>0</td><td>WO</td><td>0x0</td><td>dma_software_ackThis register is use to perform a DMA software acknowledge if a transfer needs to be terminated due to an error condition. For example, if the DMA disables the channel, then the UART should clear its request. This causes the TX request, TX single, RX request and RX single signals to de-assert. Note that this bit is &#x27;self-clearing&#x27;. It is not necessary to clear this bit.</td></tr></table>

## UART\_TCR

Address: Operational Base + offset (0x00AC)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:6</td><td>RO</td><td>0x0000000</td><td>reserved</td></tr><tr><td>5</td><td>RW</td><td>0x0</td><td>auto_deAssert DE Automatically Enable. When data is written to the TXFIFO, the DE will automatically asserted, meanwhile, the RE will automatically de-asserted.1&#x27;b0: Disable.1&#x27;b1: Enable.</td></tr><tr><td>4</td><td>RW</td><td>0x0</td><td>re_polReceiver Enable Polarity.1&#x27;b0: re signal is active low.1&#x27;b1: re signal is active high.</td></tr><tr><td>3</td><td>RW</td><td>0x0</td><td>de_polDriver Enable Polarity.1&#x27;b0: de signal is active high.1&#x27;b1: de signal is active low.</td></tr><tr><td>2</td><td>RW</td><td>0x0</td><td>xfer_modeTransfer Mode.1&#x27;b0: Hardware mode, DE and RE are mutually exclusive. By default &#x27;de&#x27; and &#x27;re&#x27; will be disabled. User should programs the DE to active the &#x27;de&#x27;. Once the TX FIFO becomes empty and the stop bit be sampled, &#x27;re&#x27; signal gets enabled and &#x27;de&#x27; signal will be disabled, automatically. In this mode, hardware will consider the de-inactive timing and turn around timing (applied for 4 wire mode only), which are programmed in the DET and TAT register, respectively. When &#x27;de&#x27; is valid, it is useless to configure RE, and vice versa.1&#x27;b1: Software mode, transmit and receive can happen simultaneously. The user can enable DE, RE at any point of time. Turn around timing as programmed in the TAT register and de-inactive timing as programmed in the DET are not applicable in this mode.</td></tr><tr><td>1</td><td>RW</td><td>0x0</td><td>re_enReceiver Enable.1&#x27;b0: RE signal disable, DE control both transmit and receive process. Correspondingly, assert DE for transmit state, while de-assert DE for receive state. In this scenario, RE-associated logics and TAT register will hold constant for lower power consumption.1&#x27;b1: RE signal enable, DE control the transmit process, and RE control the receive process.</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>rs485_enRS485 Enable.1&#x27;b0: In this mode, the transfers are still in the RS232 mode. The registers of DE, RE, DET and TAT are reserved.1&#x27;b1: In this mode, the transfers will happen in RS485 mode.</td></tr></table>

## UART\_DE

Address: Operational Base + offset (0x00B0)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>deThe &#x27;de&#x27; register bit is used to control assertion and de-assertion of &#x27;de&#x27; signal.1&#x27;b0: De-assert &#x27;de&#x27; signal.1&#x27;b1: Assert &#x27;de&#x27; signal.</td></tr></table>

## UART\_RE

Address: Operational Base + offset (0x00B4)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:1</td><td>RO</td><td>0x00000000</td><td>reserved</td></tr><tr><td>0</td><td>RW</td><td>0x0</td><td>reThe &#x27;re&#x27; register bit is used to control assertion and de-assertion of &#x27;re&#x27; signal.1&#x27;b0: De-assert &#x27;re&#x27; signal.1&#x27;b1: Assert &#x27;re&#x27; signal.</td></tr></table>

## UART\_DET

Address: Operational Base + offset (0x00B8)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:16</td><td>RO</td><td>0x0000</td><td>reserved</td></tr><tr><td>15:0</td><td>RW</td><td>0x0000</td><td>de_inact_timeThis field controls the amount of time (in terms of number of pclk periods) between the finish flag after the sampling point of the stop bit on the sout to the falling edge of Driver output enable signal. Note that, the mentioned finish flag, i.e. the actual starting effective point of the count, is located at the position of (DLL+1) sclk periods before the full stop bit ends, followed by 3~4 synchronization pclk periods. The actual time is the programmed value + 1.</td></tr></table>

## UART\_TAT

Address: Operational Base + offset (0x00BC)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:16</td><td>RO</td><td>0x0000</td><td>reserved</td></tr><tr><td>15:0</td><td>RW</td><td>0x0000</td><td>de2re_timeTurn around time (in terms of pclk) for DE De-assertion to RE assertion. The actual time is the programmed value + 1.</td></tr></table>

## UART\_CPR

Address: Operational Base + offset (0x00F4)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:24</td><td>RO</td><td>0x00</td><td>reserved</td></tr><tr><td>23:16</td><td>RO</td><td>0x04</td><td>fifo_mode8&#x27;h4 means FIFO mode is 64.</td></tr><tr><td>15:14</td><td>RO</td><td>0x0</td><td>reserved</td></tr><tr><td>13</td><td>RO</td><td>0x1</td><td>dma_extra1&#x27;b1 means DMA_EXTRA enabled.</td></tr><tr><td>12</td><td>RO</td><td>0x1</td><td>uart_add_encoded_params1&#x27;b1 means UART_ADD_ENCODED_PARAMS enabled.</td></tr><tr><td>11</td><td>RO</td><td>0x1</td><td>shadow1&#x27;b1 means SHADOW mode enabled.</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>10</td><td>RO</td><td>0x1</td><td>fifo_stat1&#x27;b1 means FIFO_STAT enabled.</td></tr><tr><td>9</td><td>RO</td><td>0x1</td><td>fifo_access1&#x27;b1 means FIFO ACCESS enabled.</td></tr><tr><td>8</td><td>RO</td><td>0x1</td><td>new_feat1&#x27;b1 means Additional features enabled.</td></tr><tr><td>7</td><td>RO</td><td>0x1</td><td>sir_lp_mode1&#x27;b1 means SIR_LP mode enabled.</td></tr><tr><td>6</td><td>RO</td><td>0x1</td><td>sir_mode1&#x27;b1 means SIR mode enabled.</td></tr><tr><td>5</td><td>RO</td><td>0x1</td><td>thre_mode1&#x27;b1 means THRE mode enabled.</td></tr><tr><td>4</td><td>RO</td><td>0x1</td><td>afce_mode1&#x27;b1 means AFCE mode enabled.</td></tr><tr><td>3:2</td><td>RO</td><td>0x0</td><td>reserved</td></tr><tr><td>1:0</td><td>RO</td><td>0x2</td><td>apb_data_width2&#x27;b10 means APB data width is 32bit.</td></tr></table>

## UART\_UCV

Address: Operational Base + offset (0x00F8)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:0</td><td>RO</td><td>0x3430322a</td><td>verASCII value for each number in the version.</td></tr></table>

## UART\_CTR

Address: Operational Base + offset (0x00FC)

<table><tr><td>Bit</td><td>Attr</td><td>Reset Value</td><td>Description</td></tr><tr><td>31:0</td><td>RO</td><td>0x44570110</td><td>peripheral_idThis register contains the peripherals identification code.</td></tr></table>

## 33.5 Interface Description

Table 33-1 UART0 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>UART0_RX_M0/JTAG_TMS_M1/GPIO0_D5_u</td><td>PMU1_IOC_GPIO0D_IOMUX_SEL_H[7:4]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>UART0_TX_M0/JTAG_TCK_M1/GPIO0_D4_u</td><td>PMU1_IOC_GPIO0D_IOMUX_SEL_H[3:0]=4&#x27;h9</td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>SDMMC0_D0/FSPI1_D0_M0/DSM_AUD_LP_M0/UART0_RX_M1/UART7_RX_M2/I2C8_SCL_M0/SPI0_MOSI_M1/CAN0_RX_M0/PWM2_CH2_M0/GPIO2_A0_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_L[3:0]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>SDMMC0_D1/FSPI1_D1_M0/DSM_AUD_LN_M0/SAI3_MCLK_M3/UART0_TX_M1/UART7_TX_M2/I2C8_SDA_M0/SPI0_MISO_M1/CAN0_TX_M0/PWM2_CH3_M0/GPIO2_A1_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_L[7:4]=4&#x27;h9</td></tr></table>

Table 33-2 UART1 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>I2C2_SDA_M0/UART1_RX_M0/CPUL IT_AVS/PWM1_CH3_M0/GPIO0_C0_d</td><td>PMU1_IOC_GPIO0C_IOMUX_SEL_L[3:0]=4&#x27;ha</td></tr><tr><td>uart_tx</td><td></td><td>I2C2_SCL_M0/UART1_TX_M0/NPU_AVS/PWM1_CH4_M0/GPIO0_B7_d</td><td>PMU1_IOC_GPIO0B_IOMUX_SEL_H[15:12]=4&#x27;ha</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">SAI0_SDI2_M1/SAI0_SDO2_M1/PDM0_SDI2_M0/I2C4_SCL_M0/PCIE0_WAKEN_M0/CPUBIG_AVS/PWM1_CH5_M0/UART1_CTSN_M0/GPIO0_D2_d</td><td rowspan="2">PMU1_IOC_GPIO0D_IOMUX_SEL_L[11:8]=4&#x27;hd</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">SAI0_SDI3_M1/SAI0_SDO1_M1/PDM0_SDI3_M0/I2C4_SDA_M0/PCIE1_WAKEN_M0/GPU_AVS/PWM2_CH0_M0/UART1_RTSN_M0/GPIO0_D3_d</td><td rowspan="2">PMU1_IOC_GPIO0D_IOMUX_SEL_L[15:12]=4&#x27;hd</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_D12/SDMMC1_D3_M1/ETH0_TXD0_M1/SAI0_SDI1_M0/PDM0_S DI2_M3/UART1_RX_M1/GPIO2_B1_d</td><td>TOP_IOC_GPIO2B_IOMUX_SEL_L[7:4]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_D13/SDMMC1_D2_M1/ETH0_TXD1_M1/SAI0_SDI0_M0/PDM0_S DI3_M3/UART1_TX_M1/GPIO2_B0_d</td><td>TOP_IOC_GPIO2B_IOMUX_SEL_L[3:0]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VI_CIF_D11/SDMMC1_CMD_M1/ETH0_TXD3_M1/SAI0_SDI2_M0/PDM0_S DI1_M3/UART1_CTSN_M1/SPI4_C SN0_M3/PCIE0_CLKREQN_M0/GPIO 2_B2_d</td><td rowspan="2">TOP_IOC_GPIO2B_IOMUX_SEL_L[11:8]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VI_CIF_D10/SDMMC1_CLK_M1/ETH0_TXCLK_M1/SAI0_SDO2_M0/PDM0_CLK1_M3/UART1_RTSN_M1/SPI4_CLK_M3/PCIE1_CLKREQN_M0/GPIO 2_B3_d</td><td rowspan="2">TOP_IOC_GPIO2B_IOMUX_SEL_L[15:12]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D21/VO_EBC_GDOE/ETH0_MDC_M0/PDM1_SDI2_M2/DSMC_DATA14/FLEXBUS0_D6/UART1_RX_M2/UART10_CTSN_M0/PWM1_CH2_M3/GPIO3_A6_d</td><td>TOP_IOC_GPIO3A_IOMUX_SEL_H[11:8]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D20/VO_EBC_VCOM/ETH0_RXCTL_M0/PDM1_CLK1_M2/DSMC_DATA13/FLEXBUS0_D5/UART1_TX_M2/UART10_RTSN_M0/GPIO3_A7_d</td><td>TOP_IOC_GPIO3A_IOMUX_SEL_H[15:12]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D23/VO_EBC_SDSHR/ETH_CLK0_25M_OUT_M0/SAI4_SDI_M1/DSMC_RDYN/FLEXBUS1_D11/FLEXBUS0_CSN_M0/UART1_CTSN_M2/SPI2_CLK_M2/PWM1_CH0_M3/GPIO</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_H[3:0]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td></td><td></td><td>3_A4_d</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D22/VO_EBC_GDSP/ETH0_MDIO_M0/PDM1_SDI3_M2/DSMC_DATA15/FLEXBUS0_D7/UART1_RTSN_M2/SPI2_CSN1_M2/PWM1_CH1_M3/GPIO3_A5_d</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_H[7:4]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr></table>

Table 33-3 UART2 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_RXD0_M1/FSPI1_D3_M1/PDM0_SDI1_M2/UART2_RX_M0/I2C8_SDA_M1/SATA_CPDET/GPIO1_C7_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_H[15:12]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>ETH1_TXCTL_M1/FSPI1_D2_M1/PDM0_SDI0_M2/UART2_TX_M0/I2C8_SCL_M1/SATA_CPPOD/GPIO1_C6_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_H[11:8]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ETH1_TXD1_M1/FSPI1_D1_M1/UART4_RX_M1/UART2_CTSN_M0/SPI2_MISO_M1/PCIE1_BUTTONRSTN/GPIO1_C5_d</td><td rowspan="2">TOP_IOC_GPIO1C_IOMUX_SEL_H[7:4]=4&#x27;ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">ETH1_TXD0_M1/FSPI1_D0_M1/UART4_TX_M1/UART2_RTSN_M0/SPI2_MOSI_M1/PCIE0_BUTTONRSTN/GPIO1_C4_d</td><td rowspan="2">TOP_IOC_GPIO1C_IOMUX_SEL_H[3:0]=4&#x27;ha</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>SPDIF_RX0_M0/FLEXBUS0_CSN_M4/UART2_RX_M1/I2C3_SDA_M0/CAN1_RX_M2/GPIO4_B4_d</td><td>TOP_IOC_GPIO4B_IOMUX_SEL_H[3:0]=4&#x27;ha</td></tr><tr><td>uart_tx</td><td></td><td>SPDIF_TX0_M0/FLEXBUS0_D15_M1/UART2_TX_M1/I2C3_SCL_M0/PCIE0_CLKREQN_M2/CAN1_TX_M2/GPIO4_B5_d</td><td>TOP_IOC_GPIO4B_IOMUX_SEL_H[7:4]=4&#x27;ha</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">SAI1_SDO2_M0/SAI1_SDI2_M0/PDM1_SDI2_M1/FLEXBUS1_D14_M1/SPI4_MOSI_M2/UART5_RX_M1/UART6_CTSN_M0/UART2_CTSN_M1/GPIO4_B1_d</td><td rowspan="2">TOP_IOC_GPIO4B_IOMUX_SEL_L[7:4]=4&#x27;hc</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">SAI1_SDO1_M0/SAI1_SDI3_M0/PDM1_CLK1_M1/FLEXBUS1_D13_M1/SPI4_CLK_M2/UART5_TX_M1/UART6_RTSN_M0/UART2_RTSN_M1/GPIO4_B0_d</td><td rowspan="2">TOP_IOC_GPIO4B_IOMUX_SEL_L[3:0]=4&#x27;hc</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D12/VO_EBC_SDDO12/ETH0_PPSTRIG_M0/SAI1_SDI0_M1/D SMC_DQS0/FLEXBUS1_D10/FLEXBUS1_CSN_M0/UART2_RX_M2/UART3_CTSN_M1/I2C4_SDA_M3/GPIO3_B7_d</td><td>TOP_IOC_GPIO3B_IOMUX_SEL_H[15:12]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D11/VO_EBC_SDDO11/ETH0_PPSCLK_M0/SAI1_SDO3_M1/D</td><td>TOP_IOC_GPIO3C_IOMUX_SEL_L[3:0]=4&#x27;h9</td></tr></table>

RK3576 TRM (Part 1)

<table><tr><td></td><td></td><td>SMC_DATA7/FLEXBUS1_D9/UART2_TX_M2/UART3_RTSN_M1/I2C4_SCL_M3/GPIO3_C0_d</td><td></td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D0/VO_EBC_SDDO0/ETH0_RXD2_M0/SAI2_SDO_M2/DSMC_CSN0/FLEXBUS1_D2/UART2_CTSN_M2/I3C1_SCL_M2/PWM2_CH5_M3/GPIO3_D3_d</td><td rowspan="2">TOP_IOC_GPIO3D_IOMUX_SEL_L[15:12]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D1/VO_EBC_SDDO1/ETH0_RXD3_M0/SAI2_SDI_M2/DSMC_CSN3/FLEXBUS0_D12/FLEXBUS1_D15_M0/FLEXBUS0_CSN_M3/UART2_RTSN_M2/SPI4_CSN1_M1/I3C1_SDA_M2/PWM2_CH4_M3/GPIO3_D2_d</td><td rowspan="2">TOP_IOC_GPIO3D_IOMUX_SEL_L[11:8]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr></table>

Table 33-4 UART3 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_VSYNC/ETH1_PPSTRIG_M0/ETH0_MDC_M1/SAI3_LRCK_M2/UART3_RX_M0/SPI3_MOSI_M0/I2C7_SDA_M1/GPIO3_A1_d</td><td>TOP_IOC_GPIO3A_IOMUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_HREF/ETH0_MDIO_M1/SAI3_SCLK_M2/UART3_TX_M0/SPI3_CLK_M0/I2C7_SCL_M1/GPIO3_A0_d</td><td>TOP_IOC_GPIO3A_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VI_CIF_CLKO/ETH1_PPSCLK_M0/ETH0_RXCTL_M1/SAI3_SDO_M2/SPDIF_RX1_M1/UART3_CTSN_M0/SPI3_MISO_M0/CAN1_TX_M3/MIPI_TE_M1/GPIO3_A2_d</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_L[11:8]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VI_CIF_CLKI/ETH1_PTP_REFCLK_M0/ETH0_RXD1_M1/SAI3_SDI_M2/SPDIF_TX1_M1/UART3_RTSN_M0/SPI3_CSN0_M0/CAN1_RX_M3/GPIO3_A3_d</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>VO_POST_EMPTY/SPDIF_TX0_M1/CAM_CLK2_OUT_M0/SAI4_SDO_M1/DSMC_INT2/FLEXBUS0_D14_M0/FLEXBUS1_D13_M0/FLEXBUS0_CSN_M1/UART3_RX_M1/I2C7_SDA_M2/GPIO4_A1_d</td><td>TOP_IOC_GPIO4A_IOMUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>SPDIF_RX0_M1/CAM_CLK1_OUT_M0/SAI4_LRCK_M1/DSMC_INT0/FLEXBUS0_D13_M0/FLEXBUS1_D14_M0/FLEXBUS1_CSN_M3/UART3_TX_M1/SPI1_CSN1_M2/I2C7_SCL_M2/MIPI_TE_M2/GPIO4_A0_d</td><td>TOP_IOC_GPIO4A_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D12/VO_EBC_SDDO12/ETH0_PPSTRIG_M0/SAI1_SDI0_M1/DSMC_DQS0/FLEXBUS1_D10/FLEXBUS1_CSN_M0/UART2_RX_M2/UART3_CTSN_M1/I2C4_SDA_M3/GPIO3_B7_d</td><td rowspan="2">TOP_IOC_GPIO3B_IOMUX_SEL_H[15:12]=4'ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D11/VO_EBC_SDDO11/ETH0_PPSCLK_M0/SAI1_SDO3_M1/DSMC_DATA7/FLEXBUS1_D9/UART2_TX_M2/UART3_RTSN_M1/I2C4_SCL_M3/GPIO3_C0_d</td><td rowspan="2">TOP_IOC_GPIO3C_IOMUX_SEL_L[3:0]=4'ha</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_TXCLK_M1/SDMMC1_CLK_M0/SAI3_MCLK_M1/PDM0_CLK0_M2/UART3_RX_M2/GPIO1_C1_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>ETH1_TXD3_M1/SDMMC1_CMD_M0/PDM0_SDI2_M2/UART3_TX_M2/SPI1_CSN1_M0/PWM0_CH0_M1/GPIO1_C0_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ETH1_RXCLK_M1/SDMMC1_D2_M0/SAI3_SDO_M1/UART3_CTSN_M2/SPI1_MISO_M0/PCIE0_CLKREQN_M1/GPIO1_B6_d</td><td rowspan="2">TOP_IOC_GPIO1B_IOMUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">ETH1_TXD2_M1/SDMMC1_D3_M0/SAI3_SDI_M1/EMMC_TESTDATA_OUT/FSPI0_TESTDATA_OUT/FSPI1_TESTDATA_OUT_M1/UART3_RTSN_M2/SPI1_CSN0_M0/PCIE0_WAKEEN_M1/GPIO1_B7_d</td><td rowspan="2">TOP_IOC_GPIO1B_IOMUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr></table>

Table 33-5 UART4 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_RXD0_M0/SAI4_SDO_M3/UART4_RX_M0/I2C6_SDA_M2/PWM2_CH1_M2/GPIO2_D1_d</td><td>TOP_IOC_GPIO2D_IOMUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>ETH1_TXCTL_M0/SAI4_SDI_M3/UART4_TX_M0/I2C6_SCL_M2/PWM2_CHO_M2/GPIO2_D0_d</td><td>TOP_IOC_GPIO2D_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ETH1_TXD0_M0/SAI4_SCLK_M3/UART4_CTSN_M0/I2C5_SCL_M2/PWM1_CH5_M2/GPIO2_C6_d</td><td rowspan="2">TOP_IOC_GPIO2C_IOMUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">ETH1_TXD1_M0/SAI4_LRCK_M3/UART4_RTSN_M0/I2C5_SDA_M2/PWM0_CH1_M2/GPIO2_C7_d</td><td rowspan="2">TOP_IOC_GPIO2C_IOMUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_TXD1_M1/FSPI1_D1_M1/UART4_RX_M1/UART2_CTSN_M0/SPI2_MISO_M1/PCIE1_BUTTONRSTN/GPIO1_C5_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>ETH1_TXD0_M1/FSPI1_D0_M1/UART4_TX_M1/UART2_RTSN_M0/SPI2_MOSI_M1/PCIE0_BUTTONRSTN/GPIO1_C4_d</td><td>TOP_IOC_GPIO1C_IOMUX_SEL_H[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ETH1_PPSTRIG_M1/SDMMC1_DETN_M0/FSPI1_CSN0_M1/UART4_CTSN_M1/I2C6_SDA_M1/SPI2_CSN0_M1/GPIO1_C3_u</td><td rowspan="2">TOP_IOC_GPIO1C_IOMUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">ETH1_PPSCLK_M1/SDMMC1_PWREN_M0/FSPI1_RSTN_M1/FSPI1_RSTN_M1/UART4_RTSN_M1/I2C6_SCL_M1/SPI2_CSN1_M1/PWM1_CH2_M1/GPIO1_C2_u</td><td rowspan="2">TOP_IOC_GPIO1C_IOMUX_SEL_L[11:8]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>REF_CLK2_OUT/I2C1_SDA_M1/UART4_RX_M2/PWM1_CH1_M0/GPIO0_B5_d</td><td>PMU1_IOC_GPIO0B_IOMUX_SEL_H[7:1]=4'ha</td></tr><tr><td>uart_tx</td><td></td><td>REF_CLK1_OUT/I2C1_SCL_M1/UART4_TX_M2/PWM1_CH0_M0/GPIO0_B4_d</td><td>PMU1_IOC_GPIO0B_IOMUX_SEL_H[3:0]=4'ha</td></tr></table>

Table 33-6 UART5 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_DEN/VO_EBC_SDLE/SAI1_SDI1_M1/DSMC_DATA0/FLEXBUS1_D1/UART5_RX_M0/SPI3_CLK_M1/I2C3_SCL_M2/GPIO3_D4_d</td><td>TOP_IOC_GPIO3D_IOMUX_SEL_H[3:0]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_HSYNC/VO_EBC_GDCLK/SAI1_SDI2_M1/DSMC_CLKP/FLEXBUS1_D0/UART5_TX_M0/SPI3_MISO_M1/I2C3_SDA_M2/GPIO3_D5_d</td><td>TOP_IOC_GPIO3D_IOMUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_VSYNC/VO_EBC_SDCLK/SAI1_SDI3_M1/DSMC_CLKN/FLEXBUS1_CLK/UART5_CTSN_M0/SPI3_MOSI_M1/PWM2_CH6_M3/GPIO3_D6_d</td><td rowspan="2">TOP_IOC_GPIO3D_IOMUX_SEL_H[11:2]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_CLK/VO_EBC_SDOE/CAM_CLK0_OUT_M0/SAI4_SCLK_M1/DSMC_RESETN/FLEXBUS0_D15_M0/FLEXBUS1_D12_M0/FLEXBUS1_CSN_M1/UART5_RTSN_M0/SPI3_CSN1_M1/PWM2_CH7_M3/GPIO3_D7_d</td><td rowspan="2">TOP_IOC_GPIO3D_IOMUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>SAI1_SDO2_M0/SAI1_SDI2_M0/PDM1_SDI2_M1/FLEXBUS1_D14_M1/SPI4_MOSI_M2/UART5_RX_M1/UART6_CTSN_M0/UART2_CTSN_M1/GPIO4_B1_d</td><td>TOP_IOC_GPIO4B_IOMUX_SEL_L[7:1]=4'ha</td></tr><tr><td>uart_tx</td><td></td><td>SAI1_SDO1_M0/SAI1_SDI3_M0/PDM1_CLK1_M1/FLEXBUS1_D13_M1/SPI4_CLK_M2/UART5_TX_M1/UART6_RTSN_M0/UART2_RTSN_M1/GPIO4_B0_d</td><td>TOP_IOC_GPIO4B_IOMUX_SEL_L[3:0]=4'ha</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">SAI1_LRCK_M0/FLEXBUS1_D12_M1/SPI4_CSN1_M2/UART5_CTSN_M1/I2C2_SDA_M2/PCIE1_CLKREQN_M2/GPIO4_A5_d</td><td rowspan="2">TOP_IOC_GPIO4A_IOMUX_SEL_H[7:4]=4'ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">SAI1_SCLK_M0/FLEXBUS1_CSN_M4/SPI3_CSN0_M2/UART5_RTSN_M1/I2C2_SCL_M2/PWM2_CH4_M1/GPIO4_A3_d</td><td rowspan="2">TOP_IOC_GPIO4A_IOMUX_SEL_L[15:12]=4'ha</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>SDMMC0_CMD/FSPI1_CSN0_M0/SAI3_SDO_M3/UART5_RX_M2/I2C5_SDA_M0/SPI0_CSN0_M1/PWM2_CH4_M0/GPIO2_A4_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_H[3:0]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>SDMMC0_CLK/FSPI1_CLK_M0/SAI3_SCLK_M3/TEST_CLK_OUT/UART5_TX_M2/I2C5_SCL_M0/SPI0_CLK_M1/I3C1_SDA_PU_M1/GPIO2_A5_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">SDMMC0_D3/FSPI1_D3_M0/DSM_AUD_RN_M0/SAI3_SDI_M3/JTAG_TMS_M0/UART5_CTSN_M2/CAN1_TX_M0/I3C1_SDA_M1/GPIO2_A3_d</td><td rowspan="2">TOP_IOC_GPIO2A_IOMUX_SEL_L[15:12]=4'ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">SDMMC0_D2/FSPI1_D2_M0/DSM_AUD_RP_M0/SAI3_LRCK_M3/JTAG_TCK_M0/UART5_RTSN_M2/SPI0_CSN1_M1/CAN1_RX_M0/I3C1_SCL_M1/GPIO2_A2_d</td><td rowspan="2">TOP_IOC_GPIO2A_IOMUX_SEL_L[11:8]=4'ha</td></tr><tr><td>uart_de</td><td></td></tr></table>

Table 33-7 UART6 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>SAI4_LRCK_M0/PDM1_CLK0_M1/FLEXBUS0_D14_M1/SPI3_MISO_M2/UART6_RX_M0/I2C4_SDA_M1/CAN0_RX_M2/GPIO4_A6_d</td><td>TOP_IOC_GPIO4A_IOMUX_SEL_H[11:8]=4'ha</td></tr><tr><td>uart_tx</td><td></td><td>SAI4_SCLK_M0/PDM1_SDI3_M1/FLEXBUS0_D13_M1/SPI3_MOSI_M2/UART6_TX_M0/I2C4_SCL_M1/CAN0_TX_M2/GPIO4_A4_d</td><td>TOP_IOC_GPIO4A_IOMUX_SEL_H[3:0]=4'h a</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">SAI1_SDO2_M0/SAI1_SDI2_M0/PDM1_SDI2_M1/FLEXBUS1_D14_M1/SPI4_MOSI_M2/UART5_RX_M1/UART6_CTSN_M0/UART2_CTSN_M1/GPIO4_B1_d</td><td rowspan="2">TOP_IOC_GPIO4B_IOMUX_SEL_L[7:4]=4'hb</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">SAI1_SDO1_M0/SAI1_SDI3_M0/PDM1_CLK1_M1/FLEXBUS1_D13_M1/SPI4_CLK_M2/UART5_TX_M1/UART6_RTSN_M0/UART2_RTSN_M1/GPIO4_B0_d</td><td rowspan="2">TOP_IOC_GPIO4B_IOMUX_SEL_L[3:0]=4'hb</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_RXCTL_M0/UART6_RX_M1/I3C1_SDA_M0/PWM2_CH3_M2/GPIO2_D3_d</td><td>TOP_IOC_GPIO2D_IOMUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>CAM_CLK0_OUT_M1/ETH1_RXD1_M0/SAI4_MCLK_M3/UART6_TX_M1/I3C1_SCL_M0/PWM2_CH2_M2/GPIO2_D2_d</td><td>TOP_IOC_GPIO2D_IOMUX_SEL_L[11:8]=4'h 9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ISP_FLASH_TRIGOUT_M0/ETH1_MDIO_M0/UART6_CTSN_M1/I2C9_SCL_M2/PWM2_CH5_M2/GPIO2_D5_d</td><td rowspan="2">TOP_IOC_GPIO2D_IOMUX_SEL_H[7:4]=4'h 9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td>ISP_PRELIGHT_TRIG_M0/ETH1_MDC_M0/UART6_RTSN_M1/I2C9_SDA_</td><td>TOP_IOC_GPIO2D_IOMUX_SEL_H[3:0]=4'h</td></tr><tr><td>uart_de</td><td></td><td>M2/PWM2_CH4_M2/GPIO2_D4_d</td><td>9</td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>EMMC_RSTN/FSPI0_CSN0/UART6_RX_M2/I2C7_SDA_M0/MIPI_TE_M3/PWM2_CH1_M0/GPIO1_B3_u</td><td>TOP_IOC_GPIO1B_IO MUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>EMMC_CMD/FSPI0_RSTN/FSPI0_CSN1/UART6_TX_M2/I2C7_SCL_M0/G PIO1_B0_u</td><td>TOP_IOC_GPIO1B_IO MUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">EMMC_D3/FSPI0_D3/SAI0_SDO2_M2/SAI0_SDI2_M2/PDM0_SDI1_M1/UART7_RX_M1/UART6_CTSN_M2/GPI O1_A3_u</td><td rowspan="2">TOP_IOC_GPIO1A_IO MUX_SEL_L[15:12]=4'ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">EMMC_D2/FSPI0_D2/SAI0_SDO1_M2/SAI0_SDI3_M2/PDM0_SDI3_M1/UART7_TX_M1/UART6_RTSN_M2/GPI O1_A2_u</td><td rowspan="2">TOP_IOC_GPIO1A_IO MUX_SEL_L[11:8]=4'h a</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX3</td></tr><tr><td>uart_rx</td><td></td><td>ISP_FLASH_TRIGOUT_M1/SAI4_SDO_M2/VP0_SYNC_OUT/SATA1_ACTLED_M1/I2C3_SDA_M3/SPI4_MOSI_M0/UART6_RX_M3/PWM2_CH5_M1/GPIO4_C5_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_H[7:4]=4'hd</td></tr><tr><td>uart_tx</td><td></td><td>ISP_PRELIGHT_TRIG_M1/SAI4_LRC_K_M2/DP_HPDIN_M0/I2C3_SCL_M3/SPI4_CSN0_M0/UART6_TX_M3/PWM2_CH6_M1/GPIO4_C4_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_H[3:0]=4'hd</td></tr></table>

Table 33-8 UART7 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_D6/ETH0_RXD2_M1/SAI0_L RCK_M0/UART7_RX_M0/UART8_CTS N_M1/I2C8_SDA_M2/GPIO2_B7_d</td><td>TOP_IOC_GPIO2B_IO MUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_D7/ETH1_PTP_REFCLK_M1/ ETH0_RXD3_M1/SAI0_SCLK_M0/UA RT7_TX_M0/UART8_RTSN_M1/I2C8 _SCL_M2/GPIO2_B6_d</td><td>TOP_IOC_GPIO2B_IO MUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VI_CIF_D9/SDMMC1_PWREN_M1/ET H0_TXD2_M1/SAI0_SDI3_M0/PDM0 _SDIO_M3/UART7_CTSN_M0/SPI4_ MOSI_M3/SATA0_ACTLED_M0/GPIO 2_B4_d</td><td rowspan="2">TOP_IOC_GPIO2B_IO MUX_SEL_H[3:0]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VI_CIF_D8/SDMMC1_DETN_M1/ETH 0_RXCLK_M1/SAI0_MCLK_M0/PDM0 _CLKO_M3/UART7_RTSN_M0/SPI4_ MISO_M3/SATA1_ACTLED_M0/GPIO 2_B5_d</td><td rowspan="2">TOP_IOC_GPIO2B_IO MUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>EMMC_D3/FSPIO_D3/SAI0_SDO2_M 2/SAI0_SDI2_M2/PDM0_SDI1_M1/U ART7_RX_M1/UART6_CTSN_M2/GPI O1_A3_u</td><td>TOP_IOC_GPIO1A_IO MUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>EMMC_D2/FSPIO_D2/SAI0_SDO1_M 2/SAI0_SDI3_M2/PDM0_SDI3_M1/UART7_TX_M1/UART6_RTSN_M2/GPI O1_A2_u</td><td>TOP_IOC_GPIO1A_IO MUX_SEL_L[11:8]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">EMMC_D1/FSP10_D1/SAI0_LRCK_M 2/UART7_CTSN_M1/I2C2_SDA_M1/ GPIO1_A1_u</td><td rowspan="2">TOP_IOC_GPIO1A_IO MUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">EMMC_D0/FSP10_D0/SAI0_SCLK_M 2/UART7_RTSN_M1/I2C2_SCL_M1/ GPIO1_A0_u</td><td rowspan="2">TOP_IOC_GPIO1A_IO MUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>SDMMC0_D0/FSP11_D0_M0/DSM_A UD_LP_M0/UART0_RX_M1/UART7_R X_M2/I2C8_SCL_M0/SPI0_MOSI_M 1/CAN0_RX_M0/PWM2_CH2_M0/GP IO2_A0_d</td><td>TOP_IOC_GPIO2A_IO MUX_SEL_L[3:0]=4'ha</td></tr><tr><td>uart_tx</td><td></td><td>SDMMC0_D1/FSP11_D1_M0/DSM_A UD_LN_M0/SAI3_MCLK_M3/UART0_ TX_M1/UART7_TX_M2/I2C8_SDA_M 0/SPI0_MISO_M1/CAN0_TX_M0/PW M2_CH3_M0/GPIO2_A1_d</td><td>TOP_IOC_GPIO2A_IO MUX_SEL_L[7:4]=4'ha</td></tr></table>

Table 33-9 UART8 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D6/VO_EBC_SDDO6/SAI1_SD00_M1/DSMC_DATA4/FLEXBUS1_D6/UART8_RX_M0/SPI1_MISO_M2/PWM2_CH2_M3/GPIO3_C5_d</td><td>TOP_IOC_GPIO3C_IOMUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D5/VO_EBC_SDDO5/SAI1_LRCK_M1/DSMC_DATA3/FLEXBUS1_D5/UART8_TX_M0/SPI1_MOSI_M2/GPIO3_C6_d</td><td>TOP_IOC_GPIO3C_IOMUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D3/VO_EBC_SDDO3/SAI1_MCLK_M1/DSMC_DATA1/FLEXBUS1_D3/UART8_CTSN_M0/SPI1_CSN0_M2/PWM2_CH3_M3/GPIO3_D0_d</td><td rowspan="2">TOP_IOC_GPIO3D_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D4/VO_EBC_SDDO4/SAI1_SCLK_M1/DSMC_DATA2/FLEXBUS1_D4/UART8_RTSN_M0/SPI1_CLK_M2/GPIO3_C7_d</td><td rowspan="2">TOP_IOC_GPIO3C_IOMUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_D14/SDMMC1_D1_M1/ETH0_TXCTL_M1/SAI0_SDO1_M0/UART8_RX_M1/I2C4_SDA_M2/GPIO2_A7_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_H[15:12]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_D15/SDMMC1_D0_M1/ETH0_RXD0_M1/SAI0_SDO0_M0/UART8_TX_M1/SPI4_CSN1_M3/I2C4_SCL_M2/GPIO2_A6_d</td><td>TOP_IOC_GPIO2A_IOMUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VI_CIF_D6/ETH0_RXD2_M1/SAI0_LRCK_M0/UART7_RX_M0/UART8_CTSN_M1/I2C8_SDA_M2/GPIO2_B7_d</td><td rowspan="2">TOP_IOC_GPIO2B_IOMUX_SEL_H[15:12]=4'ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VI_CIF_D7/ETH1_PTP_REFCLK_M1/ETH0_RXD3_M1/SAI0_SCLK_M0/UART7_TX_M0/UART8_RTSN_M1/I2C8_SCL_M2/GPIO2_B6_d</td><td rowspan="2">TOP_IOC_GPIO2B_IOMUX_SEL_H[11:8]=4'ha</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>I2C0_SDA_M1/UART8_RX_M2/I3C0_SDA_M0/GPIO0_C2_d</td><td>PMU1_IOC_GPIO0C_IOMUX_SEL_L[11:8]=4'ha</td></tr><tr><td>uart_tx</td><td></td><td>I2C0_SCL_M1/UART8_TX_M2/I3C0_SCL_M0/GPIO0_C1_d</td><td>PMU1_IOC_GPIO0C_IOMUX_SEL_L[7:4]=4'ha</td></tr></table>

Table 33-10 UART9 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_D5/ETH1_RXD2_M0/ETH0_PTP_REFCLK_M1/PDM1_SDI1_M0/UART9_RX_M0/PWM1_CH0_M2/GPIO2_C0_d</td><td>TOP_IOC_GPIO2C_IOMUX_SEL_L[3:0]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_D4/ETH1_RXD3_M0/ETH0_PSPSCLK_M1/SAI2_MCLK_M1/PDM1_CLK1_M0/UART9_TX_M0/SPI1_CSN1_M1/PWM1_CH1_M2/GPIO2_C1_d</td><td>TOP_IOC_GPIO2C_IOMUX_SEL_L[7:4]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">CAM_CLK2_OUT_M1/ETH1_MCLK_M0/ETH_CLK0_25M_OUT_M1/SAI0_SDO3_M0/SPDIF_TX0_M2/UART9_CTSN_M0/SPI3_CSN1_M0/PWM2_CH7_M2/GPIO2_D7_d</td><td rowspan="2">TOP_IOC_GPIO2D_IOMUX_SEL_H[15:8]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">CAM_CLK1_OUT_M1/ETH_CLK1_25M_OUT_M0/ETH0_MCLK_M1/SAI3_MCLK_M2/SPDIF_RX0_M2/UART9_RTSN_M0/I3C1_SDA_PU_M0/PWM2_CH6_M2/GPIO2_D6_d</td><td rowspan="2">TOP_IOC_GPIO2D_IOMUX_SEL_H[11:8]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D17/VO_EBC_SDCE1/ETH0_RXD0_M0/PDM1_SDI1_M2/DSMC_DATA11/FLEXBUS0_D3/UART9_RX_M1/I2C8_SDA_M3/GPIO3_B2_d</td><td>TOP_IOC_GPIO3B_IOMUX_SEL_L[11:8]=4'h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D16/VO_EBC_SDCE0/ETH0_TXCTL_M0/PDM1_SDI0_M2/DSMC_DATA10/FLEXBUS0_D2/UART9_TX_M1/I2C8_SCL_M3/GPIO3_B3_d</td><td>TOP_IOC_GPIO3B_IOMUX_SEL_L[15:12]=4'h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D14/VO_EBC_SDDO14/ETH0_TXD0_M0/SPDIF_TX1_M0/DSMC_DATA8/FLEXBUS0_D0/UART9_CTSN_M1/PWM1_CH5_M3/GPIO3_B5_d</td><td rowspan="2">TOP_IOC_GPIO3B_IOMUX_SEL_H[7:4]=4'h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D15/VO_EBC_SDDO15/ETH0_TXD1_M0/SPDIF_RX1_M0/DSMC_DATA9/FLEXBUS0_D1/UART9_RTSN_M1/PWM1_CH4_M3/GPIO3_B4_d</td><td rowspan="2">TOP_IOC_GPIO3B_IOMUX_SEL_H[3:0]=4'h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>DSM_AUD_RN_M1/HDMI_TX_SDA/I2C2_SDA_M3/CAN0_RX_M1/UART9_RX_M2/PWM2_CH1_M1/GPIO4_C3_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_L[15:12]=4'hd</td></tr><tr><td>uart_tx</td><td></td><td>DSM_AUD_RP_M1/HDMI_TX_SCL/I2C2_SCL_M3/CAN0_TX_M1/UART9_TX_M2/PWM2_CH0_M1/GPIO4_C2_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_L[11:8]=4'hd</td></tr></table>

Table 33-11 UART10 Interface Description

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D19/VO_EBC_SDCE3/ETH0_MCLK_M0/SAI4_MCLK_M1/DSM C_CSN1/FLEXBUS0_D8/UART10_RX_M0/SPI2_MOSI_M2/PWM0_CH0_M 3/GPIO3_B0_d</td><td>TOP_IOC_GPIO3B_IOMUX_SEL_L[3:0]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D18/VO_EBC_SDCE2/ETH0_RXD1_M0/PDM1_CLK0_M2/DSM C_DATA12/FLEXBUS0_D4/UART10_TX_M0/SPI4_CSN0_M1/PWM1_CH3_M3/GPIO3_B1_d</td><td>TOP_IOC_GPIO3B_IOMUX_SEL_L[7:4]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D21/VO_EBC_GDOE/ETH 0_MDC_M0/PDM1_SDI2_M2/DSMC_DATA14/FLEXBUS0_D6/UART1_RX_M2/UART10_CTSN_M0/PWM1_CH2_M3/GPIO3_A6_d</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_H[11:8]=4&#x27;ha</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D20/VO_EBC_VCOM/ETH 0_RXCTL_M0/PDM1_CLK1_M2/DSM C_DATA13/FLEXBUS0_D5/UART1_TX_M2/UART10_RTSN_M0/GPIO3_A7_d</td><td rowspan="2">TOP_IOC_GPIO3A_IOMUX_SEL_H[15:12]=4&#x27;ha</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>ETH1_RXCTL_M1/SAI2_SCLK_M0/UART10_RX_M1/I3C0_SDA_PU_M1/G PIO1_D1_d</td><td>TOP_IOC_GPIO1D_IOMUX_SEL_L[7:4]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>ETH1_RXD1_M1/SAI2_SDO_M0/UART10_TX_M1/GPIO1_D0_d</td><td>TOP_IOC_GPIO1D_IOMUX_SEL_L[3:0]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">ETH_CLK1_25M_OUT_M1/FSPI1_CLK_M1/PDM0_CLK1_M2/SPDIF_TX1_M2/UART10_CTSN_M1/I2C5_SDA_M1/SPI2_CLK_M1/SATA_MPSWIT/CLK 1_32K_OUT/GPIO1_D5_d</td><td rowspan="2">TOP_IOC_GPIO1D_IOMUX_SEL_H[7:4]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">ETH1_MCLK_M1/SAI2_MCLK_M0/PDM0_SDI3_M2/SPDIF_RX1_M2/UART 10_RTSN_M1/I2C5_SCL_M1/GPIO1_D4_d</td><td rowspan="2">TOP_IOC_GPIO1D_IOMUX_SEL_H[3:0]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>SAI0_SDOO_M1/DP_HPDIN_M1/UART10_RX_M2/I3C0_SDA_PU_M0/GPIO0_C5_d</td><td>PMU1_IOC_GPIO0C_IOMUX_SEL_H[7:4]=4&#x27;ha</td></tr><tr><td>uart_tx</td><td></td><td>SAI0_MCLK_M1/PDM0_CLK0_M0/UART10_TX_M2/PWM0_CH0_M0/GPIO0_C4_d</td><td>PMU1_IOC_GPIO0C_IOMUX_SEL_H[3:0]=4&#x27;ha</td></tr></table>

Table 33-12 UART11 Interface Description

RK3576 TRM (Part 1)

<table><tr><td>Module Pin</td><td></td><td>Pad Name</td><td>IOMUX Setting</td></tr><tr><td colspan="4">IOMUX0</td></tr><tr><td>uart_rx</td><td></td><td>VO_LCDC_D10/VO_EBC_SDDO10/ETH0_PTP_REFCLK_M0/SAI1_SDO2_M1/DSMC_DATA6/FLEXBUS1_D8/UART11_RX_M0/SPI2_MISO_M2/I2C5_SDA_M3/CAN0_RX_M3/GPIO3_C1_d</td><td>TOP_IOC_GPIO3C_IOMUX_SEL_L[7:4]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VO_LCDC_D7/VO_EBC_SDDO7/SAI1_SDO1_M1/DSMC_DATA5/FLEXBUS1_D7/UART11_TX_M0/SPI2_CSN0_M2/I2C5_SCL_M3/CAN0_TX_M3/GPIO3_C4_d</td><td>TOP_IOC_GPIO3C_IOMUX_SEL_H[3:0]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VO_LCDC_D8/VO_EBC_SDDO8/ETH0_TXD2_M0/SAI2_LRCK_M2/DSMC_INT3/FLEXBUS0_D10/FLEXBUS0_CSN_M2/UART11_CTSN_M0/SPI4_MOSI_M1/I2C9_SDA_M3/PWM2_CH1_M3/GPIO3_C3_d</td><td rowspan="2">TOP_IOC_GPIO3C_IOMUX_SEL_L[15:12]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VO_LCDC_D9/VO_EBC_SDDO9/ETH0_TXD3_M0/SAI2_SCL_M2/DSMC_INT1/FLEXBUS0_D9/UART11_RTSN_M0/SPI4_MISO_M1/I2C9_SCL_M3/PWM2_CH0_M3/GPIO3_C2_d</td><td rowspan="2">TOP_IOC_GPIO3C_IOMUX_SEL_L[11:8]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX1</td></tr><tr><td>uart_rx</td><td></td><td>VI_CIF_D0/ETH1_TXCLK_M0/SAI2_SDI_M1/PDM1_CLK0_M0/UART11_RX_M1/SPI1_CLK_M1/PWM1_CH4_M2/GPIO2_C5_d</td><td>TOP_IOC_GPIO2C_IOMUX_SEL_H[7:4]=4&#x27;h9</td></tr><tr><td>uart_tx</td><td></td><td>VI_CIF_D1/ETH1_TXD3_M0/SAI2_SDO_M1/PDM1_SDIO_M0/UART11_TX_M1/SPI1_CSN0_M1/PWM1_CH3_M2/GPIO2_C4_d</td><td>TOP_IOC_GPIO2C_IOMUX_SEL_H[3:0]=4&#x27;h9</td></tr><tr><td>uart_cts_n</td><td></td><td rowspan="2">VI_CIF_D3/ETH1_RXCLK_M0/ETH0_PPSTRIG_M1/SAI2_SCL_M1/PDM1_SDI2_M0/UART11_CTSN_M1/SPI1_MOSI_M1/PWM1_CH2_M2/GPIO2_C2_d</td><td rowspan="2">TOP_IOC_GPIO2C_IOMUX_SEL_L[11:8]=4&#x27;h9</td></tr><tr><td>uart_re</td><td></td></tr><tr><td>uart_rts_n</td><td></td><td rowspan="2">VI_CIF_D2/ETH1_TXD2_M0/SAI2_LRCK_M1/PDM1_SDI3_M0/UART11_RTSN_M1/SPI1_MISO_M1/PWM0_CH0_M2/GPIO2_C3_d</td><td rowspan="2">TOP_IOC_GPIO2C_IOMUX_SEL_L[15:12]=4&#x27;h9</td></tr><tr><td>uart_de</td><td></td></tr><tr><td colspan="4">IOMUX2</td></tr><tr><td>uart_rx</td><td></td><td>DSM_AUD_LN_M1/HDMI_TX_HPDIN_M0/PCIE1_CLKREQN_M3/I2C7_SDA_M3/EDP_TX_HPDIN_M0/UART11_RX_M2/PWM0_CH1_M1/GPIO4_C1_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_L[7:4]=4&#x27;hd</td></tr><tr><td>uart_tx</td><td></td><td>DSM_AUD_LP_M1/SAI4_MCLK_M2/HDMI_TX_CEC_M0/I2C7_SCL_M3/SPI4_CSN1_M0/UART11_TX_M2/PWM1_CH5_M1/GPIO4_C0_d</td><td>VCCIO6_IOC_GPIO4C_IOMUX_SEL_L[3:0]=4&#x27;hd</td></tr></table>

## 33.6 Application Notes

## 33.6.1 No FIFO Mode Transfer Flow

![](images/62bb24a5c37ab64a8fefe7f473210a394b243710a9adc922bfec384a917abce1.jpg)  
Fig. 33-7 UART None FIFO mode

33.6.2 FIFO Mode Transfer Flow

![](images/bdcd2b8e36068aaeed3afcbbeba24b371d1337ee0952eb7cbb841f58da6dc4c4.jpg)

![](images/16516a9c9816220bfb1e80f376983d86d58d1008075c5d5029e7cbad80eed2c3.jpg)  
Fig. 33-8 UART FIFO mode  
The UART is an APB slave performing:

Serial-to-parallel conversion on data received from a peripheral device. Parallel-to-serial conversion on data transmitted to the peripheral device. The CPU reads and writes data and control/status information through the APB interface. The transmitting and receiving paths are buffered with internal FIFO memories enabling up to 64-bytes to be stored independently in both transmit and receive modes. A baud rate generator can generate a common transmit and receive internal clock input. The baud rates will depend on the internal clock frequency. The UART will also provide transmit, receive and exception interrupts to system. A DMA interface is implemented for improving the system performance.

## 33.6.3 Baud Rate Calculation

## UART clock generation

UARTs source clocks (SCLK) can be selected from different PLL outputs, or from XIN24M. SCLK can be divided to different frequency, please refer to CRU chapter.

## UART baud rate configuration

The following table provides some reference configuration for different UART baud rates. Table 33-13 UART Baud Rate Configuration

<table><tr><td>Baud Rate</td><td>Reference Configuration</td></tr><tr><td>115.2 Kbps</td><td>Configure PLL to get 1188MHz clock output; Divide 1188MHz clock by 1536/24750 to get 73.728MHz clock by shared fractional divider; Divide 73.728MHz clock by 20 to get 3.6864MHz clock by independent integer divider; Configure UART_DLL to 2.</td></tr><tr><td>460.8 Kbps</td><td>Configure PLL to get 1188MHz clock output; Divide 1188MHz clock by 1536/24750 to get 73.728MHz clock by shared fractional divider; Divide 73.728MHz clock by 10 to get 460.8KHz clock by independent integer divider; Configure UART_DLL to 1.</td></tr></table>
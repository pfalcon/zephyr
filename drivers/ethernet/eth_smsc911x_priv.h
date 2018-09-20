/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This file is the re-implementation of mps2_ethernet_api and Selftest's ETH_MPS2.
 * MPS2 Selftest:https://silver.arm.com/browse/VEI10 ->
 *     \ISCM-1-0\AN491\software\Selftest\v2m_mps2\
 */
#ifndef _SMSC9220_ETH_H_
#define _SMSC9220_ETH_H_

#ifndef __I
#define __I
#endif
#ifndef __O
#define __O
#endif
#ifndef __IO
#define __IO
#endif

#define uint32_t u32_t
#define uint16_t u16_t
#define uint8_t u8_t

#define GET_BITFIELD(val, lsb, msb) (((val) >> (lsb)) & ((1 << ((msb) - (lsb) + 1)) - 1))
#define BFIELD(val, name) GET_BITFIELD(val, name ## _Lsb, name ## _Msb)
#define SMSC9220_BFIELD(reg, bfield) BFIELD(SMSC9220->reg, reg ## _ ## bfield)

/******************************************************************************/
/*                       SMSC9220 Register Definitions                        */
/******************************************************************************/

typedef struct {
/*   Receive FIFO Ports (offset 0x0) */
__I	uint32_t  RX_DATA_PORT;
	uint32_t  RESERVED1[0x7];
/*   Transmit FIFO Ports (offset 0x20) */
__O   uint32_t  TX_DATA_PORT;
      uint32_t  RESERVED2[0x7];

/*   Receive FIFO status port (offset 0x40) */
__I   uint32_t  RX_STAT_PORT;
/*   Receive FIFO status peek (offset 0x44) */
__I   uint32_t  RX_STAT_PEEK;
/*   Transmit FIFO status port (offset 0x48) */
__I   uint32_t  TX_STAT_PORT;
/*   Transmit FIFO status peek (offset 0x4C) */
__I   uint32_t  TX_STAT_PEEK;

/*   Chip ID and Revision (offset 0x50) */
__I   uint32_t  ID_REV;
/*   Main Interrupt Configuration (offset 0x54) */
__IO  uint32_t  IRQ_CFG;
/*   Interrupt Status (offset 0x58) */
__IO  uint32_t  INT_STS;
/*   Interrupt Enable Register (offset 0x5C) */
__IO  uint32_t  INT_EN;
/*   Reserved for future use (offset 0x60) */
      uint32_t  RESERVED3;
/*   Read-only byte order testing register 87654321h (offset 0x64) */
__I   uint32_t  BYTE_TEST;
/*   FIFO Level Interrupts (offset 0x68) */
__IO  uint32_t  FIFO_INT;
/*   Receive Configuration (offset 0x6C) */
__IO  uint32_t  RX_CFG;
/*   Transmit Configuration (offset 0x70) */
__IO  uint32_t  TX_CFG;
/*   Hardware Configuration (offset 0x74) */
__IO  uint32_t  HW_CFG;
/*   RX Datapath Control (offset 0x78) */
__IO  uint32_t  RX_DP_CTRL;
/*   Receive FIFO Information (offset 0x7C) */
__I   uint32_t  RX_FIFO_INF;
/*   Transmit FIFO Information (offset 0x80) */
__I   uint32_t  TX_FIFO_INF;
/*   Power Management Control (offset 0x84) */
__IO  uint32_t  PMT_CTRL;
/*   General Purpose IO Configuration (offset 0x88) */
__IO  uint32_t  GPIO_CFG;
/*   General Purpose Timer Configuration (offset 0x8C) */
__IO  uint32_t  GPT_CFG;
/*   General Purpose Timer Count (offset 0x90) */
__I   uint32_t  GPT_CNT;
/*   Reserved for future use (offset 0x94) */
      uint32_t  RESERVED4;
/*   WORD SWAP Register (offset 0x98) */
__IO  uint32_t  ENDIAN;
/*   Free Run Counter (offset 0x9C) */
__I   uint32_t  FREE_RUN;
/*   RX Dropped Frames Counter (offset 0xA0) */
__I   uint32_t  RX_DROP;
/*   MAC CSR Synchronizer Command (offset 0xA4) */
__IO  uint32_t  MAC_CSR_CMD;
/*   MAC CSR Synchronizer Data (offset 0xA8) */
__IO  uint32_t  MAC_CSR_DATA;
/*   Automatic Flow Control Configuration (offset 0xAC) */
__IO  uint32_t  AFC_CFG;
/*   EEPROM Command (offset 0xB0) */
__IO  uint32_t  E2P_CMD;
/*   EEPROM Data (offset 0xB4) */
__IO  uint32_t  E2P_DATA;

} SMSC9220_TypeDef;

#define HW_CFG_SRST BIT(0)

#define RX_STAT_PORT_PKT_LEN_Lsb 16
#define RX_STAT_PORT_PKT_LEN_Msb 29

#define PMT_CTRL_READY BIT(0)

#define RX_DP_CTRL_RX_FFWD BIT(31)

#define RX_FIFO_INF_RXSUSED_Lsb 16
#define RX_FIFO_INF_RXSUSED_Msb 23
#define RX_FIFO_INF_RXDUSED_Lsb 0
#define RX_FIFO_INF_RXDUSED_Msb 15

#define MAC_CSR_CMD_BUSY  BIT(31)
#define MAC_CSR_CMD_READ  BIT(30)
#define MAC_CSR_CMD_WRITE 0

#if 0
void smsc9220_wakeup(void)
{
/* LAN9118 Datasheet 3.10.2.1*/
/*
A write to the BYTE_TEST register, regardless of whether a wake-up frame or Magic Packet was detected, will return
LAN9118 to the D0 state and will reset the PM_MODE field to the D0 state. As noted above, the host is required to check
the READY bit and verify that it is set before attempting any other reads or writes of the device.
*/
}
#endif

/* SMSC9220 MAC Registers       Indices */
#define SMSC9220_MAC_CR         0x1
#define SMSC9220_MAC_ADDRH      0x2
#define SMSC9220_MAC_ADDRL      0x3
#define SMSC9220_MAC_HASHH      0x4
#define SMSC9220_MAC_HASHL      0x5
#define SMSC9220_MAC_MII_ACC    0x6
#define SMSC9220_MAC_MII_DATA   0x7
#define SMSC9220_MAC_FLOW       0x8
#define SMSC9220_MAC_VLAN1      0x9
#define SMSC9220_MAC_VLAN2      0xA
#define SMSC9220_MAC_WUFF       0xB
#define SMSC9220_MAC_WUCSR      0xC

#define MAC_MII_ACC_MIIBZY BIT(0)
#define MAC_MII_ACC_WRITE  BIT(1)
#define MAC_MII_ACC_READ   0

/* SMSC9220 PHY Registers       Indices */
#define SMSC9220_PHY_BCONTROL   0x0
#define SMSC9220_PHY_BSTATUS    0x1
#define SMSC9220_PHY_ID1        0x2
#define SMSC9220_PHY_ID2        0x3
#define SMSC9220_PHY_ANEG_ADV   0x4
#define SMSC9220_PHY_ANEG_LPA   0x5
#define SMSC9220_PHY_ANEG_EXP   0x6
#define SMSC9220_PHY_MCONTROL   17
#define SMSC9220_PHY_MSTATUS    18
#define SMSC9220_PHY_CSINDICATE 27
#define SMSC9220_PHY_INTSRC     29
#define SMSC9220_PHY_INTMASK    30
#define SMSC9220_PHY_CS         31

#ifndef SMSC9220_BASE
#define SMSC9220_BASE           DT_SMSC_LAN9220_0_BASE_ADDRESS /* Ethernet SMSC9220 Base Address */
#endif
#define SMSC9220                ((volatile SMSC9220_TypeDef *) SMSC9220_BASE)

enum smsc9220_interrupt_source {
    enum_smsc9220_interrupt_gpio0 = 0,
    enum_smsc9220_interrupt_gpio1 = 1,
    enum_smsc9220_interrupt_gpio2 = 2,
    enum_smsc9220_interrupt_rxstatus_fifo_level = 3,
    enum_smsc9220_interrupt_rxstatus_fifo_full = 4,
    /* 5 Reserved according to Datasheet */
    enum_smsc9220_interrupt_rx_dropped_frame = 6,
    enum_smsc9220_interrupt_txstatus_fifo_level = 7,
    enum_smsc9220_interrupt_txstatus_fifo_full = 8,
    enum_smsc9220_interrupt_txdata_fifo_available = 9,
    enum_smsc9220_interrupt_txdata_fifo_overrun = 10,
    /* 11, 12 Reserved according to Datasheet */
    enum_smsc9220_interrupt_transmit_error = 13,
    enum_smsc9220_interrupt_receive_error = 14,
    enum_smsc9220_interrupt_receive_watchdog_timeout = 15,
    enum_smsc9220_interrupt_txstatus_overflow = 16,
    enum_smsc9220_interrupt_power_management = 17,
    enum_smsc9220_interrupt_phy = 18,
    enum_smsc9220_interrupt_gp_timer = 19,
    enum_smsc9220_interrupt_rx_dma = 20,
    enum_smsc9220_interrupt_tx_ioc = 21,
    /* 22 Reserved according to Datasheet*/
    enum_smsc9220_interrupt_rx_dropped_frame_half = 23,
    enum_smsc9220_interrupt_rx_stopped = 24,
    enum_smsc9220_interrupt_tx_stopped = 25,
    /* 26 - 30 Reserved according to Datasheet*/
    enum_smsc9220_interrupt_sw = 31
};

#endif

/*
	Copyright (C) 2014  Commtech, Inc.

	This file is part of serialfc-linux.

	serialfc-linux is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	serialfc-linux is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with serialfc-linux.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SERIALFC_H
#define SERIALFC_H

#define COMMTECH_VENDOR_ID 0x18f7

#define FC_422_2_PCI_335_ID 0x0004
#define FC_422_4_PCI_335_ID 0x0002
#define FC_232_4_PCI_335_ID 0x000a
#define FC_232_8_PCI_335_ID 0x000b
#define FC_422_4_PCIe_ID 0x0020
#define FC_422_8_PCIe_ID 0x0021

#define FSCC_ID 0x000f
#define SFSCC_ID 0x0014
#define SFSCC_104_LVDS_ID 0x0015
#define FSCC_232_ID 0x0016
#define SFSCC_104_UA_ID 0x0017
#define SFSCC_4_UA_ID 0x0018
#define SFSCC_UA_ID 0x0019
#define SFSCC_LVDS_ID 0x001a
#define FSCC_4_UA_ID 0x001b
#define SFSCC_4_LVDS_ID 0x001c
#define FSCC_UA_ID 0x001d
#define SFSCCe_4_ID 0x001e
#define SFSCC_4_CPCI_ID 0x001f
#define SFSCCe_4_LVDS_UA_ID 0x0022
#define SFSCC_4_UA_CPCI_ID 0x0023
#define SFSCC_4_UA_LVDS_ID 0x0025
#define SFSCC_UA_LVDS_ID 0x0026
#define FSCCe_4_UA_ID 0x0027

#define DLL_OFFSET 0x0
#define DLM_OFFSET 0x1
#define DLD_OFFSET 0x2

#define MPIOINT_OFFSET 0x8f
#define MPIOLVL_OFFSET 0x90
#define MPIO3T_OFFSET 0x91
#define MPIOINV_OFFSET 0x92
#define MPIOSEL_OFFSET 0x93
#define MPIOOD_OFFSET 0x94

#define MPIOINTH_OFFSET 0x95
#define MPIOLVLH_OFFSET 0x96
#define MPIO3TH_OFFSET 0x97
#define MPIOINVH_OFFSET 0x98
#define MPIOSELH_OFFSET 0x99
#define MPIOODH_OFFSET 0x9a

#define UART_EXAR_8XMODE 0x88 /* 8X sampling rate select */
#define UART_EXAR_4XMODE 0x89 /* 4X sampling rate select */
#define UART_EXAR_TXTRG 0x0a /* Tx FIFO trigger level write-only */
#define UART_EXAR_RXTRG 0x0b /* Rx FIFO trigger level write-only */
#define UART_EXAR_FCTR 0x08 /* Feature Control Register */

enum FASTCOM_CARD_TYPE { CARD_TYPE_PCI, CARD_TYPE_PCIe, CARD_TYPE_FSCC, CARD_TYPE_UNKNOWN };

/* Normal registers */
#define FCR_OFFSET 0x2
#define LCR_OFFSET 0x3
#define MCR_OFFSET 0x4
#define SPR_OFFSET 0x7

/* Extended 650 registers when LCR = 0xbf */
#define EFR_OFFSET 0x2
#define TTL_OFFSET 0x4

/* Extended 950 registers */
#define ICR_OFFSET 0x5

/* Indexed control register set */
#define ACR_OFFSET 0x00
#define TCR_OFFSET 0x02
#define CKS_OFFSET 0x03
#define RTL_OFFSET 0x05
#define MDM_OFFSET 0x0e
#define EXT_OFFSET 0x16
#define EXTH_OFFSET 0x17
#define FLR_OFFSET 0x20

/* FSCC Bar0 register set */
#define VSTR_OFFSET 0x4c

struct ioctl_get_dev_info_struct {
      short vendor;       /* vendor ID */
      short device;       /* device ID */
      unsigned char bus;  /* PCI bus number */
      unsigned char slot; /* slot number */
};

#define SERIALFC_IOCTL_MAGIC 0x19

#define IOCTL_FASTCOM_ENABLE_RS485 _IO(SERIALFC_IOCTL_MAGIC, 0)
#define IOCTL_FASTCOM_DISABLE_RS485 _IO(SERIALFC_IOCTL_MAGIC, 1)
#define IOCTL_FASTCOM_GET_RS485 _IOR(SERIALFC_IOCTL_MAGIC, 2, unsigned *)

#define IOCTL_FASTCOM_ENABLE_ECHO_CANCEL _IO(SERIALFC_IOCTL_MAGIC, 3)
#define IOCTL_FASTCOM_DISABLE_ECHO_CANCEL _IO(SERIALFC_IOCTL_MAGIC, 4)
#define IOCTL_FASTCOM_GET_ECHO_CANCEL _IOR(SERIALFC_IOCTL_MAGIC, 5, unsigned *)

#define IOCTL_FASTCOM_ENABLE_TERMINATION _IO(SERIALFC_IOCTL_MAGIC, 6)
#define IOCTL_FASTCOM_DISABLE_TERMINATION _IO(SERIALFC_IOCTL_MAGIC, 7)
#define IOCTL_FASTCOM_GET_TERMINATION _IOR(SERIALFC_IOCTL_MAGIC, 8, unsigned *)

#define IOCTL_FASTCOM_SET_SAMPLE_RATE _IOW(SERIALFC_IOCTL_MAGIC, 9, const unsigned)
#define IOCTL_FASTCOM_GET_SAMPLE_RATE _IOR(SERIALFC_IOCTL_MAGIC, 10, unsigned *)

#define IOCTL_FASTCOM_SET_TX_TRIGGER _IOW(SERIALFC_IOCTL_MAGIC, 11, const unsigned)
#define IOCTL_FASTCOM_GET_TX_TRIGGER _IOR(SERIALFC_IOCTL_MAGIC, 12, unsigned *)

#define IOCTL_FASTCOM_SET_RX_TRIGGER _IOW(SERIALFC_IOCTL_MAGIC, 13, const unsigned)
#define IOCTL_FASTCOM_GET_RX_TRIGGER _IOR(SERIALFC_IOCTL_MAGIC, 14, unsigned *)

#define IOCTL_FASTCOM_SET_CLOCK_RATE _IOW(SERIALFC_IOCTL_MAGIC, 15, const unsigned)

#define IOCTL_FASTCOM_ENABLE_ISOCHRONOUS _IOW(SERIALFC_IOCTL_MAGIC, 16, const unsigned)
#define IOCTL_FASTCOM_DISABLE_ISOCHRONOUS _IO(SERIALFC_IOCTL_MAGIC, 17)
#define IOCTL_FASTCOM_GET_ISOCHRONOUS _IOR(SERIALFC_IOCTL_MAGIC, 18, unsigned *)

#define IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT _IOW(SERIALFC_IOCTL_MAGIC, 19, const unsigned)
#define IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT _IO(SERIALFC_IOCTL_MAGIC, 20)
#define IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT _IOR(SERIALFC_IOCTL_MAGIC, 21, unsigned *)

#define IOCTL_FASTCOM_SET_FRAME_LENGTH _IOW(SERIALFC_IOCTL_MAGIC, 22, const unsigned)
#define IOCTL_FASTCOM_GET_FRAME_LENGTH _IOR(SERIALFC_IOCTL_MAGIC, 23, int *)

#define IOCTL_FASTCOM_GET_CARD_TYPE _IOR(SERIALFC_IOCTL_MAGIC, 24, unsigned *)

#define IOCTL_FASTCOM_ENABLE_9BIT _IO(SERIALFC_IOCTL_MAGIC, 25)
#define IOCTL_FASTCOM_DISABLE_9BIT _IO(SERIALFC_IOCTL_MAGIC, 26)
#define IOCTL_FASTCOM_GET_9BIT _IOR(SERIALFC_IOCTL_MAGIC, 27, unsigned *)

/* Next release will match IOCTL numbers with Windows */
#ifdef RELEASE_PREVIEW
#define IOCTL_FASTCOM_SET_CLOCK_BITS _IOW(SERIALFC_IOCTL_MAGIC, 31, const unsigned char[20])

#define IOCTL_FASTCOM_ENABLE_FIXED_BAUD_RATE _IOW(SERIALFC_IOCTL_MAGIC, 28, const unsigned)
#define IOCTL_FASTCOM_DISABLE_FIXED_BAUD_RATE _IO(SERIALFC_IOCTL_MAGIC, 29)
#define IOCTL_FASTCOM_GET_FIXED_BAUD_RATE _IOR(SERIALFC_IOCTL_MAGIC, 30, unsigned *)
#else
#define IOCTL_FASTCOM_SET_CLOCK_BITS _IOW(SERIALFC_IOCTL_MAGIC, 28, const unsigned char[20])

#define IOCTL_FASTCOM_ENABLE_FIXED_BAUD_RATE _IOW(SERIALFC_IOCTL_MAGIC, 29, const unsigned)
#define IOCTL_FASTCOM_DISABLE_FIXED_BAUD_RATE _IO(SERIALFC_IOCTL_MAGIC, 30)
#define IOCTL_FASTCOM_GET_FIXED_BAUD_RATE _IOR(SERIALFC_IOCTL_MAGIC, 31, unsigned *)
#endif

#define IOCTL_FASTCOM_GET_DEV_INFO _IOR(SERIALFC_IOCTL_MAGIC, 32, struct ioctl_get_dev_info_struct *)

#define IOCTL_FASTCOM_GET_BAUD_RATE _IOR(SERIALFC_IOCTL_MAGIC, 33, unsigned long*)
#define IOCTL_FASTCOM_SET_BAUD_RATE _IOW(SERIALFC_IOCTL_MAGIC, 34, unsigned long)

#endif

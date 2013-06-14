/*
	Copyright (C) 2013  Commtech, Inc.

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

#endif

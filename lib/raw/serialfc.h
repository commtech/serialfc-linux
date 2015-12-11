/*
    Copyright (C) 2014 Commtech, Inc.

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

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/ioctl.h>

enum card_type { SERIALFC_CARD_TYPE_PCI=0, SERIALFC_CARD_TYPE_PCIE, SERIALFC_CARD_TYPE_FSCC, SERIALFC_CARD_TYPE_UNKNOWN };

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

#define IOCTL_FASTCOM_SET_CLOCK_BITS _IOW(SERIALFC_IOCTL_MAGIC, 28, const unsigned char[20])

#define IOCTL_FASTCOM_GET_BAUD_RATE _IOR(SERIALFC_IOCTL_MAGIC, 33, unsigned long*)
#define IOCTL_FASTCOM_SET_BAUD_RATE _IOW(SERIALFC_IOCTL_MAGIC, 34, unsigned long)

#ifdef __cplusplus
}
#endif

#endif

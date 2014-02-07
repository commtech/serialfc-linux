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

#ifndef UTILS_H
#define UTILS_h

#include <linux/pci.h> /* struct pci_dev */

#include "config.h"
#include "card.h"
#include "port.h"

#define return_if_untrue(expr) \
	if (expr) {} else \
	{ \
		printk(KERN_ERR DEVICE_NAME " %s %s\n", #expr, "is untrue."); \
		return; \
	}

#define return_val_if_untrue(expr, val) \
	if (expr) {} else \
	{ \
		printk(KERN_ERR DEVICE_NAME " %s %s\n", #expr, "is untrue."); \
		return val; \
	}

unsigned is_serialfc_device(struct pci_dev *pdev);

int fastcom_set_sample_rate(struct serialfc_port *port, unsigned value);
void fastcom_get_sample_rate(struct serialfc_port *port, unsigned *value);

int fastcom_set_tx_trigger(struct serialfc_port *port, unsigned value);
int fastcom_get_tx_trigger(struct serialfc_port *port, unsigned *value);

int fastcom_set_rx_trigger(struct serialfc_port *port, unsigned value);
int fastcom_get_rx_trigger(struct serialfc_port *port, unsigned *value);

int fastcom_set_termination(struct serialfc_port *port, int enable);
int fastcom_enable_termination(struct serialfc_port *port);
int fastcom_disable_termination(struct serialfc_port *port);
int fastcom_get_termination(struct serialfc_port *port, int *enabled);

void fastcom_set_echo_cancel(struct serialfc_port *port, int enable);
void fastcom_enable_echo_cancel(struct serialfc_port *port);
void fastcom_disable_echo_cancel(struct serialfc_port *port);
void fastcom_get_echo_cancel(struct serialfc_port *port, int *enabled);

int fastcom_set_rs485(struct serialfc_port *port, int enable);
void fastcom_enable_rs485(struct serialfc_port *port);
void fastcom_disable_rs485(struct serialfc_port *port);
int fastcom_get_rs485(struct serialfc_port *port, int *enabled);

int fastcom_set_clock_rate(struct serialfc_port *port, unsigned rate);
int fastcom_set_clock_bits(struct serialfc_port *port, void *clock_data);

int fastcom_set_isochronous(struct serialfc_port *port, int mode);
int fastcom_enable_isochronous(struct serialfc_port *port, unsigned mode);
int fastcom_disable_isochronous(struct serialfc_port *port);
int fastcom_get_isochronous(struct serialfc_port *port, int *mode);

int fastcom_set_external_transmit(struct serialfc_port *port, unsigned num_chars);
int fastcom_enable_external_transmit(struct serialfc_port *port, unsigned num_chars);
int fastcom_disable_external_transmit(struct serialfc_port *port);
int fastcom_get_external_transmit(struct serialfc_port *port, unsigned *num_chars);

int fastcom_set_frame_length(struct serialfc_port *port, unsigned num_chars);
int fastcom_get_frame_length(struct serialfc_port *port, unsigned *num_chars);

int fastcom_set_9bit(struct serialfc_port *port, int enable);
int fastcom_get_9bit(struct serialfc_port *port, int *enabled);
int fastcom_enable_9bit(struct serialfc_port *port);
int fastcom_disable_9bit(struct serialfc_port *port);

int pcie_set_baud_rate(struct serialfc_port *port, unsigned value);

enum FASTCOM_CARD_TYPE fastcom_get_card_type2(struct serialfc_card *card);
enum FASTCOM_CARD_TYPE fastcom_get_card_type(struct serialfc_port *port);

void fastcom_init_gpio(struct serialfc_port *port);
void fastcom_init_triggers(struct serialfc_port *port);

unsigned char fscc_get_frev(struct serialfc_port *port);

int fscc_enable_async_mode(struct serialfc_port *port);
int fscc_disable_async_mode(struct serialfc_port *port);

__u8 fscc_get_FREV(struct serialfc_port *port);
__u8 fscc_get_PREV(struct serialfc_port *port);
__u16 fscc_get_PDEV(struct serialfc_port *port);

#endif

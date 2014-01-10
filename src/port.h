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

#ifndef PORT_H
#define PORT_H

#include <linux/list.h>
#include <linux/cdev.h> /* struct cdev */
#include <linux/version.h> /* LINUX_VERSION_CODE, KERNEL_VERSION */

#include "card.h"

struct serialfc_port {
	struct list_head list;
	dev_t dev_t;
	struct class *class;
	struct cdev cdev;
	struct device *device;
	char *name;
	void __iomem *addr;
	unsigned channel;
	struct serialfc_card *card;
	unsigned sample_rate;
	unsigned clock_rate;
	unsigned char ACR;
	unsigned tx_trigger;
	unsigned rx_trigger;
};

struct serialfc_port *serialfc_port_new(struct serialfc_card *card,
                                        unsigned channel,
                                        unsigned major_number,
                                        unsigned minor_number,
                                        void __iomem *addr,
                                        struct device *parent,
                                        struct class *class,
                                        struct file_operations *fops);
void serialfc_port_delete(struct serialfc_port *port);

#endif

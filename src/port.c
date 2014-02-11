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

#include <linux/slab.h>
#include <linux/pci.h> /* struct pci_dev */
#include <linux/module.h>

#include "port.h"
#include "serialfc.h"
#include "config.h"
#include "utils.h"
#include "sysfs.h"

extern unsigned fscc_enable_async;

struct serialfc_port *serialfc_port_new(struct serialfc_card *card, unsigned channel,
							unsigned major_number, unsigned minor_number,
                            void __iomem *addr, struct device *parent, struct class *class,
							struct file_operations *fops)
{
	struct serialfc_port *port = 0;
	unsigned default_clock_rate = 1843200;

	port = kmalloc(sizeof(*port), GFP_KERNEL);

	if (port == NULL) {
		printk(KERN_ERR DEVICE_NAME "kmalloc failed\n");
		return 0;
	}

	port->name = kmalloc(10, GFP_KERNEL);
	if (port->name == NULL) {
		kfree(port);

		printk(KERN_ERR DEVICE_NAME "kmalloc failed\n");
		return 0;
	}

	sprintf(port->name, "%s%u", DEVICE_NAME, minor_number);

	port->class = class;
	port->dev_t = MKDEV(major_number, minor_number);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
		port->device = device_create(port->class, parent, port->dev_t, port,
									 "%s", port->name);
	#else
		port->device = device_create(port->class, parent, port->dev_t, "%s",
									 port->name);

		dev_set_drvdata(port->device, port);
	#endif

#else
	class_device_create(port->class, 0, port->dev_t, port->device, "%s",
						port->name);
#endif

	if (port->device <= 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
		device_destroy(port->class, port->dev_t);
#endif

		if (port->name)
			kfree(port->name);

		kfree(port);

		printk(KERN_ERR DEVICE_NAME " %s: device_create failed\n", port->name);
		return 0;
	}

	/* The sysfs structures I use in sysfs.c don't work prior to 2.6.25 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)

	if (sysfs_create_group(&port->device->kobj, &port_settings_attr_group)) {
		dev_err(port->device, "sysfs_create_group\n");
		return 0;
	}

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25) */

	port->channel = channel;
	port->card = card;
	port->addr = addr;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
    case CARD_TYPE_FSCC:
        default_clock_rate = 18432000;
        break;

    case CARD_TYPE_PCIe:
        default_clock_rate = 125000000;
        break;

    default:
        default_clock_rate = 1843200;
    }

    fastcom_init_gpio(port);
    fastcom_init_triggers(port);

    fastcom_set_clock_rate(port, default_clock_rate);
    fastcom_set_rs485(port, DEFAULT_RS485);
    fastcom_set_sample_rate(port, DEFAULT_SAMPLE_RATE);
    fastcom_set_tx_trigger(port, DEFAULT_TX_TRIGGER_LEVEL);
    fastcom_set_rx_trigger(port, DEFAULT_RX_TRIGGER_LEVEL);
    fastcom_set_termination(port, DEFAULT_TERMINATION);
    fastcom_set_echo_cancel(port, DEFAULT_ECHO_CANCEL);
    fastcom_set_isochronous(port, DEFAULT_ISOCHRONOUS);
    fastcom_set_frame_length(port, DEFAULT_FRAME_LENGTH);

    if (fscc_enable_async && fastcom_get_card_type(port) == CARD_TYPE_FSCC)
        fscc_enable_async_mode(port);

	cdev_init(&port->cdev, fops);
	port->cdev.owner = THIS_MODULE;

	if (cdev_add(&port->cdev, port->dev_t, 1) < 0) {
		dev_err(port->device, "cdev_add failed\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
		device_destroy(port->class, port->dev_t);
#endif

		if (port->name)
			kfree(port->name);

		kfree(port);

		return 0;
	}

	if (fastcom_get_card_type(port) == CARD_TYPE_FSCC) {
		dev_info(port->device, "%s (%x.%02x)\n",
				 serialfc_card_get_name(port->card),
				 fscc_get_PREV(port), fscc_get_FREV(port));
	}
	else {
		dev_info(port->device, "%s\n", serialfc_card_get_name(port->card));
	}

	return port;
}

void serialfc_port_delete(struct serialfc_port *port)
{
	return_if_untrue(port);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
	device_destroy(port->class, port->dev_t);
#endif

	cdev_del(&port->cdev);

	if (port->name)
		kfree(port->name);

	kfree(port);
}


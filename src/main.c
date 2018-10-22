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

#include <linux/module.h> /* poll_wait, POLL* */
#include <linux/pci.h> /* struct pci_dev */
#include <linux/fs.h> /* struct file_operations */
#include <linux/version.h>
#include <asm/uaccess.h>

#include "card.h"
#include "utils.h"
#include "serialfc.h"
#include "utils.h"


static int serialfc_major_number;
static struct class *serialfc_class = 0;

unsigned fscc_enable_async = DEFAULT_FSCC_ASYNC_MODE;

struct serialfc_card *serialfc_card_find(struct pci_dev *pdev,
                                         struct list_head *card_list);

LIST_HEAD(serialfc_cards);

struct pci_device_id fc_id_table[] = {
	{ COMMTECH_VENDOR_ID, FC_422_2_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_8_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_8_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },

	{ COMMTECH_VENDOR_ID, FSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_104_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_232_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_104_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_4_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCCe_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_UA_CPCI_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCCe_4_LVDS_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_UA_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_UA_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCCe_4_UA_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

int serialfc_open(struct inode *inode, struct file *file)
{
	struct serialfc_port *current_port = 0;

	current_port = container_of(inode->i_cdev, struct serialfc_port, cdev);
	file->private_data = current_port;

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 11)
long serialfc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
int serialfc_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
			       unsigned long arg)
#endif
{
    struct ioctl_get_dev_info_struct l_dev_data;

    struct serialfc_port *port = 0;
	int error_code = 0;
	char clock_bits[20];
	unsigned int tmp=0;
	long unsigned int baud_rate=0;
	unsigned int clock_rate=0;
	port = file->private_data;

	switch (cmd) {
    case IOCTL_FASTCOM_ENABLE_RS485:
		fastcom_enable_rs485(port);
		break;

    case IOCTL_FASTCOM_DISABLE_RS485:
		fastcom_disable_rs485(port);
		break;

    case IOCTL_FASTCOM_GET_RS485:
		error_code = fastcom_get_rs485(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_ENABLE_ECHO_CANCEL:
		fastcom_enable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_DISABLE_ECHO_CANCEL:
		fastcom_disable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_GET_ECHO_CANCEL:
		fastcom_get_echo_cancel(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_ENABLE_TERMINATION:
		error_code = fastcom_enable_termination(port);
		break;

    case IOCTL_FASTCOM_DISABLE_TERMINATION:
		error_code = fastcom_disable_termination(port);
		break;

    case IOCTL_FASTCOM_GET_TERMINATION:
		error_code = fastcom_get_termination(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_SAMPLE_RATE:
		copy_from_user(&tmp, (unsigned *)arg, sizeof(tmp));
		error_code = fastcom_set_sample_rate(port, tmp);
		break;

    case IOCTL_FASTCOM_GET_SAMPLE_RATE:
		fastcom_get_sample_rate(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_TX_TRIGGER:
		copy_from_user(&tmp, (unsigned *)arg, sizeof(tmp));
		error_code = fastcom_set_tx_trigger(port, tmp);
		break;

    case IOCTL_FASTCOM_GET_TX_TRIGGER:
		error_code = fastcom_get_tx_trigger(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_RX_TRIGGER:
		copy_from_user(&tmp, (unsigned *)arg, sizeof(tmp));
		error_code = fastcom_set_rx_trigger(port, tmp);
		break;

    case IOCTL_FASTCOM_GET_RX_TRIGGER:
		error_code = fastcom_get_rx_trigger(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_CLOCK_RATE:
		copy_from_user(&clock_rate, (unsigned *)arg, sizeof(clock_rate));
		error_code = fastcom_set_clock_rate(port, clock_rate);
		break;

    case IOCTL_FASTCOM_SET_CLOCK_BITS:
		copy_from_user(clock_bits, (char *)arg, 20);
		error_code = fastcom_set_clock_bits(port, clock_bits);
		break;

    case IOCTL_FASTCOM_ENABLE_ISOCHRONOUS:
		error_code = fastcom_enable_isochronous(port, (unsigned)arg);
		break;

    case IOCTL_FASTCOM_DISABLE_ISOCHRONOUS:
		error_code = fastcom_disable_isochronous(port);
		break;

    case IOCTL_FASTCOM_GET_ISOCHRONOUS:
		error_code = fastcom_get_isochronous(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT:
		error_code = fastcom_enable_external_transmit(port, (unsigned)arg);
		break;

    case IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT:
		error_code = fastcom_disable_external_transmit(port);
		break;

    case IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT:
		error_code = fastcom_get_external_transmit(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_FRAME_LENGTH:
		copy_from_user(&tmp, (unsigned *)arg, sizeof(tmp));
		error_code = fastcom_set_frame_length(port, tmp);
		break;

    case IOCTL_FASTCOM_GET_FRAME_LENGTH:
		error_code = fastcom_get_frame_length(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_GET_CARD_TYPE:
		tmp = fastcom_get_card_type(port);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_ENABLE_9BIT:
		error_code = fastcom_enable_9bit(port);
		break;

    case IOCTL_FASTCOM_DISABLE_9BIT:
		error_code = fastcom_disable_9bit(port);
		break;

    case IOCTL_FASTCOM_GET_9BIT:
		error_code = fastcom_get_9bit(port, &tmp);
		copy_to_user((unsigned *)arg, &tmp, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_SET_BAUD_RATE:
		copy_from_user(&baud_rate, (unsigned *)arg, sizeof(baud_rate));
		error_code = fastcom_set_baud_rate(port, baud_rate);
		break;

    case IOCTL_FASTCOM_GET_BAUD_RATE:
		error_code = fastcom_get_baud_rate(port, &baud_rate);
		copy_to_user((unsigned *)arg, &baud_rate, sizeof(tmp));
		break;

    case IOCTL_FASTCOM_ENABLE_FIXED_BAUD_RATE:
    case IOCTL_FASTCOM_DISABLE_FIXED_BAUD_RATE:
    case IOCTL_FASTCOM_GET_FIXED_BAUD_RATE:
        error_code = -EPROTONOSUPPORT;
        break;

    case IOCTL_FASTCOM_GET_DEV_INFO:
            /* gather device data and pass back to user */
            l_dev_data.vendor = port->card->pci_dev->vendor;
            l_dev_data.device = port->card->pci_dev->device;
            l_dev_data.bus    = port->card->pci_dev->bus->number;

            if (port->card->pci_dev->slot == NULL)
                l_dev_data.slot = 0; /* nothing else to do */
            else
                l_dev_data.slot = port->card->pci_dev->slot->number;

            if (copy_to_user((void *)arg, &l_dev_data, sizeof(l_dev_data))) {
	            error_code = -ENODATA; // Report error
                printk(KERN_ERR DEVICE_NAME " ioctl get-dev-info failed copy_to_user\n");
            }
            break;

	default:
		dev_dbg(port->device, "unknown ioctl 0x%x\n", cmd);
		return -ENOTTY;
	}

	return error_code;
}

static struct file_operations serialfc_fops = {
	.owner = THIS_MODULE,
	.open = serialfc_open,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 11)
	.unlocked_ioctl = serialfc_ioctl,
#else
	.ioctl = serialfc_ioctl,
#endif
};

#if 0
static int fc_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct serialfc_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	new_card = serialfc_card_new(pdev, serialfc_major_number, serialfc_class,
						         &serialfc_fops);

	if (new_card)
		list_add_tail(&new_card->list, &serialfc_cards);

	return 0;
}
#endif

static void fc_remove(struct pci_dev *pdev)
{
	struct serialfc_card *card = 0;

	card = serialfc_card_find(pdev, &serialfc_cards);

	if (card == 0)
		return;

	list_del(&card->list);
	serialfc_card_delete(card);

	pci_disable_device(pdev);
}

struct pci_driver serialfc_pci_driver = {
	.name = DEVICE_NAME,
	//.probe = fc_probe,
	.remove = fc_remove,
	.id_table = fc_id_table,
};

static int __init serialfc_init(void)
{
	unsigned error_code = 0;

	serialfc_class = class_create(THIS_MODULE, DEVICE_NAME);

	if (IS_ERR(serialfc_class)) {
		printk(KERN_ERR DEVICE_NAME " class_create failed\n");
		return PTR_ERR(serialfc_class);
	}

	serialfc_major_number = error_code = register_chrdev(0, "serialfc", &serialfc_fops);

	if (serialfc_major_number < 0) {
		printk(KERN_ERR DEVICE_NAME " register_chrdev failed\n");
		class_destroy(serialfc_class);
		return error_code;
	}

	error_code = pci_register_driver(&serialfc_pci_driver);

	if (error_code < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		unregister_chrdev(serialfc_major_number, "serialfc");
		class_destroy(serialfc_class);
		return error_code;
	}

	if (error_code == 0) {
		struct pci_dev *pdev = NULL;
        unsigned num_devices = 0;
        struct serialfc_card *new_card = 0;

		pdev = pci_get_device(COMMTECH_VENDOR_ID, PCI_ANY_ID, pdev);

		while (pdev != NULL) {
			if (is_serialfc_device(pdev)) {
				++num_devices;

			    if (pci_enable_device(pdev))
				    return -EIO;

			    new_card = serialfc_card_new(pdev, serialfc_major_number,
			                                 serialfc_class, &serialfc_fops);

			    if (new_card)
				    list_add_tail(&new_card->list, &serialfc_cards);
	        }

			pdev = pci_get_device(COMMTECH_VENDOR_ID, PCI_ANY_ID, pdev);
		}

		if (num_devices == 0) {
			pci_unregister_driver(&serialfc_pci_driver);
		    unregister_chrdev(serialfc_major_number, "serialfc");
		    class_destroy(serialfc_class);
			return -ENODEV;
		}
	}

	return 0;
}

static void __exit serialfc_exit(void)
{
	struct list_head *current_node = 0;
	struct list_head *temp_node = 0;

	list_for_each_safe(current_node, temp_node, &serialfc_cards) {
		struct serialfc_card *current_card = 0;

		current_card = list_entry(current_node, struct serialfc_card, list);

		list_del(current_node);
		serialfc_card_delete(current_card);
	}

	pci_unregister_driver(&serialfc_pci_driver);
	unregister_chrdev(serialfc_major_number, DEVICE_NAME);
	class_destroy(serialfc_class);
}

struct serialfc_card *serialfc_card_find(struct pci_dev *pdev,
                                         struct list_head *card_list)
{
	struct serialfc_card *current_card = 0;

	return_val_if_untrue(pdev, 0);
	return_val_if_untrue(card_list, 0);

	list_for_each_entry(current_card, card_list, list) {
		if (current_card->pci_dev == pdev)
			return current_card;
	}

	return 0;
}

MODULE_LICENSE("GPL");
MODULE_VERSION("2.3.2");
MODULE_AUTHOR("William Fagan <willf@commtech-fastcom.com>");

MODULE_DESCRIPTION("Registers the UARTs on the async series of Commtech cards "\
                   "with the serial driver.");

module_param(fscc_enable_async, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(fscc_enable_async, 
    "Enable asynchronous mode on FSCC cards when loaded (removes fscc-linux " \
    "requirement).");

module_init(serialfc_init);
module_exit(serialfc_exit);

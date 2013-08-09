/*
	Copyright (C) 2012  Commtech, Inc.

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

#include "card.h"
#include "utils.h"
#include "serialfc.h"
#include "utils.h"


static int serialfc_major_number;
static struct class *serialfc_class = 0;

struct fc_card *fc_card_find(struct pci_dev *pdev,
                                   struct list_head *card_list);

LIST_HEAD(fc_cards);

struct pci_device_id fc_id_table[] = {
	{ COMMTECH_VENDOR_ID, FC_422_2_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_8_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_8_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

int serialfc_open(struct inode *inode, struct file *file)
{
	struct fc_port *current_port = 0;

	current_port = container_of(inode->i_cdev, struct fc_port, cdev);
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
    struct fc_port *port = 0;
	int error_code = 0;

	port = file->private_data;

	switch (cmd) {
    case IOCTL_FASTCOM_ENABLE_RS485:
		fastcom_enable_rs485(port);
		break;

    case IOCTL_FASTCOM_DISABLE_RS485:
		fastcom_disable_rs485(port);
		break;

    case IOCTL_FASTCOM_GET_RS485:
		error_code = fastcom_get_rs485(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_ENABLE_ECHO_CANCEL:
		fastcom_enable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_DISABLE_ECHO_CANCEL:
		fastcom_disable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_GET_ECHO_CANCEL:
		fastcom_get_echo_cancel(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_ENABLE_TERMINATION:
		fastcom_enable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_DISABLE_TERMINATION:
		fastcom_disable_echo_cancel(port);
		break;

    case IOCTL_FASTCOM_GET_TERMINATION:
		error_code = fastcom_get_termination(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_SET_SAMPLE_RATE:
		error_code = fastcom_set_sample_rate(port, (int)arg);
		break;

    case IOCTL_FASTCOM_GET_SAMPLE_RATE:
		fastcom_get_sample_rate(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_SET_TX_TRIGGER:
		error_code = fastcom_set_tx_trigger(port, (int)arg);
		break;

    case IOCTL_FASTCOM_GET_TX_TRIGGER:
		error_code = fastcom_get_tx_trigger(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_SET_RX_TRIGGER:
		error_code = fastcom_set_rx_trigger(port, (int)arg);
		break;

    case IOCTL_FASTCOM_GET_RX_TRIGGER:
		error_code = fastcom_get_rx_trigger(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_SET_CLOCK_RATE:
		error_code = fastcom_set_clock_rate(port, (int)arg);
		break;

    case IOCTL_FASTCOM_ENABLE_ISOCHRONOUS:
		error_code = fastcom_enable_isochronous(port, (int)arg);
		break;

    case IOCTL_FASTCOM_DISABLE_ISOCHRONOUS:
		error_code = fastcom_disable_isochronous(port);
		break;

    case IOCTL_FASTCOM_GET_ISOCHRONOUS:
		error_code = fastcom_get_isochronous(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT:
		error_code = fastcom_enable_external_transmit(port, (int)arg);
		break;

    case IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT:
		error_code = fastcom_disable_external_transmit(port);
		break;

    case IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT:
		error_code = fastcom_get_external_transmit(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_SET_FRAME_LENGTH:
		error_code = fastcom_set_frame_length(port, (int)arg);
		break;

    case IOCTL_FASTCOM_GET_FRAME_LENGTH:
		error_code = fastcom_get_frame_length(port, (unsigned *)arg);
		break;

    case IOCTL_FASTCOM_GET_CARD_TYPE:
		*(unsigned *)arg = fastcom_get_card_type(port);
		break;

	default:
		dev_dbg(port->device, "unknown ioctl 0x%x\n", cmd);
		return -ENOTTY;
	}

	return 0;
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

static int fc_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *id)
{
	struct fc_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	new_card = fc_card_new(pdev, serialfc_major_number, serialfc_class,
						   &serialfc_fops);

	if (new_card)
		list_add_tail(&new_card->list, &fc_cards);

	return 0;
}

static void fc_remove(struct pci_dev *pdev)
{
	struct fc_card *card = 0;

	card = fc_card_find(pdev, &fc_cards);

	if (card == 0)
		return;

	list_del(&card->list);
	fc_card_delete(card);

	pci_disable_device(pdev);
}

struct pci_driver fc_pci_driver = {
	.name = DEVICE_NAME,
	.probe = fc_probe,
	.remove = fc_remove,
	.id_table = fc_id_table,
};

static int __init fc_init(void)
{
	unsigned error_code = 0;
	unsigned num_devices = 0;
	struct fc_card *new_card = 0;

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

	error_code = pci_register_driver(&fc_pci_driver);

	if (error_code < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		unregister_chrdev(serialfc_major_number, "serialfc");
		class_destroy(serialfc_class);
		return error_code;
	}
//#if 0
    //Temporary since probe doesn't get called above 3.8
	if (error_code == 0) {
		struct pci_dev *pdev = NULL;

		pdev = pci_get_device(COMMTECH_VENDOR_ID, PCI_ANY_ID, pdev);

		if (pci_enable_device(pdev))
			return -EIO;

		new_card = fc_card_new(pdev, serialfc_major_number, serialfc_class,
						   &serialfc_fops);

		if (new_card)
			list_add_tail(&new_card->list, &fc_cards);

		num_devices = 1;
/*
		while (pdev != NULL) {
			if (is_serialfc_device(pdev))
				++num_devices;

			pdev = pci_get_device(COMMTECH_VENDOR_ID, PCI_ANY_ID, pdev);

			if (pci_enable_device(pdev))
				return -EIO;

			new_card = fc_card_new(pdev);

			if (new_card)
				list_add_tail(&new_card->list, &fc_cards);
		}
*/
		if (num_devices == 0) {
			pci_unregister_driver(&fc_pci_driver);
			return -ENODEV;
		}
	}
//#endif
	return 0;
}

static void __exit fc_exit(void)
{
	struct list_head *current_node = 0;
	struct list_head *temp_node = 0;

	list_for_each_safe(current_node, temp_node, &fc_cards) {
		struct fc_card *current_card = 0;

		current_card = list_entry(current_node, struct fc_card, list);

		list_del(current_node);
		fc_card_delete(current_card);
	}



	pci_unregister_driver(&fc_pci_driver);
	unregister_chrdev(serialfc_major_number, DEVICE_NAME);
	class_destroy(serialfc_class);
}

struct fc_card *fc_card_find(struct pci_dev *pdev,
                                  struct list_head *card_list)
{
	struct fc_card *current_card = 0;

	return_val_if_untrue(pdev, 0);
	return_val_if_untrue(card_list, 0);

	list_for_each_entry(current_card, card_list, list) {
		if (current_card->pci_dev == pdev)
			return current_card;
	}

	return 0;
}

MODULE_LICENSE("GPL");
MODULE_VERSION("1.1.0");
MODULE_AUTHOR("William Fagan <willf@commtech-fastcom.com>");

MODULE_DESCRIPTION("Registers the UARTs on the async series of Commtech cards "\
                   "with the serial driver.");

module_init(fc_init);
module_exit(fc_exit);
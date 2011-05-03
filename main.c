/*
	Copyright (C) 2011  Commtech, Inc.

	This file is part of fscc-linux.

	fscc-linux is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	fscc-linux is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with fscc-linux.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <linux/module.h> /* poll_wait, POLL* */
#include <linux/pci.h> /* struct pci_dev */
#include <linux/fs.h> /* struct file_operations */
#include <linux/8250_pci.h> /* struct serial_private */

#define COMMTECH_VENDOR_ID 0x18f7

#define FC_422_2_335_ID 0x0004
#define FC_422_4_335_ID 0x0002
#define FC_232_4_335_ID 0x000a
#define FC_232_8_335_ID 0x000b

#define FSCC_ID 0x000f
#define SFSCC_ID 0x0014
#define FSCC_232_ID 0x0016
#define SFSCC_NO950_ID 0x0017	//SuperFSCC/104
#define SFSCC_4_ID 0x0018
#define SFSCC_LVDS_ID 0x001a
#define FSCC_4_ID 0x001b
#define SFSCC_4_LVDS_ID 0x001c
#define SFSCCe_4_ID 0x001e

#define DEVICE_NAME "fc-async"
#define FCR_OFFSET 0x00

#define MPIOSEL_OFFSET 0x93
#define MPIOINV_OFFSET 0x92
#define MPIO3T_OFFSET 0x91
#define MPIOLVL_OFFSET 0x90
#define MPIOINT_OFFSET 0x89

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

unsigned init = 1;

struct fscc_card {
	struct list_head list;
	struct pci_dev *pci_dev;
	struct serial_private *serial_priv;
	void __iomem *bar[3];
};

struct fc335_card {
	struct list_head list;
	struct pci_dev *pci_dev;
	struct serial_private *serial_priv;
	void __iomem *bar[1];
};

struct fscc_card *fscc_card_new(struct pci_dev *pdev);

void fscc_card_delete(struct fscc_card *card);

struct fscc_card *fscc_card_find(struct pci_dev *pdev,
                                 struct list_head *card_list);

struct fc335_card *fc335_card_new(struct pci_dev *pdev);

void fc335_card_delete(struct fc335_card *card);

struct fc335_card *fc335_card_find(struct pci_dev *pdev,
                                   struct list_head *card_list);

LIST_HEAD(fscc_cards);
LIST_HEAD(fc335_cards);

struct pci_device_id fscc_id_table[] __devinitdata = {
	{ COMMTECH_VENDOR_ID, FSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_232_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_NO950_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCCe_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

struct pci_device_id fc335_id_table[] __devinitdata = {
	{ COMMTECH_VENDOR_ID, FC_422_2_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_4_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_8_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

static int __devinit fscc_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *id)
{
	struct fscc_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	new_card = fscc_card_new(pdev);

	if (new_card)
		list_add_tail(&new_card->list, &fscc_cards);

	return 0;
}

static int __devinit fc335_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *id)
{
	struct fc335_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	new_card = fc335_card_new(pdev);

	if (new_card)
		list_add_tail(&new_card->list, &fc335_cards);

	return 0;
}

static void __devexit fscc_remove(struct pci_dev *pdev)
{
	struct fscc_card *card = 0;

	card = fscc_card_find(pdev, &fscc_cards);

	if (card == 0)
		return;

	list_del(&card->list);
	fscc_card_delete(card);

	pci_disable_device(pdev);
}

static void __devexit fc335_remove(struct pci_dev *pdev)
{
	struct fc335_card *card = 0;

	card = fc335_card_find(pdev, &fc335_cards);

	if (card == 0)
		return;

	list_del(&card->list);
	fc335_card_delete(card);

	pci_disable_device(pdev);
}

struct pci_driver fscc_pci_driver = {
	.name = "fscc-async",
	.probe = fscc_probe,
	.remove = fscc_remove,
	.id_table = fscc_id_table,
};

struct pci_driver fc335_pci_driver = {
	.name = "fc335-async",
	.probe = fc335_probe,
	.remove = fc335_remove,
	.id_table = fc335_id_table,
};

static int __init fc_async_init(void)
{
	unsigned error_code = 0;

	error_code = pci_register_driver(&fscc_pci_driver);

	if (error_code < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		return error_code;
	}

	error_code = pci_register_driver(&fc335_pci_driver);

	if (error_code < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		return error_code;
	}

	return 0;
}

static void __exit fc_async_exit(void)
{
	pci_unregister_driver(&fscc_pci_driver);
	pci_unregister_driver(&fc335_pci_driver);
}

struct pciserial_board fscc_pci_board = {
	.flags = FL_BASE1,
	.num_ports = 2,
	.base_baud = 1500000,
	.uart_offset = 0x8,
};

struct pciserial_board fc335_2_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 2,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct pciserial_board fc335_4_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 4,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct pciserial_board fc335_8_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 8,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct fscc_card *fscc_card_new(struct pci_dev *pdev)
{
	struct fscc_card *card = 0;
	unsigned i = 0;

	card = kmalloc(sizeof(*card), GFP_KERNEL);

	return_val_if_untrue(card != NULL, 0);

	INIT_LIST_HEAD(&card->list);

	card->pci_dev = pdev;

	/* This requests the pci regions for us. Doing so again will cause our
	   uarts not to appear correctly. */
	card->serial_priv = pciserial_init_ports(pdev, &fscc_pci_board);

	if (IS_ERR(card->serial_priv)) {
		dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		return 0;
	}

	pci_set_drvdata(pdev, card->serial_priv);

	for (i = 0; i < 3; i++) {
		card->bar[i] = pci_iomap(card->pci_dev, i, 0);

		if (card->bar[i] == NULL) {
			dev_err(&card->pci_dev->dev, "pci_iomap failed on bar #%i\n", i);
			return 0;
		}
	}

	if (init)
		iowrite32(0x03000000, card->bar[2] + FCR_OFFSET);

	return card;
}

struct fc335_card *fc335_card_new(struct pci_dev *pdev)
{
	struct fc335_card *card = 0;
	unsigned i = 0;

	card = kmalloc(sizeof(*card), GFP_KERNEL);

	return_val_if_untrue(card != NULL, 0);
	INIT_LIST_HEAD(&card->list);

	card->pci_dev = pdev;

	switch (pdev->device) {
	case FC_422_2_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc335_2_pci_board);
		break;

	case FC_422_4_335_ID:
	case FC_232_4_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc335_4_pci_board);
		break;

	case FC_232_8_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc335_8_pci_board);
		break;
	}

	if (IS_ERR(card->serial_priv)) {
		dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		return 0;
	}

	pci_set_drvdata(pdev, card->serial_priv);

	card->bar[0] = pci_iomap(card->pci_dev, 0, 0);

	if (card->bar[i] == NULL) {
		dev_err(&card->pci_dev->dev, "pci_iomap failed on bar #%i\n", i);
		return 0;
	}

	switch (pdev->device) {
	case FC_422_2_335_ID:
	case FC_422_4_335_ID:
		iowrite8(0x00, card->bar[0] + MPIOSEL_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIOINV_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIO3T_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIOINT_OFFSET);
		udelay(20);
		iowrite8(0x78, card->bar[0] + MPIOLVL_OFFSET);
		break;

	case FC_232_4_335_ID:
	case FC_232_8_335_ID:
		iowrite8(0xc0, card->bar[0] + MPIOSEL_OFFSET);
		udelay(20);
		iowrite8(0xc0, card->bar[0] + MPIOINV_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIO3T_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIOINT_OFFSET);
		udelay(20);
		iowrite8(0x00, card->bar[0] + MPIOLVL_OFFSET);
		break;
	}

	return card;
}

void fscc_card_delete(struct fscc_card *card)
{
	return_if_untrue(card);

	pciserial_remove_ports(card->serial_priv);
	pci_set_drvdata(card->pci_dev, NULL);

	kfree(card);
}

void fc335_card_delete(struct fc335_card *card)
{
	return_if_untrue(card);

	pciserial_remove_ports(card->serial_priv);
	pci_set_drvdata(card->pci_dev, NULL);

	kfree(card);
}

struct fscc_card *fscc_card_find(struct pci_dev *pdev,
                                 struct list_head *card_list)
{
	struct fscc_card *current_card = 0;

	return_val_if_untrue(pdev, 0);
	return_val_if_untrue(card_list, 0);

	list_for_each_entry(current_card, card_list, list) {
		if (current_card->pci_dev == pdev)
			return current_card;
	}

	return 0;
}

struct fc335_card *fc335_card_find(struct pci_dev *pdev,
                                  struct list_head *card_list)

{
	struct fc335_card *current_card = 0;

	return_val_if_untrue(pdev, 0);
	return_val_if_untrue(card_list, 0);

	list_for_each_entry(current_card, card_list, list) {
		if (current_card->pci_dev == pdev)
			return current_card;
	}

	return 0;
}

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.1");
MODULE_AUTHOR("William Fagan <willf@commtech-fastcom.com>");

MODULE_DESCRIPTION("Registers the UARTs on the FSCC and 335 series of Commtech "\
                    "cards with the serial driver.");

module_param(init, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(init, "Whether or not to initialize FSCC ports for "\
                        "asynchronous communication (FCR = 0x03000000).");

module_init(fc_async_init);
module_exit(fc_async_exit);

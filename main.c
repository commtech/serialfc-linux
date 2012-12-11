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
#include <linux/8250_pci.h> /* struct serial_private */

#define COMMTECH_VENDOR_ID 0x18f7

#define FC_422_2_PCI_335_ID 0x0004
#define FC_422_4_PCI_335_ID 0x0002
#define FC_232_4_PCI_335_ID 0x000a
#define FC_232_8_PCI_335_ID 0x000b
#define FC_422_4_PCIe_ID 0x0020
#define FC_422_8_PCIe_ID 0x0021

#define DEVICE_NAME "serialfc"

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
#define UART_EXAR_TXTRG 0x0a /* Tx FIFO trigger level write-only */
#define UART_EXAR_RXTRG 0x0b /* Rx FIFO trigger level write-only */
#define UART_EXAR_FCTR 0x08 /* Feature Control Register */

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

struct fc_card {
	struct list_head list;
	struct pci_dev *pci_dev;
	struct serial_private *serial_priv;
	void __iomem *addr;
};

struct fc_card *fc_pci_card_new(struct pci_dev *pdev);
struct fc_card *fc_pcie_card_new(struct pci_dev *pdev);

void fc_card_delete(struct fc_card *card);

struct fc_card *fc_card_find(struct pci_dev *pdev,
                                   struct list_head *card_list);

LIST_HEAD(fc_cards);

struct pci_device_id fc_id_table[] __devinitdata = {
	{ COMMTECH_VENDOR_ID, FC_422_2_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_4_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_232_8_PCI_335_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_4_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FC_422_8_PCIe_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

static int __devinit fc_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *id)
{
	struct fc_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	switch (pdev->device) {
	case FC_422_2_PCI_335_ID:
	case FC_422_4_PCI_335_ID:
	case FC_232_4_PCI_335_ID:
	case FC_232_8_PCI_335_ID:
		new_card = fc_pci_card_new(pdev);
		break;

	case FC_422_4_PCIe_ID:
	case FC_422_8_PCIe_ID:
		new_card = fc_pcie_card_new(pdev);
		break;
	}

	if (new_card)
		list_add_tail(&new_card->list, &fc_cards);

	return 0;
}

static void __devexit fc_remove(struct pci_dev *pdev)
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

	error_code = pci_register_driver(&fc_pci_driver);

	if (error_code < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		return error_code;
	}

	return 0;
}

static void __exit fc_exit(void)
{
	pci_unregister_driver(&fc_pci_driver);
}

struct pciserial_board fc_2_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 2,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct pciserial_board fc_4_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 4,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct pciserial_board fc_8_pci_board = {
	.flags = FL_BASE0,
	.num_ports = 8,
	.base_baud = 1152000,
	.uart_offset = 0x200,
};

struct pciserial_board fc_2_pcie_board = {
	.flags = FL_BASE0,
	.num_ports = 2,
	.base_baud = 7812500,
	.uart_offset = 0x400,
	.reg_shift = 0,
	.first_offset = 0,
};

struct pciserial_board fc_4_pcie_board = {
	.flags = FL_BASE0,
	.num_ports = 4,
	.base_baud = 7812500,
	.uart_offset = 0x400,
	.reg_shift = 0,
	.first_offset = 0,
};

struct pciserial_board fc_8_pcie_board = {
	.flags = FL_BASE0,
	.num_ports = 8,
	.base_baud = 7812500,
	.uart_offset = 0x400,
	.reg_shift = 0,
	.first_offset = 0,
};


struct fc_card *fc_pci_card_new(struct pci_dev *pdev)
{
	struct fc_card *card = 0;

	card = kmalloc(sizeof(*card), GFP_KERNEL);

	return_val_if_untrue(card != NULL, 0);
	INIT_LIST_HEAD(&card->list);

	card->pci_dev = pdev;

	switch (pdev->device) {
	case FC_422_2_PCI_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc_2_pci_board);
		break;

	case FC_422_4_PCI_335_ID:
	case FC_232_4_PCI_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc_4_pci_board);
		break;

	case FC_232_8_PCI_335_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc_8_pci_board);
		break;
	}

	if (IS_ERR(card->serial_priv)) {
		dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		return 0;
	}

	pci_set_drvdata(pdev, card->serial_priv);

	card->addr = pci_iomap(card->pci_dev, 0, 0);

	if (card->addr == NULL) {
		dev_err(&card->pci_dev->dev, "pci_iomap failed\n");
		return 0;
	}

	switch (pdev->device) {
	case FC_422_2_PCI_335_ID:
	case FC_422_4_PCI_335_ID:
		iowrite8(0x00, card->addr + MPIOSEL_OFFSET);
		iowrite8(0x00, card->addr + MPIOINV_OFFSET);
		iowrite8(0x78, card->addr + MPIOLVL_OFFSET);
		iowrite8(0x00, card->addr + MPIOOD_OFFSET);
		break;

	case FC_232_4_PCI_335_ID:
	case FC_232_8_PCI_335_ID:
		iowrite8(0xc0, card->addr + MPIOSEL_OFFSET);
		iowrite8(0xc0, card->addr + MPIOINV_OFFSET);
		iowrite8(0x00, card->addr + MPIOLVL_OFFSET);
		iowrite8(0x00, card->addr + MPIOOD_OFFSET);
		break;
	}

	iowrite8(0x00, card->addr + MPIO3T_OFFSET);
	iowrite8(0x00, card->addr + MPIOINT_OFFSET);

	iowrite8(0x00, card->addr + UART_EXAR_8XMODE);
	iowrite8(0xc0, card->addr + UART_EXAR_FCTR);
	iowrite8(32, card->addr + UART_EXAR_TXTRG);
	iowrite8(32, card->addr + UART_EXAR_RXTRG);

	return card;
}

struct fc_card *fc_pcie_card_new(struct pci_dev *pdev)
{
	struct fc_card *card = 0;
	unsigned i = 0;

	card = kmalloc(sizeof(*card), GFP_KERNEL);

	return_val_if_untrue(card != NULL, 0);
	INIT_LIST_HEAD(&card->list);

	card->pci_dev = pdev;

	switch (pdev->device) {
	case FC_422_4_PCIe_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc_4_pcie_board);
		break;

	case FC_422_8_PCIe_ID:
		card->serial_priv = pciserial_init_ports(pdev, &fc_8_pcie_board);
		break;
	}

	if (IS_ERR(card->serial_priv)) {
		dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		return 0;
	}

	pci_set_drvdata(pdev, card->serial_priv);

	card->addr = pci_iomap(card->pci_dev, 0, 0);

	if (card->addr == NULL) {
		dev_err(&card->pci_dev->dev, "pci_iomap failed\n");
		return 0;
	}

	for (i = MPIOINT_OFFSET; i <= MPIOODH_OFFSET; i++)
		iowrite8(0x00, card->addr + i);

	iowrite8(0x00, card->addr + UART_EXAR_8XMODE);
	iowrite8(0xc0, card->addr + UART_EXAR_FCTR);
	iowrite8(128, card->addr + UART_EXAR_TXTRG);
	iowrite8(128, card->addr + UART_EXAR_RXTRG);

	return card;
}

void fc_card_delete(struct fc_card *card)
{
	return_if_untrue(card);

	pciserial_remove_ports(card->serial_priv);
	pci_set_drvdata(card->pci_dev, NULL);

	kfree(card);
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

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

#include "card.h"
#include "utils.h"
#include "serialfc.h"


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

static int fc_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *id)
{
	struct fc_card *new_card = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	new_card = fc_card_new(pdev);

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

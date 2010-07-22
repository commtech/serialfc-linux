/*
	Copyright (C) 2010  Commtech, Inc.
	
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

#define FSCC_ID 0x000f
#define SFSCC_ID 0x0014
#define FSCC_232_ID 0x0016
#define SFSCC_4_ID 0x0018
#define FSCC_4_ID 0x001b
#define SFSCC_4_LVDS_ID 0x001c
#define SFSCCe_4_ID 0x001e

#define DEVICE_NAME "fscc"

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

struct fscc_card {
	struct list_head list;
	struct pci_dev *pci_dev;
	struct serial_private *serial_priv;
};

struct fscc_card *fscc_card_new(struct pci_dev *pdev,
                                const struct pci_device_id *id);
                                
void fscc_card_delete(struct fscc_card *card);

struct fscc_card *fscc_card_find(struct pci_dev *pdev, 
                                 struct list_head *card_list);

LIST_HEAD(fscc_cards);

struct pci_device_id fscc_id_table[] __devinitdata = {
	{ COMMTECH_VENDOR_ID, FSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_232_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, FSCC_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCC_4_LVDS_ID, PCI_ANY_ID, 0, 0, 0 },
	{ COMMTECH_VENDOR_ID, SFSCCe_4_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0, },
};

static int __devinit fscc_probe(struct pci_dev *pdev, 
                                const struct pci_device_id *id)
{
	struct fscc_card *new_card = 0;
	
	if (pci_enable_device(pdev))
		return -EIO;
	
	switch (id->device) {
	case FSCC_ID:			
	case FSCC_232_ID:
	case FSCC_4_ID:
	case SFSCC_ID:
	case SFSCC_4_ID:
	case SFSCC_4_LVDS_ID:
	case SFSCCe_4_ID:		
		new_card = fscc_card_new(pdev, id);
		
		if (new_card)                         
			list_add_tail(&new_card->list, &fscc_cards);
				
		break;
			
	default:
		printk(KERN_DEBUG DEVICE_NAME " unknown device\n");
	}

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

struct pci_driver fscc_pci_driver = {
	.name = "fscc",
	.probe = fscc_probe,
	.remove = fscc_remove,
	.id_table = fscc_id_table,
};

static int __init fscc_init(void)
{
	unsigned err;
	
	err = pci_register_driver(&fscc_pci_driver);
	
	if (err < 0) {
		printk(KERN_ERR DEVICE_NAME " pci_register_driver failed");
		return err;
	}
	
	return 0;
}

static void __exit fscc_exit(void)
{
	pci_unregister_driver(&fscc_pci_driver);
}

struct pciserial_board pci_board = {
	.flags = FL_BASE1,
	.num_ports = 2,
	.base_baud = 921600,
	.uart_offset = 8,
};

struct fscc_card *fscc_card_new(struct pci_dev *pdev,
                                const struct pci_device_id *id)
{
	struct fscc_card *card = 0;
	
	card = kmalloc(sizeof(*card), GFP_KERNEL);
	
	return_val_if_untrue(card != NULL, 0);
	
	INIT_LIST_HEAD(&card->list);
	
	card->pci_dev = pdev;
	
	/* This requests the pci regions for us. Doing so again will cause our
	   uarts not to appear correctly. */
	card->serial_priv = pciserial_init_ports(pdev, &pci_board);

	if (IS_ERR(card->serial_priv)) {
		dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		return 0;
	}
	
	pci_set_drvdata(pdev, card->serial_priv);
	
	return card;
}

void fscc_card_delete(struct fscc_card *card)
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

MODULE_DEVICE_TABLE(pci, fscc_id_table);

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_AUTHOR("William Fagan <willf@commtech-fastcom.com>");

MODULE_DESCRIPTION("Registers the async ports with the serial driver for the FSCC series of cards."); 

module_init(fscc_init);
module_exit(fscc_exit); 
 

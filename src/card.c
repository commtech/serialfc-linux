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
#include <linux/version.h> /* poll_wait, POLL* */

#include "card.h"
#include "port.h"
#include "utils.h"
#include "serialfc.h"

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

struct pciserial_board fscc_board = {
	.flags = FL_BASE1,
	.num_ports = 2,
	.base_baud = 1152000,
	.uart_offset = 8,
};

struct serialfc_card *serialfc_card_new(struct pci_dev *pdev,
                                        unsigned major_number,
							            struct class *class,
							            struct file_operations *fops)
{
	struct serialfc_card *card = 0;
	struct serialfc_port *port_iter = 0;
	struct pciserial_board *board = 0;
	static unsigned minor_number = 0;
	unsigned i = 0;

	card = kmalloc(sizeof(*card), GFP_KERNEL);

	return_val_if_untrue(card != NULL, 0);

	INIT_LIST_HEAD(&card->list);
	INIT_LIST_HEAD(&card->ports);

	card->pci_dev = pdev;

	switch (pdev->device) {
	case FC_422_2_PCI_335_ID:
		board = &fc_2_pci_board;
		break;

	case FC_422_4_PCI_335_ID:
	case FC_232_4_PCI_335_ID:
		board = &fc_4_pci_board;
		break;

	case FC_232_8_PCI_335_ID:
		board = &fc_8_pci_board;
		break;

	case FC_422_4_PCIe_ID:
		board = &fc_4_pcie_board;
		break;

	case FC_422_8_PCIe_ID:
		board = &fc_8_pcie_board;
		break;

	case FSCC_ID:
	case SFSCC_ID:
	case SFSCC_104_LVDS_ID:
	case FSCC_232_ID:
	case SFSCC_104_UA_ID:
	case SFSCC_4_UA_ID:
	case SFSCC_UA_ID:
	case SFSCC_LVDS_ID:
	case FSCC_4_UA_ID:
	case SFSCC_4_LVDS_ID:
	case FSCC_UA_ID:
	case SFSCCe_4_ID:
	case SFSCC_4_UA_CPCI_ID:
	case SFSCC_4_UA_LVDS_ID:
	case SFSCC_UA_LVDS_ID:
	case FSCCe_4_UA_ID:
		board = &fscc_board;
		break;
	}

	card->serial_priv = 0;

	if (fastcom_get_card_type2(card) == CARD_TYPE_FSCC) {
	    card->serial_priv = pciserial_init_ports(pdev, board);

	    if (IS_ERR(card->serial_priv)) {
		    dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		    return 0;
	    }
	}
	else {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
	    card->serial_priv = pciserial_init_ports(pdev, board);

	    if (IS_ERR(card->serial_priv)) {
		    dev_err(&card->pci_dev->dev, "pciserial_init_ports failed\n");
		    return 0;
	    }
#endif
    }

	if (fastcom_get_card_type2(card) == CARD_TYPE_FSCC)
	    card->addr = pci_iomap(card->pci_dev, 1, 0);
	else
	    card->addr = pci_iomap(card->pci_dev, 0, 0);

	if (card->addr == NULL) {
		dev_err(&card->pci_dev->dev, "pci_iomap failed\n");
		return 0;
	}

	if (fastcom_get_card_type2(card) == CARD_TYPE_FSCC) {
	    card->bar0 = pci_iomap(card->pci_dev, 0, 0);

	    if (card->bar0 == NULL) {
		    dev_err(&card->pci_dev->dev, "pci_iomap failed\n");
		    return 0;
	    }

	    card->bar2 = pci_iomap(card->pci_dev, 2, 0);

	    if (card->bar2 == NULL) {
		    dev_err(&card->pci_dev->dev, "pci_iomap failed\n");
		    return 0;
	    }
	}

	/* There are two ports per card. */
	for (i = 0; i < board->num_ports; i++) {
		port_iter = serialfc_port_new(card, i, major_number, minor_number,
		                        card->addr + (board->uart_offset * i),
		                        &card->pci_dev->dev, class, fops);

		if (port_iter)
			list_add_tail(&port_iter->list, &card->ports);

		minor_number += 1;
	}

	return card;
}

void serialfc_card_delete(struct serialfc_card *card)
{
	struct list_head *current_node = 0;
	struct list_head *temp_node = 0;

	return_if_untrue(card);

	list_for_each_safe(current_node, temp_node, &card->ports) {
		struct serialfc_port *current_port = 0;

		current_port = list_entry(current_node, struct serialfc_port, list);

		list_del(current_node);
		serialfc_port_delete(current_port);
	}

	if (card->serial_priv)
	    pciserial_remove_ports(card->serial_priv);

	kfree(card);
}

char *serialfc_card_get_name(struct serialfc_card *card)
{
	switch (card->pci_dev->device) {
    case FC_422_2_PCI_335_ID:
		return "422/2-PCI-335";
    case FC_422_4_PCI_335_ID:
		return "422/4-PCI-335";
    case FC_232_4_PCI_335_ID:
		return "232/4-PCI-335";
    case FC_232_8_PCI_335_ID:
		return "232/8-PCI-335";
    case FC_422_4_PCIe_ID:
		return "422/4-PCIe";
    case FC_422_8_PCIe_ID:
		return "422/8-PCIe";
	case FSCC_ID:
	case FSCC_UA_ID:
		return "FSCC PCI";
	case SFSCC_ID:
	case SFSCC_UA_ID:
		return "SuperFSCC PCI";
	case SFSCC_104_LVDS_ID:
		return "SuperFSCC-104-LVDS PC/104+";
	case FSCC_232_ID:
		return "FSCC-232 PCI";
	case SFSCC_104_UA_ID:
		return "SuperFSCC-104 PC/104+";
	case SFSCC_4_UA_ID:
		return "SuperFSCC/4 PCI";
	case SFSCC_LVDS_ID:
	case SFSCC_UA_LVDS_ID:
		return "SuperFSCC-LVDS PCI";
	case FSCC_4_UA_ID:
		return "FSCC/4 PCI";
	case SFSCC_4_LVDS_ID:
	case SFSCC_4_UA_LVDS_ID:
		return "SuperFSCC/4-LVDS PCI";
	case SFSCCe_4_ID:
		return "SuperFSCC/4 PCIe";
	case SFSCC_4_CPCI_ID:
	case SFSCC_4_UA_CPCI_ID:
		return "SuperFSCC/4 cPCI";
	case FSCCe_4_UA_ID:
		return "FSCC/4 PCIe";
	default:
		return "Unknown Device";
	}

}

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

#ifndef CARD_H
#define CARD_H

#include <linux/list.h>
#include <linux/fs.h> /* struct file_operations */
#include <linux/8250_pci.h> /* struct serial_private */

struct serialfc_card {
	struct list_head list;
	struct list_head ports;
	struct pci_dev *pci_dev;
	struct serial_private *serial_priv;
	void __iomem *addr;

	/* fscc only */
	void __iomem *bar0;
	void __iomem *bar2;
};

struct serialfc_card *serialfc_card_new(struct pci_dev *pdev,
                                        unsigned major_number,
							            struct class *class,
							            struct file_operations *fops);
void serialfc_card_delete(struct serialfc_card *card);
char *serialfc_card_get_name(struct serialfc_card *card);

#endif

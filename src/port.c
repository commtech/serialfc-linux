#include <linux/slab.h>
#include <linux/pci.h> /* struct pci_dev */
#include <linux/module.h>

#include "port.h"
#include "serialfc.h"
#include "config.h"
#include "utils.h"

struct serialfc_port *serialfc_port_new(struct serialfc_card *card, unsigned channel,
							unsigned major_number, unsigned minor_number,
                            void __iomem *addr, struct device *parent, struct class *class,
							struct file_operations *fops)
{
	struct serialfc_port *port = 0;
	unsigned default_clock_rate = 1843200;
	unsigned i = 0;

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

    switch (port->card->pci_dev->device) {
    case FC_422_2_PCI_335_ID:
    case FC_422_4_PCI_335_ID:
        iowrite8(0x00, port->addr + MPIOSEL_OFFSET);
        iowrite8(0x00, port->addr + MPIOINV_OFFSET);
        iowrite8(0x00, port->addr + MPIOOD_OFFSET);
        iowrite8(0x00, port->addr + MPIO3T_OFFSET);
        iowrite8(0x00, port->addr + MPIOINT_OFFSET);
        break;

    case FC_232_4_PCI_335_ID:
    case FC_232_8_PCI_335_ID:
        iowrite8(0xc0, port->addr + MPIOSEL_OFFSET);
        iowrite8(0xc0, port->addr + MPIOINV_OFFSET);
        iowrite8(0x00, port->addr + MPIOOD_OFFSET);
        iowrite8(0x00, port->addr + MPIO3T_OFFSET);
        iowrite8(0x00, port->addr + MPIOINT_OFFSET);
        break;

    case FC_422_4_PCIe_ID:
    case FC_422_8_PCIe_ID:
        for (i = MPIOINT_OFFSET; i <= MPIOODH_OFFSET; i++)
            iowrite8(0x00, port->addr + i);
        break;
    }

    if (port->card->pci_dev->device >= 0x14 && port->card->pci_dev->device <= 0x1F) {
        unsigned char init_lcr;
        unsigned char init_fcr;

        init_lcr = 0x00;
        init_fcr = 0x01; /* Enable FIFO (combined with enhanced enables 950 mode) */

        iowrite8(init_lcr, port->addr + LCR_OFFSET);
        iowrite8(init_fcr, port->addr + FCR_OFFSET);

        iowrite8(0xbf, port->addr + LCR_OFFSET); /* Set to 0xbf to access 650 registers */
        iowrite8(0x10, port->addr + EFR_OFFSET); /* Enable enhanced mode */

        iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
        iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
        port->ACR = 0x20;
        iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Enable 950 trigger to ACR through ICR */

        iowrite8(init_lcr, port->addr + LCR_OFFSET);
    }

    fastcom_set_clock_rate(port, default_clock_rate);
    fastcom_set_rs485(port, DEFAULT_RS485);
    fastcom_set_sample_rate(port, DEFAULT_SAMPLE_RATE);
    fastcom_set_tx_trigger(port, DEFAULT_TX_TRIGGER_LEVEL);
    fastcom_set_rx_trigger(port, DEFAULT_RX_TRIGGER_LEVEL);
    fastcom_set_termination(port, DEFAULT_TERMINATION);
    fastcom_set_echo_cancel(port, DEFAULT_ECHO_CANCEL);
    fastcom_set_isochronous(port, DEFAULT_ISOCHRONOUS);
    fastcom_set_frame_length(port, DEFAULT_FRAME_LENGTH);

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

	dev_info(port->device, "%s\n", serialfc_card_get_name(port->card));

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


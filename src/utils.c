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

#include "utils.h"
#include "serialfc.h"
#include "port.h"
#include "config.h"

unsigned is_serialfc_device(struct pci_dev *pdev)
{
	switch (pdev->device) {
		case FC_422_2_PCI_335_ID:
		case FC_422_4_PCI_335_ID:
		case FC_232_4_PCI_335_ID:
		case FC_232_8_PCI_335_ID:
		case FC_422_4_PCIe_ID:
		case FC_422_8_PCIe_ID:
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
       		return 1;
	}

	return 0;
}

enum FASTCOM_CARD_TYPE fastcom_get_card_type2(struct serialfc_card *card)
{
    switch (card->pci_dev->device) {
    case FC_422_2_PCI_335_ID:
    case FC_422_4_PCI_335_ID:
    case FC_232_4_PCI_335_ID:
    case FC_232_8_PCI_335_ID:
        return CARD_TYPE_PCI;

    case FC_422_4_PCIe_ID:
    case FC_422_8_PCIe_ID:
        return CARD_TYPE_PCIe;
    }

    if (card->pci_dev->device == 0x0f ||
        (card->pci_dev->device >= 0x14 && card->pci_dev->device <= 0x1F) ||
        (card->pci_dev->device >= 0x22 && card->pci_dev->device <= 0x27))
        return CARD_TYPE_FSCC;

    return CARD_TYPE_UNKNOWN;
}

enum FASTCOM_CARD_TYPE fastcom_get_card_type(struct serialfc_port *port)
{
    return fastcom_get_card_type2(port->card);
}

int fastcom_set_sample_rate_pci(struct serialfc_port *port, unsigned value)
{
    unsigned char current_8x_mode, new_8x_mode;

    if (value != 8 && value != 16)
        return -EINVAL;

    current_8x_mode = ioread8(port->addr + UART_EXAR_8XMODE);

    switch (value) {
    case 8:
        new_8x_mode = current_8x_mode | (1 << port->channel);
        break;

    case 16:
        new_8x_mode = current_8x_mode & ~(1 << port->channel);
        break;

    default: // This should never be reached due to the initial if check
        return -EINVAL;
    }

    iowrite8(new_8x_mode, port->addr + UART_EXAR_8XMODE);

    return 0;
}

int fastcom_set_sample_rate_pcie(struct serialfc_port *port, unsigned value)
{
    unsigned char current_8x_mode, new_8x_mode;
    unsigned char current_4x_mode, new_4x_mode;

    if (value != 4 && value != 8 && value != 16)
        return -EINVAL;

    current_4x_mode = ioread8(port->addr + UART_EXAR_4XMODE);
    current_8x_mode = ioread8(port->addr + UART_EXAR_8XMODE);

    switch (value) {
    case 4:
        new_4x_mode = current_4x_mode | (1 << port->channel);
        new_8x_mode = current_8x_mode & ~(1 << port->channel);
        break;

    case 8:
        new_4x_mode = current_4x_mode & ~(1 << port->channel);
        new_8x_mode = current_8x_mode | (1 << port->channel);
        break;

    case 16:
        new_4x_mode = current_4x_mode & ~(1 << port->channel);
        new_8x_mode = current_8x_mode & ~(1 << port->channel);
        break;

    default: // This should never be reached due to the initial if check
        return -EINVAL;
    }

    iowrite8(new_4x_mode, port->addr + UART_EXAR_4XMODE);
    iowrite8(new_8x_mode, port->addr + UART_EXAR_8XMODE);

    return 0;
}

int fastcom_set_sample_rate_fscc(struct serialfc_port *port, unsigned value)
{
    unsigned char orig_lcr;

    if (value < 4 || value > 16)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(TCR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to TCR */
    iowrite8((unsigned char)value, port->addr + ICR_OFFSET); /* Actually writing to TCR through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_set_sample_rate(struct serialfc_port *port, unsigned value)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        status = fastcom_set_sample_rate_pci(port, value);
        break;

    case CARD_TYPE_PCIe:
        status = fastcom_set_sample_rate_pcie(port, value);
        break;

    case CARD_TYPE_FSCC:
        status = fastcom_set_sample_rate_fscc(port, value);
        break;

	default:
		/* This makes sure the sample rate value is set for the non Fastcom ports. */
		status = 0;
		break;
    }

    if (status == 0) {
	    dev_dbg(port->device, "Sample rate = %i\n", value);

        port->sample_rate = value;
    }

    return status;
}

void fastcom_get_sample_rate(struct serialfc_port *port, unsigned *value)
{
    *value = port->sample_rate;
}

int fastcom_set_tx_trigger_pci(struct serialfc_port *port, unsigned value)
{
    if (value > 64)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_TXTRG);

    return 0;
}

int fastcom_set_tx_trigger_pcie(struct serialfc_port *port, unsigned value)
{
    if (value > 255)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_TXTRG);

    return 0;
}

int fastcom_set_tx_trigger_fscc(struct serialfc_port *port, unsigned value)
{
    unsigned char orig_lcr;

    if (value > 127)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(TTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to TTL */
    iowrite8((unsigned char)value, port->addr + ICR_OFFSET); /* To allow access to TTL */

    /* TODO: When 950 trigger levels are fixed, use the TTLH value also */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    return 0;
}

int fastcom_set_tx_trigger(struct serialfc_port *port, unsigned value)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        status = fastcom_set_tx_trigger_pci(port, value);
        break;

    case CARD_TYPE_PCIe:
        status = fastcom_set_tx_trigger_pcie(port, value);
        break;

    case CARD_TYPE_FSCC:
        status = fastcom_set_tx_trigger_fscc(port, value);
        break;

    default:
        status = -EPROTONOSUPPORT;
    }

    if (status == 0) {
	    dev_dbg(port->device, "Transmit trigger level = %i\n", value);

        port->tx_trigger = value;
    }

    return status;
}

void fastcom_get_tx_trigger_fscc(struct serialfc_port *port, unsigned *value)
{
    unsigned char orig_lcr;
    unsigned char ttl;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR | 0x40, port->addr + ICR_OFFSET); /* Enable ICR read enable */
    iowrite8(TTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to TTL */

    ttl = ioread8(port->addr + ICR_OFFSET); /* Get TTL through ICR */

    *value = ttl & 0x7F;

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET);  /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
}

int fastcom_get_tx_trigger(struct serialfc_port *port, unsigned *value)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
    case CARD_TYPE_PCIe:
        *value = port->tx_trigger; // The register is write-only so we have to use this variable
        break;

    case CARD_TYPE_FSCC:
        fastcom_get_tx_trigger_fscc(port, value);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

    return 0;
}

int fastcom_set_rx_trigger_pci(struct serialfc_port *port, unsigned value)
{
    if (value > 64)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_RXTRG);

    return 0;
}

int fastcom_set_rx_trigger_pcie(struct serialfc_port *port, unsigned value)
{
    if (value > 255)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_RXTRG);

    return 0;
}

int fastcom_set_rx_trigger_fscc(struct serialfc_port *port, unsigned value)
{
    unsigned char orig_lcr;

    if (value > 127)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(RTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to RTL */
    iowrite8((unsigned char)value, port->addr + ICR_OFFSET); /* Set the trigger level to RTL through ICR */

    /* TODO: When 950 trigger levels are fixed, use the TTLH value also */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_set_rx_trigger(struct serialfc_port *port, unsigned value)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        status = fastcom_set_rx_trigger_pci(port, value);
        break;

    case CARD_TYPE_PCIe:
        status = fastcom_set_rx_trigger_pcie(port, value);
        break;

    case CARD_TYPE_FSCC:
        status = fastcom_set_rx_trigger_fscc(port, value);
        break;

    default:
        status = -EPROTONOSUPPORT;
        break;
    }

    if (status == 0) {
	    dev_dbg(port->device, "Receive trigger level = %i\n", value);

        port->rx_trigger = value;
    }

    return status;
}

void fastcom_get_rx_trigger_fscc(struct serialfc_port *port, unsigned *value)
{
    unsigned char orig_lcr;
    unsigned char rtl;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to RTL */
    iowrite8(port->ACR | 0x40, port->addr + ICR_OFFSET); /* Enable ICR read enable */
    iowrite8(RTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to RTL */

    rtl = ioread8(port->addr + ICR_OFFSET); /* Get RTL through ICR */

    *value = rtl & 0x7F;

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
}

int fastcom_get_rx_trigger(struct serialfc_port *port, unsigned *value)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
    case CARD_TYPE_PCIe:
        *value = port->rx_trigger; // The register is write-only so we have to use this variable
        break;

    case CARD_TYPE_FSCC:
        fastcom_get_rx_trigger_fscc(port, value);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

    return 0;
}

void fastcom_set_rs485_pci(struct serialfc_port *port, int enable)
{
    unsigned char current_fctr, new_fctr;
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);
    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable) {
        new_fctr = current_fctr | 0x20; /* Enable Auto 485 on UART */
        new_mpio_lvl = current_mpio_lvl & ~(0x8 << port->channel); /* Enable 485 on transmitters */
    }
    else {
        new_fctr = current_fctr & ~0x20; /* Disable Auto 485 on UART */
        new_mpio_lvl = current_mpio_lvl | (0x8 << port->channel); /* Disable 485 on transmitters */
    }

    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
    iowrite8(new_fctr, port->addr + UART_EXAR_FCTR);
}

void fastcom_set_rs485_pcie(struct serialfc_port *port, int enable)
{
    unsigned char current_mcr, new_mcr;
    unsigned char current_fctr, new_fctr;

    current_mcr = ioread8(port->addr + MCR_OFFSET);
    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);

    if (enable) {
        new_mcr = current_mcr | 0x04;  /* Use DTR for Auto 485 */
        new_fctr = current_fctr | 0x20; /* Enable Auto 485 on UART */
    }
    else {
        new_mcr = current_mcr & ~0x04;  /* Disable using DTR for Auto 485 */
        new_fctr = current_fctr & ~0x20; /* Disable Auto 485 on UART */
    }

    iowrite8(new_mcr, port->addr + MCR_OFFSET);
    iowrite8(new_fctr, port->addr + UART_EXAR_FCTR);
}

void fastcom_set_rs485_fscc(struct serialfc_port *port, int enable)
{
    unsigned char orig_lcr;
    __u32 current_fcr, new_fcr;
    __u32 bit_mask;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);
    current_fcr = ioread32(port->card->bar2);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */

    switch (port->channel) {
    case 0:
        bit_mask = 0x00040000;
        break;

    case 1:
        bit_mask = 0x00400000;
        break;

    default:
        bit_mask = 0x00000000;
    }

    if (enable) {
        port->ACR |= 0x10;
        new_fcr = current_fcr | bit_mask;
    }
    else {
        port->ACR &= ~0x10;
        new_fcr = current_fcr & ~bit_mask;
    }

    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* DTR is active during transmission to turn on drivers */
    iowrite32(new_fcr, port->card->bar2);

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);
}

int fastcom_set_rs485(struct serialfc_port *port, int enable)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        fastcom_set_rs485_pci(port, enable);
        break;

    case CARD_TYPE_PCIe:
        fastcom_set_rs485_pcie(port, enable);
        break;

    case CARD_TYPE_FSCC:
        fastcom_set_rs485_fscc(port, enable);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

	dev_dbg(port->device, "RS485 = %i\n", enable);

    return 0;
}

void fastcom_get_rs485_pci(struct serialfc_port *port, int *enabled)
{
    unsigned char current_fctr;

    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);

    *enabled = (current_fctr & 0x20) ? 1 : 0;
}

void fastcom_get_rs485_fscc(struct serialfc_port *port, int *enabled)
{
    __u32 current_fcr;
    int dtr_enable_active, transmitter_485_active;
    __u32 bit_mask;

    switch (port->channel) {
    case 0:
        bit_mask = 0x00040000;
        break;

    case 1:
        bit_mask = 0x00400000;
        break;

    default:
        bit_mask = 0x00000000;
    }

    current_fcr = ioread32(port->card->bar2);
    dtr_enable_active = (port->ACR & 0x10) ? 1 : 0; /* DTR is active during transmission to turn on drivers */
    transmitter_485_active = (current_fcr & bit_mask) ? 1 : 0;

    *enabled = (dtr_enable_active && transmitter_485_active) ? 1 : 0;
}

int fastcom_get_rs485(struct serialfc_port *port, int *enabled)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
    case CARD_TYPE_PCIe:
        fastcom_get_rs485_pci(port, enabled);
        return 0;

    case CARD_TYPE_FSCC:
        fastcom_get_rs485_fscc(port, enabled);
        return 0;

    default:
        return -EPROTONOSUPPORT;
    }
}

void fastcom_enable_rs485(struct serialfc_port *port)
{
    fastcom_set_rs485(port, 1);
}

void fastcom_disable_rs485(struct serialfc_port *port)
{
    fastcom_set_rs485(port, 0);
}

int fastcom_set_isochronous_fscc(struct serialfc_port *port, int mode)
{
    unsigned char orig_lcr;
    unsigned char new_cks = 0;
    unsigned char new_mdm = 0;

    if (mode > 10 || mode < -1)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    switch (mode) {
    /* Enable receive using external DSR# */
    case 2:
    case 3:
    case 4:
    case 10:
        new_cks |= 0x09;
        new_mdm |= 0x02;
        break;

    /* Enable receive using internal BRG */
    case 5:
    case 6:
    case 7:
        new_cks |= 0x0A;
        break;

    /* Enable receive using transmit clock */
    case 8:
        new_cks |= 0x0B;
        break;
    }

    switch (mode) {
    /* Enable transmit using external RI# */
    case 0:
    case 3:
    case 6:
    case 8:
        new_cks |= 0xD0;
        new_mdm |= 0x04;
        break;

    /* Transmit using internal BRG */
    case 1:
    case 4:
    case 7:
        new_cks |= 0x90;
        break;

    case 9:
    case 10:
        new_cks |= 0x10;
        break;
    }

    iowrite8(MDM_OFFSET, port->addr + SPR_OFFSET); /* To allow access to MDM */
    iowrite8(new_mdm, port->addr + ICR_OFFSET); /* Set interrupts to MDM through ICR */

    iowrite8(CKS_OFFSET, port->addr + SPR_OFFSET); /* To allow access to CKS */
    iowrite8(new_cks, port->addr + ICR_OFFSET); /* Set clock mode to CKS through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

void fastcom_get_isochronous_fscc(struct serialfc_port *port, int *mode)
{
    unsigned char orig_lcr;
    unsigned char cks;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR | 0x40, port->addr + ICR_OFFSET); /* Enable ICR read enable */
    iowrite8(CKS_OFFSET, port->addr + SPR_OFFSET); /* To allow access to CLK */

    cks = ioread8(port->addr + ICR_OFFSET); /* Get CKS through ICR */

    switch (cks) {
    case 0x00:
        *mode = -1;
        break;

    case 0xD0:
        *mode = 0;
        break;

    case 0x90:
        *mode = 1;
        break;

    case 0x09:
        *mode = 2;
        break;

    case 0xD9:
        *mode = 3;
        break;

    case 0x99:
        *mode = 4;
        break;

    case 0x0A:
        *mode = 5;
        break;

    case 0xDA:
        *mode = 6;
        break;

    case 0x9A:
        *mode = 7;
        break;

    case 0xDB:
        *mode = 8;
        break;
        break;

    case 0x10:
        *mode = 9;
        break;
        break;

    case 0x19:
        *mode = 10;
        break;
    }

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
}

int fastcom_set_isochronous(struct serialfc_port *port, int mode)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        status = fastcom_set_isochronous_fscc(port, mode);
        break;

    default:
        status = -EPROTONOSUPPORT;
        break;
    }

    if (status == 0)
	    dev_dbg(port->device, "Isochronous mode = %i\n", mode);

    return status;
}

int fastcom_get_isochronous(struct serialfc_port *port, int *mode)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        fastcom_get_isochronous_fscc(port, mode);
        return 0;

    default:
        return -EPROTONOSUPPORT;
    }
}

int fastcom_enable_isochronous(struct serialfc_port *port, unsigned mode)
{
    return fastcom_set_isochronous(port, mode);
}

int fastcom_disable_isochronous(struct serialfc_port *port)
{
    return fastcom_set_isochronous(port, -1);
}

// Active low
void fastcom_set_termination_pcie(struct serialfc_port *port, int enable)
{
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable)
        new_mpio_lvl = current_mpio_lvl & ~(0x1 << port->channel); /* Enable termination */
    else
        new_mpio_lvl = current_mpio_lvl | (0x1 << port->channel); /* Disable termination */

    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
}

// Active low
void fastcom_get_termination_pcie(struct serialfc_port *port, int *enabled)
{
    unsigned char mpio_lvl;

    mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    *enabled = mpio_lvl & (0x1 << port->channel) ? 0 : 1;
}

int fastcom_set_termination(struct serialfc_port *port, int enable)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCIe:
        fastcom_set_termination_pcie(port, enable);
        status = 0;
        break;

    default:
        status = -EPROTONOSUPPORT;
        break;
    }

    if (status == 0)
	    dev_dbg(port->device, "Termination = %i\n", enable);

    return status;
}

int fastcom_get_termination(struct serialfc_port *port, int *enabled)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCIe:
        fastcom_get_termination_pcie(port, enabled);
        return 0;

    default:
        return -EPROTONOSUPPORT;
    }
}

int fastcom_enable_termination(struct serialfc_port *port)
{
    return fastcom_set_termination(port, 1);
}

int fastcom_disable_termination(struct serialfc_port *port)
{
    return fastcom_set_termination(port, 0);
}

void fastcom_set_echo_cancel_pci(struct serialfc_port *port, int enable)
{
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable)
        new_mpio_lvl = current_mpio_lvl | 0x80; /* Enable echo cancel */
    else
        new_mpio_lvl = current_mpio_lvl & ~0x80; /* Disable echo cancel */

    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
}

void fastcom_set_echo_cancel_pcie(struct serialfc_port *port, int enable)
{
    unsigned char current_mpio_lvlh, new_mpio_lvlh;

    current_mpio_lvlh = ioread8(port->addr + MPIOLVLH_OFFSET);

    if (enable)
        new_mpio_lvlh = current_mpio_lvlh | (0x1 << port->channel); /* Enable echo cancel */
    else
        new_mpio_lvlh = current_mpio_lvlh & ~(0x1 << port->channel); /* Disable echo cancel */

    iowrite8(new_mpio_lvlh, port->addr + MPIOLVLH_OFFSET);
}

void fastcom_set_echo_cancel_fscc(struct serialfc_port *port, int enable)
{
    __u32 current_fcr, new_fcr;
	__u32 bit_mask;

    current_fcr = ioread32(port->card->bar2);

	switch (port->channel) {
	case 0:
		bit_mask = 0x00010000;
		break;

	case 1:
		bit_mask = 0x00100000;
		break;

    default:
		bit_mask = 0x00000000;
	}

	if (enable)
		new_fcr = current_fcr | bit_mask;
	else
		new_fcr = current_fcr & ~bit_mask;

    iowrite32(new_fcr, port->card->bar2);
}

void fastcom_set_echo_cancel(struct serialfc_port *port, int enable)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        fastcom_set_echo_cancel_pci(port, enable);
        break;

    case CARD_TYPE_PCIe:
        fastcom_set_echo_cancel_pcie(port, enable);
        break;

    case CARD_TYPE_FSCC:
        fastcom_set_echo_cancel_fscc(port, enable);
        break;

    default:
        break; //TODO
    }

	dev_dbg(port->device, "Echo cancel = %i\n", enable);
}

void fastcom_enable_echo_cancel(struct serialfc_port *port)
{
    fastcom_set_echo_cancel(port, 1);
}

void fastcom_disable_echo_cancel(struct serialfc_port *port)
{
    fastcom_set_echo_cancel(port, 0);
}

void fastcom_get_echo_cancel_pci(struct serialfc_port *port, int *enabled)
{
    unsigned char mpio_lvl;

    mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    *enabled = mpio_lvl & 0x80 ? 1 : 0;
}

void fastcom_get_echo_cancel_pcie(struct serialfc_port *port, int *enabled)
{
    unsigned char mpio_lvlh;

    mpio_lvlh = ioread8(port->addr + MPIOLVLH_OFFSET);

    *enabled = mpio_lvlh & (0x1 << port->channel) ? 1 : 0;
}

void fastcom_get_echo_cancel_fscc(struct serialfc_port *port, int *enabled)
{
    __u32 fcr;
	__u32 bit_mask;

    fcr = ioread32(port->card->bar2);

	switch (port->channel) {
	case 0:
		bit_mask = 0x00010000;
		break;

	case 1:
		bit_mask = 0x00100000;
		break;

	default:
		bit_mask = 0x00000000;
	}

	*enabled = (fcr & bit_mask) ? 1 : 0;
}

void fastcom_get_echo_cancel(struct serialfc_port *port, int *enabled)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        fastcom_get_echo_cancel_pci(port, enabled);
        break;

    case CARD_TYPE_PCIe:
        fastcom_get_echo_cancel_pcie(port, enabled);
        break;

    case CARD_TYPE_FSCC:
        fastcom_get_echo_cancel_fscc(port, enabled);
        break;

    default:
        break; //TODO
    }
}

#define STRB_BASE 0x00000008
#define DTA_BASE 0x00000001
#define CLK_BASE 0x00000002

int fastcom_set_clock_bits_fscc(struct serialfc_port *port,
							    unsigned char *clock_data)
{
	__u32 orig_fcr_value = 0;
	__u32 new_fcr_value = 0;
	int j = 0; // Must be signed because we are going backwards through the array
	int i = 0; // Must be signed because we are going backwards through the array
	unsigned strb_value = STRB_BASE;
	unsigned dta_value = DTA_BASE;
	unsigned clk_value = CLK_BASE;

	__u32 *data = 0;
	unsigned data_index = 0;


#ifdef DISABLE_XTAL
	clock_data[15] &= 0xfb;
#else
	/* This enables XTAL on all cards except green FSCC cards with a revision
       greater than 6 and 232 cards. Some old protoype SuperFSCC cards will 
       need to manually disable XTAL as they are not supported in this driver 
       by default. */
    if ((fscc_get_PDEV(port) == 0x0f && fscc_get_PREV(port) <= 6) ||
        fscc_get_PDEV(port) == 0x16) {
        clock_data[15] &= 0xfb;
    }
    else {
        clock_data[15] |= 0x04;
    }
#endif


	data = kmalloc(sizeof(__u32) * 323, GFP_KERNEL);

	if (data == NULL) {
		printk(KERN_ERR DEVICE_NAME "kmalloc failed\n");
		return 1;
	}

	if (port->channel == 1) {
		strb_value <<= 0x08;
		dta_value <<= 0x08;
		clk_value <<= 0x08;
	}

	orig_fcr_value = ioread32(port->card->bar2);

	data[data_index++] = new_fcr_value = orig_fcr_value & 0xfffff0f0;

	for (i = 19; i >= 0; i--) {
		for (j = 7; j >= 0; j--) {
			int bit = ((clock_data[i] >> j) & 1);

            /* This is required for 4-port cards. I'm not sure why at the
               moment */
			data[data_index++] = new_fcr_value;

			if (bit)
				new_fcr_value |= dta_value; /* Set data bit */
			else
				new_fcr_value &= ~dta_value; /* Clear clock bit */

			data[data_index++] = new_fcr_value |= clk_value; /* Set clock bit */
			data[data_index++] = new_fcr_value &= ~clk_value; /* Clear clock bit */

			new_fcr_value = orig_fcr_value & 0xfffff0f0;
		}
	}

	new_fcr_value = orig_fcr_value & 0xfffff0f0;

	new_fcr_value |= strb_value; /* Set strobe bit */
	new_fcr_value &= ~clk_value; /* Clear clock bit	*/

	data[data_index++] = new_fcr_value;
	data[data_index++] = orig_fcr_value;

	iowrite32_rep(port->card->bar2, data, data_index);

	kfree(data);

    return 0;
}


// Copied from old code. Needs a major code cleanup. TODO
// ICS307-02
#define MPIO_SDTA 0x01 // Each bit of MPIOLVL register
#define MPIO_SCLK 0x02
#define MPIO_SSTB 0x04

int fastcom_set_clock_bits_pci(struct serialfc_port *port,
                               __u32 clock_data)
{
    unsigned long tempValue = 0;
    unsigned char data = 0;
    unsigned char saved = 0;
    unsigned long i = 0;

    tempValue = (clock_data & 0x00ffffff);

    data = saved = ioread8(port->addr + MPIOLVL_OFFSET); // Save MPIO pin state

    for (i = 0; i < 24; i++) {
        //data bit set
        if ((tempValue & 0x800000) != 0)
            data |= MPIO_SDTA;
        else
            data &= ~MPIO_SDTA;

        iowrite8(data, port->addr + MPIOLVL_OFFSET);
        //KeStallExecutionProcessor(20);

        //clock high, data still there
        data |= MPIO_SCLK;
        iowrite8(data, port->addr + MPIOLVL_OFFSET);
        //KeStallExecutionProcessor(20);

        //clock low, data still there
        data &= MPIO_SDTA;
        iowrite8(data, port->addr + MPIOLVL_OFFSET);
        //KeStallExecutionProcessor(20);

        tempValue <<= 1;
    }

    data &= 0xF8;
    data |= MPIO_SSTB; // strobe on
    iowrite8(data, port->addr + MPIOLVL_OFFSET);
    //KeStallExecutionProcessor(20);

    data &= ~MPIO_SSTB; // all off
    iowrite8(data, port->addr + MPIOLVL_OFFSET);
    //KeStallExecutionProcessor(20);

    //Put MPIO pins back to saved state.
    iowrite8(saved, port->addr + MPIOLVL_OFFSET);
printk("\a");
    return 0;
}

int fastcom_set_clock_bits(struct serialfc_port *port, void *clock_data)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        status = fastcom_set_clock_bits_fscc(port, (unsigned char *)clock_data);
        break;
    case CARD_TYPE_PCI:
        status = fastcom_set_clock_bits_pci(port, *((unsigned long *)clock_data));
        break;

    default:
        status = -EPROTONOSUPPORT;
        break;
    }

    return status;
}

/* Includes non floating point math from David Higgins */
int pcie_set_baud_rate(struct serialfc_port *port, unsigned value)
{
    const unsigned input_freq = 125000000;
    const unsigned prescaler = 1;
    unsigned divisor = 0;
    unsigned char orig_lcr = 0;
    unsigned char dlm = 0;
    unsigned char dll = 0;
    unsigned char dld = 0;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(orig_lcr | 0x80, port->addr + LCR_OFFSET);

    divisor = (input_freq * 16) / prescaler / (value * port->sample_rate);

    dlm = divisor >> 12; /* Was shifted by 8 but need to adjust for additional
                            four bits */
    dll = (divisor >> 4) & 0xff;

    dld = ioread8(port->addr + DLD_OFFSET);
    dld &= 0xf0;
    dld |= (divisor & 0xf);

    iowrite8(dlm, port->addr + DLM_OFFSET);
    iowrite8(dll, port->addr + DLL_OFFSET);
    iowrite8(dld, port->addr + DLD_OFFSET);

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

// Not supported in Linux at the moment, use set_clock_bits instead
int fastcom_set_clock_rate(struct serialfc_port *port, unsigned value)
{
    return -EPROTONOSUPPORT;
}

int fastcom_set_external_transmit_fscc(struct serialfc_port *port, unsigned num_chars)
{
    unsigned char orig_lcr;

    if (num_chars > 8191)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);
    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */


    if (num_chars != 0) {
        iowrite8(EXTH_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXTH */
        iowrite8(0x80 | (num_chars >> 8), port->addr + ICR_OFFSET); /* Actually writing to EXTH through ICR */

        iowrite8(EXT_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXTH */
        iowrite8((char)num_chars, port->addr + ICR_OFFSET); /* Actually writing to EXT through ICR */
    }
    else {
        iowrite8(EXTH_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXTH */
        iowrite8(0x00, port->addr + ICR_OFFSET); /* Actually writing to EXTH through ICR */

        iowrite8(EXT_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXTH */
        iowrite8(0x00, port->addr + ICR_OFFSET); /* Actually writing to EXT through ICR */
    }

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

void fastcom_get_external_transmit_fscc(struct serialfc_port *port, unsigned *num_chars)
{
    unsigned char orig_lcr;
    unsigned char ext, exth;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR | 0x40, port->addr + ICR_OFFSET); /* Enable ICR read enable */

    iowrite8(EXT_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXT */
    ext = ioread8(port->addr + ICR_OFFSET); /* Get EXT through ICR */

    iowrite8(EXTH_OFFSET, port->addr + SPR_OFFSET); /* To allow access to EXTH */
    exth = ioread8(port->addr + ICR_OFFSET); /* Get EXTH through ICR */

    *num_chars = ((exth & 0x1F) << 8) + ext;

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET);
}

int fastcom_get_external_transmit(struct serialfc_port *port, unsigned *num_chars)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        fastcom_get_external_transmit_fscc(port, num_chars);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

    return 0;
}

int fastcom_set_external_transmit(struct serialfc_port *port, unsigned num_chars)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        status = fastcom_set_external_transmit_fscc(port, num_chars);
        break;

    default:
        status = -EPROTONOSUPPORT;
    }

    if (status == 0)
	    dev_dbg(port->device, "External transmit = %i\n", num_chars);

    return status;
}

int fastcom_enable_external_transmit(struct serialfc_port *port, unsigned num_chars)
{
    return fastcom_set_external_transmit(port, num_chars);
}

int fastcom_disable_external_transmit(struct serialfc_port *port)
{
    return fastcom_set_external_transmit(port, 0);
}

int fastcom_set_frame_length_fscc(struct serialfc_port *port, unsigned num_chars)
{
    unsigned char orig_lcr;
    unsigned char frev;

    if (num_chars == 0 || num_chars > 256)
        return -EINVAL;

    frev = fscc_get_frev(port);

    if (frev < 0x20)
        return -EPROTONOSUPPORT;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);
    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    iowrite8(FLR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to FLR */
    iowrite8(num_chars - 1, port->addr + ICR_OFFSET); /* Actually writing to EXT through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_get_frame_length_fscc(struct serialfc_port *port, unsigned *num_chars)
{
    unsigned char orig_lcr;
    unsigned char flr;
    unsigned char frev;

    frev = fscc_get_frev(port);

    if (frev < 0x20)
        return -EPROTONOSUPPORT;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR | 0x40, port->addr + ICR_OFFSET); /* Enable ICR read enable */

    iowrite8(FLR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to FLR */
    flr = ioread8(port->addr + ICR_OFFSET); /* Get FLR through ICR */

    *num_chars = flr + 1;

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_set_frame_length(struct serialfc_port *port, unsigned num_chars)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        status = fastcom_set_frame_length_fscc(port, num_chars);
        break;

    default:
        status = -EPROTONOSUPPORT;
    }

    if (status == 0)
	    dev_dbg(port->device, "Frame length = %i\n", num_chars);

    return status;
}

int fastcom_get_frame_length(struct serialfc_port *port, unsigned *num_chars)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        status = fastcom_get_frame_length_fscc(port, num_chars);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

    return status;
}

int fastcom_set_9bit(struct serialfc_port *port, int enable)
{
    return -EPROTONOSUPPORT;
}

int fastcom_get_9bit(struct serialfc_port *port, int *enabled)
{
    return -EPROTONOSUPPORT;
}

int fastcom_enable_9bit(struct serialfc_port *port)
{
    return fastcom_set_9bit(port, 1);
}

int fastcom_disable_9bit(struct serialfc_port *port)
{
    return fastcom_set_9bit(port, 0);
}

void fastcom_init_gpio(struct serialfc_port *port)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
        switch (port->card->pci_dev->device) {
        case FC_422_2_PCI_335_ID:
        case FC_422_4_PCI_335_ID:
            /* Switch GPIO pins to outputs */
            iowrite8(0x00, port->addr + MPIOSEL_OFFSET);
            break;

        case FC_232_4_PCI_335_ID:
        case FC_232_8_PCI_335_ID:
            iowrite8(0xc0, port->addr + MPIOSEL_OFFSET);
            iowrite8(0xc0, port->addr + MPIOINV_OFFSET);
            break;
        }
        break;

    case CARD_TYPE_PCIe:
         /* Switch GPIO pins to outputs */
        iowrite8(0x00, port->addr + MPIOSEL_OFFSET);
        iowrite8(0x00, port->addr + MPIOSELH_OFFSET);
        break;

    default:
        break;
    }
}

void fastcom_init_triggers(struct serialfc_port *port)
{
    /* Enable programmable trigger levels */
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCI:
    case CARD_TYPE_PCIe: {
            unsigned char current_fctr, new_fctr;

            current_fctr = ioread8(port->addr + UART_EXAR_FCTR);

            new_fctr = current_fctr | 0xc0; /* Enable programmable triggers */

            iowrite8(0x01, port->addr + FCR_OFFSET); /* Enable TX & RX FIFO's */
            iowrite8(new_fctr, port->addr + UART_EXAR_FCTR);
        }
        break;

    case CARD_TYPE_FSCC:
        iowrite8(0x01, port->addr + FCR_OFFSET); /* Enable FIFO (combined with enhanced enables 950 mode) */

        iowrite8(0xbf, port->addr + LCR_OFFSET); /* Set to 0xbf to access 650 registers */
        iowrite8(0x10, port->addr + EFR_OFFSET); /* Enable enhanced mode */

        /* Temporarily disable 950 trigger levels due to either interrupts not
           firing or not being handled correctly. */
#if 0
        iowrite8(0x00, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
        iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
        port->ACR = 0x20;
        iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Enable 950 trigger to ACR through ICR */
#endif
        port->ACR = 0x00;
        break;

    default:
        break;
    }
}

unsigned char fscc_get_frev(struct serialfc_port *port)
{
    return ioread32(port->card->bar0 + VSTR_OFFSET + (port->channel * 0x80)) & 0x000000ff;
}

int fscc_enable_async_mode(struct serialfc_port *port)
{
    __u32 orig_fcr, new_fcr;

    if (fastcom_get_card_type(port) != CARD_TYPE_FSCC)
        return -EPROTONOSUPPORT;

    orig_fcr = ioread32(port->card->bar2);

    if ((orig_fcr & (0x01000000 << port->channel)) == 0) {
        /* UART_{A,B} */
        new_fcr = orig_fcr | (0x01000000 << port->channel);

        iowrite32(new_fcr, port->card->bar2);
    }

    return 0;
}

int fscc_disable_async_mode(struct serialfc_port *port)
{
    __u32 orig_fcr, new_fcr;

    if (fastcom_get_card_type(port) != CARD_TYPE_FSCC)
        return -EPROTONOSUPPORT;

    orig_fcr = ioread32(port->card->bar2);

    /* UART_{A,B} */
    new_fcr = orig_fcr & ~(0x01000000 << port->channel);

    iowrite32(new_fcr, port->card->bar2);

    return 0;
}

__u8 fscc_get_FREV(struct serialfc_port *port)
{
    return ioread32(port->card->bar0 + VSTR_OFFSET + (port->channel * 0x80)) & 0x000000ff;
}

__u8 fscc_get_PREV(struct serialfc_port *port)
{
    return (ioread32(port->card->bar0 + VSTR_OFFSET + (port->channel * 0x80)) & 0x0000ff00) >> 8;
}

__u16 fscc_get_PDEV(struct serialfc_port *port)
{
    return (ioread32(port->card->bar0 + VSTR_OFFSET + (port->channel * 0x80)) & 0xffff0000) >> 16;
}

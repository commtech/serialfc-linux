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
       		return 1;
	}

	return 0;
}

enum FASTCOM_CARD_TYPE fastcom_get_card_type(struct fc_port *port)
{
    switch (port->card->pci_dev->device) {
    case FC_422_2_PCI_335_ID:
    case FC_422_4_PCI_335_ID:
    case FC_232_4_PCI_335_ID:
    case FC_232_8_PCI_335_ID:
        return CARD_TYPE_PCI;

    case FC_422_4_PCIe_ID:
    case FC_422_8_PCIe_ID:
        return CARD_TYPE_PCIe;
    }

    if (port->card->pci_dev->device == 0x0f ||
        (port->card->pci_dev->device >= 0x14 && port->card->pci_dev->device <= 0x1F) ||
        (port->card->pci_dev->device >= 0x22 && port->card->pci_dev->device <= 0x27))
        return CARD_TYPE_FSCC;

    return CARD_TYPE_UNKNOWN;
}

int fastcom_set_sample_rate_pci(struct fc_port *port, unsigned value)
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

int fastcom_set_sample_rate_pcie(struct fc_port *port, unsigned value)
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

int fastcom_set_sample_rate_fscc(struct fc_port *port, unsigned value)
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

int fastcom_set_sample_rate(struct fc_port *port, unsigned value)
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
	    dev_info(port->device, "Sample rate = %i\n", value);

        port->sample_rate = value;
    }

    return status;
}

void fastcom_get_sample_rate(struct fc_port *port, unsigned *value)
{
    *value = port->sample_rate;
}

int fastcom_set_tx_trigger_pci(struct fc_port *port, unsigned value)
{
    if (value > 64)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_TXTRG);

    return 0;
}

int fastcom_set_tx_trigger_pcie(struct fc_port *port, unsigned value)
{
    if (value > 255)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_TXTRG);

    return 0;
}

int fastcom_set_tx_trigger_fscc(struct fc_port *port, unsigned value)
{
    unsigned char orig_lcr;

    if (value > 127)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(TTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to TTL */
    iowrite8((unsigned char)value, port->addr + ICR_OFFSET); /* To allow access to TTL */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    return 0;
}

int fastcom_set_tx_trigger(struct fc_port *port, unsigned value)
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
	    dev_info(port->device, "Transmit trigger level = %i\n", value);

        port->tx_trigger = value;
    }

    return status;
}

void fastcom_get_tx_trigger_fscc(struct fc_port *port, unsigned *value)
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

int fastcom_get_tx_trigger(struct fc_port *port, unsigned *value)
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

int fastcom_set_rx_trigger_pci(struct fc_port *port, unsigned value)
{
    if (value > 64)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_RXTRG);

    return 0;
}

int fastcom_set_rx_trigger_pcie(struct fc_port *port, unsigned value)
{
    if (value > 255)
        return -EINVAL;

    iowrite8((unsigned char)value, port->addr + UART_EXAR_RXTRG);

    return 0;
}

int fastcom_set_rx_trigger_fscc(struct fc_port *port, unsigned value)
{
    unsigned char orig_lcr;

    if (value > 127)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(RTL_OFFSET, port->addr + SPR_OFFSET); /* To allow access to RTL */
    iowrite8((unsigned char)value, port->addr + ICR_OFFSET); /* Set the trigger level to RTL through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_set_rx_trigger(struct fc_port *port, unsigned value)
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
	    dev_info(port->device, "Receive trigger level = %i\n", value);

        port->rx_trigger = value;
    }

    return status;
}

void fastcom_get_rx_trigger_fscc(struct fc_port *port, unsigned *value)
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

int fastcom_get_rx_trigger(struct fc_port *port, unsigned *value)
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

void fastcom_set_rs485_pci(struct fc_port *port, int enable)
{
    unsigned char current_mcr, new_mcr;
    unsigned char current_fctr, new_fctr;
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_mcr = ioread8(port->addr + MCR_OFFSET);
    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);
    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable) {
        new_mcr = current_mcr | 0x3;  /* Force RTS/DTS to low (not sure why yet) */
        new_fctr = current_fctr | 0x20; /* Enable Auto 485 on UART */
        new_mpio_lvl = current_mpio_lvl & ~(0x8 << port->channel); /* Enable 485 on transmitters */
    }
    else {
        new_mcr = current_mcr & ~0x3;  /* Force RTS/DTS to high (not sure why yet) */
        new_fctr = current_fctr & ~0x20; /* Disable Auto 485 on UART */
        new_mpio_lvl = current_mpio_lvl | (0x8 << port->channel); /* Disable 485 on transmitters */
    }

    iowrite8(new_mcr, port->addr + MCR_OFFSET);
    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
    iowrite8(new_fctr, port->addr + UART_EXAR_FCTR);
}

void fastcom_set_rs485_pcie(struct fc_port *port, int enable)
{
    unsigned char current_mcr, new_mcr;
    unsigned char current_fctr, new_fctr;

    current_mcr = ioread8(port->addr + MCR_OFFSET);
    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);

    if (enable) {
        new_mcr = current_mcr | 0x1;  /* Enable 485 on transmitters using DTR pin */
        new_fctr = current_fctr | 0x20; /* Enable Auto 485 on UART */
    }
    else {
        new_mcr = current_mcr & ~0x1;  /* Disable 485 on transmitters using DTR pin */
        new_fctr = current_fctr & ~0x20; /* Disable Auto 485 on UART */
    }

    iowrite8(new_mcr, port->addr + MCR_OFFSET);
    iowrite8(new_fctr, port->addr + UART_EXAR_FCTR);
}

//TODO: Needs FCR changes
void fastcom_set_rs485_fscc(struct fc_port *port, int enable)
{
    unsigned char orig_lcr;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */

    if (enable)
        port->ACR |= 0x10;
    else
        port->ACR &= ~0x10;

    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Enable 950 trigger to ACR through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);
}

int fastcom_set_rs485(struct fc_port *port, int enable)
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

	dev_info(port->device, "RS485 = %i\n", enable);

    return 0;
}

void fastcom_get_rs485_pci(struct fc_port *port, int *enabled)
{
    unsigned char current_fctr;

    current_fctr = ioread8(port->addr + UART_EXAR_FCTR);

    *enabled = (current_fctr & 0x20) ? 1 : 0;
}

//TODO: Needs FCR changes
void fastcom_get_rs485_fscc(struct fc_port *port, int *enabled)
{
    *enabled = (port->ACR & 0x10) ? 1 : 0;
}

int fastcom_get_rs485(struct fc_port *port, int *enabled)
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

void fastcom_enable_rs485(struct fc_port *port)
{
    fastcom_set_rs485(port, 1);
}

void fastcom_disable_rs485(struct fc_port *port)
{
    fastcom_set_rs485(port, 0);
}

int fastcom_set_isochronous_fscc(struct fc_port *port, int mode)
{
    unsigned char orig_lcr;
    unsigned char new_cks = 0;
    unsigned char new_mdm = 0;

    if (mode > 8 || mode < -1)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    switch (mode) {
    /* Enable receive using external DSR# */
    case 2:
    case 3:
    case 4:
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
    }

    iowrite8(MDM_OFFSET, port->addr + SPR_OFFSET); /* To allow access to MDM */
    iowrite8(new_mdm, port->addr + ICR_OFFSET); /* Set interrupts to MDM through ICR */

    iowrite8(CKS_OFFSET, port->addr + SPR_OFFSET); /* To allow access to CKS */
    iowrite8(new_cks, port->addr + ICR_OFFSET); /* Set clock mode to CKS through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

void fastcom_get_isochronous_fscc(struct fc_port *port, int *mode)
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
    }

    iowrite8(ACR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to ACR */
    iowrite8(port->ACR, port->addr + ICR_OFFSET); /* Restore original ACR value */
    iowrite8(orig_lcr, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */
}

int fastcom_set_isochronous(struct fc_port *port, int mode)
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
	    dev_info(port->device, "Isochronous mode = %i\n", mode);

    return status;
}

int fastcom_get_isochronous(struct fc_port *port, int *mode)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        fastcom_get_isochronous_fscc(port, mode);
        return 0;

    default:
        return -EPROTONOSUPPORT;
    }
}

int fastcom_enable_isochronous(struct fc_port *port, unsigned mode)
{
    return fastcom_set_isochronous(port, mode);
}

int fastcom_disable_isochronous(struct fc_port *port)
{
    return fastcom_set_isochronous(port, -1);
}

void fastcom_set_termination_pcie(struct fc_port *port, int enable)
{
    unsigned char current_mpio_lvlh, new_mpio_lvlh;

    current_mpio_lvlh = ioread8(port->addr + MPIOLVLH_OFFSET);

    if (enable)
        new_mpio_lvlh = current_mpio_lvlh | (0x1 << port->channel); /* Enable termination */
    else
        new_mpio_lvlh = current_mpio_lvlh & (0x1 << port->channel); /* Disable termination */

    iowrite8(new_mpio_lvlh, port->addr + MPIOLVLH_OFFSET);
}

void fastcom_get_termination_pcie(struct fc_port *port, int *enabled)
{
    unsigned char mpio_lvlh;

    mpio_lvlh = ioread8(port->addr + MPIOLVLH_OFFSET);

    *enabled = mpio_lvlh & (0x1 << port->channel) ? 1 : 0;
}

int fastcom_set_termination(struct fc_port *port, int enable)
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
	    dev_info(port->device, "Termination = %i\n", enable);

    return status;
}

int fastcom_get_termination(struct fc_port *port, int *enabled)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_PCIe:
        fastcom_get_termination_pcie(port, enabled);
        return 0;

    default:
        return -EPROTONOSUPPORT;
    }
}

int fastcom_enable_termination(struct fc_port *port)
{
    return fastcom_set_termination(port, 1);
}

int fastcom_disable_termination(struct fc_port *port)
{
    return fastcom_set_termination(port, 0);
}

void fastcom_set_echo_cancel_pci(struct fc_port *port, int enable)
{
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable)
        new_mpio_lvl = current_mpio_lvl | 0x80; /* Enable echo cancel */
    else
        new_mpio_lvl = current_mpio_lvl & ~0x80; /* Disable echo cancel */

    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
}

void fastcom_set_echo_cancel_pcie(struct fc_port *port, int enable)
{
    unsigned char current_mpio_lvl, new_mpio_lvl;

    current_mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    if (enable)
        new_mpio_lvl = current_mpio_lvl | (0x1 << port->channel); /* Enable echo cancel */
    else
        new_mpio_lvl = current_mpio_lvl & ~(0x1 << port->channel); /* Disable echo cancel */

    iowrite8(new_mpio_lvl, port->addr + MPIOLVL_OFFSET);
}

void fastcom_set_echo_cancel_fscc(struct fc_port *port, int enable)
{
#if 0
    UINT32 current_fcr, new_fcr;
	UINT32 bit_mask;

    current_fcr = READ_PORT_ULONG(ULongToPtr(pDevExt->Bar2));

	switch (pDevExt->Channel) {
	case 0:
		bit_mask = 0x00010000;
		break;

	case 1:
		bit_mask = 0x00100000;
		break;
	}

	if (enable)
		new_fcr = current_fcr | bit_mask;
	else
		new_fcr = current_fcr & ~bit_mask;

    WRITE_PORT_ULONG(ULongToPtr(pDevExt->Bar2), new_fcr);
#endif
}

void fastcom_set_echo_cancel(struct fc_port *port, int enable)
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

	dev_info(port->device, "Echo cancel = %i\n", enable);
}

void fastcom_enable_echo_cancel(struct fc_port *port)
{
    fastcom_set_echo_cancel(port, 1);
}

void fastcom_disable_echo_cancel(struct fc_port *port)
{
    fastcom_set_echo_cancel(port, 0);
}

void fastcom_get_echo_cancel_pci(struct fc_port *port, int *enabled)
{
    unsigned char mpio_lvl;

    mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    *enabled = mpio_lvl & 0x80 ? 1 : 0;
}

void fastcom_get_echo_cancel_pcie(struct fc_port *port, int *enabled)
{
    unsigned char mpio_lvl;

    mpio_lvl = ioread8(port->addr + MPIOLVL_OFFSET);

    *enabled = mpio_lvl & (0x1 << port->channel) ? 1 : 0;
}

void fastcom_get_echo_cancel_fscc(struct fc_port *port, int *enabled)
{
#if 0
    UINT32 fcr;
	UINT32 bit_mask;

    fcr = READ_PORT_unsigned long(ULongToPtr(port->Bar2));

	switch (port->channel) {
	case 0:
		bit_mask = 0x00010000;
		break;

	case 1:
		bit_mask = 0x00100000;
		break;
	}

	*enabled = (fcr & bit_mask) ? 1 : 0;
#endif
}

void fastcom_get_echo_cancel(struct fc_port *port, int *enabled)
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

#if 0
struct ResultStruct {
    double target;
    double freq;
    double errorPPM;
    int VCO_Div;
    int refDiv;
    int outDiv;
    int failed;
};

struct IcpRsStruct {
    double pdf;
    double nbw;
    double ratio;
    double df;
    unsigned long Rs;
    double icp;
    unsigned long icpnum;   //I have to use this in the switch statement because 8.75e-6 becomes 874
};

//WILL: Range 200 Hz - 270MHz
int GetICS30703Data(unsigned long desired, unsigned long ppm, struct ResultStruct *theOne, struct IcpRsStruct *theOther, unsigned char *progdata)
{
//  double inputfreq=18432000.0;
    double inputfreq=24000000.0;

    unsigned long od=0; //Output Divider
    unsigned r=0;
    unsigned v=0;
    unsigned V_Divstart=0;
    double freq=0;
    unsigned long bestFreq=0;
    unsigned long check=0;
    unsigned maxR;
    unsigned minR;
    unsigned max_V=2055;
    unsigned min_V=12;
    double allowable_error;
    double freq_err;
    struct ResultStruct Results;
    unsigned long i,j;
    struct IcpRsStruct IRStruct;
    unsigned count;
    unsigned long Rs;
    double optimal_ratio=15.0;
    double optimal_df=0.7;
    double best_ratio=0;
    double best_df=0;
    double rule1, rule2;
    int tempint;

    int InputDivider=0;
    int VCODivider=0;
    unsigned long ChargePumpCurrent=0;
    unsigned long LoopFilterResistor=0;
    unsigned long OutputDividerOut1=0;
    unsigned long OutputDividerOut2=0;
    unsigned long OutputDividerOut3=0;
    unsigned long temp=0;
    unsigned long requestedppm;

    requestedppm=ppm;

    if( inputfreq == 18432000.0) 
    {
        maxR = 921;
        minR = 1;
    }
    else if( inputfreq == 24000000.0) 
    {
        maxR = 1200;
        minR = 1;
    }

    ppm=0;
increaseppm:
//  DbgPrint("ICS30703: ppm = %d\n",ppm);
    allowable_error  = ppm * desired/1e6; // * 1e6

    for( r = minR; r <= maxR; r++ )
    {
        rule2 = inputfreq /(double)r;
        if ( (rule2 < 20000.0) || (rule2 > 100000000.0) )
        {
//          DbgPrint("Rule2(r=%d): 20,000<%f<100000000\n",r,rule2);
            continue;   //next r
        }

        od=8232;
        while(od > 1)
        {
            //set starting VCO setting with output freq just below target
            V_Divstart = (int) (((desired - (allowable_error) ) * r * od) / (inputfreq));

            //check if starting VCO setting too low
            if (V_Divstart < min_V)
                V_Divstart = min_V;

            //check if starting VCO setting too high
            else if (V_Divstart > max_V)
                V_Divstart = max_V;

            /** Loop thru VCO divide settings**/
            //Loop through all VCO divider ratios
            for( v = V_Divstart; v <= max_V; v++ ) //Check all Vco divider settings
            {
                rule1 = (inputfreq * ((double)v / (double)r) );

                        if(od==2)
                {
                    if( (rule1 < 90000000.0) || (rule1 > 540000000.0)  )
                    {
                        continue;   //next VCO_Div
                    }
                }
                else if(od==3)
                {
                    if( (rule1 < 90000000.0) || (rule1 > 720000000.0)  )
                    {
                        continue;   //next VCO_Div
                    }
                }
                else if( (od>=38) && (od<=1029) )
                {
                    if( (rule1 < 90000000.0) || (rule1 > 570000000.0)  )
                    {
                        continue;   //next VCO_Div
                    }
                }
                else
                {
                if( (rule1 < 90000000.0) || (rule1 > 730000000.0)  )
                {
//                  printf("Rule1: 90MHz<%f<730MHz\n",rule1);
                    continue;   //next VCO_Div
                    }
                }

                freq = (inputfreq * ((double)v / ((double)r * (double)od)));
                freq_err    = fabs(freq - desired) ; //Hz

                if ((freq_err) > allowable_error)
                {
                    continue; //next VCO_Div
                }
                else if((freq_err) <= allowable_error)
                {
                    count=0;
                    for(i=0;i<4;i++)
                    {
                        switch(i)
                        {
                        case 0:
                            Rs = 64000;
                            break;
                        case 1:
                            Rs = 52000;
                            break;
                        case 2:
                            Rs = 16000;
                            break;
                        case 3:
                            Rs = 4000;
                            break;
                        default:
                            return 1;
                        }

                        for(j=0;j<20;j++)
                        {
                            IRStruct.Rs=Rs;
                            switch(j)
                            {
                            case 0:
                                IRStruct.icp=1.25e-6;
                                IRStruct.icpnum=125;
                                break;
                            case 1:
                                IRStruct.icp=2.5e-6;
                                IRStruct.icpnum=250;
                                break;
                            case 2:
                                IRStruct.icp=3.75e-6;
                                IRStruct.icpnum=375;
                                break;
                            case 3:
                                IRStruct.icp=5.0e-6;
                                IRStruct.icpnum=500;
                                break;
                            case 4:
                                IRStruct.icp=6.25e-6;
                                IRStruct.icpnum=625;
                                break;
                            case 5:
                                IRStruct.icp=7.5e-6;
                                IRStruct.icpnum=750;
                                break;
                            case 6:
                                IRStruct.icp=8.75e-6;
                                IRStruct.icpnum=875;
                                break;
                            case 7:
                                IRStruct.icp=10.0e-6;
                                IRStruct.icpnum=1000;
                                break;
                            case 8:
                                IRStruct.icp=11.25e-6;
                                IRStruct.icpnum=1125;
                                break;
                            case 9:
                                IRStruct.icp=12.5e-6;
                                IRStruct.icpnum=1250;
                                break;
                            case 10:
                                IRStruct.icp=15.0e-6;
                                IRStruct.icpnum=1500;
                                break;
                            case 11:
                                IRStruct.icp=17.5e-6;
                                IRStruct.icpnum=1750;
                                break;
                            case 12:
                                IRStruct.icp=18.75e-6;
                                IRStruct.icpnum=1875;
                                break;
                            case 13:
                                IRStruct.icp=20.0e-6;
                                IRStruct.icpnum=2000;
                                break;
                            case 14:
                                IRStruct.icp=22.5e-6;
                                IRStruct.icpnum=2250;
                                break;
                            case 15:
                                IRStruct.icp=25.0e-6;
                                IRStruct.icpnum=2500;
                                break;
                            case 16:
                                IRStruct.icp=26.25e-6;
                                IRStruct.icpnum=2625;
                                break;
                            case 17:
                                IRStruct.icp=30.0e-6;
                                IRStruct.icpnum=3000;
                                break;
                            case 18:
                                IRStruct.icp=35.0e-6;
                                IRStruct.icpnum=3500;
                                break;
                            case 19:
                                IRStruct.icp=40.0e-6;
                                IRStruct.icpnum=4000;
                                break;
                            default:
                                DbgPrint("ICS30703: switch(j:icp) - You shouldn't get here! %d\n",j);
                            }//end switch(j)
//                          printf("Rs=%5d ",IRStruct.Rs);
//                          printf("Icp=%2.2f ",IRStruct.icp*10e5);

                            IRStruct.pdf = (inputfreq / (double)r) ;
//                          printf("pdf=%12.2f ",IRStruct.pdf);

                            IRStruct.nbw = ( ((double)IRStruct.Rs * IRStruct.icp * 310.0e6) / (2.0 * 3.14159 * (double)v) );
//                          printf("nbw=%15.3f ",IRStruct.nbw);

                            IRStruct.ratio = (IRStruct.pdf/IRStruct.nbw);

                            tempint = (int)(IRStruct.ratio*10.0);
                            if((IRStruct.ratio*10.0)-tempint>=0.0) tempint++;
                            IRStruct.ratio = (double)tempint/10.0;

//                          IRStruct.ratio = ceil(IRStruct.ratio*10.0); //these two statements make the
//                          IRStruct.ratio = IRStruct.ratio/10.0;       //ratio a little nicer to compare

//                          printf("ratio=%2.4f ",IRStruct.ratio);

                            IRStruct.df = ( ((double)IRStruct.Rs / 2) * (sqrt( ((IRStruct.icp * 0.093) / (double)v))) );
//                          printf("ndf=%12.3f\n",IRStruct.df);

                            count++;
                            if( (IRStruct.ratio>30) || (IRStruct.ratio<7) || (IRStruct.df>2.0) || (IRStruct.df<0.2) )
                            {
                                continue;
                            }
                            else
                            {
                                Results.target    = desired;
                                Results.freq      = freq;
                                Results.errorPPM     = freq_err / desired * 1.0e6 ;
                                Results.VCO_Div   = v;
                                Results.refDiv    = r;
                                Results.outDiv    = od;
                                Results.failed = 0;
                                goto finished;
                            }
                        }//end for(j=0;j<20;j++)
                    }//end for(i=0;i<4;i++)
                }
            }//end of for( v = V_Divstart; v < max_V; v++ )

            if(od<=1030)
                od--;
            else if(od<=2060)
                od=od-2;
            else if(od<=4120)
                od=od-4;
            else od=od-8;

        }//end of while(od <= 8232)
    }//end of for( r = maxR, *saved_result_num = 0; r >= minR; r-- )

    ppm++;
    if(ppm>requestedppm)
    {
        return 2;
    }
    else
    {
//      DbgPrint("ICS30703: increasing ppm to %d\n",ppm);
        goto increaseppm;
    }

finished:

    memcpy(theOne,&Results,sizeof(struct ResultStruct));

    memcpy(theOther,&IRStruct,sizeof(struct IcpRsStruct));
/*
    DbgPrint("ICS30703: Best result is \n");
    DbgPrint("\tRD = %4i,",Results.refDiv);
    DbgPrint(" VD = %4i,",Results.VCO_Div);
    DbgPrint(" OD = %4i,",Results.outDiv);
    DbgPrint(" freq_Hz = %ld,\n",(unsigned long)Results.freq);

    DbgPrint("\tRs = %5d, ",IRStruct.Rs);
    DbgPrint("Icp = %d, ",(unsigned long)(IRStruct.icp*1e6));
    //  DbgPrint("pdf = %d, ",(unsigned long)IRStruct.pdf);
    //  DbgPrint("nbw = %d, ",(unsigned long)IRStruct.nbw);
    DbgPrint("ratio = %d, ",(unsigned long)IRStruct.ratio);
    DbgPrint("df = %d\n",(unsigned long)IRStruct.df*1000);
*/
//  DbgPrint("ICS307-03 freq_Hz = %ld,\n",(unsigned long)Results.freq);
/*
first, choose the best dividers (V, R, and OD) with

1st key best accuracy
2nd key lowest reference divide
3rd key highest VCO frequency (OD)

then choose the best loop filter with

1st key best PDF/NBW ratio (between 7 and 30, 15 is optimal)
2nd key best damping factor (between 0.2 and 2, 0.7 is optimal)
*/
/* this is 1MHz
    progdata[19]=0xff;
    progdata[18]=0xff;
    progdata[17]=0xff;
    progdata[16]=0xf0;
    progdata[15]=0x00;
    progdata[14]=0x01;
    progdata[13]=0x43;
    progdata[12]=0x1a;
    progdata[11]=0x9c;
    progdata[10]=0x00;
    progdata[9]=0x00;
    progdata[8]=0x00;
    progdata[7]=0x00;
    progdata[6]=0x00;
    progdata[5]=0x00;
    progdata[4]=0x00;
    progdata[3]=0x00;
    progdata[2]=0x0c;
    progdata[1]=0xdf;
    progdata[0]=0xee;
    goto doitnow;
*/
/* 10 MHz
    progdata[19]=0xff;
    progdata[18]=0xff;
    progdata[17]=0xff;
    progdata[16]=0x00;
    progdata[15]=0x80;
    progdata[14]=0x01;
    progdata[13]=0x00;
    progdata[12]=0x66;
    progdata[11]=0x38;
    progdata[10]=0x00;
    progdata[9]=0x00;
    progdata[8]=0x00;
    progdata[7]=0x00;
    progdata[6]=0x00;
    progdata[5]=0x00;
    progdata[4]=0x00;
    progdata[3]=0x00;
    progdata[2]=0x07;
    progdata[1]=0x20;
    progdata[0]=0x12;
    goto doitnow;
*/

    progdata[19]=0xff;
    progdata[18]=0xff;
    progdata[17]=0xff;
    progdata[16]=0x00;
    progdata[15]=0x00;
    progdata[14]=0x00;
    progdata[13]=0x00;
    progdata[12]=0x00;
    progdata[11]=0x00;
    progdata[10]=0x00;
    progdata[9]=0x00;
    progdata[8]=0x00;
    progdata[7]=0x00;
    progdata[6]=0x00;
    progdata[5]=0x00;
    progdata[4]=0x00;
    progdata[3]=0x00;
    progdata[2]=0x00;
    progdata[1]=0x00;
    progdata[0]=0x00;

//  progdata[16]|=0x02; //enable CLK3
//  progdata[15]&=0xef; //CLK3 source select: 1=CLK1, 0=CLK1 before OD 
//  progdata[15]|=0x08; //CLK2 source select: 1=CLK1, 0=CLK1 before OD
//  progdata[15]|=0x40; //reference source is: 1=crystal, 0=clock
    progdata[14]|=0x01; //1=Power up, 0=power down feedback counter, charge pump and VCO
//  progdata[13]|=0x80; //enable CLK2
    progdata[13]|=0x40; //enable CLK1

    InputDivider = theOne->refDiv;
    VCODivider = theOne->VCO_Div;
    ChargePumpCurrent = theOther->icpnum;
    LoopFilterResistor = theOther->Rs;
    OutputDividerOut1 = theOne->outDiv;

    //InputDivider=2;
    //VCODivider=60;
    //OutputDividerOut1 = 45;
    //LoopFilterResistor=16000;
    //ChargePumpCurrent=3500;

    /* Table 1: Input Divider */

    if( (InputDivider==1)||(InputDivider==2) )
    {
        switch(InputDivider)
        {
        case 1:
            progdata[0]&=0xFC;
            progdata[1]&=0xF0;
            break;
        case 2:
            progdata[0]&=0xFC;
            progdata[0]|=0x01;
            progdata[1]&=0xF0;
            break;
        }
//  printf("1 0x%2.2X,0x%2.2X\n",progdata[1],progdata[0]);
    }
    else if( (InputDivider>=3) && (InputDivider<=17) )
    {
        temp=~(InputDivider-2);
        temp = (temp << 2);
        progdata[0]=(unsigned char)temp&0xff;

        progdata[0]&=0x3e;  //set bit 0 to a 0
        progdata[0]|=0x02;  //set bit 1 to a 1

//      printf("2 0x%2.2X,0x%2.2X\n",progdata[1],progdata[0]);
    }
    else if( (InputDivider>=18) && (InputDivider<=2055) )
    {
        temp=InputDivider-8;
        temp = (temp << 2);
        progdata[0]=(unsigned char)temp&0xff;
        progdata[1]=(unsigned char)((temp>>8)&0xff);

        progdata[0]|=0x03;  //set bit 0 and 1 to a 1

//      printf("3 0x%2.2X,0x%2.2X\n",progdata[1],progdata[0]);

    }
    else
        return 3;

    /* Table 2 VCO Divider */

    if( (VCODivider >= 12) && (VCODivider <=2055) )
    {
        temp=VCODivider-8;
        temp=(temp << 5);
        progdata[1]|=temp&0xff;
        progdata[2]|=((temp>>8)&0xff);
//      printf("4 0x%2.2X,0x%2.2X\n",progdata[2],progdata[1]);
    }
    else return 4;

    /* Table 4 Loop Filter Resistor */

    switch(LoopFilterResistor)
    {
    case 64000:
        progdata[11]&=0xf9; //bit 89 and 90 = 0
        break;
    case 52000:
        progdata[11]&=0xf9; //bit 89 = 0
        progdata[11]|=0x04; //bit 90 = 1
        break;
    case 16000:
        progdata[11]&=0xf9; //bit 90 = 0
        progdata[11]|=0x02; //bit 89 = 1
        break;
    case 4000:
        progdata[11]|=0x06; //bit 89 and 90 = 1
        break;
    default:
        return 5;
    }
//  printf("5 0x%2.2X\n",progdata[11]);

    /* Table 3 Charge Pump Current */

    switch(ChargePumpCurrent)
    {
    case 125:
        progdata[11]|=0x38;
        progdata[15]&=0x7f;
        progdata[16]&=0xfe;
//      printf("125\n");
        break;

    case 250:
        progdata[11]|=0x38;
        progdata[15]|=0x80;
        progdata[16]&=0xfe;
        break;

    case 375:
        progdata[11]|=0x38;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 500:
        progdata[11]|=0x38;
        progdata[15]|=0x80;
        progdata[16]|=0x01;
        break;

    case 625:
        progdata[11]|=0x18;
        progdata[11]&=0xdf;
        progdata[15]&=0x7f;
        progdata[16]&=0xfe;
        break;

    case 750:
        progdata[11]|=0x10;
        progdata[11]&=0xd7;
        progdata[15]&=0x7f;
        progdata[16]&=0xfe;
        break;

    case 875:
        progdata[11]|=0x08;
        progdata[11]&=0xcf;
        progdata[15]&=0x7f;
        progdata[16]&=0xfe;
        break;

    case 1000:
        progdata[11]&=0xc7;
        progdata[15]&=0x7f;
        progdata[16]&=0xfe;
        break;

    case 1125:
        progdata[11]|=0x28;
        progdata[11]&=0xef;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 1250:
        progdata[11]|=0x18;
        progdata[11]&=0xdf;
        progdata[15]|=0x80;
        progdata[16]&=0xfe;
        break;

    case 1500:
        progdata[11]|=0x28;
        progdata[11]&=0xef;
        progdata[15]|=0x80;
        progdata[16]|=0x01;
        break;

    case 1750:
        progdata[11]|=0x08;
        progdata[11]&=0xcf;
        progdata[15]|=0x80;
        progdata[16]&=0xfe;
        break;

    case 1875:
        progdata[11]|=0x18;
        progdata[11]&=0xdf;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 2000:
        progdata[11]&=0xc7;
        progdata[15]|=0x80;
        progdata[16]&=0xfe;
        break;

    case 2250:
        progdata[11]|=0x10;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 2500:
        progdata[11]|=0x18;
        progdata[11]&=0xdf;
        progdata[15]|=0x80;
        progdata[16]|=0x01;
        break;

    case 2625:
        progdata[11]|=0x08;
        progdata[11]&=0xcf;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 3000:
        progdata[11]&=0xc7;
        progdata[15]&=0x7f;
        progdata[16]|=0x01;
        break;

    case 3500:
        progdata[11]|=0x08;
        progdata[11]&=0xcf;
        progdata[15]|=0x80;
        progdata[16]|=0x01;
        break;

    case 4000:
        progdata[11]&=0xc7;
        progdata[15]|=0x80;
        progdata[16]|=0x01;
        break;

    default:
        return 6;
    }//end switch(j)
//  printf("6 0x%2.2X, 0x%2.2X, 0x%2.2X\n",progdata[16],progdata[15],progdata[11]);

    /* Table 5 Output Divider for Output 1 */
//OutputDividerOut1=38;
    if( (OutputDividerOut1 >= 2) && (OutputDividerOut1 <= 8232) )
    {
        switch(OutputDividerOut1)
        {
        case 2:
            progdata[11]&=0x7f;
            progdata[12]&=0x00;
            progdata[13]&=0xc0;
            break;

        case 3:
            progdata[11]|=0x80;
            progdata[12]&=0x00;
            progdata[13]&=0xc0;
            break;

        case 4:
            progdata[11]&=0x7f;
            progdata[12]|=0x04;
            progdata[13]&=0xc0;
            break;

        case 5:
            progdata[11]&=0x7f;
            progdata[12]|=0x01;
            progdata[13]&=0xc0;
            break;

        case 6:
            progdata[11]|=0x80;
            progdata[12]|=0x04;
            progdata[13]&=0xc0;
            break;

        case 7:
            progdata[11]|=0x80;
            progdata[12]|=0x01;
            progdata[13]&=0xc0;
            break;

        case 11:
            progdata[11]|=0x80;
            progdata[12]|=0x09;
            progdata[13]&=0xc0;
            break;

        case 9:
            progdata[11]|=0x80;
            progdata[12]|=0x05;
            progdata[13]&=0xc0;
            break;

        case 13:
            progdata[11]|=0x80;
            progdata[12]|=0x0d;
            progdata[13]&=0xc0;
            break;

        case 8: case 10: case 12: case 14: case 15: case 16: case 17:case 18: case 19:
        case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
            temp = ~(OutputDividerOut1-6);
            temp = (temp << 2);
            progdata[12] = (unsigned char)temp & 0x7f;

            progdata[11]&=0x7f;
            progdata[12]&=0xfe;
            progdata[12]|=0x02;
            progdata[13]&=0xc0;
            break;

        default:

            for(i=0;i<512;i++)
            {
                if( OutputDividerOut1 == ((((i+3)*2)+0)*(1)) )
                {
//                  printf("1 x=%d, y=0, z=0\n",i);
//                  DbgPrint("outputdivider1\n");
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xe7;
                    progdata[12]|=0x04;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+0)*(2)) )
                {
//                  printf("2 x=%d, y=0, z=1\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xef;
                    progdata[12]|=0x0c;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+0)*(4)) )
                {
//                  printf("3 x=%d, y=0, z=2\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xf7;
                    progdata[12]|=0x14;
                    break;
                }

                else if( OutputDividerOut1 == ( (((i+3)*2)+0)*(8)) )
                {
//                  printf("4 x=%d, y=0, z=3\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]|=0x1c;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+1)*(1)) )
                {
//                  printf("5 x=%d, y=1, z=0\n",i);
//                  DbgPrint("outputdivider5\n");
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xe3;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+1)*(2)) )
                {
//                  printf("6 x=%d, y=1, z=1\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xeb; //power of 1
                    progdata[12]|=0x08;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+1)*(4)) )
                {
//                  printf("7 x=%d, y=1, z=2\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xf7;
                    progdata[12]|=0x10;
                    break;
                }

                else if( OutputDividerOut1 == ((((i+3)*2)+1)*(8)) )
                {
//                  printf("8 x=%d, y=1, z=3\n",i);
                    temp = (i<< 5);
                    progdata[12]|=(temp & 0xff);
                    progdata[13]|=(temp >> 8)&0xff;

                    progdata[12]&=0xfb;
                    progdata[12]|=0x18;
                    break;
                }
            }
            progdata[11]|=0x80; //1
            progdata[12]&=0xfe; //0
            progdata[12]|=0x02; //1
        }
//      printf("0x%2.2x, 0x%2.2x, 0x%2.2x\n\n",progdata[13]&0x3f, progdata[12], progdata[11]&0x80);
    }
    else return 7;
//doitnow:

/*  progdata[15]|=0x03; //this will set
    progdata[14]|=0xc0; //the OD of clock 3
    progdata[11]&=0xbf; //to 2
*/
    return 0;

}
//end of GetICS30703Bits

#define STRB_BASE 0x00000008
#define DTA_BASE 0x00000001
#define CLK_BASE 0x00000002

int FastcomSetClockRateFSCC(struct fc_port *port, unsigned value)
{
    UINT32 orig_fcr_value = 0;
    UINT32 new_fcr_value = 0;
    int j = 0; // Must be signed because we are going backwards through the array
    int i = 0; // Must be signed because we are going backwards through the array
    unsigned strb_value = STRB_BASE;
    unsigned dta_value = DTA_BASE;
    unsigned clk_value = CLK_BASE;
    UINT32 *data = 0;
    unsigned data_index = 0;
    unsigned char clock_data[20];

    struct ResultStruct solutiona;  //final results for ResultStruct data calculations
    struct IcpRsStruct solutionb;   //final results for IcpRsStruct data calculations

    if (value < 200 || value > 270000000)
        return -1; // TODO: STATUS_NOT_SUPPORTED;

    memset(&solutiona,0,sizeof(struct ResultStruct));
    memset(&solutionb,0,sizeof(struct IcpRsStruct));

    GetICS30703Data(value, 2, &solutiona, &solutionb, clock_data);


#if 0 // TODO
#ifdef DISABLE_XTAL
    clock_data[15] &= 0xfb;
#else
    /* This enables XTAL on all cards except green FSCC cards with a revision
       greater than 6. Some old protoype SuperFSCC cards will need to manually
       disable XTAL as they are not supported in this driver by default. */
    if (fscc_port_get_PDEV(port) == 0x0f && fscc_port_get_PREV(port) <= 6)
        clock_data[15] &= 0xfb;
    else
        clock_data[15] |= 0x04;
#endif
#endif
    clock_data[15] |= 0x04;

    data = (UINT32 *)ExAllocatePoolWithTag(NonPagedPool, sizeof(UINT32) * 323, 'stiB');

    if (data == NULL) {
        SerialDbgPrintEx(TRACE_LEVEL_ERROR, DBG_PNP,
                        "--> ExAllocatePoolWithTag failed\n");
        return -1; // TODO: STATUS_UNSUCCESSFUL;
    }
    
    if (port->channel == 1) {
        strb_value <<= 0x08;
        dta_value <<= 0x08;
        clk_value <<= 0x08;
    }

    orig_fcr_value = READ_PORT_unsigned long(ULongToPtr(port->Bar2));

    data[data_index++] = new_fcr_value = orig_fcr_value & 0xfffff0f0;

    for (i = 19; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            int bit = ((clock_data[i] >> j) & 1);

            if (bit)
                new_fcr_value |= dta_value; /* Set data bit */
            else
                new_fcr_value &= ~dta_value; /* Clear clock bit */

            data[data_index++] = new_fcr_value |= clk_value; /* Set clock bit */
            data[data_index++] = new_fcr_value &= ~clk_value; /* Clear clock bit */
        }
    }

    new_fcr_value = orig_fcr_value & 0xfffff0f0;

    new_fcr_value |= strb_value; /* Set strobe bit */
    new_fcr_value &= ~clk_value; /* Clear clock bit */

    data[data_index++] = new_fcr_value;
    data[data_index++] = orig_fcr_value;

    WRITE_PORT_BUFFER_unsigned long(ULongToPtr(port->Bar2), data, data_index);
    
    ExFreePoolWithTag (data, 'stiB');

    return 0;
}

// Copied from old code. Needs a major code cleanup. TODO
// ICS307-02
#define MPIO_SDTA           0x01    //Each bit of MPIOLVL register
#define MPIO_SCLK           0x02
#define MPIO_SSTB           0x04

//WILL: Range 6MHz - 200MHz
int FastcomSetClockRatePCI(struct fc_port *port, unsigned rate)
{
    #define STARTWRD 0x1e05
    #define MIDWRD   0x1e04
    #define ENDWRD   0x1e00
    
    unsigned long bestVDW=1;    //Best calculated VCO Divider Word
    unsigned long bestRDW=1;    //Best calculated Reference Divider Word
    unsigned long bestOD=1;     //Best calculated Output Divider
    unsigned long result=0;
    unsigned long t=0;
    unsigned long i=0;
    unsigned long j=0;
    unsigned long tempValue=0;
    unsigned long temp=0;
    unsigned long vdw=1;        //VCO Divider Word
    unsigned long rdw=1;        //Reference Divider Word
    unsigned long od=1;         //Output Divider
    unsigned long lVDW=1;       //Lowest vdw
    unsigned long lRDW=1;       //Lowest rdw
    unsigned long lOD=1;        //Lowest OD
    unsigned long hVDW=1;       //Highest vdw
    unsigned long hRDW=1;       //Highest rdw
    unsigned long hOD=1;        //Highest OD
    unsigned long hi;       //initial range freq Max
    unsigned long low;  //initial freq range Min
    unsigned long check;        //Calculated clock
    unsigned long clk1;     //Actual clock 1 output
    unsigned long inFreq=18432000;  //Input clock frequency
    unsigned long range1=0;     //Desired frequency range limit per ics307 mfg spec.
    unsigned long range2=0;     ////Desired frequency range limit per ics307 mfg spec.
    unsigned char data = 0;
    unsigned char saved = 0;
    Punsigned char base_add;
    int odskip=0;



    if (rate < 6000000 || rate > 200000000)
        return -1; //TODO: return -EOPNOTSUPP;

    //DBGP("SerialSetClock Executing %d\n",*rate);

    base_add = port->Controller;
    
    //DBGP("baseaddress = %8.8x \n", base_add);
    

    saved = READ_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET);  //Save MPIO pin state
    data = saved;

    hi = (rate + (rate / 10));
    low = (rate - (rate / 10));

    od = 2;
    while (od <= 10)
    {
        switch(od)  //check maximum frequency with given OD for industrial temp chips
        {
        case 2:
            if(rate>180000000)
                odskip=1;
            break;
        case 3:
            if(rate>120000000)
                odskip=1;
            break;
        case 4:
            if(rate>90000000)
                odskip=1;
            break;
        case 5:
            if(rate>72000000)
                odskip=1;
            break;
        case 6:
            if(rate>60000000)
                odskip=1;
            break;
        case 7:
            if(rate>50000000)
                odskip=1;
            break;
        case 8:
            if(rate>45000000)
                odskip=1;
            break;
        case 9: //OD=9 not allowed
            odskip=1;
        case 10:
            if(rate>36000000)
                odskip=1;
            break;
        default:
            //DBGP("Case 1 Invalid OD = %ld.\n",od);
            return -1; // TODO: STATUS_UNSUCCESSFUL;
        }

        rdw = 1;
        while ( (rdw <= 127) && (odskip==0) )
        {
            vdw = 4;
            while (vdw <= 511)
            {
                check = (((inFreq * 2) / ((rdw + 2)*od)) * (vdw + 8) ); //calculate a check frequency
                range1 = ((inFreq * 2 * (vdw + 8)) / (rdw + 2));
                range2 = (inFreq / (rdw + 2));
                //Calculate operating ranges

                if ( ((range1) > 60000000) && ((range1) < 360000000) && ((range2) > 200000) )   //check operating ranges
                {
                    if (check == low)   //If this combination of variables == the current lowest set
                    {
                        if (lRDW > rdw) //If this combination has a lower rdw
                        {
                            lVDW=vdw;
                            lRDW=rdw;
                            lOD=od;
                            low=check;
                        }
                        else if ((lRDW == rdw) && (lVDW < vdw)) //If this combo has the same rdw then take the higher vdw
                        {
                            lVDW=vdw;
                            lRDW=rdw;
                            lOD=od;
                            low=check;
                        }

                    }
                    else if (check == hi)   //If this combination of variables == the current lowest set
                    {
                        if (hRDW > rdw) //If this combination has a lower rdw
                        {
                            hVDW=vdw;
                            hRDW=rdw;
                            hOD=od;
                            hi=check;
                        }
                        else if ((hRDW == rdw) && (hVDW < vdw)) //If this combo has the same rdw then take the higher vdw   
                        {
                            hVDW=vdw;
                            hRDW=rdw;
                            hOD=od;
                            hi=check;
                        }

                    }


                    if ((check > low) && (check < hi))      //if difference is less than previous difference
                    {
                        if (check > rate)    //if the new combo is larger than the rate, new hi combination
                        {
                            hi=check;
                            hVDW=vdw;
                            hRDW=rdw;
                            hOD=od;
                        }
                            
                        else    //if the new combo is less than the rate, new low combination
                        {
                            low=check;
                            lVDW = vdw;
                            lRDW = rdw;
                            lOD = od;
                        }
                }
            }

            vdw++;
        }

        rdw++;
    }
    odskip=0;
    od++;
    if (od==9)
        od++;
    }

    if ((hi - rate) < (rate - low))
    {
        bestVDW=hVDW;
        bestRDW=hRDW;
        bestOD=hOD;
        clk1=hi;
    }
    else
    {
        bestVDW=lVDW;
        bestRDW=lRDW;
        bestOD=lOD;
        clk1=low;
    }
    switch(bestOD)
    {
    case 2:
        result=0x11;
        break;
    case 3:
        result=0x16;
        break;
    case 4:
        result=0x13;
        break;
    case 5:
        result=0x14;
        break;
    case 6:
        result=0x17;
        break;
    case 7:
        result=0x15;
        break;
    case 8:
        result=0x12;
        break;
    case 10:
        result=0x10;
        break;
    default:
        //DBGP("Case 2 Invalid OD=%ld.\n",od);
        return -1; // TODO: STATUS_UNSUCCESSFUL;
        
    }
    range1 = (inFreq * 2 * ((bestVDW + 8)/(bestRDW + 2)));
    range2 = (inFreq/(bestRDW + 2));
    clk1 = (((inFreq * 2) / ((bestRDW + 2)*bestOD)) * (bestVDW + 8) );
//      DBGP(" 60 MHz < %d MHz < 360 MHz\n", range1);
//      DBGP(" 200 kHz < %d kHz \n", range2*1000);
    result<<=9;
    result|=bestVDW;
    result<<=7;
    result|=bestRDW;
//      result|=0x200000;   //1 = set levels to TTL, 0 = CMOS

//      DBGP("Clock bytes = %X\n",result);

    //DBGP("The rate clock control bytes are: %X\n",result);
//      DBGP("The rate frequendy is: %dHz\n", rate);
//      DBGP("The base clock is: %dHz\n", inFreq);
//      DBGP("High: %d    ",hi);
//      DBGP("Low: %d.\n",low);
    //DBGP("The best calculated clock is: %dHz\n", clk1);
//      DBGP("The best VDW = %d  RDW = %d  OD = %d.\n", bestVDW, bestRDW, bestOD);
//      DBGP("CH_ID = %d \n", extension->chid);


    /*********************** Begin clock set *************************/

    tempValue = (result & 0x00ffffff);


    for(i=0;i<24;i++)
    {
        //data bit set
        if ((tempValue & 0x800000)!=0) 
        {
        data |= MPIO_SDTA;
//              DBGP("1");
        }
        else 
        {
            data &= ~MPIO_SDTA;
//              DBGP("0");
        }
        WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,data);
        KeStallExecutionProcessor(20);  

                //clock high, data still there
                data |= MPIO_SCLK;
        WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,data);
        KeStallExecutionProcessor(20);  
                //clock low, data still there
                data &= MPIO_SDTA;
        WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,data);
        KeStallExecutionProcessor(20);  

        tempValue=tempValue<<1;
    }
//      DBGP("\n");
    
    data &= 0xF8;
    data |= MPIO_SSTB;      //strobe on
    WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,data);
    KeStallExecutionProcessor(20);  
    
    data &= ~MPIO_SSTB;     //all off
    WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,data);
    KeStallExecutionProcessor(20);

    WRITE_REGISTER_unsigned char(base_add+MPIOLVL_OFFSET,saved);   //Put MPIO pins back to saved state.
    return 0;
}
#endif

int pcie_set_baud_rate(struct fc_port *port, unsigned value)
{
    const unsigned input_freq = 125000000;
    const unsigned prescaler = 1;
    float divisor = 0;
    unsigned char orig_lcr = 0;
    unsigned char dlm = 0;
    unsigned char dll = 0;
    unsigned char dld = 0;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);

    iowrite8(orig_lcr | 0x80, port->addr + LCR_OFFSET);

    divisor = (float)input_freq / prescaler / (value * port->sample_rate);

    dlm = (int)divisor >> 8;
    dll = (int)divisor % 0xff;

    dld = ioread8(port->addr + DLD_OFFSET);
    dld &= 0xf0;
    dld |= (int)(((divisor - (int)divisor) * port->sample_rate) + 0.5);

    iowrite8(dlm, port->addr + DLM_OFFSET);
    iowrite8(dll, port->addr + DLL_OFFSET);
    iowrite8(dld, port->addr + DLD_OFFSET);

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

int fastcom_set_clock_rate(struct fc_port *port, unsigned value)
{
    int status;

    switch (fastcom_get_card_type(port)) {
    //case CARD_TYPE_PCI:
        //TODO: status = fastcom_set_clock_rate_pci(port, value);
    //    break;

    //case CARD_TYPE_FSCC:
        //TODO: status = fastcom_set_clock_rate_fscc(port, value);
    //    break;

    default:
        status = -EPROTONOSUPPORT;
        break;
    }

    if (status == 0) {
	    dev_info(port->device, "Clock rate = %i\n", value);

        port->clock_rate = value;
    }

    return status;
}

int fastcom_set_external_transmit_fscc(struct fc_port *port, unsigned num_chars)
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

void fastcom_get_external_transmit_fscc(struct fc_port *port, unsigned *num_chars)
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

int fastcom_get_external_transmit(struct fc_port *port, unsigned *num_chars)
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

int fastcom_set_external_transmit(struct fc_port *port, unsigned num_chars)
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
	    dev_info(port->device, "External transmit = %i\n", num_chars);

    return status;
}

int fastcom_enable_external_transmit(struct fc_port *port, unsigned num_chars)
{
    return fastcom_set_external_transmit(port, num_chars);
}

int fastcom_disable_external_transmit(struct fc_port *port)
{
    return fastcom_set_external_transmit(port, 0);
}

int fastcom_set_frame_length_fscc(struct fc_port *port, unsigned num_chars)
{
    unsigned char orig_lcr;

    if (num_chars == 0 || num_chars > 256)
        return -EINVAL;

    orig_lcr = ioread8(port->addr + LCR_OFFSET);
    iowrite8(0, port->addr + LCR_OFFSET); /* Ensure last LCR value is not 0xbf */

    iowrite8(FLR_OFFSET, port->addr + SPR_OFFSET); /* To allow access to FLR */
    iowrite8(num_chars - 1, port->addr + ICR_OFFSET); /* Actually writing to EXT through ICR */

    iowrite8(orig_lcr, port->addr + LCR_OFFSET);

    return 0;
}

void fastcom_get_frame_length_fscc(struct fc_port *port, unsigned *num_chars)
{
    unsigned char orig_lcr;
    unsigned char flr;

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
}

int fastcom_set_frame_length(struct fc_port *port, unsigned num_chars)
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
	    dev_info(port->device, "Frame length = %i\n", num_chars);

    return status;
}

int fastcom_get_frame_length(struct fc_port *port, unsigned *num_chars)
{
    switch (fastcom_get_card_type(port)) {
    case CARD_TYPE_FSCC:
        fastcom_get_frame_length_fscc(port, num_chars);
        break;

    default:
        return -EPROTONOSUPPORT;
    }

    return 0;
}

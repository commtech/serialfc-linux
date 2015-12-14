/*
	Copyright (C) 2014 Commtech, Inc.

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
	along with serialfc-linux.	If not, see <http://www.gnu.org/licenses/>.

*/

#include <linux/version.h>
#include "sysfs.h"
#include "utils.h" /* str_to_register_offset */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)


static ssize_t nine_bit_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_9bit(port, value);

	return count;
}

static ssize_t nine_bit_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned enabled = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_9bit(port, &enabled);

	return sprintf(buf, "%i\n", enabled);
}

static ssize_t echo_cancel_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_echo_cancel(port, value);

	return count;
}

static ssize_t echo_cancel_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned enabled = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_echo_cancel(port, &enabled);

	return sprintf(buf, "%i\n", enabled);
}

static ssize_t external_transmit_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_external_transmit(port, value);

	return count;
}

static ssize_t external_transmit_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_external_transmit(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t frame_length_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_frame_length(port, value);

	return count;
}

static ssize_t frame_length_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_frame_length(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t isochronous_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_isochronous(port, value);

	return count;
}

static ssize_t isochronous_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_isochronous(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t rs485_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_rs485(port, value);

	return count;
}

static ssize_t rs485_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned enabled = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_rs485(port, &enabled);

	return sprintf(buf, "%i\n", enabled);
}

static ssize_t rx_trigger_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_rx_trigger(port, value);

	return count;
}

static ssize_t rx_trigger_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_rx_trigger(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t sample_rate_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_sample_rate(port, value);

	return count;
}

static ssize_t sample_rate_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_sample_rate(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t tx_trigger_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_tx_trigger(port, value);

	return count;
}

static ssize_t termination_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_termination(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t termination_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_termination(port, value);

	return count;
}

static ssize_t tx_trigger_show(struct kobject *kobj,
							struct kobj_attribute *attr, char *buf)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

    fastcom_get_tx_trigger(port, &value);

	return sprintf(buf, "%i\n", value);
}

static ssize_t clock_rate_store(struct kobject *kobj,
							struct kobj_attribute *attr, const char *buf,
							size_t count)
{
	struct serialfc_port *port = 0;
	unsigned value = 0;
	char *end = 0;

	port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

	value = (unsigned)simple_strtoul(buf, &end, 16);

	fastcom_set_clock_rate(port, value);

	return count;
}

static ssize_t baud_rate_show(struct kobject *kobj,
                     struct kobj_attribute *attr, char *buf)
{
   struct serialfc_port *port = 0;
   unsigned long value = 0;

   port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

   fastcom_get_baud_rate(port, &value);

   return sprintf(buf, "%lu\n", value);
}

static ssize_t baud_rate_store(struct kobject *kobj,
                     struct kobj_attribute *attr, const char *buf,
                     size_t count)
{
   struct serialfc_port *port = 0;
   unsigned long value = 0;
   char *end = 0;

   port = (struct serialfc_port *)dev_get_drvdata((struct device *)kobj);

   value = simple_strtoul(buf, &end, 10);

   fastcom_set_baud_rate(port, value);

   return count;
}

static struct kobj_attribute nine_bit_attribute =
	__ATTR(nine_bit, SYSFS_READ_WRITE_MODE, nine_bit_show, nine_bit_store);

static struct kobj_attribute echo_cancel_attribute =
	__ATTR(echo_cancel, SYSFS_READ_WRITE_MODE, echo_cancel_show, echo_cancel_store);

static struct kobj_attribute external_transmit_attribute =
	__ATTR(external_transmit, SYSFS_READ_WRITE_MODE, external_transmit_show, external_transmit_store);

static struct kobj_attribute frame_length_attribute =
	__ATTR(frame_length, SYSFS_READ_WRITE_MODE, frame_length_show, frame_length_store);

static struct kobj_attribute isochronous_attribute =
	__ATTR(isochronous, SYSFS_READ_WRITE_MODE, isochronous_show, isochronous_store);

static struct kobj_attribute rs485_attribute =
	__ATTR(rs485, SYSFS_READ_WRITE_MODE, rs485_show, rs485_store);

static struct kobj_attribute rx_trigger_attribute =
	__ATTR(rx_trigger, SYSFS_READ_WRITE_MODE, rx_trigger_show, rx_trigger_store);

static struct kobj_attribute sample_rate_attribute =
	__ATTR(sample_rate, SYSFS_READ_WRITE_MODE, sample_rate_show, sample_rate_store);

static struct kobj_attribute termination_attribute =
	__ATTR(termination, SYSFS_READ_WRITE_MODE, termination_show, termination_store);

static struct kobj_attribute tx_trigger_attribute =
	__ATTR(tx_trigger, SYSFS_READ_WRITE_MODE, tx_trigger_show, tx_trigger_store);

static struct kobj_attribute clock_rate_attribute =
	__ATTR(clock_rate, SYSFS_READ_MODE, NULL, clock_rate_store);

static struct kobj_attribute baud_rate_attribute =
   __ATTR(baud_rate, SYSFS_READ_WRITE_MODE, baud_rate_show, baud_rate_store);

static struct attribute *settings_attrs[] = {
	&nine_bit_attribute.attr,
	&echo_cancel_attribute.attr,
	&external_transmit_attribute.attr,
	&frame_length_attribute.attr,
	&isochronous_attribute.attr,
	&rs485_attribute.attr,
	&rx_trigger_attribute.attr,
	&sample_rate_attribute.attr,
	&termination_attribute.attr,
	&tx_trigger_attribute.attr,
	&clock_rate_attribute.attr,
	&baud_rate_attribute.attr,
	NULL,
};

struct attribute_group port_settings_attr_group = {
	.name = "settings",
	.attrs = settings_attrs,
};


#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25) */


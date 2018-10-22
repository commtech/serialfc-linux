# ChangeLog

## [2.3.2](https://github.com/commtech/serialfc-linux/releases/tag/v2.3.2) (10/22/2018)
- Added copy_to/from_user to IOCTLs

## [2.3.1](https://github.com/commtech/serialfc-linux/releases/tag/v2.3.1) (06/30/2017)
- Added support of SFSCC LVDS PCIe
- Removed automatic sampling rate change in set_baud_rate
- Added ability to set custom baud rates on 422/X-PCIe via IOCTL and sysfs

## [2.3.0](https://github.com/commtech/serialfc-linux/releases/tag/v2.3.0) (02/13/2014)
- Fixed 950 trigger level issue causing missing interrupts
- Added support for setting the clock rate on 335 cards

## [2.2.1](https://github.com/commtech/serialfc-linux/releases/tag/v2.2.1) (01/10/2014)
- Added fixed baud rate placeholders for external library development

## [2.2.0](https://github.com/commtech/serialfc-linux/releases/tag/v2.2.0) (12/06/2013)
- Added support for isochronous mode 9 and 10
- Added check for FSCC support before frame length changes
- Added option to enable asynchronous support by default on FSCC cards
- Added support for setting the FSCC clock frequency using user generated clock bits
- Fixed enabling RS485 on the 2nd port of FSCC cards

## [2.1.1](https://github.com/commtech/serialfc-linux/releases/tag/v2.1.1) (11/19/2013)
- Improve port initialization to ensure trigger levels are programmable

## [2.1.0](https://github.com/commtech/serialfc-linux/releases/tag/v2.1.0) (10/04/2013)
- Fixed echo cancel and termination being set incorrectly on 422/X-PCIe cards
- Added sysfs support
- Fixed an issue where FSCC card conflicts could happen with the FSCC driver
- Added documentation and example files
- Added header file
- Improved installer

## [2.0.0](https://github.com/commtech/serialfc-linux/releases/tag/v2.0.0) (08/14/2013)
- Rewrite to allow /dev/serialfc* port configuration

## [1.1.0](https://github.com/commtech/serialfc-linux/releases/tag/v1.1.0) (12/04/2012)
- Added support for the new 422 PCIe cards
- Faster loading
- Better port initialization

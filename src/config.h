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

#define DEVICE_NAME "serialfc"

#define DEFAULT_RS485 0
#define DEFAULT_SAMPLE_RATE 16
#define DEFAULT_TX_TRIGGER_LEVEL 32
#define DEFAULT_RX_TRIGGER_LEVEL 32
#define DEFAULT_TERMINATION 1
#define DEFAULT_ECHO_CANCEL 0
#define DEFAULT_ISOCHRONOUS -1
#define DEFAULT_FRAME_LENGTH 1

#define DEFAULT_FSCC_ASYNC_MODE 0

#define SYSFS_READ_WRITE_MODE S_IWUGO | S_IRUGO
#define SYSFS_READ_MODE S_IRUGO

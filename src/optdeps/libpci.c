#include <stdbool.h>
#include <stdlib.h>

#include "optdeps.h"

#ifdef LIBPCI_EXISTS
#include <pci/pci.h>
#include <string.h>
#include "../config/config.h"
#endif // GLIB_EXISTS

void get_gpus(char **gpus) {
#ifdef LIBPCI_EXISTS
    // based on https://github.com/pciutils/pciutils/blob/master/example.c
    char device_class[256], namebuf[768];
    struct pci_dev *dev;
    struct pci_access *pacc = pci_alloc();		// get the pci_access structure;
    pci_init(pacc);		// initialize the PCI library
    pci_scan_bus(pacc);		// we want to get the list of devices
    int i = 0;
    for(dev=pacc->devices; dev; dev=dev->next)	{ // iterates over all devices
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);	// fill in header info
        pci_lookup_name(pacc, device_class, 256, PCI_LOOKUP_CLASS, dev->device_class);
        if(strcmp(device_class, "VGA compatible controller") == 0 || strcmp(device_class, "3D controller") == 0) {
            // look up the full name of the device
            if(config.gpu_index == 0) {
                gpus[i] = pci_lookup_name(pacc, namebuf+i*256, 256, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
                
                if(i < 2)
                    ++i;
                else
                    break;
            }
            else {
                if(i == config.gpu_index-1) {
                    gpus[0] = pci_lookup_name(pacc, namebuf, 256, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
                    break;
                }
                if(i < 2)
                    ++i;
                else
                    break;
            }
        }
    }

    pci_cleanup(pacc);  // close everything
#else

#endif // GLIB_EXISTS
}

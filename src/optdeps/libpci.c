#include <stdlib.h>
#include <string.h>

#include "optdeps.h"
#include "../config/config.h"
#include "../info/info.h"

#ifdef LIBPCI_EXISTS
#include <pci/pci.h>
#else
#include "../utils/utils.h"
#include "../utils/wrappers.h"
#endif // LIBPCI_EXISTS

#define CLASS0 "VGA compatible controller"
#define CLASS1 "3D controller"

// use libpci directly if possible, else try with lspci
void get_gpus(char **gpus) {
#ifdef LIBPCI_EXISTS
    // based on https://github.com/pciutils/pciutils/blob/master/example.c
    char device_class[DEST_SIZE];
    static char namebuf[DEST_SIZE * 3];
    struct pci_dev *dev;
    struct pci_access *pacc = pci_alloc(); // get the pci_access structure;
    pci_init(pacc);                        // initialize the PCI library
    pci_scan_bus(pacc);                    // we want to get the list of devices
    int i = 0;

    for(dev = pacc->devices; dev; dev = dev->next) {                          // iterates over all devices
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS); // fill in header info
        pci_lookup_name(pacc, device_class, DEST_SIZE, PCI_LOOKUP_CLASS, dev->device_class);
        if(strcmp(device_class, CLASS0) == 0 || strcmp(device_class, CLASS1) == 0) {
            // look up the full name of the device
            if(config.gpu_index == 0) {
                gpus[i] = pci_lookup_name(pacc, namebuf + i * DEST_SIZE, DEST_SIZE, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);

                if(i < 2)
                    ++i;
                else
                    break;
            } else {
                if(i == config.gpu_index - 1) {
                    gpus[0] = pci_lookup_name(pacc, namebuf, DEST_SIZE, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
                    break;
                }
                if(i < 2)
                    ++i;
                else
                    break;
            }
        }
    }

    pci_cleanup(pacc); // close everything
#else
    char *temp_gpus[] = {NULL, NULL, NULL};

    char *lspci = malloc(0x2000);
    if(lspci == NULL)
        return;
    char *args[] = {"lspci", "-mm", NULL};
    exec_cmd(lspci, 0x2000, args);
    char *current = lspci;
    char *ptr;

    for(int i = 0; i < 3; ++i) {
        ptr = strstr(current, CLASS0);
        if(ptr == NULL) {
            ptr = strstr(current, CLASS1);
            if(ptr == NULL) {
                break;
            }
        }
        char *current = ptr;

        for(int j = 0; j < 4; ++j) {
            current = strchr(current, '"');
            if(current == NULL)
                break;
            ++current;

            /* class" "manufacturer" "name"
            *  "manufacturer" "name"
            * manufacturer" "name"
            *  "name"
            * name"
            */
        }

        ptr = strchr(current, '"'); // name
        if(ptr == NULL)
            break;
        *ptr = 0;

        temp_gpus[i] = current;
        current = ptr + 1;
    }

    if(config.gpu_index == 0) {
        static char gpus_buf[768];
        for(int i = 0; i < 3 && temp_gpus[i] != NULL; ++i) {
            safe_strncpy(gpus_buf + i * DEST_SIZE, temp_gpus[0], DEST_SIZE);

            gpus[0] = gpus_buf + i * DEST_SIZE;
        }
    } else if(temp_gpus[config.gpu_index - 1] != NULL) {
        static char gpu_buf[DEST_SIZE];
        safe_strncpy(gpu_buf, temp_gpus[config.gpu_index - 1], DEST_SIZE);

        gpus[0] = gpu_buf;
    }

    free(lspci);
#endif // LIBPCI_EXISTS
}

#include <stdio.h>
#include <locale>
#include <libusb-1.0/libusb.h>

// std library
#include <vector>
#include <algorithm>


const bool debInfo = 1;         // вывод отладочной информации

using namespace std;

static void print_devs()
{
    libusb_device **devs;
    size_t cnt = libusb_get_device_list(NULL, &devs);
    printf("Number of devices in the system = %d:\n", (int)cnt);
    if (cnt < 0) return;


    vector<int> listDeviceMT48;
    libusb_device *dev;
    int vid = 0x04b4;
    int pid = 0x1002;

    int i = 0;
    int cntDev = 0;

    while ((dev = devs[i++]) != NULL) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            fprintf(stderr, "failed to get device descriptor");
            return;
        }
        if (debInfo){
            printf("%04x:%04x (bus %d, device %d)",
                   desc.idVendor, desc.idProduct,
                   libusb_get_bus_number(dev), libusb_get_device_address(dev));
            printf("\n");
        }

        cntDev++;
        if ( desc.idVendor == vid && desc.idProduct == pid){
            listDeviceMT48.push_back(desc.bcdDevice);
        }

    }
    printf("=============================================\n");
    printf("All devices = %d, MT-48 = %d\n", cntDev, listDeviceMT48.size());

    if ( listDeviceMT48.size() > 0 ){
        printf("\nNumbers MT48 in the system: \n");
        sort(listDeviceMT48.begin(), listDeviceMT48.end());
        vector<int>::const_iterator iter;
        int cntMT48 = 1;
        for(iter = listDeviceMT48.begin(); iter != listDeviceMT48.end(); iter++, cntMT48++){
            printf("%i ", *iter);
            if (cntMT48%6 == 0)
                printf("\n");
        }
        printf("\n\n");
    }

    libusb_free_device_list(devs, 1);
}

void print_help(void)
{
    printf("Valid commands are:\n");
    printf("\t L - print list of devices\n");
    printf("\t H - print help\n");
    printf("\t Q, <ESC> - exit programm\n");
}

int main(void)
{
    int r = libusb_init(NULL);
    if (r < 0)
        return r;

    print_help();

    char cmd = 0;
    while (1){
        cmd = toupper( getchar() );
        if (cmd < '0') continue;

        switch (cmd)
        {
            case 'L':
                print_devs();
                break;
            case 'H':
                print_help();
                break;
            case 'Q': case 033:
                libusb_exit(NULL);
                return 0;
            default:
                printf("Invalid command %c (%d)\n", cmd, cmd);
                break;
        }
    }

    return 0;
}

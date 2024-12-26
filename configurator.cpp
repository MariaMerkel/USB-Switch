#include <string>

#include "CH9326DLL.h"

int main () {
    if (CH9326DllInt() != true) {
        return -1;
    }

    HANDLE handle = CH9326OpenDevice(0x1A86, 0xE010);
    if (handle == INVALID_HANDLE_VALUE) {
        return -2;
    }

    if (!CH9326SetDeviceCfg(handle,
                           0x337B,
                           0x4001,
                           0x32,
                           "Blahaj Ltd",
                           10,
                           "USB Switch Button",
                           17,
                           "123456",
                           6)) {
        return -3;
    }

    return 0;
}
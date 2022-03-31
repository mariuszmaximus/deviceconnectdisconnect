#ifndef _deviceconnectdisconnect_udev_h
#define _deviceconnectdisconnect_udev_h



#if defined(_WIN32)
// KOD WINDOWS 
#else
#include "deviceconnectdisconnect.h"
// KOD LINUX 
class DeviceConnectDisconnect_udev
{
  public:
    static void assign_callback(CallbackAfterDeviceChange p_callback);
    static void assign_filter(device_filter filter);
    static void start();
    static void stop();
};
#endif //defined(_WIN32)



#endif
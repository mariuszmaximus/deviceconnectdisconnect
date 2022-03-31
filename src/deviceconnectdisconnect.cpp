#include "deviceconnectdisconnect.h"

#if defined(_WIN32)
#include "deviceconnectdisconnect_win.h"
#else
#include "deviceconnectdisconnect_udev.h"
#endif


void DeviceConnectDisconnect::assign_callback(CallbackAfterDeviceChange p_callback, int timer)
{
#if defined(_WIN32)
    DeviceConnectDisconnect_win::assign_callback(p_callback, timer);
#else
    DeviceConnectDisconnect_udev::assign_callback(p_callback);
#endif
    return;
}

void DeviceConnectDisconnect::assign_filter(device_filter filter)
{
#if defined(_WIN32)
    DeviceConnectDisconnect_win::assign_filter(filter);
#else
    DeviceConnectDisconnect_udev::assign_filter(filter);
#endif
    return;
}

void DeviceConnectDisconnect::start()
{
#if defined(_WIN32)
    DeviceConnectDisconnect_win::start();
#else
    DeviceConnectDisconnect_udev::start();
#endif
    return;
}

void DeviceConnectDisconnect::stop()
{
#if defined(_WIN32)
    DeviceConnectDisconnect_win::stop();
#else
    DeviceConnectDisconnect_udev::stop();
#endif
    return;
}

#ifndef _deviceconnectdisconnect_H
#define _deviceconnectdisconnect_H

// rodzaj zdarzenia 
enum device_event
{
  device_event_disconnect,
  device_event_connect
};

// Filtr urzadzenia 
// Linux: udev
//      str1 = "usb"            klasa 
//      str2 = "usb_interface"  rodzaj
//      str3 = "403/601f/0"     identyfikator sprzetu 
// Windows: RegisterDeviceNotification + WM_DEVICECHANGE
//      str1 = GUID np. {d1e8fe6a-ab75-4d9e-97d2-06fa22c7736c}
// TCP/IP
//      str1 = url
struct device_filter
{
    const char *str1;
    const char *str2;
    const char *str3;
};

typedef int (*CallbackAfterDeviceChange)(device_event, const char *); // ,

class DeviceConnectDisconnect
{
  private:
    device_filter m_filter{0};
    CallbackAfterDeviceChange _callback{0};
  public:
    static void assign_callback(CallbackAfterDeviceChange p_callback);
    static void assign_filter(device_filter filter);
    static void start();
    static void stop();
};


#endif
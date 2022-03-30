#include <cstdio>  // printf
#include "deviceconnectdisconnect.h"   // DeviceConnectDisconnect


int callback_implemenation(device_event event, const char *syspath)
{
  if (event == device_event_connect)
    printf("callback_implemenation device_event_connect :%s\n", syspath);
  if (event == device_event_disconnect)
    printf("callback_implemenation device_event_disconnect :%s\n", syspath);
  return 0;
}


int main(int, char **)
{
#if defined(_WIN32)  
  device_filter filter={"{d1e8fe6a-ab75-4d9e-97d2-06fa22c7736c}"};
#else
  device_filter filter={"usb","usb_interface","403/601f/0"};
#endif  

  DeviceConnectDisconnect::assign_filter(filter);
  DeviceConnectDisconnect::assign_callback(callback_implemenation);
  DeviceConnectDisconnect::start();

  printf("nacisnij enter\n");
  getchar();

  DeviceConnectDisconnect::stop();  // stop thread if exists 
  printf("nacisnij enter II\n");
  getchar();
  return 0;
}

#ifndef deviceconnectdisconnect_win_h
#define deviceconnectdisconnect_win_h

#if defined(_WIN32)

// KOD WINDOWS 
class DeviceConnectDisconnect_win
{
  public:
    static void assign_callback(CallbackAfterDeviceChange p_callback,  int timer = 0);
    static void assign_filter(device_filter filter);
    static void start();
    static void stop();
};

#else  // defined(_WIN32)
// KOD LINUX 
#endif  // defined(_WIN32)

#endif // deviceconnectdisconnect_win_h


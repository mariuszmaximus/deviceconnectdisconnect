
#if defined(_WIN32)

#include <Windows.h>
#include <devguid.h>
#include <Dbt.h>
#include <iostream>

#include <thread>

#include "deviceconnectdisconnect.h"
#include "deviceconnectdisconnect_win.h"

CallbackAfterDeviceChange _callback_dcd = 0;
int _timerTime = 0;
// GUID: FTD3XX {d1e8fe6a-ab75-4d9e-97d2-06fa22c7736c}
GUID GUID_HARDWARE{0xd1e8fe6a, 0xab75, 0x4d9e, {0x97, 0xd2, 0x06, 0xfa, 0x22, 0xc7, 0x73, 0x6c}};
WNDPROC wndProc = NULL;
HDEVNOTIFY devNotify = NULL;
WNDCLASSEX WindowClassEx;
std::thread *_pProcessingThread;
HWND hWnd = 0;

#define TIMER_ID 1

BOOL DoRegisterDeviceInterfaceToHwnd(
    IN GUID InterfaceClassGuid,
    IN HWND hWnd,
    OUT HDEVNOTIFY *hDeviceNotify)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = InterfaceClassGuid;

    *hDeviceNotify = RegisterDeviceNotification(
        hWnd,                       // events recipient
        &NotificationFilter,        // type of device
        DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );

    if (NULL == *hDeviceNotify)
    {
        return FALSE;
    }

    return TRUE;
}

INT_PTR WINAPI WinProcCallback(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT lRet = 1;
    static HDEVNOTIFY hDeviceNotify;

    switch (message)
    {
    case WM_CREATE:
        if (!DoRegisterDeviceInterfaceToHwnd(
                GUID_HARDWARE,
                hWnd,
                &hDeviceNotify))
        {
            printf("DoRegisterDeviceInterfaceToHwnd -> FAIL !\n");
            ExitProcess(1);
        }
        break;

    case WM_DEVICECHANGE:

        switch (wParam)
        {
        case DBT_DEVICEARRIVAL:
        {
            if(_timerTime ==0)
            {
                if (_callback_dcd)
                {
                    _callback_dcd(device_event_connect, "Device Connected");
                }
            }
            else
            {
                SetTimer(hWnd, TIMER_ID, _timerTime, (TIMERPROC)NULL);
            }
        }
        break;

        case DBT_DEVICEREMOVECOMPLETE:
        {
            if (_callback_dcd)
            {
                _callback_dcd(device_event_disconnect, "Device DisConnected");
            }
            if(_timerTime)
            {
                KillTimer(hWnd, TIMER_ID);
            }
        }
        break;

        case DBT_DEVNODES_CHANGED:
            break;
        default:
            break;
        }
        break;

    case WM_CLOSE:
        UnregisterDeviceNotification(hDeviceNotify);
        DestroyWindow(hWnd);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case TIMER_ID:
            // process the 10-second timer
            KillTimer(hWnd, TIMER_ID);
            if (_callback_dcd)
            {
                _callback_dcd(device_event_connect, "Device Connected");
            }

            return 0;
        }
        break;

    default:
        lRet = DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return lRet;
}

BOOL createHiddenWindow(HINSTANCE hInstanceExe)
{

    HINSTANCE histance = hInstanceExe;
    ZeroMemory(&WindowClassEx, sizeof(WNDCLASSEX));
    /*********/
    WindowClassEx.cbSize = sizeof(WNDCLASSEX);
    WindowClassEx.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
    WindowClassEx.lpfnWndProc = reinterpret_cast<WNDPROC>(WinProcCallback);
    WindowClassEx.hInstance = histance;
    WindowClassEx.lpszClassName = "dummy";
    /*********/
    if (RegisterClassEx(&WindowClassEx) != 0)
    {
        return TRUE;
    }
    return FALSE;
}

void DeviceConnectDisconnect_win::assign_callback(CallbackAfterDeviceChange p_callback, int timer)
{
    _timerTime = timer;
    _callback_dcd = p_callback;
};

// https://gist.github.com/vincenthsu/8fab51834e3a04074a57
GUID StringToGuid(const std::string &str)
{
    GUID guid{0};
    sscanf(str.c_str(),
           "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
           &guid.Data1, &guid.Data2, &guid.Data3,
           &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
           &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);

    return guid;
}

void DeviceConnectDisconnect_win::assign_filter(device_filter filter)
{
    GUID_HARDWARE = StringToGuid(filter.str1);
#ifdef REPLACE_GUID_HARDWARE
    printf("%s",REPLACE_GUID_HARDWARE);
    GUID_HARDWARE = GUID_DEVCLASS_USB;
#endif
};

void messageReceiver(HWND hWnd)
{
    MSG msg;
    int retVal;

    while ((retVal = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (retVal == -1)
        {
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void Execute()
{
    HINSTANCE hInstanceExe = GetModuleHandle(nullptr);
    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
    memset(&notificationFilter, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE));

    notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    notificationFilter.dbcc_classguid = GUID_HARDWARE;

    devNotify =
        RegisterDeviceNotification(GetConsoleWindow(),
                                   &notificationFilter,
                                   DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);

    if (createHiddenWindow(hInstanceExe))
    {
        hWnd = CreateWindowEx(
            WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
            WindowClassEx.lpszClassName,
            NULL,
            WS_OVERLAPPEDWINDOW, 
            CW_USEDEFAULT, 0,
            0, 0,
            NULL, NULL,
            hInstanceExe,
            NULL);

        ShowWindow(hWnd, SW_HIDE);
        messageReceiver(hWnd);
    }
};

void DeviceConnectDisconnect_win::start()
{
    if (!_pProcessingThread)
    {
        _pProcessingThread = new std::thread(Execute);
    }
}

void DeviceConnectDisconnect_win::stop()
{
    if (hWnd)
    {
        PostMessage(hWnd, WM_QUIT, 0, 0);
    }
    if (_pProcessingThread)
    {
        _pProcessingThread->join();
    }
};

#else
// KOD LINUX
#endif
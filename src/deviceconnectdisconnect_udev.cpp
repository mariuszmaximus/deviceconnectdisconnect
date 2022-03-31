
#if defined(_WIN32)
// KOD WINDOWS 
#else
// KOD LINUX 

#include <thread>
#include <libudev.h>
#include <string.h> // strcmp
#include "deviceconnectdisconnect.h"
#include "deviceconnectdisconnect_udev.h"

device_filter _filter{0};
CallbackAfterDeviceChange _callback{0};

std::thread *_pProcessingThread;

// ustawienie 1 konczy watek
int stop_thread = 0;

void udev_loop(int *p_stop)
{

    udev *v_udev = udev_new();
    udev_monitor *v_mon = udev_monitor_new_from_netlink(v_udev, "udev");

    int v_result = udev_monitor_filter_add_match_subsystem_devtype(v_mon, _filter.str1, _filter.str2);
    // int v_result = udev_monitor_filter_add_match_subsystem_devtype(v_mon, "usb", "usb_interface");
    // printf("result udev_monitor_filter_add_match_subsystem_devtype: %d\n", v_result);

    v_result = udev_monitor_enable_receiving(v_mon);
    // printf("result: %d\n", v_result);

    int v_fd = udev_monitor_get_fd(v_mon);
    // printf("v_fd: %d\n", v_fd);

    char *v_syspath = NULL; // kopia  nazwy urzadzenia dla callback
    int was_connect = 0;

    std::chrono::steady_clock::time_point m_tstart;

    while (!*p_stop)
    {
        udev_device *v_dev = udev_monitor_receive_device(v_mon);
        if (v_dev)
        {

            struct udev_list_entry *list_entry;

            // udev_list_entry_foreach(list_entry, udev_device_get_devlinks_list_entry(v_dev))
            //         printf("S: %s\n", udev_list_entry_get_name(list_entry) );

            // udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(v_dev))
            //         printf("E: %s=%s\n", udev_list_entry_get_name(list_entry), udev_list_entry_get_value(list_entry));

            // iteracja po liscie wlasciwosci
            int product_ok = 0;
            udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(v_dev))
            {
                // poszukuje produktu
                const char *name = udev_list_entry_get_name(list_entry);
                if (strcmp(name, "PRODUCT") == 0)
                {
                    const char *value = udev_list_entry_get_value(list_entry);
                    if (strcmp(value, _filter.str3) == 0) // if (strcmp(value, "403/601f/0") == 0)
                        product_ok = 1;
                }
                //
            };

            // //udev_list_entry =   udev_device_get_properties_list_entry(v_dev);

            // const char* vid = udev_device_get_sysattr_value(v_dev, "idVendor");
            // const char* pid = udev_device_get_sysattr_value(v_dev, "idProduct");

            // const char* product = udev_device_get_sysattr_value(v_dev, "product");
            // printf("product == %s", product);

            // wykonuje tylko dla wybranego urzadzenia
            if (product_ok)
            {
                // kopia dla callback !!!
                if (v_syspath)
                    free(v_syspath);
                v_syspath = strdup(udev_device_get_syspath(v_dev));

                const char *action_cstr = udev_device_get_action(v_dev);
                if (strcmp(action_cstr, "add") == 0)
                {
                    // ustawiam flage oraz
                    was_connect = 1;
                    m_tstart = std::chrono::steady_clock::now();
                }
                else if (strcmp(action_cstr, "remove") == 0)
                {
                    was_connect = 0;
                    if (_callback)
                        _callback(device_event_disconnect, udev_device_get_syspath(v_dev));
                }
            }

            //   const char *str;
            //   str = udev_device_get_sysattr_value(v_dev, "idVendor");
            // if (!str)
            // {
            // 	printf("udev_device_get_sysattr_value() failed");
            // 	return 0;
            // }

#if 0

    //int vidx=0;  
    //sscanf(vid, "%x", &vidx); 
    //printf("sssssssssssssss =%s=\n",vid);

    //writeln('Device '+inttostr(integer(v_dev)));
    printf("Got Device\n");
    printf("   Devpath: %s\n", udev_device_get_devpath(v_dev));
    printf("   Subsystem: %s\n", udev_device_get_subsystem(v_dev));
    printf("   Devtype: %s\n", udev_device_get_devtype(v_dev));
    printf("   Syspath: %s\n", udev_device_get_syspath(v_dev));
    printf("   Sysname: %s\n", udev_device_get_sysname(v_dev));
    printf("   Sysnum: %s\n", udev_device_get_sysnum(v_dev));
    printf("   Node: %s\n",   udev_device_get_devnode(v_dev));

    printf("   Driver: %s\n",  udev_device_get_driver(v_dev));
    printf("   DevNum: %s\n",  udev_device_get_devnum(v_dev));


    printf("   Action: %s\n", udev_device_get_action(v_dev));

    printf("   vid: %s\n", udev_device_get_sysattr_value(v_dev, "idVendor"));
    printf("   pid: %s\n", udev_device_get_sysattr_value(v_dev, "idProduct"));

#endif

            udev_device_unref(v_dev);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

            // przeskoczenie problemu wazanego z reenumeracja USB
            // urzadzenie nadal podlaczone i mija okreslony czas
            if (was_connect)
            {
                std::chrono::steady_clock::time_point m_tstop = std::chrono::steady_clock::now();
                auto t_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_tstop - m_tstart);
                float t_s = (float)t_elapsed.count() * 0.001;
                if (t_s > 1.2)
                {

                    if (_callback)
                        _callback(device_event_connect, v_syspath);

                    was_connect = 0;
                }
            }
        }
    }
}

void Execute()
{
    udev_loop(&stop_thread);
}

void DeviceConnectDisconnect_udev::assign_filter(device_filter filter)
{
    _filter = filter;
}

void DeviceConnectDisconnect_udev::assign_callback(CallbackAfterDeviceChange p_callback)
{
    _callback = p_callback;
}

void DeviceConnectDisconnect_udev::start()
{
    if (!_pProcessingThread)
    {
        _pProcessingThread = new std::thread(Execute);
    }
}

void DeviceConnectDisconnect_udev::stop()
{
    if (_pProcessingThread)
    {
        stop_thread = 1;
        _pProcessingThread->join();
        delete _pProcessingThread;
        _pProcessingThread = 0;
    }
}

#endif //defined(_WIN32)

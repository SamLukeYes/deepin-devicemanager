#include "DeviceGenerator.h"
#include "CmdTool.h"
#include "../DeviceManager/DeviceManager.h"
#include "../DeviceManager/DeviceMouse.h"
#include "../DeviceManager/DeviceCpu.h"
#include "../DeviceManager/DeviceGpu.h"
#include "../DeviceManager/DeviceStorage.h"
#include "../DeviceManager/DeviceMemory.h"
#include "../DeviceManager/DeviceMonitor.h"
#include "../DeviceManager/DeviceBios.h"
#include "../DeviceManager/DeviceAudio.h"
#include "../DeviceManager/DeviceBluetooth.h"
#include "../DeviceManager/DeviceNetwork.h"
#include "../DeviceManager/DeviceKeyboard.h"
#include "../DeviceManager/DeviceImage.h"
#include "../DeviceManager/DeviceOthers.h"
#include "../DeviceManager/DeviceComputer.h"
#include "../DeviceManager/DevicePower.h"
#include "../DeviceManager/DeviceCdrom.h"
#include "../DeviceManager/DevicePrint.h"

#include <QDebug>

DeviceGenerator::DeviceGenerator(QObject *parent)
    : QObject(parent)
{

}
DeviceGenerator::~DeviceGenerator()
{

}

void DeviceGenerator::generatorComputerDevice()
{
    const QList<QMap<QString, QString> >  &cmdInfo = DeviceManager::instance()->cmdInfo("cat_os_release");
    DeviceComputer device;

    // home url
    if (cmdInfo.size() > 0) {
        QString value = cmdInfo[0]["HOME_URL"];
        device.setHomeUrl(value.replace("\"", ""));
    }

    // name type
    const QList<QMap<QString, QString> >  &sysInfo = DeviceManager::instance()->cmdInfo("lshw_system");
    if (sysInfo.size() > 0) {
        device.setType(sysInfo[0]["description"]);
        device.setVendor(sysInfo[0]["vendor"]);
        device.setName(sysInfo[0]["product"]);
    }

    // setOsDescription
    QString os = "UOS 20";
    DSysInfo::UosEdition type = DSysInfo::uosEditionType();
    if (DSysInfo::UosProfessional == type) {
        os =  "UOS 20";
    } else if (DSysInfo::UosHome == type) {
        os =  "UnionTech OS Desktop 20 Home";
    } else if (DSysInfo::UosCommunity == type) {
        os =  "Deepin 20";
    } else if (DSysInfo::UosEnterprise == type) {
        os =  "UnionTech OS Server 20 Enterprise";
    } else if (DSysInfo::UosEnterpriseC == type) {
        os =  "UnionTech OS Server 20 Enterprise-C";
    } else if (DSysInfo::UosEuler == type) {
        os =  "UnionTech OS Server 20 Euler";
    }
    device.setOsDescription(os);


    // os
    const QList<QMap<QString, QString> >  &verInfo = DeviceManager::instance()->cmdInfo("cat_version");
    if (verInfo.size() > 0) {
        QString info = verInfo[0]["OS"].trimmed();
        info = info.trimmed();
        QRegExp reg("\\(gcc [\\s\\S]*(\\([\\s\\S]*\\))\\)", Qt::CaseSensitive);
        int index = reg.indexIn(info);
        if (index != -1) {
            QString tmp = reg.cap(0);
            info.remove(tmp);
            info.insert(index, reg.cap(1));
        }
        device.setOS(info);
    }
    DeviceManager::instance()->addComputerDevice(device);
}

void DeviceGenerator::generatorCpuDevice()
{
    const QList<QMap<QString, QString> >  &lstCatCpu = DeviceManager::instance()->cmdInfo("cat_cpuinfo");
    if (lstCatCpu.size() == 0) {
        return;
    }

    const QList<QMap<QString, QString> >  &lsCpu = DeviceManager::instance()->cmdInfo("lscpu");
    const QMap<QString, QString> &lscpu = lsCpu.size() > 0 ? lsCpu[0] : QMap<QString, QString>();

    const QList<QMap<QString, QString> >  &lshwCpu = DeviceManager::instance()->cmdInfo("lshw_cpu");
    const QMap<QString, QString> &lshw = lshwCpu.size() > 0 ? lshwCpu[0] : QMap<QString, QString>();

    const QList<QMap<QString, QString> >  &dmidecode4 = DeviceManager::instance()->cmdInfo("dmidecode4");
//    const QMap<QString, QString> &dmidecode = dmidecode4.size() > 1 ? dmidecode4[1] : QMap<QString, QString>();

    // calculate core num
    int coreNum = 0;
    QList<int> coreCountList;
    QList<QMap<QString, QString> >::const_iterator dit = dmidecode4.begin();
    for (; dit != dmidecode4.end(); ++dit) {
        if ((*dit).find("Core Count") != (*dit).end()) {
            coreNum += (*dit)["Core Count"].trimmed().toInt();
            coreCountList << (*dit)["Core Count"].trimmed().toInt();  // 统计物理CPU以及其中的核数
        }
    }

    QList<QMap<QString, QString> >::const_iterator it = lstCatCpu.begin();
    for (; it != lstCatCpu.end(); ++it) {
        DeviceCpu device;

        int i = 0;
        int count = 0;   // 累计逻辑CPU数目
        int cpuSize = DeviceManager::instance()->getCPUDevices().size();   // 现已添加的逻辑CPU个数

        // 判断当前逻辑CPU属于哪个物理CPU信息
        while (i < coreCountList.size()) {
            // 从第一个物理CPU开始匹配
            if (count == 0) {
                count += coreCountList[i];
            }

            // 已有逻辑CPU小于已统计的物理CPU中的逻辑个数
            if (cpuSize < count) {
                break;
            } else {
                ++i;
                if (i < coreCountList.size())
                    count += coreCountList[i];  // 累加CPU核数
            }
        }

        // 设置逻辑CPU信息
        device.setCpuInfo(lscpu, lshw, dmidecode4[i], *it, lstCatCpu.size(), coreNum);
        DeviceManager::instance()->addCpuDevice(device);
    }
}

void DeviceGenerator::generatorBiosDevice()
{
    getBiosInfo();
    getSystemInfo();
    getBaseBoardInfo();
    getChassisInfo();
    getBiosMemoryInfo();
}

void DeviceGenerator::generatorMemoryDevice()
{
    getMemoryInfoFromLshw();
    getMemoryInfoFromDmidecode();
}

void DeviceGenerator::generatorDiskDevice()
{
    // 添加从hwinfo中获取的信息
    getDiskInfoFromHwinfo();
    // 添加从lshw中获取的信息
    getDiskInfoFromLshw();

    getDiskInfoFromLsblk();
    getDiskInfoFromSmartCtl();
}

void DeviceGenerator::generatorGpuDevice()
{
    getGpuInfoFromHwinfo();
    getGpuInfoFromLshw();
    getGpuInfoFromXrandr();
    getGpuSizeFromDmesg();
}

void DeviceGenerator::generatorMonitorDevice()
{
    getMonitorInfoFromHwinfo();
    getMonitorInfoFromXrandrVerbose();
    getMonitorRefreshRateFromXrandr();
}

void DeviceGenerator::generatorNetworkDevice()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("lshw_network");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        // (eth[0-9]$)|(wlan[0-9]$) 过滤虚拟网卡
        QRegExp reg("(eth[0-9]$)|(wlan[0-9]$)");
        if (!reg.exactMatch((*it)["logical name"])){
            continue;
        }
        DeviceNetwork device;
        device.setInfoFromLshw(*it);
        DeviceManager::instance()->addNetworkDevice(device);
    }
}

void DeviceGenerator::generatorAudioDevice()
{
    getAudioInfoFromHwinfo();
    getAudioChipInfoFromDmesg();
    getAudioInfoFromLshw();
    getAudioInfoFromCatInput();
}

void DeviceGenerator::generatorBluetoothDevice()
{
    getBluetoothInfoFromHciconfig();
//    getBlueToothInfoFromHwinfo();
//    getBluetoothInfoFromLshw();
}

void DeviceGenerator::generatorKeyboardDevice()
{
    getKeyboardInfoFromHwinfo();
    getKeyboardInfoFromLshw();
    getKeyboardInfoFromCatDevices();
}

void DeviceGenerator::generatorMouseDevice()
{
    getMouseInfoFromHwinfo();
    getMouseInfoFromLshw();
    getMouseInfoFromCatDevices();
}

void DeviceGenerator::generatorPrinterDevice()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("printer");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DevicePrint device;
        device.setInfo(*it);
        DeviceManager::instance()->addPrintDevice(device);
    }
}

void DeviceGenerator::generatorCameraDevice()
{
    getImageInfoFromHwinfo();
    getImageInfoFromLshw();
}

void DeviceGenerator::generatorCdromDevice()
{
    getCdromInfoFromHwinfo();
    getCdromInfoFromLshw();
}

void DeviceGenerator::generatorOthersDevice()
{
    getOthersInfoFromHwinfo();
    getOthersInfoFromLshw();
}

void DeviceGenerator::generatorPowerDevice()
{
    const QList<QMap<QString, QString> > &daemon = DeviceManager::instance()->cmdInfo("Daemon");
    bool hasDaemon = false;
    // 守护进程信息
    if (daemon.size() > 0) {
        hasDaemon = true;
    }
    // 电池或这电源信息
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("upower");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DevicePower device;
        if (!device.setInfoFromUpower(*it)) {
            continue;
        }
        if (hasDaemon) {
            device.setDaemonInfo(daemon[0]);
        }
        DeviceManager::instance()->addPowerDevice(device);
    }
}

void DeviceGenerator::getBiosInfo()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("dmidecode0");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceBios device;
        device.setBiosInfo(*it);
        DeviceManager::instance()->addBiosDevice(device);
    }

    const QList<QMap<QString, QString>> lanInfo = DeviceManager::instance()->cmdInfo("dmidecode13");
    QList<QMap<QString, QString> >::const_iterator iter = lanInfo.begin();
    for (; iter != lanInfo.end(); ++iter) {
        if ((*iter).size() < 2) {
            continue;
        }
        DeviceManager::instance()->setLanguageInfo(*iter);
    }

}

void DeviceGenerator::getSystemInfo()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("dmidecode1");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceBios device;
        device.setSystemInfo(*it);
        DeviceManager::instance()->addBiosDevice(device);
    }
}

void DeviceGenerator::getBaseBoardInfo()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("dmidecode2");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceBios device;
        device.setBaseBoardInfo(*it);
        DeviceManager::instance()->addBiosDevice(device);
    }
}

void DeviceGenerator::getChassisInfo()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("dmidecode3");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceBios device;
        device.setChassisInfo(*it);
        DeviceManager::instance()->addBiosDevice(device);
    }
}

void DeviceGenerator::getBiosMemoryInfo()
{
    const QList<QMap<QString, QString>> lstInfo = DeviceManager::instance()->cmdInfo("dmidecode16");
    QList<QMap<QString, QString> >::const_iterator it = lstInfo.begin();
    for (; it != lstInfo.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceBios device;
        device.setMemoryInfo(*it);
        DeviceManager::instance()->addBiosDevice(device);
    }
}

void DeviceGenerator::getMemoryInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMemory = DeviceManager::instance()->cmdInfo("lshw_memory");
    if (lstMemory.size() == 0) {
        return;
    } else if (lstMemory.size() == 1) {
        DeviceMemory device;
        device.setInfoFromLshw(lstMemory[0]);
        DeviceManager::instance()->addMemoryDevice(device);
    } else if (lstMemory.size() > 1) {
        QList<QMap<QString, QString> >::const_iterator it = lstMemory.begin();
        for (; it != lstMemory.end(); ++it) {
            if ((!(*it)["size"].contains("GiB") && !(*it)["size"].contains("MiB")) || (*it)["description"] == "System Memory") {
                continue;
            }
            DeviceMemory device;
            device.setInfoFromLshw(*it);
            DeviceManager::instance()->addMemoryDevice(device);
        }
    }
}

void DeviceGenerator::getMemoryInfoFromDmidecode()
{
    // 加载从dmidecode获取的信息
    const QList<QMap<QString, QString>> dmiMemory = DeviceManager::instance()->cmdInfo("dmidecode17");
    QList<QMap<QString, QString> >::const_iterator dIt = dmiMemory.begin();
    for (; dIt != dmiMemory.end(); ++dIt) {
        if ((*dIt).size() < 2 || (*dIt)["size"] == "No Module Installed") {
            continue;
        }
        DeviceManager::instance()->setMemoryInfoFromDmidecode(*dIt);
    }
}

void DeviceGenerator::getDiskInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstDisk = DeviceManager::instance()->cmdInfo("hwinfo_disk");
    QList<QMap<QString, QString> >::const_iterator dIt = lstDisk.begin();
    for (; dIt != lstDisk.end(); ++dIt) {
        if ((*dIt).size() < 2) {
            continue;
        }
        DeviceStorage device;
        if (device.setHwinfoInfo(*dIt) && device.isValid()) {
            DeviceManager::instance()->addStorageDeivce(device);
            addBusIDFromHwinfo((*dIt)["SysFS BusID"]);
        }
    }
}

void DeviceGenerator::getDiskInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstDisk = DeviceManager::instance()->cmdInfo("lshw_disk");
    QList<QMap<QString, QString> >::const_iterator dIt = lstDisk.begin();
    for (; dIt != lstDisk.end(); ++dIt) {
        if ((*dIt).size() < 2) {
            continue;
        }
        DeviceManager::instance()->addLshwinfoIntoStorageDevice(*dIt);
    }

    /*if (lstDisk.size() == 0)*/ {
        const QList<QMap<QString, QString>> lstDisk = DeviceManager::instance()->cmdInfo("lshw_storage");
        QList<QMap<QString, QString> >::const_iterator dIt = lstDisk.begin();
        for (; dIt != lstDisk.end(); ++dIt) {
            if ((*dIt).size() < 2) {
                continue;
            }
            DeviceManager::instance()->addLshwinfoIntoNVMEStorageDevice(*dIt);
        }
    }
}

void DeviceGenerator::getDiskInfoFromLsblk()
{
    const QList<QMap<QString, QString>> lstblk = DeviceManager::instance()->cmdInfo("lsblk_d");
    if (lstblk.size() > 0) {
        QMap<QString, QString>::const_iterator it = lstblk[0].begin();
        for (; it != lstblk[0].end(); ++it) {
            DeviceManager::instance()->setStorageDeviceMediaType(it.key(), it.value());
        }
    }
}

void DeviceGenerator::getDiskInfoFromSmartCtl()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("smart");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceManager::instance()->setStorageInfoFromSmartctl((*it)["ln"], *it);
    }
}

void DeviceGenerator::getGpuInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_display");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceGpu device;
        device.setHwinfoInfo(*it);
        DeviceManager::instance()->addGpuDevice(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}

void DeviceGenerator::getGpuInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_display");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceManager::instance()->setGpuInfoFromLshw(*it);
    }
}

void DeviceGenerator::getGpuInfoFromXrandr()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("xrandr");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->setGpuInfoFromXrandr(*it);
    }
}

void DeviceGenerator::getGpuSizeFromDmesg()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("dmesg");
    if (lstMap.size() > 0) {
        DeviceManager::instance()->setGpuSizeFromDmesg(lstMap[0]["Size"]);
    }
}

void DeviceGenerator::getMonitorInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_monitor");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceMonitor device;
        device.setInfoFromHwinfo(*it);
        DeviceManager::instance()->addMonitor(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}

void DeviceGenerator::getMonitorInfoFromXrandrVerbose()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("xrandr_verbose");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->setMonitorInfoFromXrandr((*it)["mainInfo"], (*it)["edid"]);
    }
}

void DeviceGenerator::getMonitorRefreshRateFromXrandr()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("xrandr");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->setCurrentResolution((*it)["curResolution"], (*it)["rate"]);
    }
}

void DeviceGenerator::getAudioInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_sound");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceAudio device;
        device.setInfoFromHwinfo(*it);
        DeviceManager::instance()->addAudioDevice(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}

void DeviceGenerator::getAudioInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_multimedia");

    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceManager::instance()->setAudioInfoFromLshw(*it);
    }
}

void DeviceGenerator::getAudioInfoFromCatInput()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("cat_devices");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it)["Sysfs"].contains("sound", Qt::CaseInsensitive) == true) {
            DeviceAudio device;
            device.setInfoFromCatDevices(*it);
            DeviceManager::instance()->addAudioDevice(device);
        }
    }
}

void DeviceGenerator::getAudioChipInfoFromDmesg()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("audiochip");
    if (lstMap.size() > 0) {
        DeviceManager::instance()->setAudioChipFromDmesg(lstMap[0]["chip"]);
    }
}

void DeviceGenerator::getBluetoothInfoFromHciconfig()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hciconfig");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceBluetooth device;
        device.setInfoFromHciconfig(*it);
        DeviceManager::instance()->addBluetoothDevice(device);
    }
}

void DeviceGenerator::getBlueToothInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        if ((*it)["Hardware Class"] == "hub" || (*it)["Hardware Class"] == "mouse" || (*it)["Hardware Class"] == "keyboard") {
            continue;
        }
        if ((*it)["Hardware Class"] == "bluetooth" || (*it)["Driver"] == "btusb" || (*it)["Device"] == "BCM20702A0") {
            if (DeviceManager::instance()->setBluetoothInfoFromHwinfo(*it))
                addBusIDFromHwinfo((*it)["SysFS BusID"]);
        }
    }
}

void DeviceGenerator::getBluetoothInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->setBluetoothInfoFromLshw(*it);
    }
}

void DeviceGenerator::getKeyboardInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_keyboard");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }

        // 下面这句代码忘了这么写的原因，先去掉
        //if ((*it).contains("Device Files")) {}

        DeviceKeyboard device;
        device.setInfoFromHwinfo(*it);
        DeviceManager::instance()->addKeyboardDevice(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}


void DeviceGenerator::getKeyboardInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->setKeyboardInfoFromLshw(*it);
    }
}

void DeviceGenerator::getKeyboardInfoFromCatDevices()
{

}

void DeviceGenerator::getMouseInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_mouse");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }

        // 不让数位板显示在鼠标设备里面(武汉那边的数位板)
        if ((*it)["Device"].contains("PM")) {
            continue;
        }

        DeviceMouse device;
        device.setInfoFromHwinfo(*it);
        DeviceManager::instance()->addMouseDevice(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}

void DeviceGenerator::getMouseInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 1) {
            continue;
        }
        DeviceManager::instance()->addMouseInfoFromLshw(*it);
    }
}

void DeviceGenerator::getMouseInfoFromCatDevices()
{

}

void DeviceGenerator::getImageInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        if ((*it)["Model"].contains("camera", Qt::CaseInsensitive) ||
                (*it)["Device"].contains("camera", Qt::CaseInsensitive) ||
                (*it)["Driver"].contains("uvcvideo", Qt::CaseInsensitive) ||
                (*it)["Model"].contains("webcam", Qt::CaseInsensitive)) { // "webcam"  bug39981
            DeviceImage device;
            device.setInfoFromHwinfo(*it);
            DeviceManager::instance()->addImageDevice(device);
            addBusIDFromHwinfo((*it)["SysFS BusID"]);
        }
    }
}

void DeviceGenerator::getImageInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceManager::instance()->setCameraInfoFromLshw(*it);
    }
}

void DeviceGenerator::getCdromInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_cdrom");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }
        DeviceCdrom device;
        device.setInfoFromHwinfo(*it);
        DeviceManager::instance()->addCdromDevice(device);
        addBusIDFromHwinfo((*it)["SysFS BusID"]);
    }
}

void DeviceGenerator::getCdromInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstDisk = DeviceManager::instance()->cmdInfo("lshw_cdrom");
    QList<QMap<QString, QString> >::const_iterator dIt = lstDisk.begin();
    for (; dIt != lstDisk.end(); ++dIt) {
        if ((*dIt).size() < 2) {
            continue;
        }
        DeviceManager::instance()->addLshwinfoIntoCdromDevice(*dIt);
    }
}

void DeviceGenerator::getOthersInfoFromHwinfo()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("hwinfo_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 5) {
            continue;
        }

        bool isOtherDevice = true;
        QString curBus = (*it)["SysFS BusID"];
        curBus.replace(QRegExp("\\.[0-9]{1,2}$"), "");
        const QStringList &lstBusId = DeviceManager::instance()->getBusId();
        if (lstBusId.indexOf(curBus) != -1) {
            isOtherDevice = false;
        }

        if (isOtherDevice) {
            DeviceOthers device;
            device.setInfoFromHwinfo(*it);
            DeviceManager::instance()->addOthersDevice(device);
        }
    }
}

void DeviceGenerator::getOthersInfoFromLshw()
{
    const QList<QMap<QString, QString>> lstMap = DeviceManager::instance()->cmdInfo("lshw_usb");
    QList<QMap<QString, QString> >::const_iterator it = lstMap.begin();
    for (; it != lstMap.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        DeviceManager::instance()->setOthersDeviceInfoFromLshw(*it);
    }
}

void DeviceGenerator::addBusIDFromHwinfo(const QString &sysfsBusID)
{
    if (sysfsBusID.isEmpty()) {
        return;
    }

    QString busID = sysfsBusID;
    busID.replace(QRegExp("\\.[0-9]+$"), "");

    m_ListBusID.append(busID);
}

const QStringList &DeviceGenerator::getBusIDFromHwinfo()
{
    return m_ListBusID;
}


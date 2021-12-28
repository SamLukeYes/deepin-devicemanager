// 项目自身文件
#include "DeviceNetwork.h"

// 其它头文件
#include "EnableManager.h"

DeviceNetwork::DeviceNetwork()
    : DeviceBaseInfo()
    , m_Name("")
    , m_Vendor("")
    , m_Model(""), m_Version("")
    , m_BusInfo("")
    , m_LogicalName("")
    , m_MACAddress("")
    , m_Irq("")
    , m_Memory("")
    , m_Width("")
    , m_Clock("")
    , m_Capabilities("")
    , m_Autonegotiation("")
    , m_Broadcast("")
    , m_Driver("")
    , m_DriverVersion("")
    , m_Duplex("")
    , m_Firmware("")
    , m_Port("")
    , m_Link("")
    , m_Ip("")
    , m_Speed("")
    , m_Capacity("")
    , m_Latency("")
    , m_Multicast("")
{
    // 初始化可显示属性
    initFilterKey();

    // 设备可禁用
    m_CanEnable = true;
}

void DeviceNetwork::setInfoFromLshw(const QMap<QString, QString> &mapInfo)
{
    // 设置由lshw获取的信息
    setAttribute(mapInfo, "description", m_Model);
    setAttribute(mapInfo, "product", m_Name);
    setAttribute(mapInfo, "description", m_Name, false);
    setAttribute(mapInfo, "vendor", m_Vendor);
    setAttribute(mapInfo, "version", m_Version);
    setAttribute(mapInfo, "bus info", m_BusInfo);
    setAttribute(mapInfo, "logical name", m_LogicalName);
    setAttribute(mapInfo, "serial", m_MACAddress);
    setAttribute(mapInfo, "irq", m_Irq);
    setAttribute(mapInfo, "memory", m_Memory);
    setAttribute(mapInfo, "width", m_Width);
    setAttribute(mapInfo, "clock", m_Clock);
    setAttribute(mapInfo, "capabilities", m_Capabilities);
    setAttribute(mapInfo, "autonegotiation", m_Autonegotiation);
    setAttribute(mapInfo, "broadcast", m_Broadcast);
    setAttribute(mapInfo, "driver", m_Driver);
    setAttribute(mapInfo, "driverversion", m_DriverVersion);
    setAttribute(mapInfo, "duplex", m_Duplex);
    setAttribute(mapInfo, "firmware", m_Firmware);
    setAttribute(mapInfo, "port", m_Port);
    setAttribute(mapInfo, "link", m_Link);
    setAttribute(mapInfo, "ip", m_Ip);
    setAttribute(mapInfo, "size", m_Speed);
    setAttribute(mapInfo, "capacity", m_Capacity);
    setAttribute(mapInfo, "latency", m_Latency);
    setAttribute(mapInfo, "multicast", m_Multicast);

    // 加载其他信息
    getOtherMapInfo(mapInfo);
}

bool DeviceNetwork::setInfoFromHwinfo(const QMap<QString, QString> &mapInfo)
{
    // 设置由hwinfo获取的信息
    if (mapInfo["Device File"] != m_LogicalName)
        return false;

    setAttribute(mapInfo, "Model", m_Name);
    return true;
}

const QString &DeviceNetwork::name()const
{
    return m_Name;
}

const QString &DeviceNetwork::driver()const
{
    return m_Driver;
}

QString DeviceNetwork::subTitle()
{
    return m_Name;
}

const QString DeviceNetwork::getOverviewInfo()
{
    // 获取概况信息
    return m_Name.isEmpty() ? m_Model : m_Name;
}

EnableDeviceStatus DeviceNetwork::setEnable(bool e)
{
    // 设置网卡禁用启用
    return EnableManager::instance()->enableNetworkByIfconfig(m_LogicalName, e);
}

bool DeviceNetwork::enable()
{
    // 通过ifconfig配置网卡禁用启用
    m_Enable = EnableManager::instance()->isNetworkEnableByIfconfig(m_LogicalName);
    return m_Enable;
}

void DeviceNetwork::correctCurrentLinkStatus(QString linkStatus)
{
    if (m_Link != linkStatus)
        m_Link = linkStatus;
}

QString DeviceNetwork::logicalName()
{
    return m_LogicalName;
}

void DeviceNetwork::initFilterKey()
{
    // 初始化可显示属性
    addFilterKey(QObject::tr("ioport"));
    addFilterKey(QObject::tr("physical id"));
    addFilterKey(QObject::tr("network"));
}

void DeviceNetwork::loadBaseDeviceInfo()
{
    // 添加基本信息
    addBaseDeviceInfo(tr("Name"), m_Name);
    addBaseDeviceInfo(tr("Vendor"), m_Vendor);
    addBaseDeviceInfo(tr("Type"), m_Model);
    addBaseDeviceInfo(tr("Version"), m_Version);
    addBaseDeviceInfo(tr("Bus Info"), m_BusInfo);
    addBaseDeviceInfo(tr("Capabilities"), m_Capabilities);
    addBaseDeviceInfo(tr("Driver"), m_Driver);
    addBaseDeviceInfo(tr("Driver Version"), m_DriverVersion);
}

void DeviceNetwork::loadOtherDeviceInfo()
{
    // 添加其他信息,成员变量
    addOtherDeviceInfo(tr("Capacity"), m_Capacity);
    addOtherDeviceInfo(tr("Speed"), m_Speed);
    addOtherDeviceInfo(tr("Port"), m_Port);
    addOtherDeviceInfo(tr("Multicast"), m_Multicast);
    addOtherDeviceInfo(tr("Link"), m_Link);
    addOtherDeviceInfo(tr("Latency"), m_Latency);
    addOtherDeviceInfo(tr("IP"), m_Ip);
    addOtherDeviceInfo(tr("Firmware"), m_Firmware);
    addOtherDeviceInfo(tr("Duplex"), m_Duplex);
    addOtherDeviceInfo(tr("Broadcast"), m_Broadcast);
    addOtherDeviceInfo(tr("Auto Negotiation"), m_Autonegotiation);
    addOtherDeviceInfo(tr("Clock"), m_Clock);
    addOtherDeviceInfo(tr("Width"), m_Width);
    addOtherDeviceInfo(tr("Memory"), m_Memory);
    addOtherDeviceInfo(tr("IRQ"), m_Irq);
    addOtherDeviceInfo(tr("MAC Address"), m_MACAddress);
    addOtherDeviceInfo(tr("Logical Name"), m_LogicalName);

    // 将QMap<QString, QString>内容转存为QList<QPair<QString, QString>>
    mapInfoToList();
}

void DeviceNetwork::loadTableData()
{
    // 根据是否禁用设置设备名称
    QString name;
    if (!enable())
        name = "(" + tr("Disable") + ") " + m_Name;
    else
        name = m_Name;

    // 加载表格数据信息
    m_TableData.append(name);
    m_TableData.append(m_Vendor);
    m_TableData.append(m_Model);
}

void DeviceNetwork::setInfoFromXml(QString key, QList<QMap<QString, QString>> &lstMap)
{
    if(lstMap.length() < 1){
        return;
    }
    QMap<QString, QString> * map = nullptr;
    for (int i = 0; i< lstMap.length();i++) {
        if(lstMap[i][key] == m_BusInfo){
            map = &lstMap[i];
            break;
        }
    }
    if(nullptr == map){
        return;
    }

    setAttribute(*map, "Name", m_Name);
    setAttribute(*map, "Vendor", m_Vendor);
    setAttribute(*map, "Model", m_Model);
    setAttribute(*map, "Version", m_Version);
    //setAttribute(*map, "BusInfo", m_BusInfo);
    setAttribute(*map, "LogicalName", m_LogicalName);
    setAttribute(*map, "MACAddress", m_MACAddress);
    setAttribute(*map, "Irq", m_Irq);
    setAttribute(*map, "Memory", m_Memory);
    setAttribute(*map, "Width", m_Width);
    setAttribute(*map, "Clock", m_Clock);
    setAttribute(*map, "Capabilities", m_Capabilities);
    setAttribute(*map, "Autonegotiation", m_Autonegotiation);
    setAttribute(*map, "Broadcast", m_Broadcast);
    setAttribute(*map, "Driver", m_Driver);
    setAttribute(*map, "DriverVersion", m_DriverVersion);
    setAttribute(*map, "Duplex       ", m_Duplex       );
    setAttribute(*map, "Firmware     ", m_Firmware     );
    setAttribute(*map, "Port         ", m_Port         );
    setAttribute(*map, "Link         ", m_Link         );
    setAttribute(*map, "Ip           ", m_Ip           );
    setAttribute(*map, "Speed        ", m_Speed        );
    setAttribute(*map, "Capacity     ", m_Capacity     );
    setAttribute(*map, "Latency      ", m_Latency      );
    setAttribute(*map, "Multicast    ", m_Multicast    );

    // 获取其他cpu信息
    getOtherMapInfo(*map);
}

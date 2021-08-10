#include "DeviceCpu.h"

#include <math.h>

DeviceCpu::DeviceCpu()
    : m_Vendor("")
    , m_Name("")
    , m_PhysicalID("")
    , m_CoreID("")
    , m_ThreadNum("")
    , m_Frequency("")
    , m_CurFrequency("")
    , m_BogoMIPS("")
    , m_Architecture(""),
      m_Familly("")
    , m_Model("")
    , m_Step("")
    , m_CacheL1Data("")
    , m_CacheL1Order("")
    , m_CacheL2("")
    , m_CacheL3("")
    , m_Extensions("")
    , m_Flags("")
    , m_HardwareVirtual("")
    , m_LogicalCPUNum("")
    , m_CPUCoreNum("")
    , m_FrequencyIsRange(false)
    , m_LogicalNum(0)
    , m_CoreNum(0)
{
    initFilterKey();
}

void DeviceCpu::initFilterKey()
{
    addFilterKey(QObject::tr("CPU implementer"));
    addFilterKey(QObject::tr("CPU architecture"));
    addFilterKey(QObject::tr("CPU variant"));
    addFilterKey(QObject::tr("CPU part"));
    addFilterKey(QObject::tr("CPU revision"));
}

void DeviceCpu::setCpuInfo(const QMap<QString, QString> &mapLscpu, const QMap<QString, QString> &mapLshw, const QMap<QString, QString> &mapDmidecode, const QMap<QString, QString> &catInfo, int logicalNum, int coreNum)
{
    // logical num and core num
    m_LogicalNum = logicalNum;
    m_CoreNum = coreNum;

    setInfoFromLscpu(mapLscpu);
    setInfoFromLshw(mapLshw);
    setInfoFromDmidecode(mapDmidecode);
    setInfoFromCatCpuinfo(catInfo);

    // CPU 名称后面不需要加个数
    m_Name.replace(QRegExp("/[0-9]*$"), "");
    m_Name.replace(QRegExp("x [0-9]*$"), "");
}

void DeviceCpu::setfrequencyShowRange(bool isRange, const QString &maxSpeed)
{
    // 针对HW定制
    // 设置频率不显示范围
    m_FrequencyIsRange = isRange;

    // 频率显示为dmidecode -t 4 中的最大值
    m_Frequency = maxSpeed;

    // 单位由 MHz 改为 GHz
    if (m_Frequency.contains("MHz")) {
        m_Frequency = m_Frequency.replace("MHz", "").trimmed();
        double frequencyNum = m_Frequency.toDouble() / 1000.0;

        m_Frequency = QString::number(frequencyNum) + " GHz";
    }
}

const QString &DeviceCpu::vendor() const
{
    return m_Vendor;
}

const QString &DeviceCpu::name() const
{
    return m_Name;
}

const QString &DeviceCpu::physicalID() const
{
    return m_PhysicalID;
}

const QString &DeviceCpu::coreID() const
{
    return m_CoreID;
}

const QString &DeviceCpu::threadNum() const
{
    return m_ThreadNum;
}

const QString &DeviceCpu::frequency() const
{
    return m_Frequency;
}

const QString &DeviceCpu::curFrequency() const
{
    return m_CurFrequency;
}

const QString &DeviceCpu::bogoMIPS() const
{
    return m_BogoMIPS;
}

const QString &DeviceCpu::architecture() const
{
    return m_Architecture;
}

const QString &DeviceCpu::familly() const
{
    return m_Familly;
}

const QString &DeviceCpu::model() const
{
    return m_Model;
}

const QString &DeviceCpu::step() const
{
    return m_Step;
}

const QString &DeviceCpu::cacheL1Data() const
{
    return m_CacheL1Data;
}

const QString &DeviceCpu::cacheL1Order() const
{
    return m_CacheL1Order;
}

const QString &DeviceCpu::cacheL2() const
{
    return m_CacheL2;
}

const QString &DeviceCpu::cacheL3() const
{
    return m_CacheL3;
}

const QString &DeviceCpu::extensions() const
{
    return m_Extensions;
}

const QString &DeviceCpu::flags() const
{
    return m_Flags;
}

const QString &DeviceCpu::hardwareVirtual() const
{
    return m_HardwareVirtual;
}

int DeviceCpu::logicalCPUNum() const
{
    return m_LogicalNum;
}

int DeviceCpu::cpuCores() const
{
    return m_CoreNum;
}

bool DeviceCpu::frequencyIsRange()const
{
    return m_FrequencyIsRange;
}

void DeviceCpu::setInfoFromLscpu(const QMap<QString, QString> &mapInfo)
{
    setAttribute(mapInfo, "Model name", m_Name);
    setAttribute(mapInfo, "Vendor ID", m_Vendor);

    setAttribute(mapInfo, "Thread(s) per core", m_ThreadNum);
    setAttribute(mapInfo, "每个核的线程数", m_ThreadNum);   // 服务器欧拉版

    setAttribute(mapInfo, "BogoMIPS", m_BogoMIPS);
    setAttribute(mapInfo, "Architecture", m_Architecture);

    // 服务器欧拉版，是中文 架构 不是 Architecture
    setAttribute(mapInfo, "架构", m_Architecture);
    setAttribute(mapInfo, "CPU family", m_Familly);

    setAttribute(mapInfo, "Model", m_Model);
    setAttribute(mapInfo, "型号", m_Model);              // 服务器欧拉版

    setAttribute(mapInfo, "Stepping", m_Step);
    setAttribute(mapInfo, "步进", m_Step);              // 服务器欧拉版

//    setAttribute(mapInfo, "L1d cache", m_CacheL1Data);
//    setAttribute(mapInfo, "L1d 缓存", m_CacheL1Data);
//    setAttribute(mapInfo, "L1i cache", m_CacheL1Order);
//    setAttribute(mapInfo, "L1i 缓存", m_CacheL1Order);
//    setAttribute(mapInfo, "L2 cache", m_CacheL2);
//    setAttribute(mapInfo, "L2 缓存", m_CacheL2);
//    setAttribute(mapInfo, "L3 cache", m_CacheL3);
//    setAttribute(mapInfo, "L3 缓存", m_CacheL3);

    setAttribute(mapInfo, "Flags", m_Flags);
    setAttribute(mapInfo, "标记", m_Flags);

    setAttribute(mapInfo, "Virtualization", m_HardwareVirtual);
    setAttribute(mapInfo, "虚拟化", m_HardwareVirtual);

    setAttribute(mapInfo, "CPU(s)", m_LogicalCPUNum);
    setAttribute(mapInfo, "CPU", m_LogicalCPUNum);

    // 计算频率范围
    bool min = mapInfo.find("CPU min MHz") != mapInfo.end();
    bool max = mapInfo.find("CPU max MHz") != mapInfo.end();
    if (min && max) {
        double minHz = mapInfo["CPU min MHz"].toDouble() / 1000;
        double maxHz = mapInfo["CPU max MHz"].toDouble() / 1000;
        m_Frequency = QString("%1-%2 GHz").arg(minHz).arg(maxHz);
        m_FrequencyIsRange = true;

        // 如果最大最小频率相等则不显示范围
        if (fabs(minHz - maxHz) < 0.001) {
            m_FrequencyIsRange = false;
        }
    }

    //获取扩展指令集
    QStringList orders = {"MMX", "SSE", "SSE2", "SSE3", "3D Now", "SSE4", "SSSE3", "SSE4_1", "SSE4_2", "AMD64", "EM64T"};
    foreach (const QString &order, orders) {
        if (mapInfo["Flags"].contains(order, Qt::CaseInsensitive)) {
            m_Extensions += QString("%1 ").arg(order);
        }
        if (mapInfo["标记"].contains(order, Qt::CaseInsensitive)) {
            m_Extensions += QString("%1 ").arg(order);
        }
    }

    loadOtherDeviceInfo(mapInfo);
}

void DeviceCpu::setInfoFromLshw(const QMap<QString, QString> &mapInfo)
{
    // longxin CPU型号不从lshw中获取
    // bug39874
    if (m_Name.contains("Loongson", Qt::CaseInsensitive)) {
        setAttribute(mapInfo, "version", m_Name, false);
    } else {
        setAttribute(mapInfo, "version", m_Name);
    }

    setAttribute(mapInfo, "vendor", m_Vendor);
//    setAttribute(mapInfo,"Thread(s) per core",m_ThreadNum);
//    setAttribute(mapInfo,"BogoMIPS",m_BogoMIPS);
//    setAttribute(mapInfo,"Architecture",m_Architecture);
//    setAttribute(mapInfo,"CPU family",m_Familly);
//    setAttribute(mapInfo,"Model",m_Model);
//    setAttribute(mapInfo,"Stepping",m_Step);
//    setAttribute(mapInfo,"L1d cache",m_CacheL1Data);
//    setAttribute(mapInfo,"L1i cache",m_CacheL1Order);
//    setAttribute(mapInfo,"L2 cache",m_CacheL2);
//    setAttribute(mapInfo,"L3 cache",m_CacheL3);
//    setAttribute(mapInfo,"Flags",m_Flags);
//    setAttribute(mapInfo,"Virtualization",m_HardwareVirtual);
//    setAttribute(mapInfo,"CPU(s)",m_LogicalCPUNum);

//    setAttribute(mapInfo, "capacity", m_Frequency, false);
//    setAttribute(mapInfo, "capacity", m_CurFrequency, false);

//    // 联想FT的环境没有capacity字段，但是有size字段
//    setAttribute(mapInfo, "size", m_Frequency, false);
//    setAttribute(mapInfo, "size", m_CurFrequency, false);

    loadOtherDeviceInfo(mapInfo);
}

void DeviceCpu::setInfoFromDmidecode(const QMap<QString, QString> &mapInfo)
{
    // longxin CPU型号不从dmidecode中获取
    // bug39874
    if (m_Name.contains("Loongson", Qt::CaseInsensitive)) {
        setAttribute(mapInfo, "product", m_Name, false);
    } else {
        setAttribute(mapInfo, "product", m_Name);
    }

    setAttribute(mapInfo, "Manufacturer", m_Vendor);
    setAttribute(mapInfo, "Max Speed", m_Frequency, false);
    setAttribute(mapInfo, "Current Speed", m_CurFrequency);
//    setAttribute(mapInfo,"Thread(s) per core",m_ThreadNum);
//    setAttribute(mapInfo,"BogoMIPS",m_BogoMIPS);
//    setAttribute(mapInfo,"Architecture",m_Architecture);
    setAttribute(mapInfo, "Family", m_Familly);
//    setAttribute(mapInfo,"Model",m_Model);
//    setAttribute(mapInfo,"Stepping",m_Step);
//    setAttribute(mapInfo,"L1d cache",m_CacheL1Data);
//    setAttribute(mapInfo,"L1i cache",m_CacheL1Order);
//    setAttribute(mapInfo,"L2 cache",m_CacheL2);
//    setAttribute(mapInfo,"L3 cache",m_CacheL3);
//    setAttribute(mapInfo,"Flags",m_Flags);
//    setAttribute(mapInfo,"Virtualization",m_HardwareVirtual);
    setAttribute(mapInfo, "Core Count", m_CPUCoreNum);

    // 获取其他cpu信息
    loadOtherDeviceInfo(mapInfo);
}

void DeviceCpu::setInfoFromCatCpuinfo(const QMap<QString, QString> &mapInfo)
{
    setAttribute(mapInfo, "processor", m_PhysicalID);

    // 兆芯和X86机器的关键字 core id  龙芯的关键字是 core
    setAttribute(mapInfo, "core id", m_CoreID);
    setAttribute(mapInfo, "core", m_CoreID);

    // 在FT-2000和pangu(都是arm) 的机器上没有 core 和 core id
    setAttribute(mapInfo, "processor", m_CoreID, false);

    // 在FT-2000和pangu(都是arm) 的机器上没有 cpu cores
    setAttribute(mapInfo, "cpu cores", m_CPUCoreNum);
    if (m_CPUCoreNum.isEmpty()) {
        m_CPUCoreNum = m_LogicalCPUNum;
    }

    // 龙芯机器无法获取型号 但是有cpu model
    setAttribute(mapInfo, "cpu model", m_Model, false);

    // 龙芯机器无法获取特性，需要在cat cpu中使用Loongson Features
    setAttribute(mapInfo, "Loongson Features", m_Flags, false);


    setAttribute(mapInfo, "CPU MHz", m_Frequency, false);
    setAttribute(mapInfo, "cpu MHz", m_Frequency, false);

    if (m_Frequency.contains(".")) {
        m_Frequency.replace(QRegExp("\\.00"), "MHz");
    }
    if (!m_Frequency.contains("MHz") && !m_Frequency.contains("GHz")) {
        m_Frequency += "MHz";
    }

    // 获取其他cpu信息
    loadOtherDeviceInfo(mapInfo);
}

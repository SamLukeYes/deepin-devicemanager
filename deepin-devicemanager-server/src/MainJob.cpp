#include "MainJob.h"
#include <QDebug>
#include "ThreadPool.h"
#include "RRServer.h"
#include "DetectThread.h"
#include "DebugTimeManager.h"
#include "DBusInterface.h"
#include "DeviceInfoManager.h"

#include <unistd.h>
#include <QDateTime>
#include <QThread>
#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include <QDBusConnection>

static QMutex mutex;
const QString SERVICE_NAME = "com.deepin.devicemanager";
const QString SERVICE_PATH = "/com/deepin/devicemanager";

MainJob::MainJob(QObject *parent)
    : QObject(parent)
    , mp_Pool(new ThreadPool)
    , mp_ZmqServer(nullptr)
    , mp_DetectThread(nullptr)
    , m_UpdateUI(false)
    , mp_IFace(new DBusInterface)
    , m_FirstUpdate(true)
{
    // 守护进程启动的时候加载所有信息
    if (!isZhaoXin()) {
        qInfo() << "001 ************************************* Not ZhaoXin Meche";
        updateAllDevice();
    } else {
        qInfo() << "002 ************************************* ZhaoXin Meche";
        QTimer::singleShot(15 * 60 * 1000, this, &MainJob::onFirstUpdate);
    }
}

MainJob::~MainJob()
{
    if (mp_ZmqServer) {
        delete mp_ZmqServer;
        mp_ZmqServer = nullptr;
    }
}

void MainJob::working()
{
    // 启动dbus
    if (!initDBus()) {
        qInfo() << "005 ************************************* Failed to Init Dbus";
        exit(1);
    }

    // 启动线程监听客户端询问
    if (mp_ZmqServer != nullptr) {
        delete  mp_ZmqServer;
        mp_ZmqServer = nullptr;
    }

    mp_ZmqServer = new RRServer(this);
    char ch[] = "tcp://127.0.0.1:8700";
    bool suc = mp_ZmqServer->initTo(ch);
    qInfo() << "Bind to tcp://127.0.0.1:8700 ************ " << suc;
    mp_ZmqServer->start();
    connect(mp_ZmqServer, &RRServer::instruction, this, &MainJob::slotExecuteClientInstructions);

    // 启动线程监听USB是否有新的设备
    mp_DetectThread = new DetectThread(this);
    mp_DetectThread->start();
    connect(mp_DetectThread, &DetectThread::usbChanged, this, &MainJob::slotUsbChanged, Qt::ConnectionType::QueuedConnection);
}

void MainJob::executeClientInstruction(const QString &instructions)
{
    if (instructions.startsWith("UPDATE_UI")) {
        if (m_FirstUpdate) {
            updateAllDevice();
        }
        mp_ZmqServer->setReturnStr("2");
        return ;
    }
    handleInstruction("ZMQ#" + instructions);
}

bool MainJob::isZhaoXin()
{
    QString cmd = "sudo dmidecode -t 4";
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);
    QString info = process.readAllStandardOutput();
    if (info.contains("ZHAOXIN KaiXian KX-U")) {
        return true;
    } else {
        return false;
    }
}

void MainJob::slotUsbChanged()
{
    handleInstruction("DETECT");
}

void MainJob::slotExecuteClientInstructions(const QString &instructions)
{
    handleInstruction("ZMQ#" + instructions);
}

void MainJob::onFirstUpdate()
{
    if (m_FirstUpdate) {
        qInfo() << "003 ************************************* First Update In QTimer";
        updateAllDevice();
    } else {
        qInfo() << "004 ************************************* Not Update but In QTimer";
    }
}

void MainJob::handleInstruction(const QString &instruction)
{
    QMutexLocker locker(&mutex);
    if (instruction.startsWith("DETECT")) {
        if (m_UpdateUI) {
            sleep(2);
            m_UpdateUI = false;
        }
        updateAllDevice();
    } else if (instruction.startsWith("ZMQ")) {
        if (instruction.startsWith("ZMQ#DRIVER")) {
            driverInstruction(instruction);
        } else if (instruction.startsWith("ZMQ#IFCONFIG")) {
            ifconfigInstruction(instruction);
        } else if (instruction.startsWith("ZMQ#UNINSTALL")) {

        } else if (instruction.startsWith("ZMQ#UPDATE_UI")) {
            reqUpdateInstruction();
            m_UpdateUI = true;
        } else {
            nullInstruction();
        }
    }
}

void MainJob::updateAllDevice()
{
//    PERF_PRINT_BEGIN("POINT-01", "MainJob::updateAllDevice()");
//    if (m_FirstUpdate)
//        mp_Pool->generateDeviceFile();
//    else
//        mp_Pool->updateDeviceFile();
//    mp_Pool->waitForDone(-1);
//    sleep(1);
//    PERF_PRINT_END("POINT-01");
//    m_FirstUpdate = false;
}

void MainJob::nullInstruction()
{
    mp_ZmqServer->setReturnStr("0");
}

void MainJob::driverInstruction(const QString &instruction)
{
    QStringList lst = instruction.split("#");
    if (lst.size() != 3) {
        mp_ZmqServer->setReturnStr("0");
    }
    const QString &cmd = lst[2];
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);
    int exitcode = process.exitCode();
    if (exitcode == 127 || exitcode == 126) {
        mp_ZmqServer->setReturnStr("0");
    } else {
        QString output = process.readAllStandardOutput();
        qInfo() << output;
        if (output == "") {
            mp_ZmqServer->setReturnStr("2");
        } else {
            mp_ZmqServer->setReturnStr("1");
        }
    }
}

void MainJob::ifconfigInstruction(const QString &instruction)
{
    QStringList lst = instruction.split("#");
    if (lst.size() != 3) {
        mp_ZmqServer->setReturnStr("0");
    }
    const QString &cmd = lst[2];
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);
    int exitcode = process.exitCode();
    if (exitcode == 127 || exitcode == 126) {
        mp_ZmqServer->setReturnStr("0");
    } else {
        QString output = process.readAllStandardOutput();
        if (output == "") {
            mp_ZmqServer->setReturnStr("2");
        } else {
            mp_ZmqServer->setReturnStr("1");
        }
    }
}

void MainJob::reqUpdateInstruction()
{
    mp_ZmqServer->setReturnStr("2");
}

bool MainJob::initDBus()
{
    //1. 申请一个总线连接
    if (!QDBusConnection::systemBus().isConnected()) {
        return false;
    }

    //2. 在总线连接上挂载服务，这样其他进程才能请求该服务
    if (!QDBusConnection::systemBus().registerService(SERVICE_NAME)) {
        return false;
    }

    //3. 在挂载的服务上注册一个执行服务的对象
    if (!QDBusConnection::systemBus().registerObject(SERVICE_PATH, mp_IFace, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qInfo() << QDBusConnection::systemBus().lastError();
        return false;
    }

    return true;
}

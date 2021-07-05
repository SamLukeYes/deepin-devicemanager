#include "ThreadPoolTask.h"

#include <QTime>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <unistd.h>

#include<DeviceInfoManager.h>

ThreadPoolTask::ThreadPoolTask(QString cmd, QString file, bool replace, int waiting, QObject *parent)
    : QObject(parent),
      m_Cmd(cmd),
      m_File(file),
      m_CanNotReplace(replace),
      m_Waiting(waiting)
{

}

ThreadPoolTask::~ThreadPoolTask()
{

}

void ThreadPoolTask::run()
{
    runCmdToCache(m_Cmd);
}

void ThreadPoolTask::runCmd(const QString &cmd)
{
    QProcess process;
    QStringList options;
    options << "-c" << cmd;
    process.start("/bin/bash", options);
    process.waitForFinished(m_Waiting);
}

void ThreadPoolTask::runCmd(const QString &cmd, QString &info)
{
    QProcess process;
    QString cmdT = cmd;
//    process.start(cmdT.replace(QString(" >  ") + PATH + m_File, ""));

    QStringList options;
    options << "-c" << cmdT.replace(QString(" >  ") + PATH + m_File, "");
    process.start("/bin/bash", options);

    process.waitForFinished(m_Waiting);
    info = process.readAllStandardOutput();
}

void ThreadPoolTask::runCmdToCache(const QString &cmd)
{
    QString key = m_File;
    key.replace(".txt", "");
    bool existed = DeviceInfoManager::getInstance()->isInfoExisted(key);

    // 1. 先判断通过该命令获取的信息是不是需要刷新的,如果是cpu，内存条，主板等信息则只需要开机获取即可
    if (m_CanNotReplace && existed) {
        return;
    }

    // 2. 执行命令获取设备信息
    QString info;
    runCmd(cmd, info);
    // 3. 管理设备信息
    // 如果命令是 lsblk  , 则需要执行 smartctl --all /dev/***命令
    if (m_File == "lsblk_d.txt") {
        loadSmartCtlInfoToCache(info);
    }

    // 如果命令是 ls /dev/sg* ,则需要执行 smartctl --all /dev/*** 命令
    if (m_File == "ls_sg.txt") {
        loadSgSmartCtlInfoToCache(info);
    }

    // 如果命令是 lspci  , 则需要执行 lspci -v -s %1 > lspci_vs.txt 命令
    if (m_File == "lspci.txt") {
        loadLspciVSInfoToCache(info);
    }

    if ("hwinfo_display.txt" == m_File) {
        loadDisplayWidth(info);
    }

    DeviceInfoManager::getInstance()->addInfo(key, info);
}

void ThreadPoolTask::loadSmartCtlInfoToCache(const QString &info)
{
    QStringList lines = info.split("\n");
    foreach (QString line, lines) {
        QStringList words = line.replace(QRegExp("[\\s]+"), " ").split(" ");
        // NAME ROTA
        if (words.size() != 2 || words[0] == "NAME") {
            continue;
        }

        QString smartCmd = QString("smartctl --all /dev/%1").arg(words[0].trimmed());
        QString sInfo;
        runCmd(smartCmd, sInfo);
        DeviceInfoManager::getInstance()->addInfo(QString("smartctl_%1").arg(words[0].trimmed()), sInfo);
    }
}

void ThreadPoolTask::loadSgSmartCtlInfoToCache(const QString &info)
{
    QStringList lines = info.split("\n");

    foreach (QString line, lines) {
        if (line.isEmpty()) {
            continue;
        }

        QStringList words = line.split("/");

        QString smartCmd = QString("smartctl --all /dev/%1").arg(words[2].trimmed());
        QString sInfo;
        runCmd(smartCmd, sInfo);
        DeviceInfoManager::getInstance()->addInfo(QString("smartctl_%1").arg(words[2]), sInfo);
    }
}

void ThreadPoolTask::loadLspciVSInfoToCache(const QString &info)
{
    QStringList lines = info.split("\n");
    foreach (const QString &line, lines) {
        QStringList words = line.split(" ");
        if (words.size() < 2) {
            continue;
        }
        if (words[1] == QString("ISA")) {
            QString cmd = QString("lspci -v -s %1").arg(words[0].trimmed()); //  > /tmp/device-info/lspci_vs.txt
            QString sInfo;
            runCmd(cmd, sInfo);
            DeviceInfoManager::getInstance()->addInfo("lspci_vs", sInfo);
            break;
        }
    }
}

void ThreadPoolTask::loadDisplayWidth(const QString &info)
{
    QString widthS;
    QStringList params = info.split("\n\n");
    foreach (const QString &param, params) {

        QStringList lines = param.split("\n");
        if (lines.size() < 5)
            continue;
        bool res = false;
        foreach (const QString &line, lines) {
            if (line.contains("SysFS ID")) {
                QString pci = line.right(7);
                int width = 64;
                res = getDisplayWidthFromLspci(pci, width);
                widthS += pci;
                widthS += "-";
                widthS += QString::number(width);
                widthS += "\n";
                break;
            }
        }
        if (res)
            DeviceInfoManager::getInstance()->addInfo("width", widthS);
    }
}

bool ThreadPoolTask::getDisplayWidthFromLspci(const QString &info, int &width)
{
    // 添加nvidia-smi 命令
    // 英伟达GT1010 1G版的显卡位宽是32，但是lshw获取的是64，目前只能通过nvidia-smi判断
    //QString size("978MiB");
    QRegExp reg("9[0-9][0-9]MiB");

    QString cmd = QString("nvidia-smi");
    QString sInfo;
    runCmd(cmd, sInfo);

    // 判断是否是该显卡，通过pci判断
    if (!sInfo.contains(info)) {
        return false;
    }

    // 根据显卡大小判断位宽
    if (sInfo.contains(reg)) {
        width = 32;
    } else {
        width = 64;
    }

    return true;
}

void ThreadPoolTask::runCmdToFile(const QString &cmd)
{
    // 1. 先判断通过该命令获取的信息是不是需要刷新的,如果是cpu，内存条，主板等信息则只需要开机获取即可
    QString path = PATH + m_File;
    QFile file(path);
    if (m_CanNotReplace && file.exists()) {
        return;
    }

    // 2. 执行命令获取设备信息
    runCmd(cmd);

    if (m_File == "lsblk_d.txt") {
        // 如果命令是 lsblk  , 则需要执行 smartctl --all /dev/***命令
        loadSmartctlInfoToFile(file);
    } else if (m_File == "lspci.txt") {
        // 如果命令是 lspci  , 则需要执行 lspci -v -s %1 > lspci_vs.txt 命令
        loadLspciVSInfoToFile(file);
    }
}


void ThreadPoolTask::loadSmartctlInfoToFile(QFile &file)
{
    if (file.open(QIODevice::ReadOnly)) {
        QString info = file.readAll();
        QStringList lines = info.split("\n");
        foreach (QString line, lines) {
            QStringList words = line.replace(QRegExp("[\\s]+"), " ").split(" ");
            // NAME ROTA
            if (words.size() != 2 || words[0] == "NAME") {
                continue;
            }

            QString smartCmd = QString("smartctl --all /dev/%1 > /tmp/device-info/smartctl_%2.txt").arg(words[0].trimmed()).arg(words[0].trimmed());
            runCmd(smartCmd);
        }
        file.close();
    }
}

void ThreadPoolTask::loadLspciVSInfoToFile(QFile &file)
{
    if (file.open(QIODevice::ReadOnly)) {
        QString info = file.readAll();
        QStringList lines = info.split("\n");
        foreach (const QString &line, lines) {
            QStringList words = line.split(" ");
            if (words.size() < 2) {
                continue;
            }
            if (words[1] == QString("ISA")) {
                QString cmd = QString("lspci -v -s %1 > /tmp/device-info/lspci_vs.txt").arg(words[0].trimmed());
                runCmd(cmd);
                break;
            }
        }
        file.close();
    }
}

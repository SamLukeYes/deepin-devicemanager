#include "DBusInterface.h"
#include "DeviceInfoManager.h"
#include "MainJob.h"
#include "EnableSqlManager.h"

#include <QDebug>
#include <QFile>

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
{

}

QString DBusInterface::getInfo(const QString &key)
{
    // 不能返回用常引用
    if("is_server_running" != key){
        return DeviceInfoManager::getInstance()->getInfo(key);
    }else{
        return MainJob::serverIsRunning()?"1":"0";
    }
}

bool DBusInterface::isFirstUpdate()
{
    return MainJob::isFirstUpdate();
}

void DBusInterface::refreshInfo()
{
    emit update();
}

/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     AaronZhang <ya.zhang@archermind.com>
 *
 * Maintainer: AaronZhang <ya.zhang@archermind.com>
 * Maintainer: Yaobin <yao.bin@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "diskwidget.h"
#include "deviceinfoparser.h"
#include <DApplication>
//#include <QDebug>

DWIDGET_USE_NAMESPACE

DiskWidget::DiskWidget(QWidget *parent) : DeviceInfoWidgetBase(parent, tr("Storage"))
{
    initWidget();
}

void DiskWidget::initWidget()
{
    QStringList diskList = DeviceInfoParser::Instance().getLshwDiskNameList();
    int expect_nvme_index = 1;
    QStringList lsblkList = DeviceInfoParser::Instance().getLsblkDiskNameList();
    if(diskList.size() < 1)
    {
        setCentralInfo(tr("No disk found"));
        return;
    }

    QList<QStringList> tabList;
    QList<ArticleStruct> articles;
    QSet<QString> existArticles;

    int i = 0;
    foreach(const QString& disk, diskList)
    {
        bool isNvmeDisk = false;
        QString logicalName;
        if (disk.contains("storage",Qt::CaseInsensitive) == false) {
            logicalName = DeviceInfoParser::Instance().queryData("lshw", disk, "logical name");
        }
        else
        {
            int searched_nvme_index = 0;
            foreach (QString diskName,lsblkList) {
                isNvmeDisk = diskName.contains("nvme",Qt::CaseInsensitive);
                if (isNvmeDisk) {
                    searched_nvme_index ++;
                    if (searched_nvme_index == expect_nvme_index) {
                        //if ture, the diskName is corresponding lsblk key ,also smartctl db key
                        expect_nvme_index ++;
                        logicalName = QString("/dev/%1").arg(diskName);
                        break;
                    }
                }
            }
        }

        QString modelStr = DeviceInfoParser::Instance().queryData("lshw", disk, "product");
        QString vendorStr = DeviceInfoParser::Instance().queryData("lshw", disk, "vendor");
        QString mediaTypeStr = "Unknown";
        QString sizeStr = DeviceInfoParser::Instance().queryData("lshw", disk, "size");
//        sizeStr.replace("GiB","GB");
        int brackets = sizeStr.indexOf('(');
//        qDebug() << sizeStr << brackets;
        QString sizeStr1 = sizeStr.mid(0,brackets);
        if (sizeStr1.contains("GiB")){
            sizeStr1.replace("GiB","GB");
        }
        if (sizeStr1.contains("MiB")){
            sizeStr1.replace( "MiB", " MB" );
        }

        articles.clear();
        existArticles.clear();

        ArticleStruct model(tr("Model","stroage info"));
        model.value = modelStr;
        articles.push_back(model);
        existArticles.insert("product");

        ArticleStruct vendor(tr("Vendor","stroage info"));
        vendor.value = vendorStr;
        if(vendor.isValid() == false)
        {
            if( model.value.startsWith("ST", Qt::CaseInsensitive))
            {
                vendor.value = "Seagate";
                vendor.valueTranslate = true;
            }
            else if(model.value.startsWith("SanDisk", Qt::CaseInsensitive))
            {
                vendor.value = "SanDisk";
                vendor.valueTranslate = true;
            }
        }

        articles.push_back(vendor);
        existArticles.insert("vendor");

        ArticleStruct mediaType(tr("Media Type","stroage info"));
        //firstly,try to get media type from smartctl
        if( DeviceInfoParser::Instance().isToolSuccess("smartctl") )
        {
            QString rotationRate = DeviceInfoParser::Instance().queryData("smartctl", logicalName, "Rotation Rate");
            QString modelFamilyStr = DeviceInfoParser::Instance().queryData("smartctl", logicalName, "Model Family");
            QString deviceModelStr = DeviceInfoParser::Instance().queryData("smartctl", logicalName, "Device Model");
            QString modelStr = DeviceInfoParser::Instance().queryData("lshw", disk, "product");

            if( getDiskType(rotationRate, mediaTypeStr) == false )
            {
                if( getDiskType(modelFamilyStr, mediaTypeStr) == false )
                {
                    if(getDiskType(deviceModelStr, mediaTypeStr) == false){
                        getDiskType(modelStr, mediaTypeStr);
                    }
                }
            }
        }
        //secondly, try to get media type from lshw
        if(mediaTypeStr == "Unknown")
        {
            if (disk.contains("usb",Qt::CaseInsensitive)) {
                 mediaTypeStr = " USB disk";
            }
            else
            {
                QString curKey = disk;
                QString parentKey = curKey.mid(0,curKey.lastIndexOf("_"));
                auto &parent_dev = DeviceInfoParser::Instance().toolDatabase_.value("lshw").value(parentKey);
                if (parentKey.endsWith("core") == false) {
                    if(parent_dev.value("configuration").contains("usb")) {
                         mediaTypeStr = " USB disk";
                    }
                    if(parent_dev.value("bus info").contains("usb")) {
                         mediaTypeStr = " USB disk";
                    }
                }
            }
        }
        //thirdly, try to get media type if media type is unknown, and try to get nvme disk size
        if (isNvmeDisk) {
            auto  &db = DeviceInfoParser::Instance().toolDatabase_;
            QString logicalName_ = logicalName;
            if (mediaTypeStr == "Unknown") {
                if (db.value("smartctl").value(logicalName).value("Rotation Rate").contains("rpm",Qt::CaseInsensitive))
                {
                    mediaTypeStr = "HDD";
                }
                if (db.value("smartctl").value(logicalName).value("Model Family").contains("HHD",Qt::CaseInsensitive))
                {
                    mediaTypeStr = "HHD";
                }
                if (db.value("smartctl").value(logicalName).value("Model Family").contains("HDD",Qt::CaseInsensitive))
                {
                    mediaTypeStr= "HDD";
                }
            }
            //get nvme disk used size
            sizeStr1 = db.value("lsblk").value(logicalName_.remove("/dev/")).value("size");
            sizeStr1.replace(QRegExp("(G|g)(B|b){0,1}"),"GB");
            sizeStr1.replace(QRegExp("(M|m)(B|b){0,1}"),"MB");

            //try to get nvme disk total size
            QString totalNVMSize = db.value("smartctl").value(logicalName).value("Total NVM Capacity");
            if (totalNVMSize.isEmpty() == false) {
//                QRegExp rx("^(.*)\\[(.*GB)\\]$");
                QRegExp rx("^(.*)\\[(.*GB)\\]");
                QStringList capTexts ;
                if (rx.exactMatch(totalNVMSize)) {
                    capTexts = rx.capturedTexts();
                    if (capTexts.isEmpty() == false) {
                        sizeStr1 += QString(" (%1)").arg(capTexts.last());
                    }
                }
            }
         }
        mediaType.value = mediaTypeStr;
        articles.push_back(mediaType);


        ArticleStruct interface(tr("Interface","stroage info"));
        QStringList lst = disk.split("_");
        interface.value = tr("Unknown");
        if( lst.size() > 2 )
        {
            interface.value = lst.at(lst.size() -2);
        }

        ArticleStruct size(tr("Size","stroage info"));
        size.value = sizeStr1;
        articles.push_back(size);
        existArticles.insert("size");

        ArticleStruct serial(tr("Serial Number","stroage info"));
        serial.queryData("lshw", disk, "serial");
        existArticles.insert("serial");
        existArticles.insert("Serial Number");

        if( DeviceInfoParser::Instance().isToolSuccess("smartctl") )
        {
            QString rotationRate = DeviceInfoParser::Instance().queryData("smartctl", logicalName, "Rotation Rate");

            QString sataVersion = DeviceInfoParser::Instance().queryData("smartctl", logicalName, "SATA Version");
            QString version;
            QString speed;

            ArticleStruct mf(tr("Model Family","stroage info"));
            mf.queryData("smartctl", logicalName, "Model Family");
            articles.push_back(mf);
            existArticles.insert("Model Family");

            ArticleStruct dm(tr("Device Model","stroage info"));
            dm.queryData("smartctl", logicalName, "Device Model");
            articles.push_back(dm);
            existArticles.insert("Device Model");

            ArticleStruct description(tr("Description","stroage info"));
            description.queryData("lshw", disk, "description");
            articles.push_back(description);
            existArticles.insert("description");

            int index = sataVersion.indexOf(",");
            if(index>0)
            {
                ArticleStruct speed(tr("Speed","stroage info"));
                speed.value = sataVersion.mid( index + 1 );
                speed.value = speed.value.trimmed();
                articles.push_back(speed);

                interface.value = sataVersion.mid(0, index);
                interface.value = interface.value.trimmed();
            }
            articles.push_back(interface);
            existArticles.insert("SATA Version");

            ArticleStruct rr(tr("Rotation Rate","stroage info"));
            rr.value = rotationRate;
            articles.push_back(rr);
            existArticles.insert("Rotation Rate");

            articles.push_back(serial);

            ArticleStruct fromFactor(tr("Form Factor","stroage info"));
            fromFactor.queryData("smartctl", logicalName, "Form Factor");
            fromFactor.value.replace("inches", tr("inch"));
            articles.push_back(fromFactor);
            existArticles.insert("Form Factor");

            ArticleStruct powerOnHours(tr("Power On Hours","stroage info"));
            powerOnHours.queryData("smartctl", logicalName, "Power_On_Hours", existArticles);
            if(powerOnHours.isValid())
            {
                powerOnHours.value += (" "+ tr("Hours"));
            }
            articles.push_back(powerOnHours);

            ArticleStruct powerOnMinutes(tr("Power_On_Minutes","stroage info"));
            powerOnMinutes.queryData("smartctl", logicalName, "Power_On_Minutes", existArticles);
            if(powerOnMinutes.isValid())
            {
                powerOnMinutes.value += (" " + tr("Minutes","Power_On_Minutes"));
            }
            articles.push_back(powerOnMinutes);


            ArticleStruct powerOnHalfMinutes(tr("Power_On_Half_Minutes","stroage info"));
            powerOnHalfMinutes.queryData("smartctl", logicalName, "Power_On_Half_Minutes", existArticles);
            if(powerOnHalfMinutes.isValid())
            {
                powerOnHalfMinutes.value += " ";
                powerOnHalfMinutes.value += tr("Half Minutes");
            }
            articles.push_back(powerOnHalfMinutes);


            ArticleStruct powerOnSeconds(tr("Power_On_Seconds","stroage info"));
            powerOnSeconds.queryData("smartctl", logicalName, "Power_On_Seconds", existArticles);
            if(powerOnSeconds.isValid())
            {
                powerOnSeconds.value += " ";
                powerOnSeconds.value += tr("Seconds");
            }
            articles.push_back(powerOnSeconds);

            ArticleStruct powerCycleCount(tr("Power Cycle Count","stroage info"));
            powerCycleCount.queryData("smartctl", logicalName, "Power_Cycle_Count");
            if(powerCycleCount.isValid())
            {
                powerCycleCount.value += " ";
                powerCycleCount.value += tr("Times","Power Cycle Count");
            }
            articles.push_back(powerCycleCount);
            existArticles.insert("Power_Cycle_Count");
        }

        ArticleStruct version(tr("Version","stroage info"));
        version.queryData("lshw", disk, "version");
        articles.push_back(version);
        existArticles.insert("version");

        ArticleStruct capabilities(tr("Capabilities","stroage info"));
        capabilities.queryData("lshw", disk, "capabilities");
        articles.push_back(capabilities);
        existArticles.insert("capabilities");

        DeviceInfoParser::Instance().queryRemainderDeviceInfo("lshw", disk, articles, existArticles,"ManulTrack__Disk","Disk Information");

        if( DeviceInfoParser::Instance().isToolSuccess("smartctl") )
        {
            DeviceInfoParser::Instance().queryRemainderDeviceInfo("smartctl", logicalName, articles, existArticles,"ManulTrack__Disk","Disk Information");
        }

        addDevice( model.value, articles, diskList.size() );
        tabList.push_back({ modelStr,vendor.value,mediaType.value,sizeStr });

        if( i == 0)
        {
            if(overviewInfo_.isValid())
            {
                overviewInfo_.value += " / ";
            }

            model.value.remove(vendor.value, Qt::CaseInsensitive);

            QList<ArticleStruct> asList;
            asList << vendor << model;

            overviewInfo_.value += joinArticle( asList );
            overviewInfo_.value += " (";
            QString diskSize = size.value;
            QRegExp reg("^[\\s\\S]*\\(([\\s\\S]+)\\)$");
            if(reg.exactMatch(diskSize))
            {
                overviewInfo_.value += reg.cap(1);
            }
            else
            {
                overviewInfo_.value += diskSize;
            }

            overviewInfo_.value += ")";
        }
    }

    if( diskList.size() > 1 )
    {
        QStringList headers = { tr("Model"),  tr("Vendor"), tr("Media Type"), tr("Size") };
        addTable(headers, tabList);
    }
}

bool DiskWidget::getDiskType(const QString& diskProperty, QString& type)
{
    if( diskProperty.contains("SSD", Qt::CaseInsensitive) || diskProperty.contains("Solid", Qt::CaseInsensitive) )
    {
        type = "SSD";
        return true;
    }

    if(diskProperty.contains("HDD", Qt::CaseInsensitive) || diskProperty.contains("7200", Qt::CaseInsensitive) \
            || diskProperty.contains("5400", Qt::CaseInsensitive) || diskProperty.contains("rpm", Qt::CaseInsensitive))
    {
        type = "HDD";
        return true;
    }

    if( diskProperty.contains("Western Digital", Qt::CaseInsensitive) )
    {
        if( diskProperty.contains("Black", Qt::CaseInsensitive) ||
            diskProperty.contains("Green", Qt::CaseInsensitive) ||
            diskProperty.contains("Blue", Qt::CaseInsensitive) ||
            diskProperty.contains("Red", Qt::CaseInsensitive) ||
            diskProperty.contains("Scorpio", Qt::CaseInsensitive) )
        type = "HDD";
        return true;
    }

    return false;
}


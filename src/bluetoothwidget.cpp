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

#include "bluetoothwidget.h"
#include "deviceinfoparser.h"
#include <DApplication>

DWIDGET_USE_NAMESPACE


BluetoothWidget::BluetoothWidget(QWidget *parent) : DeviceInfoWidgetBase(parent, tr("Bluetooth"))
{
    initWidget();
}

void BluetoothWidget::initWidget()
{
    QStringList bluetoothList = DeviceInfoParser::Instance().getLshwBluetoothList();
    QStringList hciconfigBluetoothList = DeviceInfoParser::Instance().getHciconfigBluetoothControllerList();
    if (hciconfigBluetoothList.isEmpty()) {
        setCentralInfo(tr("No Bluetooth device found"));
        return;
    }

    if (bluetoothList.isEmpty()) {
        for (int i = 0;i < hciconfigBluetoothList.count();i++) {
            bluetoothList.push_back(QString(""));
        }
    }

    QStringList pairedDevicesList /*= DeviceInfoParser::Instance().getOtherBluetoothctlPairedDevicesList()*/;
    int connectedDeviceNumber = DeviceInfoParser::Instance().getOtherBluetoothctlPairedAndConnectedDevicesList().size();
    QList<QStringList> tabList;
    QList<ArticleStruct> articles;
    QSet<QString> existArticles;
    QSet<QString> existArticles2;
    QSet<QString> existArticles3;

    int i = 0;
    foreach(const QString& device, bluetoothList)
    {
        articles.clear();
        existArticles.clear();
        existArticles2.clear();
        existArticles3.clear();

        ArticleStruct name(tr("Bluetooth Name"));
        ArticleStruct vendor(tr("Vendor"));
        ArticleStruct product(tr("Product"));
        ArticleStruct description(tr("Description"));
        ArticleStruct mac(tr("MAC Address"));

        if( i < hciconfigBluetoothList.size() )
        {
            QString hciconfigName = hciconfigBluetoothList.at(i);

            product.queryData( "lshw", device, "product", existArticles, articles);

            vendor.queryData("lshw", device, "vendor", existArticles, articles);
            vendor.queryData("hciconfig", hciconfigName, "Manufacturer", existArticles2, articles);

            name.queryData("hciconfig", hciconfigName, "Name");
            name.value.remove("\'");
            articles.push_back(name);
            existArticles2.insert("Name");

            mac.queryData("hciconfig", hciconfigName, "BD Address", existArticles2, articles);


            description.queryData("lshw", device, "description", existArticles, articles);

            ArticleStruct serial(tr("Serial Number"));
            serial.queryData( "lshw", device, "serial", existArticles, articles);

            ArticleStruct linkPolicy(tr("Link Policy","Bluetooth Info"));
            linkPolicy.queryData("hciconfig", hciconfigName, "Link policy", existArticles2, articles);

            ArticleStruct linkMode(tr("Link Mode","Bluetooth Info"));
            linkMode.queryData("hciconfig", hciconfigName, "Link mode", existArticles2, articles);

            if(mac.isValid())
            {
                ArticleStruct powered(tr("Powered","Bluetooth Info"));
                powered.queryData("bluetoothctl", mac.value, "Powered", existArticles3, articles);

                ArticleStruct discoverable(tr("Discoverable","Bluetooth Info"));
                discoverable.queryData("bluetoothctl", mac.value, "Discoverable", existArticles3, articles);

                ArticleStruct pairable(tr("Pairable","Bluetooth Info"));
                pairable.queryData("bluetoothctl", mac.value, "Pairable", existArticles3, articles);

                ArticleStruct modalias(tr("Modalias","Bluetooth Info"));
                modalias.queryData("bluetoothctl", mac.value, "Modalias", existArticles3, articles);

                ArticleStruct discovering(tr("Discovering","Bluetooth Info"));
                discovering.queryData("bluetoothctl", mac.value, "Discovering", existArticles3, articles);
            }
        }

        ArticleStruct physicalId(tr("Physical ID","Bluetooth Info"));
        physicalId.queryData("lshw", device, "physical id", existArticles, articles);

        ArticleStruct busInfo(tr("Bus Info","Bluetooth Info"));
        busInfo.queryData("lshw", device, "bus info", existArticles, articles);

        ArticleStruct version(tr("Version","Bluetooth Info"));
        version.queryData("lshw", device, "version", existArticles, articles);

        ArticleStruct capabilities(tr("Capabilities","Bluetooth Info"));
        capabilities.queryData("lshw", device, "capabilities", existArticles, articles);

        if( i < hciconfigBluetoothList.size() )
        {
            DeviceInfoParser::Instance().queryRemainderDeviceInfo("hciconfig", hciconfigBluetoothList.at(i), articles, existArticles2,
                                                                  "ManulTrack__Bluetooth","Bluetooth information");

            if(mac.isValid())
            {
                DeviceInfoParser::Instance().queryRemainderDeviceInfo("bluetoothctl", mac.value, articles, existArticles3,
                                                                      "ManulTrack__Bluetooth","Bluetooth information");
            }
        }

        DeviceInfoParser::Instance().queryRemainderDeviceInfo("lshw", device, articles, existArticles,
                                                              "ManulTrack__Bluetooth","Bluetooth information");

        addDevice( name.value, articles,  bluetoothList.size()+ pairedDevicesList.size() );

        if( hciconfigBluetoothList.size() + pairedDevicesList.size() > 1 )
        {
            QStringList tab =
            {
                name.value,
//                "Controller",
                vendor.value
            };

            tabList.push_back(tab);
        }

        if(i == 0)
        {
            overviewInfo_.value = vendor.value;
            overviewInfo_.value += " ";
            if( product.value.isEmpty() == false && product.value != tr("Unknown") )
            {
                overviewInfo_.value += product.value;
            }
            else
            {
                overviewInfo_.value += description.value;
            }
        }

        ++i;
    }

    foreach(const QString& device, pairedDevicesList)
    {
        articles.clear();
        existArticles.clear();

        ArticleStruct name(tr("Name"));
        name.queryData("paired-devices", device, "Name");
        articles.push_back(name);
        existArticles.insert("Name");

        ArticleStruct vendor(tr("Vendor"));
        vendor.queryData("paired-devices", device, "Vendor");
        articles.push_back(vendor);
        existArticles.insert("Vendor");

        ArticleStruct alias(tr("Alias"));
        alias.queryData("paired-devices", device, "Alias");
        articles.push_back(alias);
        existArticles.insert("Alias");

        ArticleStruct appearance(tr("Appearance"));
        appearance.queryData("paired-devices", device, "Appearance");
        articles.push_back(appearance);
        existArticles.insert("Appearance");

        ArticleStruct icon(tr("Icon"));
        icon.queryData("paired-devices", device, "Icon");
        articles.push_back(icon);
        existArticles.insert("Icon");

        ArticleStruct connected(tr("Connected"));
        connected.queryData("paired-devices", device, "Connected");
        articles.push_back(connected);
        existArticles.insert("Connected");

//        if(connected.value.compare("yes", Qt::CaseInsensitive) != 0)
//        {
//            continue;
//        }

        ArticleStruct paired(tr("Paired","Bluetooth Info"));
        paired.queryData("paired-devices", device, "Paired");
        articles.push_back(paired);
        existArticles.insert("Paired");

        ArticleStruct trusted(tr("Trusted","Bluetooth Info"));
        trusted.queryData("paired-devices", device, "Trusted");
        articles.push_back(trusted);
        existArticles.insert("Trusted");

        ArticleStruct blocked(tr("Blocked","Bluetooth Info"));
        blocked.queryData("paired-devices", device, "Blocked");
        articles.push_back(blocked);
        existArticles.insert("Blocked");

        DeviceInfoParser::Instance().queryRemainderDeviceInfo("paired-devices", device, articles, existArticles,
                                                              "ManulTrack__Bluetooth","Bluetooth information");

        if( device.isEmpty() == false )
        {
            auto upower = DeviceInfoParser::Instance().getCorrespondUpower(device);

            if(upower.isEmpty() == false )
            {
                ArticleStruct power(tr("Power","Bluetooth Info"));
                power.value = " ";
                articles.push_back(power);

                DeviceInfoParser::Instance().queryRemainderDeviceInfo("upower", upower, articles,QSet<QString>(),
                                                                      "ManulTrack__Bluetooth","Bluetooth information");
            }
        }

        addDevice( name.value, articles,  bluetoothList.size()+ pairedDevicesList.size() );

        if( hciconfigBluetoothList.size() + pairedDevicesList.size() > 1 )
        {
            QStringList tab =
            {
                name.value,
//                "Device",
                vendor.value
            };

            tabList.push_back(tab);
        }

//        if(i == 0)
//        {
//            overviewInfo_.value = vendor.value;
//            overviewInfo_.value += " ";
//            if( product.value.isEmpty() == false && product.value != tr("Unknown") )
//            {
//                overviewInfo_.value += product.value;
//            }
//            else
//            {
//                overviewInfo_.value += description.value;
//            }
//        }

        ++i;
    }

    if(connectedDeviceNumber > 0 && overviewInfo_.isValid())
    {
        overviewInfo_.value += " (";
        overviewInfo_.value += QString::number(connectedDeviceNumber);
        overviewInfo_.value += (" "+tr("Bluetooth Device(s) Connected")) ;
        overviewInfo_.value += ")";
    }

    if( hciconfigBluetoothList.size() + pairedDevicesList.size() > 1 )
    {
        QStringList headers = { tr("Name"),  /*tr("Type"),*/ tr("Vendor") };
        addTable( headers, tabList);
    }
}


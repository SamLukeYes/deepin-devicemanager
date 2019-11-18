/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     AaronZhang <ya.zhang@archermind.com>
 *
 * Maintainer: AaronZhang <ya.zhang@archermind.com>
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


BluetoothWidget::BluetoothWidget(QWidget *parent) : DeviceInfoWidgetBase(parent, DApplication::translate("Main", "Bluetooth"))
{
    initWidget();
}

void BluetoothWidget::initWidget()
{
    QStringList bluetoothList = DeviceInfoParserInstance.getLshwBluetoothList();

    if( bluetoothList.size() < 1 )
    {
        setCentralInfo("No Bluetooth Device found!");
        return;
    }

    QStringList hciconfigBluetoothList = DeviceInfoParserInstance.getHciconfigBluetoothControllerList();
    QStringList pairedDevicesList /*= DeviceInfoParserInstance.getOtherBluetoothctlPairedDevicesList()*/;
    //setTitle(DApplication::translate("Main", "Bluetooth") + " " + DApplication::translate("Main", " Info"));
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

        ArticleStruct name("Name");
        ArticleStruct vendor("Vendor");
        ArticleStruct product("Product");
        ArticleStruct description("Description");
        ArticleStruct mac("Mac Address");

        if( i < hciconfigBluetoothList.size() )
        {
            QString hciconfigName = hciconfigBluetoothList.at(i);

            name.queryData("hciconfig", hciconfigName, "Name");
            name.value.remove("\'");
            articles.push_back(name);
            existArticles2.insert("Name");

            vendor.queryData("lshw", device, "vendor", existArticles, articles);
            vendor.queryData("hciconfig", hciconfigName, "Manufacturer", existArticles2, articles);

            mac.queryData("hciconfig", hciconfigName, "BD Address", existArticles2, articles);

            product.queryData( "lshw", device, "product", existArticles, articles);

            description.queryData("lshw", device, "description", existArticles, articles);

            ArticleStruct serial("Serial");
            serial.queryData( "lshw", device, "serial", existArticles, articles);

            ArticleStruct linkPolicy("Link Policy");
            linkPolicy.queryData("hciconfig", hciconfigName, "Link policy", existArticles2, articles);

            ArticleStruct linkMode("Link Mode");
            linkMode.queryData("hciconfig", hciconfigName, "Link mode", existArticles2, articles);

            if(mac.isValid())
            {
                ArticleStruct powered("Powered");
                powered.queryData("bluetoothctl", mac.value, "Powered", existArticles3, articles);

                ArticleStruct discoverable("Discoverable");
                discoverable.queryData("bluetoothctl", mac.value, "Discoverable", existArticles3, articles);

                ArticleStruct pairable("Pairable");
                pairable.queryData("bluetoothctl", mac.value, "Pairable", existArticles3, articles);

                ArticleStruct modalias("Modalias");
                modalias.queryData("bluetoothctl", mac.value, "Modalias", existArticles3, articles);

                ArticleStruct discovering("Discovering");
                discovering.queryData("bluetoothctl", mac.value, "Discovering", existArticles3, articles);
            }
        }

        ArticleStruct physicalId("Physical id");
        physicalId.queryData("lshw", device, "physical id", existArticles, articles);

        ArticleStruct busInfo("Bus Info");
        busInfo.queryData("lshw", device, "bus info", existArticles, articles);

        ArticleStruct version("Version");
        version.queryData("lshw", device, "version", existArticles, articles);

        ArticleStruct capabilities("Capabilities");
        capabilities.queryData("lshw", device, "capabilities", existArticles, articles);

        if( i < hciconfigBluetoothList.size() )
        {
            DeviceInfoParserInstance.queryRemainderDeviceInfo("hciconfig", hciconfigBluetoothList.at(i), articles, existArticles2);

            if(mac.isValid())
            {
                DeviceInfoParserInstance.queryRemainderDeviceInfo("bluetoothctl", mac.value, articles, existArticles3);
            }
        }

        DeviceInfoParserInstance.queryRemainderDeviceInfo("lshw", device, articles, existArticles);

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
            if( product.value.isEmpty() == false && product.value != DApplication::translate("Main", "Unknown") )
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

        ArticleStruct name("Name");
        name.queryData("paired-devices", device, "Name");
        articles.push_back(name);
        existArticles.insert("Name");

        ArticleStruct vendor("Vendor");
        vendor.queryData("paired-devices", device, "Vendor");
        articles.push_back(vendor);
        existArticles.insert("Vendor");

        ArticleStruct alias("Alias");
        alias.queryData("paired-devices", device, "Alias");
        articles.push_back(alias);
        existArticles.insert("Alias");

        ArticleStruct appearance("Appearance");
        appearance.queryData("paired-devices", device, "Appearance");
        articles.push_back(appearance);
        existArticles.insert("Appearance");

        ArticleStruct icon("Icon");
        icon.queryData("paired-devices", device, "Icon");
        articles.push_back(icon);
        existArticles.insert("Icon");

        ArticleStruct connected("Connected");
        connected.queryData("paired-devices", device, "Connected");
        articles.push_back(connected);
        existArticles.insert("Connected");

        if(connected.value.compare("yes", Qt::CaseInsensitive) != 0)
        {
            continue;
        }

        ArticleStruct paired("Paired");
        paired.queryData("paired-devices", device, "Paired");
        articles.push_back(paired);
        existArticles.insert("Paired");

        ArticleStruct trusted("Trusted");
        trusted.queryData("paired-devices", device, "Trusted");
        articles.push_back(trusted);
        existArticles.insert("Trusted");

        ArticleStruct blocked("Blocked");
        blocked.queryData("paired-devices", device, "Blocked");
        articles.push_back(blocked);
        existArticles.insert("Blocked");

        DeviceInfoParserInstance.queryRemainderDeviceInfo("paired-devices", device, articles, existArticles);

        if( device.isEmpty() == false )
        {
            auto upower = DeviceInfoParserInstance.getCorrespondUpower(device);

            if(upower.isEmpty() == false )
            {
                ArticleStruct power("Power");
                power.value = " ";
                articles.push_back(power);

                DeviceInfoParserInstance.queryRemainderDeviceInfo("upower", upower, articles );
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
//            if( product.value.isEmpty() == false && product.value != DApplication::translate("Main", "Unknown") )
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

    if( hciconfigBluetoothList.size() + pairedDevicesList.size() > 1 )
    {
        QStringList headers = { "Name",  /*"Type",*/ "Vendor" };
        addTable( headers, tabList);
    }
}


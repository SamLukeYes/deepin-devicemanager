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

#include "usbdevicewidget.h"
#include "deviceinfoparser.h"
#include <DApplication>

DWIDGET_USE_NAMESPACE

UsbdeviceWidget::UsbdeviceWidget(QWidget *parent) : DeviceInfoWidgetBase(parent, tr("USB Devices"))
{
    initWidget();
}

//ArticleStruct UsbdeviceWidget::addArticleStruct(const QString &name, const QString &command, const QString &item, const QString &attri)
//{
//    ArticleStruct Name(name);
//    Name.queryData(command, item, attri);
//    m_articles.push_back(Name);
//    m_existArticles.insert(attri);
//    return Name;
//}

void UsbdeviceWidget::setTabList(int usbListSize, ArticleStruct &name, ArticleStruct &vendor, ArticleStruct &description)
{
    // 添加设备
    QString title = name.isValid() ? name.value : description.value;
    addDevice(title, m_articles, usbListSize);

    QStringList tab = {
        title,
        vendor.value
    };

    m_tabList.push_back(tab);

    // 添加overview_
    if (overviewInfo_.isValid()) {
        overviewInfo_.value += " / ";
    }

    QList<ArticleStruct> overArticle;
    overArticle << vendor << name;
    if (name.isValid() == false) {
        overArticle << description;
    }
    overviewInfo_.value += joinArticle(overArticle);
}

void UsbdeviceWidget::loadUSBWidgetFromhwinfo()
{
    QStringList usbdeviceList = DeviceInfoParser::Instance().getHwinfoOtherUSBList();

    if (usbdeviceList.size() < 1) {
        setCentralInfo(tr("No USB devices found"));
        return;
    }

    // 添加从hwinfo_usb.txt中获取的信息
    foreach (const QString &device, usbdeviceList) {
        m_articles.clear();
        m_existArticles.clear();

        // 添加USB相关信息
        ArticleStruct name = addArticleStruct(tr("Name"), "USB", device, "Device");
        ArticleStruct vendor = addArticleStruct(tr("Vendor"), "USB", device, "Vendor");
        addArticleStruct(tr("Model"), "USB", device, "Model");
        addArticleStruct(tr("Serial ID"), "USB", device, "Serial ID");
        addArticleStruct(tr("Version"), "USB", device, "Revision");
        addArticleStruct(tr("Status"), "USB", device, "Config Status");
        addArticleStruct(tr("Driver"), "USB", device, "Driver");
        addArticleStruct(tr("Speed"), "USB", device, "Speed");
        addArticleStruct(tr("BusID"), "USB", device, "SysFS BusID");
        ArticleStruct description = addArticleStruct(tr("Description"), "USB", device, "description");
        addArticleStruct(tr("Unique ID"), "USB", device, "Unique ID");
        addArticleStruct(tr("SysFS ID"), "USB", device, "SysFS ID");

//        DeviceInfoParser::Instance().queryRemainderDeviceInfo("USB", device, m_articles, m_existArticles);

        setTabList(usbdeviceList.size(), name, vendor, description);
    }

    // 当有多个USB设备，添加Table显示
    if (usbdeviceList.size() > 1) {
        QStringList headers = { tr("Name"), tr("Vendor") };
        addTable(headers, m_tabList);
    }

}

void UsbdeviceWidget::initWidget()
{
    QStringList usbdeviceList = DeviceInfoParser::Instance().getLshwOtherUsbdeviceList();

    // 优先从lshw.txt 中获取信息，若果没有则从hwinfo_usb.txt中获取
    if (usbdeviceList.size() < 1) {
        QStringList hwinfoUSBList = DeviceInfoParser::Instance().getHwinfoOtherUSBList();

        if (hwinfoUSBList.size() < 1) {
            setCentralInfo(tr("No USB devices found"));
            return;
        } else {
            loadUSBWidgetFromhwinfo();
            return;
        }
    }

    // 添加从lshw.txt 中获取的信息
    foreach (const QString &device, usbdeviceList) {
        m_articles.clear();
        m_existArticles.clear();

        // 添加USB相关信息
        ArticleStruct name = addArticleStruct(tr("Name"), "lshw", device, "product");
        ArticleStruct  description = addArticleStruct(tr("Description"), "lshw", device, "description");
        ArticleStruct  vendor = addArticleStruct(tr("Vendor"), "lshw", device, "vendor");
        addArticleStruct(tr("Bus Info"), "lshw", device, "bus info");
        addArticleStruct(tr("Physical ID"), "lshw", device, "physical id");
        addArticleStruct(tr("Logical Name"), "lshw", device, "logical name");
        addArticleStruct(tr("Version"), "lshw", device, "version");
        addArticleStruct(tr("Width"), "lshw", device, "width");
        addArticleStruct(tr("Clock"), "lshw", device, "clock");
        addArticleStruct(tr("Capabilities"), "lshw", device, "capabilities");
        addArticleStruct(tr("driver"), "lshw", device, "driver");
        addArticleStruct(tr("maxpower"), "lshw", device, "maxpower");
        addArticleStruct(tr("speed"), "lshw", device, "speed");
//        addArticleStruct(tr("Capabilities"), "lshw", device, "capabilities");

        m_existArticles.insert("product");
        m_existArticles.insert("description");
        m_existArticles.insert("vendor");
        m_existArticles.insert("bus info");
        m_existArticles.insert("physical id");
        m_existArticles.insert("logical name");
        m_existArticles.insert("version");
        m_existArticles.insert("width");
        m_existArticles.insert("clock");
        m_existArticles.insert("capabilities");
        m_existArticles.insert("driver");
        m_existArticles.insert("maxpower");
        m_existArticles.insert("speed");

        // 添加USB info的其他项
        DeviceInfoParser::Instance().queryRemainderDeviceInfo("lshw", device, m_articles, m_existArticles, "ManulTrack__USB", "USB Information");

        setTabList(usbdeviceList.size(), name, vendor, description);
    }

    // 当有多个USB设备，添加Table显示
    if (usbdeviceList.size() > 1) {
        QStringList headers = { tr("Name"), tr("Vendor") };
        addTable(headers, m_tabList);
    }
}


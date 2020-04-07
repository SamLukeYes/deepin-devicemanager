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

#include "otherdevicewidget.h"
#include "deviceinfoparser.h"
#include <DApplication>

DWIDGET_USE_NAMESPACE

OtherDevicesWidget::OtherDevicesWidget(QWidget *parent) : DeviceInfoWidgetBase(parent, tr("Other Devices", "Other Input Devices"))
{
    initWidget();
}

void OtherDevicesWidget::initWidget()
{
    QStringList otherDeviceList = DeviceInfoParser::Instance().getLshwOtherDeviceList();

    if (otherDeviceList.size() < 1) {
        setCentralInfo(tr("No other devices found"));
        return;
    }

    QList<QStringList> tabList;
    QList<ArticleStruct> articles;
    QSet<QString> existArticles;

    foreach (const QString &device, otherDeviceList) {
        articles.clear();
        existArticles.clear();

        ArticleStruct name(tr("Name"));
        name.queryData("lshw", device, "product");
        articles.push_back(name);
        existArticles.insert("product");

        ArticleStruct type(tr("Type"));
        type.queryData("lshw", device, "Type");
        articles.push_back(type);
        existArticles.insert("Type");

        ArticleStruct description(tr("Description"));
        description.queryData("lshw", device, "description");
        articles.push_back(description);
        existArticles.insert("description");

        ArticleStruct vendor(tr("Vendor"));
        vendor.queryData("lshw", device, "vendor");
        articles.push_back(vendor);
        existArticles.insert("vendor");

        ArticleStruct busInfo(tr("Bus Info"));
        busInfo.queryData("lshw", device, "bus info");
        articles.push_back(busInfo);
        existArticles.insert("bus info");

        ArticleStruct physicalId(tr("Physical ID"));
        physicalId.queryData("lshw", device, "physical id");
        articles.push_back(physicalId);
        existArticles.insert("physical id");

        ArticleStruct version(tr("Version"));
        version.queryData("lshw", device, "version");
        articles.push_back(version);
        existArticles.insert("version");

        ArticleStruct width(tr("Width"));
        width.queryData("lshw", device, "width");
        articles.push_back(width);
        existArticles.insert("width");

        ArticleStruct clock(tr("Clock"));
        clock.queryData("lshw", device, "clock");
        articles.push_back(clock);
        existArticles.insert("clock");

        ArticleStruct iomemory(tr("iomemory"));
        iomemory.queryData("lshw", device, "iomemory");
        articles.push_back(iomemory);
        existArticles.insert("iomemory");

        ArticleStruct latency(tr("latency"));
        latency.queryData("lshw", device, "latency");
        articles.push_back(latency);
        existArticles.insert("latency");

        ArticleStruct memory(tr("memory"));
        memory.queryData("lshw", device, "memory");
        articles.push_back(memory);
        existArticles.insert("memory");

        ArticleStruct capabilities(tr("Capabilities"));
        capabilities.queryData("lshw", device, "capabilities");
        articles.push_back(capabilities);
        existArticles.insert("capabilities");

        DeviceInfoParser::Instance().queryRemainderDeviceInfo("lshw", device, articles, existArticles);

        QString titleValue = name.value;
        if (name.isValid() == false || name.value == vendor.value) {
            if (description.isValid()) {
                titleValue = description.value;
            } else {
                titleValue = type.value;
            }
        }

        addDevice(titleValue, articles, otherDeviceList.size());

        if (otherDeviceList.size() > 1) {
            QStringList tab = {
                titleValue,
                vendor.value
            };

            tabList.push_back(tab);
        }

        if (overviewInfo_.value.isEmpty() == true) {
            overviewInfo_.value = vendor.value;
            overviewInfo_.value += " ";
            overviewInfo_.value += titleValue;
        }
    }

    if (otherDeviceList.size() > 1) {
        QStringList headers = {tr("Name"), tr("Vendor")};
        addTable(headers, tabList);
    }
}


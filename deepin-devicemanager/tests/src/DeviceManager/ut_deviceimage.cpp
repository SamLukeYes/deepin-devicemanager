/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:      zhangkai <zhangkai@uniontech.com>
* Maintainer:  zhangkai <zhangkai@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "DeviceImage.h"
#include "EnableManager.h"

#include "../ut_Head.h"
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>

#include <gtest/gtest.h>
#include "../stub.h"

class DeviceImage_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        m_deviceImage = new DeviceImage;
    }
    void TearDown()
    {
        delete m_deviceImage;
    }
    DeviceImage *m_deviceImage;
};

TEST_F(DeviceImage_UT, DeviceImage_UT_setInfoFromLshw)
{
    QMap<QString, QString> mapinfo;
    mapinfo.insert("bus info", "1@n");
    m_deviceImage->m_KeyToLshw = "1@n";
    m_deviceImage->setInfoFromLshw(mapinfo);
}

TEST_F(DeviceImage_UT, DeviceImage_UT_setInfoFromHwinfo)
{
    QMap<QString, QString> mapinfo;
    mapinfo.insert("SysFS BusID", "/-/://");
    m_deviceImage->setInfoFromHwinfo(mapinfo);
}

TEST_F(DeviceImage_UT, DeviceImage_UT_name)
{
    m_deviceImage->name();
    m_deviceImage->driver();
    m_deviceImage->subTitle();
}

EnableDeviceStatus ut_image_enableDeviceByDriver()
{
    return EnableDeviceStatus::EDS_Faild;
}

TEST_F(DeviceImage_UT, DeviceImage_UT_getOverviewInfo)
{
    m_deviceImage->getOverviewInfo();

    Stub stub;
    stub.set(ADDR(EnableManager, enableDeviceByDriver), ut_image_enableDeviceByDriver);
    m_deviceImage->setEnable(true);
    //    m_deviceImage->enable();
}

TEST_F(DeviceImage_UT, DeviceImage_UT_initFilterKey)
{
    m_deviceImage->initFilterKey();
    m_deviceImage->loadBaseDeviceInfo();
    m_deviceImage->loadOtherDeviceInfo();
    m_deviceImage->loadTableHeader();
    m_deviceImage->loadTableData();
}

TEST_F(DeviceImage_UT, DeviceImage_UT_setInfoFromInput)
{
    m_deviceImage->setInfoFromInput();
}

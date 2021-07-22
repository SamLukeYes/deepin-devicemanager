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
#include "../src/DeviceManager/DeviceMemory.h"
#include "../src/DeviceManager/DeviceBios.h"
#include "../ut_Head.h"
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>

#include <gtest/gtest.h>
#include "../stub.h"

class DeviceMemory_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        m_deviceMemory = new DeviceMemory;
    }
    void TearDown()
    {
        delete m_deviceMemory;
    }
    DeviceMemory *m_deviceMemory;
};

TEST_F(DeviceMemory_UT, DeviceMemory_UT_setInfoFromLshw)
{
    QMap<QString, QString> mapinfo;
    mapinfo.insert("/", "/");
    m_deviceMemory->m_Size = "2GiB 4MiB";
    m_deviceMemory->m_Speed = "2MT/s";
    m_deviceMemory->setInfoFromLshw(mapinfo);
}

TEST_F(DeviceMemory_UT, DeviceMemory_UT_setInfoFromDmidecode)
{
    QMap<QString, QString> mapinfo;
    mapinfo.insert("/", "/");
    m_deviceMemory->m_ConfiguredSpeed = "2MT/s";
    m_deviceMemory->m_Type = "<OUT OF SPEC>";
    m_deviceMemory->setInfoFromDmidecode(mapinfo);
}

TEST_F(DeviceMemory_UT, DeviceMemory_UT_initFilterKey)
{
    m_deviceMemory->initFilterKey();
    m_deviceMemory->loadBaseDeviceInfo();
    m_deviceMemory->loadOtherDeviceInfo();
    m_deviceMemory->loadTableHeader();
    m_deviceMemory->loadTableData();
}

TEST_F(DeviceMemory_UT, DeviceMemory_UT_name)
{
    m_deviceMemory->name();
    m_deviceMemory->driver();
    m_deviceMemory->getOverviewInfo();
}

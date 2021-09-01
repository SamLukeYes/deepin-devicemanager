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
#include "DeviceWidget.h"
#include "LoadInfoThread.h"
#include "GetInfoPool.h"
#include "GenerateDevicePool.h"

#include "../ut_Head.h"
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>

#include <gtest/gtest.h>
#include "../stub.h"

class GetInfoPool_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        m_readFilePool = new GetInfoPool;
    }
    void TearDown()
    {
        delete m_readFilePool;
    }
    GetInfoPool *m_readFilePool;
};

class CmdTask_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        m_readFilePool = new GetInfoPool;
        m_cmdTask = new CmdTask("", "", "", m_readFilePool);
    }
    void TearDown()
    {
        delete m_readFilePool;
        delete m_cmdTask;
    }
    CmdTask *m_cmdTask;
    GetInfoPool *m_readFilePool;
};

TEST_F(CmdTask_UT, CmdTask_UT_run)
{
    m_cmdTask->run();
}

void ut_ThreadPool_start()
{
    return;
}

TEST_F(GetInfoPool_UT, ReadFilePool_UT_readAllFile)
{
    Stub stub;
    stub.set(ADDR(QThreadPool, start), ut_ThreadPool_start);
    m_readFilePool->getAllInfo();
}

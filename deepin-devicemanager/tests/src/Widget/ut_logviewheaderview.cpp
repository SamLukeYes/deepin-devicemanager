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
#include "../src/Widget/logviewheaderview.h"

#include "../ut_Head.h"
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>

#include <gtest/gtest.h>
#include "../stub.h"

class LogviewHeaderView_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        m_logViewHeaderView = new LogViewHeaderView(Qt::Horizontal);
    }
    void TearDown()
    {
        delete m_logViewHeaderView;
    }
    LogViewHeaderView *m_logViewHeaderView;
};

//typedef QVariant (*fptr)(QAbstractItemModel*);
//fptr QAbstractItemModel_headerData = (fptr)((QVariant(QAbstractItemModel::*))&QAbstractItemModel::headerData);

int ut_headerview_pixelMetric()
{
    return 10;
}

QVariant ut_model_headerData()
{
    return "header";
}

TEST_F(LogviewHeaderView_UT, UT_paintSection)
{
    QPainter painter(m_logViewHeaderView);
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *widget) const)ADDR(DStyle, pixelMetric), ut_headerview_pixelMetric);
    //        stub.set(header, ut_model_headerData);
    m_logViewHeaderView->paintSection(&painter, QRect(10, 10, 10, 10), 0);
}

TEST_F(LogviewHeaderView_UT, ut_paintEvent)
{
    QPaintEvent paint(QRect(m_logViewHeaderView->rect()));
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *widget) const)ADDR(DStyle, pixelMetric), ut_headerview_pixelMetric);
    m_logViewHeaderView->paintEvent(&paint);
}

TEST_F(LogviewHeaderView_UT, ut_sizeHint)
{
    m_logViewHeaderView->sizeHint();
}

void ut_initStyleOption()
{
    return;
}

TEST_F(LogviewHeaderView_UT, ut_sectionSizeHint)
{
    //        Stub stub;
    //        stub.set(ADDR(LogViewHeaderView,initStyleOption),ut_initStyleOption);
    //        stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *widget) const)ADDR(DStyle, pixelMetric), ut_headerview_pixelMetric);
    //        stub.set(QAbstractItemModel_headerData, ut_model_headerData);
    //        m_logViewHeaderView->sectionSizeHint(0);
}

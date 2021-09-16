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
#include "../src/Widget/TipsWidget.h"

#include "../ut_Head.h"
#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>

#include <gtest/gtest.h>
class TipsWidget_UT : public UT_HEAD
{
public:
    void SetUp()
    {
        tWidget = new TipsWidget;
    }
    void TearDown()
    {
        delete tWidget;
    }
    TipsWidget *tWidget;
};

TEST_F(TipsWidget_UT, ut_onUpdateTheme)
{
    tWidget->onUpdateTheme();
}

TEST_F(TipsWidget_UT, ut_setText)
{
    tWidget->setText(" ");
}

TEST_F(TipsWidget_UT, ut_setAlignment)
{
    tWidget->setAlignment(Qt::AlignTop);
}

TEST_F(TipsWidget_UT, ut_adjustContent)
{
    tWidget->adjustContent("/");
}

TEST_F(TipsWidget_UT, ut_PaintEvent)
{
    QPaintEvent paint(QRect(tWidget->rect()));
    tWidget->paintEvent(&paint);
}

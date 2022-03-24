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
#ifndef URLCHOOSEREDIT_H
#define URLCHOOSEREDIT_H

//#include <DLineEdit>
#include <DWidget>

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DLineEdit;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class UrlChooserEdit : public DWidget
{
    Q_OBJECT
public:
    explicit UrlChooserEdit(QWidget *parent = nullptr);

    /**
     * @brief text
     */
    QString text() const;

private:
    /**
     * @brief initUI
     */
    void initUI();

    /**
     * @brief initConnections
     */
    void initConnections();

    /**
     * @brief checkLocalFolder 检测是否本地路径
     */
    void checkLocalFolder(const QString &path);

signals:

    /**
     * @brief signalNotLocalFolder
     */
    void signalNotLocalFolder(bool isLocal);

public slots:

    /**
     * @brief slotChooseUrl
     */
    void slotChooseUrl();


private:
    Dtk::Widget::DLineEdit *mp_urlEdit;
    Dtk::Widget::DSuggestButton *mp_urlBtn;
    QString mp_folderPath;
    QString mp_elidParh;
};
#endif // URLCHOOSEREDIT_H

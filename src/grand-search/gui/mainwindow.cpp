/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
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
#include "mainwindow_p.h"
#include "mainwindow.h"
#include "entrance/entrancewidget.h"
#include "exhibition/exhibitionwidget.h"
#include "gui/datadefine.h"

#include <DApplication>
#include <DLabel>

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QScreen>
#include <QKeyEvent>

class MainWindowGlobal : public MainWindow {};
Q_GLOBAL_STATIC(MainWindowGlobal, mainWindowGlobal)

DWIDGET_USE_NAMESPACE
using namespace GrandSearch;

static const uint MainWindowWidth           = 760;      // 主界面宽度
static const uint MainWindowHeight          = 68;       // 主界面高度
static const uint MainWindowExpandHeight    = 520;      // 主界面高度

MainWindowPrivate::MainWindowPrivate(MainWindow *parent)
    : q_p(parent)
{

}

MainWindow::MainWindow(QWidget *parent)
    : DBlurEffectWidget(parent)
    , d_p(new MainWindowPrivate(this))
{
    initUI();
    initConnect();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    activeMainWindow();
    return DBlurEffectWidget::mousePressEvent(event);
}

MainWindow::~MainWindow()
{

}

void MainWindow::connectToController()
{
    Q_ASSERT(d_p->m_entranceWidget);
    Q_ASSERT(d_p->m_exhibitionWidget);

    d_p->m_entranceWidget->connectToController();
    d_p->m_exhibitionWidget->connectToController();
}

void MainWindow::showExhitionWidget(bool bShow)
{
    Q_ASSERT(d_p->m_exhibitionWidget);

    //已经显示/隐藏，就不再重复显示/隐藏
    if (d_p->m_exhibitionWidget && !d_p->m_exhibitionWidget->isHidden() == bShow)
        return;

    d_p->m_exhibitionWidget->setVisible(bShow);
}

void MainWindow::showSerachNoContent(bool bShow)
{
    Q_ASSERT(d_p->m_entranceWidget);
    Q_ASSERT(d_p->m_exhibitionWidget);

    if (Q_LIKELY(d_p->m_searchNoContentWidget))
        d_p->m_searchNoContentWidget->setVisible(bShow);

    if (bShow) {
        if (Q_LIKELY(d_p->m_exhibitionWidget))
            d_p->m_exhibitionWidget->setVisible(false);
    }
}

void MainWindow::onPrimaryScreenChanged(const QScreen *screen)
{
    // 主窗口显示在主屏
    disconnect(this, SLOT(onGeometryChanged(const QRect &)));
    onGeometryChanged(screen->geometry());
    connect(screen, &QScreen::geometryChanged, this, &MainWindow::onGeometryChanged);
}

void MainWindow::onGeometryChanged(const QRect &geometry)
{
    int sWidth = geometry.width();
    int sHeight = geometry.height();

    // 移动窗口到屏幕的居中偏上位置
    sWidth = geometry.x() + sWidth/2 - int(MainWindowWidth/2);
    sHeight = geometry.y() + sHeight/4;
    move(sWidth, sHeight);
}

void MainWindow::onSearchTextChanged(const QString &txt)
{
    // todo:后续流程优化，展示界面和无内容界面显隐控制应依赖搜索结果发出的信号
    showSerachNoContent(false);
    if (txt.isEmpty()) {
        showExhitionWidget(false);
    } else {
        showExhitionWidget(true);
    }

    updateMainWindowHeight();
}

void MainWindow::onApplicationStateChanged(const Qt::ApplicationState state)
{
    if (Qt::ApplicationInactive == state) {
        qDebug() << "application state change to inactive,so i will exit.";
        this->close();
    } else if (Qt::ApplicationActive == state) {
        qDebug() << "application state change to active.";
        activeMainWindow();
    }
}

void MainWindow::onCloseWindow()
{
    this->close();
}

MainWindow *MainWindow::instance()
{
    return mainWindowGlobal;
}

void MainWindow::initUI()
{
    // 禁用窗口管理器并置顶
    setWindowFlags(Qt::BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::NoFocus);

    // 控制界面大小和位置
    setFixedSize(MainWindowWidth, MainWindowHeight);
    QScreen *screen = qApp->primaryScreen();
    onPrimaryScreenChanged(screen);

    // 模糊模式
    setBlendMode(DBlurEffectWidget::BehindWindowBlend);

    // 搜索入口界面
    d_p->m_entranceWidget = new EntranceWidget(this);
    d_p->m_entranceWidget->setFixedHeight(MainWindowHeight);

    // 结果展示界面
    d_p->m_exhibitionWidget = new ExhibitionWidget(this);
    d_p->m_exhibitionWidget->hide();

    // 未搜到内容界面
    d_p->m_searchNoContentWidget = new DLabel(tr("no content found"),this);
    d_p->m_searchNoContentWidget->setAlignment(Qt::AlignCenter);
    d_p->m_searchNoContentWidget->hide();

    d_p->m_mainLayout = new QVBoxLayout(this);
    d_p->m_mainLayout->addWidget(d_p->m_entranceWidget);
    d_p->m_mainLayout->addWidget(d_p->m_exhibitionWidget);
    d_p->m_mainLayout->addWidget(d_p->m_searchNoContentWidget);

    // 根据设计图调整主界面布局，限制边距和内容间距为0
    d_p->m_mainLayout->setSpacing(0);
    d_p->m_mainLayout->setMargin(0);

    this->setLayout(d_p->m_mainLayout);
}

void MainWindow::initConnect()
{
    Q_ASSERT(d_p->m_entranceWidget);
    Q_ASSERT(d_p->m_exhibitionWidget);

    // 通知查询控制模块发起新的搜索
    connect(d_p->m_entranceWidget, &EntranceWidget::searchTextChanged, this, &MainWindow::searchTextChanged);

    // 展开展示界面
    connect(d_p->m_entranceWidget, &EntranceWidget::searchTextChanged, this, &MainWindow::onSearchTextChanged);

    // 监控主屏改变信号，及时更新窗口位置
    connect(qApp, &QGuiApplication::primaryScreenChanged, this, &MainWindow::onPrimaryScreenChanged);

    // 进程变为非激活状态时，退出
    connect(qApp, &QGuiApplication::applicationStateChanged, this, &MainWindow::onApplicationStateChanged);

    connect(d_p->m_entranceWidget, &EntranceWidget::sigSelectPreviousItem, d_p->m_exhibitionWidget, &ExhibitionWidget::onSelectPreviousItem);
    connect(d_p->m_entranceWidget, &EntranceWidget::sigSelectNextItem, d_p->m_exhibitionWidget, &ExhibitionWidget::onSelectNextItem);
    connect(d_p->m_entranceWidget, &EntranceWidget::sigHandleItem, d_p->m_exhibitionWidget, &ExhibitionWidget::onHandleItem);
    connect(d_p->m_entranceWidget, &EntranceWidget::sigCloseWindow, this, &MainWindow::onCloseWindow);

    connect(d_p->m_exhibitionWidget, &ExhibitionWidget::sigAppIconChanged, d_p->m_entranceWidget, &EntranceWidget::onAppIconChanged);
    connect(d_p->m_exhibitionWidget, &ExhibitionWidget::sigCloseWindow, this, &MainWindow::onCloseWindow);
}

void MainWindow::activeMainWindow()
{
    activateWindow();
    raise();
}

void MainWindow::updateMainWindowHeight()
{
    Q_ASSERT(d_p->m_exhibitionWidget);
    Q_ASSERT(d_p->m_searchNoContentWidget);

    bool bExhibitionWidgetShow = false;
    bool bSearchNoContentWidgetShow = false;

    if (Q_LIKELY(d_p->m_exhibitionWidget))
        bExhibitionWidgetShow = !d_p->m_exhibitionWidget->isHidden();
    if (Q_LIKELY(d_p->m_searchNoContentWidget))
        bSearchNoContentWidgetShow = !d_p->m_searchNoContentWidget->isHidden();

    if (bExhibitionWidgetShow || bSearchNoContentWidgetShow)
        this->setFixedSize(MainWindowWidth,MainWindowExpandHeight);
    else
        this->setFixedSize(MainWindowWidth,MainWindowHeight);
}

void MainWindow::showEvent(QShowEvent *event)
{
    emit visibleChanged(true);

    // 已禁用窗口管理器，在窗口被显示后，需要激活该窗口
    QTimer::singleShot(1, this, &MainWindow::activeMainWindow);

    return DBlurEffectWidget::showEvent(event);
}

void MainWindow::hideEvent(QHideEvent *event)
{
    emit visibleChanged(false);
    return DBlurEffectWidget::hideEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 通知查询控制器停止搜索
    emit terminateSearch();
    return DBlurEffectWidget::closeEvent(event);
}
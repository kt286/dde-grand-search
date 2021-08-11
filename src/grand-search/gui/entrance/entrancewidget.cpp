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
#include "entrancewidget_p.h"
#include "entrancewidget.h"
#include "gui/datadefine.h"

//调试使用，最后发布时需删除todo
#include "gui/exhibition/matchresult/groupwidget.h"

#include <DSearchEdit>
#include <DStyle>
#include <DLabel>

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidgetAction>
#include <QAction>
#include <QTimer>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QEvent>

DWIDGET_USE_NAMESPACE
using namespace GrandSearch;

static const uint DelayReponseTime          = 50;       // 输入延迟搜索时间
static const uint EntraceWidgetWidth        = 740;      // 搜索界面宽度度
static const uint EntraceWidgetHeight       = 48;       // 搜索界面高度
static const uint WidgetMargins             = 10;       // 界面边距
static const uint SearchMaxLength           = 512;      // 输入最大字符限制

static const uint LabelIconSize             = 26;       // 标签应用图标显示大小
static const uint LabelSize                 = 32;       // 标签大小

EntranceWidgetPrivate::EntranceWidgetPrivate(EntranceWidget *parent)
    : q_p(parent)
{
    m_delayChangeTimer = new QTimer(this);
    m_delayChangeTimer->setSingleShot(true);
    m_delayChangeTimer->setInterval(DelayReponseTime);

    connect(m_delayChangeTimer, &QTimer::timeout, this, &EntranceWidgetPrivate::notifyTextChanged);
}

void EntranceWidgetPrivate::delayChangeText()
{
    Q_ASSERT(m_delayChangeTimer);

    m_delayChangeTimer->start();
}

void EntranceWidgetPrivate::notifyTextChanged()
{
    Q_ASSERT(m_searchEdit);

    const QString &currentSearchText = m_searchEdit->text().trimmed();
    emit q_p->searchTextChanged(currentSearchText);
}

void EntranceWidgetPrivate::setLineEditFocus()
{
    Q_ASSERT(m_searchEdit);

    m_searchEdit->lineEdit()->setFocus();
}

void EntranceWidgetPrivate::onFocusObjectChanged(QObject *obj)
{
    Q_ASSERT(m_searchEdit);

    if (obj && obj != m_searchEdit->lineEdit()) {
        // 收到焦点改变信号后，必须等已有事件循环结束后，设置才能生效
        QTimer::singleShot(1, this, &EntranceWidgetPrivate::setLineEditFocus);
    }
}

void EntranceWidgetPrivate::showMenu(const QPoint &pos)
{
    Q_ASSERT(m_lineEdit);

    QMenu *menu = new QMenu;
    QAction *action = nullptr;

    action = menu->addAction(tr("Cut"));
    action->setEnabled(m_lineEdit->hasSelectedText() && m_lineEdit->echoMode() == QLineEdit::Normal);
    connect(action, &QAction::triggered, m_lineEdit, &QLineEdit::cut);

    action = menu->addAction(tr("Copy"));
    action->setEnabled(m_lineEdit->hasSelectedText() && m_lineEdit->echoMode() == QLineEdit::Normal);
    connect(action, &QAction::triggered, m_lineEdit, &QLineEdit::copy);

    action = menu->addAction(tr("Paste"));
    action->setEnabled(!QGuiApplication::clipboard()->text().isEmpty());
    connect(action, &QAction::triggered, m_lineEdit, &QLineEdit::paste);

    QRect rect(pos, menu->sizeHint());
    m_menuRect = rect;
    menu->exec(pos);
    m_menuRect = QRect();
    delete menu;
}

EntranceWidget::EntranceWidget(QWidget *parent)
    : DWidget(parent)
    , d_p(new EntranceWidgetPrivate(this))
{
    initUI();
    initConnections();
}

EntranceWidget::~EntranceWidget()
{

}

void EntranceWidget::connectToController()
{

}

void EntranceWidget::showLabelAppIcon(bool bVisile)
{
    Q_ASSERT(d_p->m_appIconLabel);

    d_p->m_appIconLabel->setVisible(bVisile);
}

QRect EntranceWidget::getMenuRect() const
{
    return d_p->m_menuRect;
}

void EntranceWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
}

bool EntranceWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_p->m_lineEdit && QEvent::KeyPress == event->type()) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (Q_LIKELY(keyEvent)) {
            int key = keyEvent->key();
            switch (key) {
            case Qt::Key_Up : {
                emit sigSelectPreviousItem();
                return true;
            }
            case Qt::Key_Tab :
            case Qt::Key_Down : {
                emit sigSelectNextItem();
                return true;
            }
            case Qt::Key_Return:
            case Qt::Key_Enter: {
                emit sigHandleItem();
                return true;
            }
            case Qt::Key_Escape : {
                emit sigCloseWindow();
                return true;
            }
            default:break;
            }
        }
    } else if (watched == d_p->m_lineEdit && QEvent::ContextMenu == event->type()) {
        QContextMenuEvent *contextMenuEvent = static_cast<QContextMenuEvent*>(event);
        if (Q_LIKELY(contextMenuEvent)) {
            d_p->showMenu(contextMenuEvent->globalPos());
            return true;
        }
    }
    return DWidget::eventFilter(watched, event);
}

void EntranceWidget::initUI()
{
    d_p->m_searchEdit = new DSearchEdit(this);
    d_p->m_lineEdit = d_p->m_searchEdit->lineEdit();
    d_p->m_lineEdit->setMaxLength(SearchMaxLength);
    d_p->m_lineEdit->installEventFilter(this);

    d_p->m_appIconLabel = new DLabel(d_p->m_searchEdit);
    d_p->m_appIconLabel->setFixedSize(LabelSize, LabelSize);

    QWidgetAction* action = new QWidgetAction(d_p->m_lineEdit);
    action->setDefaultWidget(d_p->m_appIconLabel);
    d_p->m_lineEdit->addAction(action, QLineEdit::TrailingPosition);

    // 搜索框界面布局设置
    // 必须对搜索框控件的边距和间隔设置为0,否则其内含的LineEdit不满足大小显示要求
    d_p->m_searchEdit->layout()->setSpacing(0);
    d_p->m_searchEdit->layout()->setMargin(0);
    d_p->m_searchEdit->lineEdit()->setFixedSize(EntraceWidgetWidth, EntraceWidgetHeight);

    // 搜索框提示信息设置
    d_p->m_searchEdit->setPlaceHolder(tr("Search"));
    d_p->m_searchEdit->setPlaceholderText(tr("What would you like to search for?"));

    // 搜索界面布局设置
    d_p->m_mainLayout = new QHBoxLayout(this);
    d_p->m_mainLayout->addWidget(d_p->m_searchEdit);
    // 根据设计图要求，设置边距和间隔
    d_p->m_mainLayout->setSpacing(0);
    d_p->m_mainLayout->setMargin(WidgetMargins);

    this->setLayout(d_p->m_mainLayout);

    // todo 搜索框图标与清除按钮定制
//    QAction *leftaction = d_p->m_searchEdit->findChild<QAction *>("_d_search_leftAction");
//    if (leftaction) {
//        leftaction->setIcon(QIcon(":/icons/skin/icons/search_36px.svg"));
//    }
//    QAction *clearAction = d_p->m_searchEdit->findChild<QAction *>(QLatin1String("_q_qlineeditclearaction"));
//    if (clearAction) {
//        clearAction->setIcon(QIcon(":/icons/skin/icons/clear_36px.svg"));
//    }

//    d_p->m_searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    d_p->m_searchEdit->lineEdit()->setFixedSize(SEARCHEIT_WIDTH, SEARCHEIT_HEIGHT);
//    this->setMouseTracking(true);
//    DStyle::setFocusRectVisible(d_p->m_searchEdit->lineEdit(), true);
//    setFocusPolicy(Qt::NoFocus);
}

void EntranceWidget::initConnections()
{
    Q_ASSERT(d_p->m_searchEdit);

    // 输入改变时重置定时器，避免短时间内发起大量无效调用
    connect(d_p->m_searchEdit, &DSearchEdit::textChanged, d_p.data(), &EntranceWidgetPrivate::delayChangeText);

    // 焦点改变后，将焦点设置回输入框
    connect(qApp, &QGuiApplication::focusObjectChanged, d_p.data(), &EntranceWidgetPrivate::onFocusObjectChanged);
}

void EntranceWidget::onAppIconChanged(const QString &appIconName)
{
    //app图标名称为空，隐藏appIcon显示
    if (appIconName.isEmpty()) {
        showLabelAppIcon(false);
        d_p->m_appIconName = appIconName;
        return;
    }

    showLabelAppIcon(true);

    if (appIconName == d_p->m_appIconName)
        return;

    d_p->m_appIconName = appIconName;

    //刷新应用图标显示
    const int size = LabelIconSize;
    const qreal ratio = devicePixelRatioF();
    QIcon icon = QIcon::fromTheme(appIconName);
    d_p->m_appIconLabel->setPixmap(icon.pixmap(int(size * ratio), int(size * ratio)));
}

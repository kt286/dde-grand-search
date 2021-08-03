/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liuzhangjian@uniontech.com>
 *
 * Maintainer: liuzhangjian<liuzhangjian@uniontech.com>
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
#ifndef FSWORKER_H
#define FSWORKER_H

#include "searcher/proxyworker.h"

extern "C" {
#include "fsearch.h"
}

class FsWorker : public ProxyWorker
{
public:
    explicit FsWorker(const QString &name, QObject *parent = nullptr);

    void setContext(const QString &context) Q_DECL_OVERRIDE;
    bool isAsync() const Q_DECL_OVERRIDE;
    bool working(void *context) Q_DECL_OVERRIDE;
    void terminate() Q_DECL_OVERRIDE;
    Status status() Q_DECL_OVERRIDE;
    bool hasItem() const Q_DECL_OVERRIDE;
    GrandSearch::MatchedItemMap takeAll() Q_DECL_OVERRIDE;
    void setFsearchApp(FsearchApplication *app);
private:
    QString group() const;
    static void callbackReceiveResults(void *data, void *sender);
    void appendSearchResult(const QString &fileName);
private:
    FsearchApplication *m_app = nullptr;
    QAtomicInt m_status = Ready;
    QString m_context;
    QMimeDatabase m_mimeDB;

    //搜索结果
    mutable QMutex m_mtx;
    GrandSearch::MatchedItems m_items;

    QWaitCondition m_waitCondition;
    QMutex m_conditionMtx;

    QTime m_time;
};

#endif // FSWORKER_H
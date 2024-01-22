/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include "CosuResolveAbstract.h"
#include <QObject>
#include <memory>

namespace zypp
{
class PoolQuery;
};

class CosuZypp : public CosuResolveAbstract
{
    Q_OBJECT
public:
    CosuZypp(QObject *parent = nullptr);
    ~CosuZypp();

    void resolvePackage(const QString &packageName) override;
    void refreshCache(bool force = true) override;
    void setRepositoryAlias(const QString &alias);

private:
    QString m_repositoryAlias;
    std::unique_ptr<zypp::PoolQuery> m_poolQuery;
};

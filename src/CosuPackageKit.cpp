/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "CosuPackageKit.h"
#include "cosu_debug_packagekit.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDebug>
#include <QDir>

CosuPackageKit::CosuPackageKit(QObject *parent)
    : CosuResolveAbstract(parent)
{
    connect(PackageKit::Daemon::global(),
            &PackageKit::Daemon::changed,
            this,
            &CosuPackageKit::onChanged);
    connect(PackageKit::Daemon::global(),
            &PackageKit::Daemon::networkStateChanged,
            this,
            &CosuPackageKit::networkStateChanged);
}

CosuPackageKit::~CosuPackageKit()
{
    if (m_cacheTrans) {
        if (m_cacheTrans->allowCancel())
            m_cacheTrans->cancel();
        m_cacheTrans->deleteLater();
    }
    if (m_resolveTrans) {
        if (m_resolveTrans->allowCancel())
            m_resolveTrans->cancel();
        m_resolveTrans->deleteLater();
    }
}

void CosuPackageKit::onErrorCode(PackageKit::Transaction::Error error,
                                 const QString &details)
{
    emit resolverError(QStringLiteral("PackageKit transaction error: %1 : %2")
                           .arg(error)
                           .arg(details));
}

void CosuPackageKit::onChanged()
{
    qCDebug(COSU_PACKAGEKIT) << "Daemon changed";
}

bool CosuPackageKit::isNetworkOnline() const
{
    return (PackageKit::Daemon::networkState()
            > PackageKit::Daemon::Network::NetworkOffline);
}

void CosuPackageKit::delayAction()
{
    connect(PackageKit::Daemon::global(),
            &PackageKit::Daemon::networkStateChanged,
            this,
            &CosuPackageKit::delayedAction);
    qCInfo(COSU_PACKAGEKIT)
        << "Checking" << m_currentAction << "skipped. Network is offline"
        << "waiting till online next time";
}

void CosuPackageKit::delayedAction()
{
    if (!isNetworkOnline())
        return;

    disconnect(PackageKit::Daemon::global(),
               &PackageKit::Daemon::networkStateChanged,
               this,
               &CosuPackageKit::delayedAction);
    switch (m_currentAction) {
    case Action::_refreshingCache:
        this->refreshCache(m_isRefreshForced);
        break;
    case Action::_resolvingPackage:
        this->resolvePackage(m_currentPackage);
    }
}

void CosuPackageKit::refreshCache(bool force)
{
    m_currentAction = Action::_refreshingCache;
    m_isRefreshForced = force;
    qCInfo(COSU_PACKAGEKIT) << "Checking for updates";

    if (!isNetworkOnline()) {
        this->delayAction();
        return;
    }
    if (force)
        qCInfo(COSU_PACKAGEKIT) << "Checking updates, forced" << force;

    m_cacheTrans = PackageKit::Daemon::refreshCache(force);

    // evaluate the result
    connect(m_cacheTrans.data(),
            &PackageKit::Transaction::statusChanged,
            this,
            &CosuPackageKit::onStatusChanged);
    connect(m_cacheTrans.data(),
            &PackageKit::Transaction::finished,
            this,
            &CosuPackageKit::onFinished);
    connect(m_cacheTrans.data(),
            &PackageKit::Transaction::errorCode,
            this,
            &CosuPackageKit::onErrorCode);
}

void CosuPackageKit::resolvePackage(const QString &packageName)
{
    m_currentAction = Action::_resolvingPackage;
    m_currentPackage = packageName;

    if (!isNetworkOnline()) {
        this->delayAction();
        return;
    }

    QFlags<PackageKit::Transaction::Filter> filter =
        PackageKit::Transaction::FilterBasename;

    m_resolveTrans = PackageKit::Daemon::resolve(packageName, filter);

    connect(m_resolveTrans,
            &PackageKit::Transaction::package,
            this,
            &CosuPackageKit::onResolvedPackage);
    connect(m_resolveTrans.data(),
            &PackageKit::Transaction::statusChanged,
            this,
            &CosuPackageKit::onStatusChanged);
    connect(m_resolveTrans.data(),
            &PackageKit::Transaction::finished,
            this,
            &CosuPackageKit::onFinished);
    connect(m_resolveTrans.data(),
            &PackageKit::Transaction::errorCode,
            this,
            &CosuPackageKit::onErrorCode);
}

void CosuPackageKit::onResolvedPackage(PackageKit::Transaction::Info info,
                                       const QString &packageID,
                                       const QString &summary)
{
    CosuPackage package;
    qCDebug(COSU_PACKAGEKIT) << "info: " << info << "summary: " << summary;
    package.version = PackageKit::Transaction::packageVersion(packageID);
    package.name = PackageKit::Transaction::packageName(packageID);

    emit this->resolvedPackage(package);
}

void CosuPackageKit::onStatusChanged()
{
    PackageKit::Transaction *trans;
    if ((trans = qobject_cast<PackageKit::Transaction *>(sender()))) {
        qCDebug(COSU_PACKAGEKIT)
            << "Transaction status changed:"
            << PackageKit::Daemon::enumToString<PackageKit::Transaction>(
                   (int)trans->status(),
                   "Status")
            << QStringLiteral("(%1%)").arg(trans->percentage());
        if (trans->status() == PackageKit::Transaction::StatusFinished)
            return;
    }
}

void CosuPackageKit::onFinished(PackageKit::Transaction::Exit status,
                                uint runtime)
{
    Q_UNUSED(runtime);

    PackageKit::Transaction *trans =
        qobject_cast<PackageKit::Transaction *>(sender());
    if (!trans)
        return;

    trans->deleteLater();

    qCDebug(COSU_PACKAGEKIT)
        << "Transaction" << trans->tid().path() << "finished with status"
        << PackageKit::Daemon::enumToString<PackageKit::Transaction>(
               (int)status,
               "Exit")
        << "in" << runtime / 1000 << "seconds";

    if (trans->role() == PackageKit::Transaction::RoleRefreshCache) {
        if (status == PackageKit::Transaction::ExitSuccess) {
            qCInfo(COSU_PACKAGEKIT)
                << "Cache transaction finished successfully";

            emit this->refreshFinished();
            return;
        } else {
            emit this->resolverError(
                QStringLiteral("Cache transaction didn't finish successfully"));
            return;
        }
    } else if (trans->role() == PackageKit::Transaction::RoleResolve) {
        if (status == PackageKit::Transaction::ExitSuccess) {
            qCDebug(COSU_PACKAGEKIT)
                << "Resolve transaction finished successfully";
            return;
        }
    }

    emit resolverError(
        QStringLiteral("Unhandled transaction type: %s")
            .arg(PackageKit::Daemon::enumToString<PackageKit::Transaction>(
                trans->role(),
                "Role")));
    return;
}

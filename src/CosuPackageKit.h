/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include "CosuResolveAbstract.h"
#include <QObject>
#include <QPointer>

#include <PackageKit/Daemon>
#include <PackageKit/Transaction>

class CosuPackageKit : public CosuResolveAbstract
{
    Q_OBJECT

public:
    CosuPackageKit(QObject *parent = nullptr);
    ~CosuPackageKit() override;

    void resolvePackage(const QString &packageName) override;
    void refreshCache(bool force = true) override;

signals:
    /*
      0. Refresh
      1. Finished
      2. onFinished
      3. onRefreshFinished
      4. findOsUpdatePackage
      5. Transaction search for old pkg
      6. Finished
      7. onOsUpdatePackageFound
      8. set current os version
      10. setActivity(CheckNewOsVersion)
      11. findosUpdatePackage()
      12. Transaction search for new pkg
      13. Finished
      14. osUpdatePackageFound
      15. compare versions
      16. if newer emit osUpdateReceived
      17. onOsUpdateReceived
      18. cleanup?
     */
    void osUpdatePackageFound();

    // Internal required
    void isActiveChanged();
    void networkStateChanged();
    // end

private slots:
    void onChanged();
    void onStatusChanged();
    void onFinished(PackageKit::Transaction::Exit status, uint runtime);
    void onErrorCode(PackageKit::Transaction::Error error,
                     const QString &details);

    void onResolvedPackage(PackageKit::Transaction::Info info,
                           const QString &packageID,
                           const QString &summary);
    void delayedAction();

private:
    void delayAction();

    enum Action { _refreshingCache, _resolvingPackage };
    // Q_ENUM(Action)

    bool isNetworkOnline() const;
    Action m_currentAction;
    QString m_currentPackage;
    bool m_isRefreshForced;

    QPointer<PackageKit::Transaction> m_cacheTrans;
    QPointer<PackageKit::Transaction> m_resolveTrans;
};

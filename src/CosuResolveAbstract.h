/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include <QObject>

typedef struct CosuPackage {
    QString name;
    QString version;
} CosuPackage;

class CosuResolveAbstract : public QObject
{
    Q_OBJECT

public:
    explicit CosuResolveAbstract(QObject *parent = nullptr);

    virtual void resolvePackage(const QString &packageName) = 0;
    virtual void refreshCache(bool force = true) = 0;

    enum Activity { Idle, refreshingCache, resolving };
    Q_ENUM(Activity)

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
    // Internal required
    void refreshFinished();
    void activityChanged(Activity activity);
    void resolvedPackage(CosuPackage &package);
    // end
    void resolverError(const QString &errorMessage);
};

/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#pragma once

#include <QObject>

class CosuOsUpdateServiceProxy : public QObject
{
    Q_OBJECT

public:
    CosuOsUpdateServiceProxy(QObject *parent = nullptr);
    static void triggerService();
private slots:
    void onOsUpdateStatusChanged(int status);

private:
    enum ServiceStatus { Checking, Updated, UpdateAvialable };

    ServiceStatus previousStatus;
};

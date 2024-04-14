/*  -*- c++ -*-
 * SPDX-License-Identifier: GPL-2.0
 *
 * SPDX-FileCopyrightText: 2024 Björn Bidar
 */

#include "../src/CosuVariables.h"

#include <QDebug>
#include <QObject>
#include <QTest>

class testCosuVariables : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void checkResolveString();

private:
    CosuVariables var;
    QHash<QString, QVariant> variables, urls;
};

void testCosuVariables::initTestCase()
{
    variables.insert("packagesDomain", "packages.example.com");
    variables.insert("releaseDomain", "releases.example.com");
    variables.insert("rndProtocol", "https");
    variables.insert("release", "devel");
    variables.insert("arch", "armv8");
    variables.insert("flavourName", "flavour");

    urls.insert("http://%(packagesDomain)/releases/%(release)/jolla/%(arch)/",
                "http://packages.example.com/releases/devel/jolla/armv8/");
    urls.insert(
        "%(rndProtocol)://%(releaseDomain)/nemo/%(release)-%(flavourName)/"
        "platform/%(arch)/",
        "https://releases.example.com/nemo/devel-flavour/platform/armv8/");
    // test missing operator, which should fall back to just variable value
    urls.insert(
        "%(rndProtocol)://%(unsetDomain:)/nemo/%(release)-%(flavourName)/"
        "platform/%(arch)/",
        "https:///nemo/devel-flavour/platform/armv8/");
    urls.insert(
        "%(rndProtocol)://%(releaseDomain:)/nemo/%(release)-%(flavourName)/"
        "platform/%(arch)/",
        "https://releases.example.com/nemo/devel-flavour/platform/armv8/");
    urls.insert(
        "%(rndProtocol)://%(releaseDomain:unset.example.com)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https://releases.example.com/nemo/devel-flavour/platform/armv8/");
    // check if :- works
    urls.insert(
        "%(rndProtocol)://%(releaseDomain:-unset.example.com)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https://releases.example.com/nemo/devel-flavour/platform/armv8/");
    urls.insert(
        "%(rndProtocol)://%(unsetDomain:-unset.example.com)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https://unset.example.com/nemo/devel-flavour/platform/armv8/");
    // test with empty replacement pattern
    urls.insert(
        "%(rndProtocol)://%(unsetDomain:-)/nemo/%(release)-%(flavourName)/"
        "platform/%(arch)/",
        "https:///nemo/devel-flavour/platform/armv8/");
    // check if :+ works
    // substitution of variable with set.example.com
    urls.insert(
        "%(rndProtocol)://%(releaseDomain:+set.example.com)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https://set.example.com/nemo/devel-flavour/platform/armv8/");
    // substitution of variable with variable + /set
    urls.insert(
        "%(rndProtocol)://%(releaseDomain:+%(releaseDomain)/set)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https://releases.example.com/set/nemo/devel-flavour/platform/armv8/");
    // substitution of variable with empty variable + /set -- should substitute
    // to ""
    urls.insert(
        "%(rndProtocol)://%(unsetDomain:+%(unsetDomain)/set)/nemo/"
        "%(release)-%(flavourName)/platform/%(arch)/",
        "https:///nemo/devel-flavour/platform/armv8/");
    // test :=
    urls.insert(
        "%(%(rndProtocol):=https?https://%(releaseDomain)/"
        "%(release)-%(flavourName)|http://%(releaseDomain)/"
        "%(release)-%(flavourName))",
        "https://releases.example.com/devel-flavour");
    urls.insert(
        "%(%(rndProtocol):=http?https://%(releaseDomain)/"
        "%(release)-%(flavourName)|http://%(releaseDomain)/"
        "%(release)-%(flavourName))",
        "http://releases.example.com/devel-flavour");
}

void testCosuVariables::checkResolveString()
{
    QHashIterator<QString, QVariant> i(urls);

    while (i.hasNext()) {
        i.next();
        QString result = var.resolveString(i.key(), &variables);
        qDebug() << i.key() << " resolved to " << result;
        QCOMPARE(result, i.value().toString());
    }
}

QTEST_MAIN(testCosuVariables)
#include "testCosuVariables.moc"

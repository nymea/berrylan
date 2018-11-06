/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2018 Simon Stuerz <simon.stuerz@guh.io>                  *
 *                                                                         *
 *  This file is part of nymea:app                                         *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef WIRELESSACCESSPOINT_H
#define WIRELESSACCESSPOINT_H

#include <QObject>
#include <QString>

class WirelessAccessPoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ssid READ ssid NOTIFY ssidChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString hostAddress READ hostAddress NOTIFY hostAddressChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(bool isProtected READ isProtected NOTIFY isProtectedChanged)

public:
    WirelessAccessPoint(QObject *parent = nullptr);

    QString ssid() const;
    void setSsid(const QString ssid);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QString hostAddress() const;
    void setHostAddress(const QString &hostAddress);

    int signalStrength() const;
    void setSignalStrength(const int &signalStrength);

    bool isProtected() const;
    void setProtected(const bool &isProtected);

private:
    QString m_ssid;
    QString m_macAddress;
    QString m_hostAddress;
    int m_signalStrength = 0;
    bool m_isProtected = false;

signals:
    void ssidChanged(const QString &ssid);
    void macAddressChanged(const QString &macAddress);
    void hostAddressChanged(const QString &hostAddress);
    void signalStrengthChanged(int signalStrength);
    void isProtectedChanged(bool isProtected);
};

#endif // WIRELESSACCESSPOINT_H

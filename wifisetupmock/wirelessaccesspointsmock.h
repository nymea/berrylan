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

#ifndef WIRELESSACCESSPOINTSMOCK_H
#define WIRELESSACCESSPOINTSMOCK_H

#include <QObject>
#include <QAbstractListModel>

class WirelessAccessPointMock;

class WirelessAccessPointsMock : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum BluetoothDeviceInfoRole {
        WirelessAccesspointRoleSsid = Qt::DisplayRole,
        WirelessAccesspointRoleMacAddress,
        WirelessAccesspointRoleHostAddress,
        WirelessAccesspointRoleSignalStrength,
        WirelessAccesspointRoleProtected
    };

    explicit WirelessAccessPointsMock(QObject *parent = nullptr);

    QList<WirelessAccessPointMock *> wirelessAccessPoints();
    void setWirelessAccessPoints(QList<WirelessAccessPointMock *> wirelessAccessPoints);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE WirelessAccessPointMock *getAccessPoint(const QString &ssid) const;
    Q_INVOKABLE WirelessAccessPointMock *get(int index);

    void clearModel();

    void addWirelessAccessPoint(WirelessAccessPointMock *accessPoint);
    void removeWirelessAccessPoint(WirelessAccessPointMock *accessPoint);

signals:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<WirelessAccessPointMock *> m_wirelessAccessPoints;


};

#endif // WIRELESSACCESSPOINTS_H

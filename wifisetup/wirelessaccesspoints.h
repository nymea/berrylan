/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef WIRELESSACCESSPOINTS_H
#define WIRELESSACCESSPOINTS_H

#include <QObject>
#include <QAbstractListModel>

class WirelessAccessPoint;

class WirelessAccessPoints : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum BluetoothDeviceInfoRole {
        WirelessAccesspointRoleSsid = Qt::DisplayRole,
        WirelessAccesspointRoleMacAddress,
        WirelessAccesspointRoleHostAddress,
        WirelessAccesspointRoleSignalStrength,
        WirelessAccesspointRoleProtected,
        WirelessAccessPointRoleFrequency
    };

    explicit WirelessAccessPoints(QObject *parent = nullptr);

    QList<WirelessAccessPoint *> wirelessAccessPoints();
    void setWirelessAccessPoints(QList<WirelessAccessPoint *> wirelessAccessPoints);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int count() const;
    Q_INVOKABLE WirelessAccessPoint *getAccessPoint(const QString &ssid) const;
    Q_INVOKABLE WirelessAccessPoint *get(int index);

    void clearModel();

    void addWirelessAccessPoint(WirelessAccessPoint *accessPoint);
    void removeWirelessAccessPoint(WirelessAccessPoint *accessPoint);

signals:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<WirelessAccessPoint *> m_wirelessAccessPoints;


};

#endif // WIRELESSACCESSPOINTS_H

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

#ifndef BLUETOOTHDEVICEINFO_H
#define BLUETOOTHDEVICEINFO_H

#include <QList>
#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>

class BluetoothDeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY deviceChanged)
    Q_PROPERTY(QString address READ address NOTIFY deviceChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY deviceChanged)

public:
    BluetoothDeviceInfo();
    BluetoothDeviceInfo(const QBluetoothDeviceInfo &deviceInfo);
    ~BluetoothDeviceInfo();

    QString address() const;
    QString name() const;
    bool isLowEnergy() const;
    int signalStrength() const;

    QBluetoothDeviceInfo bluetoothDeviceInfo() const;
    void setBluetoothDeviceInfo(const QBluetoothDeviceInfo &deviceInfo);

signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo m_deviceInfo;

};

#endif // BLUETOOTHDEVICEINFO_H

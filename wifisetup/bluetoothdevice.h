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

#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include <QObject>
#include <QBluetoothUuid>
#include <QLowEnergyService>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyCharacteristic>

class BluetoothDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    explicit BluetoothDevice(const QBluetoothDeviceInfo &deviceInfo, QObject *parent = nullptr);

    QString name() const;
    QBluetoothAddress address() const;
    QString addressString() const;
    bool connected() const;
    QString statusText() const;

    Q_INVOKABLE void connectDevice();
    Q_INVOKABLE void disconnectDevice();

private:
    QBluetoothDeviceInfo m_deviceInfo;
    QLowEnergyController *m_controller;
    bool m_connected;
    QString m_statusText;

    void setConnected(const bool &connected);

protected:
    QLowEnergyController *controller();
    void setStatusText(const QString &statusText);

signals:
    void connectedChanged();
    void serviceDiscoveryFinished();

    void statusTextChanged();

private slots:
    void onConnected();
    void onDisconnected();

    void onDeviceError(const QLowEnergyController::Error &error);
    void onDeviceStateChanged(const QLowEnergyController::ControllerState &state);

};

#endif // BLUETOOTHDEVICE_H

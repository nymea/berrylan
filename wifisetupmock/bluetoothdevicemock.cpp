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

#include "bluetoothdevicemock.h"
#include <QTimer>

BluetoothDeviceMock::BluetoothDeviceMock(const QString &name, const QString &address, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_address(address),
    m_connected(false)
{

}

QString BluetoothDeviceMock::name() const
{
    return m_name;
}

QBluetoothAddress BluetoothDeviceMock::address() const
{
    return QBluetoothAddress(m_address);
}

QString BluetoothDeviceMock::addressString() const
{
    return m_address;
}

bool BluetoothDeviceMock::connected() const
{
    return m_connected;
}

QString BluetoothDeviceMock::statusText() const
{
    return m_statusText;
}

void BluetoothDeviceMock::connectDevice()
{
    qDebug() << "connecting 1";
    setStatusText(QString(tr("Connecting to %1...").arg(name())));

    QTimer::singleShot(1000, this, [this](){
        qDebug() << "connecting 2";
        setStatusText(QString(tr("Connected to %1").arg(name())));
        QTimer::singleShot(1000, this, [this](){
            qDebug() << "connecting 3";
            setStatusText(QString(tr("Discovering services of %1...").arg(name())));
            QTimer::singleShot(1000, this, [this](){
                qDebug() << "connecting 4";
                setStatusText(QString(tr("Discovering services of %1...").arg(name())));
                QTimer::singleShot(1000, this, [this](){
                    qDebug() << "connecting 5";
                    setStatusText(QString(tr("%1 connected and discovered.").arg(name())));
                    setConnected(true);
                });
            });
        });
    });
}

void BluetoothDeviceMock::disconnectDevice()
{
    setStatusText(QString(tr("Disconnecting from %1...").arg(name())));
    QTimer::singleShot(1000, this, [this](){
        setStatusText(QString(tr("%1 disconnected.").arg(name())));
        setConnected(false);
    });
}

void BluetoothDeviceMock::setConnected(const bool &connected)
{
    m_connected = connected;
    emit connectedChanged();
}

void BluetoothDeviceMock::setStatusText(const QString &statusText)
{
    m_statusText = statusText;
    emit statusTextChanged();
}

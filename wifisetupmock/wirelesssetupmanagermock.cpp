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

#include "wirelesssetupmanagermock.h"
#include "wirelessaccesspointmock.h"
#include "wirelessaccesspointsmock.h"
#include "wirelessaccesspointsproxymock.h"

#include <QJsonDocument>
#include <QTimer>

WirelessSetupManagerMock::WirelessSetupManagerMock(QObject *parent) :
    BluetoothDeviceMock("Localhost", "12:34:56:78:90:ab", parent),
    m_accessPoints(new WirelessAccessPointsMock(this))
{
    m_accessPointModeAvailable = true;

    connect(this, &BluetoothDeviceMock::connectedChanged, this, [this](){
        qDebug() << "Connected!!!!";

        WirelessAccessPointMock *ap = new WirelessAccessPointMock(this);
        ap->setSsid("My WiFi");
        ap->setProtected(true);
        m_accessPoints->addWirelessAccessPoint(ap);

        ap = new WirelessAccessPointMock(this);
        ap->setSsid("Open WiFi");
        ap->setProtected(false);
        m_accessPoints->addWirelessAccessPoint(ap);

        m_initialized = true;
        emit initializedChanged();

    });
}

QString WirelessSetupManagerMock::modelNumber() const
{
    return m_modelNumber;
}

QString WirelessSetupManagerMock::manufacturer() const
{
    return m_manufacturer;
}

QString WirelessSetupManagerMock::softwareRevision() const
{
    return m_softwareRevision;
}

QString WirelessSetupManagerMock::firmwareRevision() const
{
    return m_firmwareRevision;
}

QString WirelessSetupManagerMock::hardwareRevision() const
{
    return m_hardwareRevision;
}

bool WirelessSetupManagerMock::initialized() const
{
    return m_initialized;
}

bool WirelessSetupManagerMock::working() const
{
    return m_working;
}

bool WirelessSetupManagerMock::accessPointModeAvailable() const
{
    return m_accessPointModeAvailable;
}

WirelessSetupManagerMock::NetworkStatus WirelessSetupManagerMock::networkStatus() const
{
    return m_networkStatus;
}

WirelessSetupManagerMock::WirelessStatus WirelessSetupManagerMock::wirelessStatus() const
{
    return m_wirelessStatus;
}

WirelessSetupManagerMock::WirelessDeviceMode WirelessSetupManagerMock::wirelessDeviceMode() const
{
    return m_wirelessDeviceMode;
}

bool WirelessSetupManagerMock::networkingEnabled() const
{
    return m_networkingEnabled;
}

bool WirelessSetupManagerMock::wirelessEnabled() const
{
    return m_wirelessEnabled;
}

WirelessAccessPointsMock *WirelessSetupManagerMock::accessPoints() const
{
    return m_accessPoints;
}

WirelessAccessPointMock *WirelessSetupManagerMock::currentConnection() const
{
    return m_currentConnection;
}

void WirelessSetupManagerMock::reloadData()
{
    loadNetworks();
}

void WirelessSetupManagerMock::loadNetworks()
{
}

void WirelessSetupManagerMock::loadCurrentConnection()
{
}

void WirelessSetupManagerMock::performWifiScan()
{
}

void WirelessSetupManagerMock::enableNetworking(bool enable)
{
}

void WirelessSetupManagerMock::enableWireless(bool enable)
{
}

void WirelessSetupManagerMock::connectWirelessNetwork(const QString &ssid, const QString &password)
{
    m_currentConnection = m_accessPoints->get(0);
    m_currentConnection->setHostAddress("192.168.0.42");
    m_wirelessDeviceMode = WirelessDeviceModeInfrastructure;
    emit wirelessDeviceModeChanged();
    emit currentConnectionChanged();

    QTimer::singleShot(1000, this, [this](){
        m_wirelessStatus = WirelessStatusActivated;
        emit wirelessStatusChanged();
    });
}

void WirelessSetupManagerMock::startAccessPoint(const QString &ssid, const QString &password)
{
    m_currentConnection = m_accessPoints->get(0);
    m_currentConnection->setHostAddress("192.168.0.42");
    m_wirelessDeviceMode = WirelessDeviceModeAccessPoint;
    emit wirelessDeviceModeChanged();
    emit currentConnectionChanged();

    QTimer::singleShot(1000, this, [this](){
        m_wirelessStatus = WirelessStatusActivated;
        emit wirelessStatusChanged();
    });

}

void WirelessSetupManagerMock::disconnectWirelessNetwork()
{
}

void WirelessSetupManagerMock::pressPushButton()
{
}

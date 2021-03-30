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

#include "wirelessaccesspoint.h"

#include <QDebug>

WirelessAccessPoint::WirelessAccessPoint(QObject *parent):
    QObject(parent)
{

}

QString WirelessAccessPoint::ssid() const
{
    return m_ssid;
}

void WirelessAccessPoint::setSsid(const QString ssid)
{
    if (m_ssid == ssid)
        return;

    m_ssid = ssid;
    emit ssidChanged(m_ssid);
}

QString WirelessAccessPoint::macAddress() const
{
    return m_macAddress;
}

void WirelessAccessPoint::setMacAddress(const QString &macAddress)
{
    if (m_macAddress == macAddress)
        return;

    m_macAddress = macAddress;
    emit macAddressChanged(m_macAddress);
}

QString WirelessAccessPoint::hostAddress() const
{
    return m_hostAddress;
}

void WirelessAccessPoint::setHostAddress(const QString &hostAddress)
{
    if (m_hostAddress == hostAddress)
        return;

    m_hostAddress = hostAddress;
    emit hostAddressChanged(m_hostAddress);
}

int WirelessAccessPoint::signalStrength() const
{
    return m_signalStrength;
}

void WirelessAccessPoint::setSignalStrength(int signalStrength)
{
    if (m_signalStrength == signalStrength)
        return;

    m_signalStrength = signalStrength;
    emit signalStrengthChanged(m_signalStrength);
}

bool WirelessAccessPoint::isProtected() const
{
    return m_isProtected;
}

void WirelessAccessPoint::setProtected(bool isProtected)
{
    if (m_isProtected == isProtected)
        return;

    m_isProtected = isProtected;
    emit isProtectedChanged(m_isProtected);

}

double WirelessAccessPoint::frequency() const
{
    return m_frequency;
}

void WirelessAccessPoint::setFrequency(double frequency)
{
    if (!qFuzzyCompare(m_frequency,frequency)) {
        m_frequency = frequency;
        emit frequencyChanged();
    }
}

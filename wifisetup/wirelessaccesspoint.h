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
    Q_PROPERTY(double frequency READ frequency NOTIFY frequencyChanged)

public:
    WirelessAccessPoint(QObject *parent = nullptr);

    QString ssid() const;
    void setSsid(const QString ssid);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QString hostAddress() const;
    void setHostAddress(const QString &hostAddress);

    int signalStrength() const;
    void setSignalStrength(int signalStrength);

    bool isProtected() const;
    void setProtected(bool isProtected);

    double frequency() const;
    void setFrequency(double frequency);

signals:
    void ssidChanged(const QString &ssid);
    void macAddressChanged(const QString &macAddress);
    void hostAddressChanged(const QString &hostAddress);
    void signalStrengthChanged(int signalStrength);
    void isProtectedChanged(bool isProtected);
    void frequencyChanged();

private:
    QString m_ssid;
    QString m_macAddress;
    QString m_hostAddress;
    int m_signalStrength = 0;
    bool m_isProtected = false;
    double m_frequency = 0;
};

#endif // WIRELESSACCESSPOINT_H

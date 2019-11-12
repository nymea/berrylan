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

#ifndef WIRELESSACCESSPOINTSPROXYMOCK_H
#define WIRELESSACCESSPOINTSPROXYMOCK_H

#include <QObject>
#include <QSortFilterProxyModel>

class WirelessAccessPointMock;
class WirelessAccessPointsMock;

class WirelessAccessPointsProxyMock : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(WirelessAccessPointsMock* accessPoints READ accessPoints WRITE setAccessPoints)
public:
    explicit WirelessAccessPointsProxyMock(QObject *parent = nullptr);

    WirelessAccessPointsMock *accessPoints() const;
    void setAccessPoints(WirelessAccessPointsMock *accessPoints);

    Q_INVOKABLE WirelessAccessPointMock *get(int index) const;

signals:
    void countChanged();
    void accessPointsChanged();

private:
    WirelessAccessPointsMock *m_accessPoints = nullptr;

};

#endif // WIRELESSACCESSPOINTSPROXY_H

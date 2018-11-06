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

#ifndef WIRELESSACCESSPOINTSPROXY_H
#define WIRELESSACCESSPOINTSPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class WirelessAccessPoint;
class WirelessAccessPoints;

class WirelessAccessPointsProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(WirelessAccessPoints* accessPoints READ accessPoints WRITE setAccessPoints)
public:
    explicit WirelessAccessPointsProxy(QObject *parent = nullptr);

    WirelessAccessPoints *accessPoints() const;
    void setAccessPoints(WirelessAccessPoints *accessPoints);

    Q_INVOKABLE WirelessAccessPoint* get(int index) const;

signals:
    void countChanged();
    void accessPointsChanged();

private:
    WirelessAccessPoints *m_accessPoints = nullptr;

};

#endif // WIRELESSACCESSPOINTSPROXY_H

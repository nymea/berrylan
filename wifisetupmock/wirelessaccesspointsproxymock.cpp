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

#include "wirelessaccesspointsproxymock.h"

#include "wirelessaccesspointmock.h"
#include "wirelessaccesspointsmock.h"

#include <QDebug>

WirelessAccessPointsProxyMock::WirelessAccessPointsProxyMock(QObject *parent) : QSortFilterProxyModel(parent)
{

}

WirelessAccessPointsMock *WirelessAccessPointsProxyMock::accessPoints() const
{
    return m_accessPoints;
}

void WirelessAccessPointsProxyMock::setAccessPoints(WirelessAccessPointsMock *accessPoints)
{
    m_accessPoints = accessPoints;
    emit accessPointsChanged();
    setSourceModel(m_accessPoints);
    connect(accessPoints, &WirelessAccessPointsMock::countChanged, this, &WirelessAccessPointsProxyMock::countChanged);
    setSortRole(WirelessAccessPointsMock::WirelessAccesspointRoleSignalStrength);
    sort(0, Qt::DescendingOrder);
}

WirelessAccessPointMock *WirelessAccessPointsProxyMock::get(int index) const
{
    return m_accessPoints->get(mapToSource(this->index(index, 0)).row());
}

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

#include "bluetoothdeviceinfomock.h"

#include <QBluetoothUuid>

BluetoothDeviceInfoMock::BluetoothDeviceInfoMock()
{
}

BluetoothDeviceInfoMock::BluetoothDeviceInfoMock(const QString &address, const QString &name, bool isLowEnergy)
{
    m_address = address;
    m_name = name;
    m_isLowEnergy = isLowEnergy;
}

QString BluetoothDeviceInfoMock::address() const
{
    return m_address;
}

QString BluetoothDeviceInfoMock::name() const
{
    return m_name;
}

bool BluetoothDeviceInfoMock::isLowEnergy() const
{
    return m_isLowEnergy;
}

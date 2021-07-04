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

#ifndef BLUETOOTHDEVICEINFOS_H
#define BLUETOOTHDEVICEINFOS_H

#include <QObject>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QUuid>

#include "bluetoothdeviceinfo.h"

class BluetoothDeviceInfos : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum BluetoothDeviceInfoRole {
        BluetoothDeviceInfoRoleName = Qt::DisplayRole,
        BluetoothDeviceInfoRoleAddress,
        BluetoothDeviceInfoRoleLe,
        BluetoothDeviceInfoRoleSignalStrength
    };
    Q_ENUM(BluetoothDeviceInfoRole)

    explicit BluetoothDeviceInfos(QObject *parent = nullptr);

    QList<BluetoothDeviceInfo *> deviceInfos();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int count() const;
    Q_INVOKABLE BluetoothDeviceInfo *get(int index) const;

    void addBluetoothDeviceInfo(BluetoothDeviceInfo *deviceInfo);
    Q_INVOKABLE void clearModel();

signals:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<BluetoothDeviceInfo *> m_deviceInfos;
};

class BluetoothDeviceInfosProxy: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(BluetoothDeviceInfos* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    // Workaround for allowing particular names, even if they'd fail the filters below (used for legacy compatibility)
    Q_PROPERTY(QStringList nameWhitelist READ nameWhitelist WRITE setNameWhitelist NOTIFY nameWhitelistChanged)

    Q_PROPERTY(bool filterForLowEnergy READ filterForLowEnergy WRITE setFilterForLowEnergy NOTIFY filterForLowEnergyChanged)
    Q_PROPERTY(QString filterForServiceUUID READ filterForServiceUUID WRITE setFilterForServiceUUID NOTIFY filterForServiceUUIDChanged)
    Q_PROPERTY(QString filterForName READ filterForName WRITE setFilterForName NOTIFY filterForNameChanged)
public:
    BluetoothDeviceInfosProxy(QObject *parent = nullptr);

    BluetoothDeviceInfos* model() const;
    void setModel(BluetoothDeviceInfos *model);

    QStringList nameWhitelist() const;
    void setNameWhitelist(const QStringList &nameWhitelist);

    bool filterForLowEnergy() const;
    void setFilterForLowEnergy(bool filterForLowEnergy);

    QString filterForServiceUUID() const;
    void setFilterForServiceUUID(const QString &filterForServiceUUID);

    QString filterForName() const;
    void setFilterForName(const QString &name);

    Q_INVOKABLE BluetoothDeviceInfo* get(int index) const;

signals:
    void modelChanged();
    void countChanged();
    void nameWhitelistChanged();
    void filterForLowEnergyChanged();
    void filterForServiceUUIDChanged();
    void filterForNameChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    BluetoothDeviceInfos *m_model = nullptr;
    QStringList m_nameWhitelist;
    bool m_filterForLowEnergy = false;
    QUuid m_filterForServiceUUID;
    QString m_filterForName;
};

#endif // BLUETOOTHDEVICEINFOS_H

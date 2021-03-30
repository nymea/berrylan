#ifndef BTWIFISETUP_H
#define BTWIFISETUP_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

class BluetoothDeviceInfo;
class WirelessAccessPoints;
class WirelessAccessPoint;

class BtWiFiSetup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BluetoothStatus bluetoothStatus READ bluetoothStatus NOTIFY bluetoothStatusChanged)

    Q_PROPERTY(QString modelNumber READ modelNumber NOTIFY modelNumberChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString softwareRevision READ softwareRevision NOTIFY softwareRevisionChanged)
    Q_PROPERTY(QString firmwareRevision READ firmwareRevision NOTIFY firmwareRevisionChanged)
    Q_PROPERTY(QString hardwareRevision READ hardwareRevision NOTIFY hardwareRevisionChanged)

    Q_PROPERTY(NetworkStatus networkStatus READ networkStatus NOTIFY networkStatusChanged)
    Q_PROPERTY(WirelessStatus wirelessStatus READ wirelessStatus NOTIFY wirelessStatusChanged)
    Q_PROPERTY(WirelessDeviceMode wirelessDeviceMode READ wirelessDeviceMode NOTIFY wirelessDeviceModeChanged)
    Q_PROPERTY(bool networkingEnabled READ networkingEnabled WRITE setNetworkingEnabled NOTIFY networkingEnabledChanged)
    Q_PROPERTY(bool wirelessEnabled READ wirelessEnabled WRITE setWirelessEnabled NOTIFY wirelessEnabledChanged)

    Q_PROPERTY(WirelessAccessPoints *accessPoints READ accessPoints CONSTANT)
    Q_PROPERTY(WirelessAccessPoint *currentConnection READ currentConnection NOTIFY currentConnectionChanged)

public:
    enum BluetoothStatus {
        BluetoothStatusDisconnected,
        BluetoothStatusConnectingToBluetooth,
        BluetoothStatusConnectedToBluetooth,
        BluetoothStatusLoaded,
    };
    Q_ENUM(BluetoothStatus)

    enum NetworkStatus {
        NetworkStatusUnknown = 0x00,
        NetworkStatusAsleep = 0x01,
        NetworkStatusDisconnected = 0x02,
        NetworkStatusDisconnecting = 0x03,
        NetworkStatusConnecting = 0x04,
        NetworkStatusLocal = 0x05,
        NetworkStatusConnectedSite = 0x06,
        NetworkStatusGlobal = 0x07
    };
    Q_ENUM(NetworkStatus)

    enum WirelessDeviceMode {
        WirelessDeviceModeUnknown           = 0x00,
        WirelessDeviceModeAdhoc             = 0x01,
        WirelessDeviceModeInfrastructure    = 0x02,
        WirelessDeviceModeAccessPoint       = 0x03
    };
    Q_ENUM(WirelessDeviceMode)

    enum WirelessStatus {
        WirelessStatusUnknown = 0x00,
        WirelessStatusUnmanaged = 0x01,
        WirelessStatusUnavailable = 0x02,
        WirelessStatusDisconnected = 0x03,
        WirelessStatusPrepare = 0x04,
        WirelessStatusConfig = 0x05,
        WirelessStatusNeedAuth = 0x06,
        WirelessStatusIpConfig = 0x07,
        WirelessStatusIpCheck = 0x08,
        WirelessStatusSecondaries = 0x09,
        WirelessStatusActivated = 0x0A,
        WirelessStatusDeactivating = 0x0B,
        WirelessStatusFailed = 0x0C
    };
    Q_ENUM(WirelessStatus)

    enum WirelessServiceCommand {
        WirelessServiceCommandInvalid = -1,
        WirelessServiceCommandGetNetworks           = 0x00,
        WirelessServiceCommandConnect               = 0x01,
        WirelessServiceCommandConnectHidden         = 0x02,
        WirelessServiceCommandDisconnect            = 0x03,
        WirelessServiceCommandScan                  = 0x04,
        WirelessServiceCommandGetCurrentConnection  = 0x05,
        WirelessServiceCommandStartAccessPoint      = 0x06
    };
    Q_ENUM(WirelessServiceCommand)

    enum WirelessServiceResponse {
        WirelessServiceResponseSuccess                     = 0x00,
        WirelessServiceResponseIvalidCommand               = 0x01,
        WirelessServiceResponseIvalidParameters            = 0x02,
        WirelessServiceResponseNetworkManagerNotAvailable  = 0x03,
        WirelessServiceResponseWirelessNotAvailable        = 0x04,
        WirelessServiceResponseWirelessNotEnabled          = 0x05,
        WirelessServiceResponseNetworkingNotEnabled        = 0x06,
        WirelessServiceResponseUnknownError                = 0x07
    };
    Q_ENUM(WirelessServiceResponse)

    enum NetworkServiceCommand {
        NetworkServiceCommandInvalid = -1,
        NetworkServiceCommandEnableNetworking   = 0x00,
        NetworkServiceCommandDisableNetworking  = 0x01,
        NetworkServiceCommandEnableWireless     = 0x02,
        NetworkServiceCommandDisableWireless    = 0x03,
    };
    Q_ENUM(NetworkServiceCommand)

    enum NetworkServiceResponse {
        NetworkServiceResponseSuccess                      = 0x00,
        NetworkServiceResponseIvalidValue                  = 0x01,
        NetworkServiceResponseNetworkManagerNotAvailable   = 0x02,
        NetworkServiceResponseWirelessNotAvailable         = 0x03,
        NetworkServiceResponseUnknownError                 = 0x04,
    };
    Q_ENUM(NetworkServiceResponse)

    enum SystemServiceCommand {
        SystemServiceCommandInvalid = -1,
        SystemServiceCommandPushAuthentication = 0x00
    };
    Q_ENUM(SystemServiceCommand)

    enum SystemServiceResponse {
        SystemServiceResponseSuccess                = 0x00,
        SystemServiceResponseUnknownError           = 0x01,
        SystemServiceResponseInvalidCommand         = 0x02,
        SystemServiceResponseInvalidValue           = 0x03,
        SystemServiceResponsePushServiceUnavailable = 0x04,
    };
    Q_ENUM(SystemServiceResponse)


    explicit BtWiFiSetup(QObject *parent = nullptr);
    ~BtWiFiSetup() override;

    Q_INVOKABLE void connectToDevice(const BluetoothDeviceInfo *device);
    Q_INVOKABLE void disconnectFromDevice();
    Q_INVOKABLE void connectDeviceToWiFi(const QString &ssid, const QString &password);
    Q_INVOKABLE void startAccessPoint(const QString &ssid, const QString &password);
    Q_INVOKABLE void disconnectDeviceFromWiFi();
    Q_INVOKABLE void scanWiFi();
    Q_INVOKABLE bool pressPushButton();

    BluetoothStatus bluetoothStatus() const;

    QString modelNumber() const;
    QString manufacturer() const;
    QString softwareRevision() const;
    QString firmwareRevision() const;
    QString hardwareRevision() const;

    NetworkStatus networkStatus() const;
    WirelessStatus wirelessStatus() const;
    WirelessDeviceMode wirelessDeviceMode() const;
    bool networkingEnabled() const;
    void setNetworkingEnabled(bool networkingEnabled);

    bool wirelessEnabled() const;
    void setWirelessEnabled(bool wirelessEnabled) const;

    WirelessAccessPoints *accessPoints() const;
    WirelessAccessPoint *currentConnection() const;

signals:
    void bluetoothStatusChanged(BluetoothStatus status);
    void bluetoothConnectionError();
    void wifiSetupError();

    void modelNumberChanged();
    void manufacturerChanged();
    void softwareRevisionChanged();
    void firmwareRevisionChanged();
    void hardwareRevisionChanged();

    void networkStatusChanged();
    void wirelessStatusChanged();
    void wirelessDeviceModeChanged();
    void networkingEnabledChanged();
    void wirelessEnabledChanged();

    void currentConnectionChanged();

private:
    void setupServices();
    void streamData(QLowEnergyService *service, const QUuid &characteristicUuid, const QVariantMap &request);
    void processWiFiPacket(const QVariantMap &data);

    void loadNetworks();
    void loadCurrentConnection();

private slots:
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

private:
    BluetoothStatus m_bluetoothStatus = BluetoothStatusDisconnected;
    QLowEnergyController *m_btController = nullptr;

    QLowEnergyService *m_deviceInformationService = nullptr;
    QLowEnergyService *m_networkService = nullptr;
    QLowEnergyService *m_wifiService = nullptr;
    QLowEnergyService *m_systemService = nullptr;

    QHash<QUuid, QByteArray> m_inputBuffers;

    QString m_modelNumber;
    QString m_manufacturer;
    QString m_softwareRevision;
    QString m_firmwareRevision;
    QString m_hardwareRevision;

    NetworkStatus m_networkStatus = NetworkStatusUnknown;
    WirelessStatus m_wirelessStatus = WirelessStatusUnknown;
    WirelessDeviceMode m_wirelessDeviceMode = WirelessDeviceModeUnknown;

    bool m_networkingEnabled = false;
    bool m_wirelessEnabled = false;

    WirelessAccessPoints *m_accessPoints = nullptr;
    WirelessAccessPoint *m_currentConnection = nullptr;

};

#endif // BTWIFISETUP_H

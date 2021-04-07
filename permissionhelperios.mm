#include "permissionhelperios.h"

#import <CoreBluetooth/CoreBluetooth.h>
#import <UIKit/UIKit.h>

@interface BluetoothManagerDelegate: NSObject<CBCentralManagerDelegate>
@end
@implementation BluetoothManagerDelegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)manager {
    qDebug() << "Bt cental manager state update!";
    emit PermissionHelper::instance()->bluetoothPermissionChanged();
    ((PermissionHelperIOS*)PermissionHelperIOS::instance())->m_bluetoothManager = nil;
}
@end

PermissionHelper::PermissionStatus PermissionHelperIOS::bluetoothPermission() const
{
    // iOS 13.0 would have an api but it's more complicated and also deprecated... Ignoring...
    if (@available(iOS 13.1, *)) {
        switch (CBCentralManager.authorization) {
        case CBManagerAuthorizationAllowedAlways:
        case CBManagerAuthorizationRestricted:
            return PermissionStatusGranted;
        case CBManagerAuthorizationDenied:
            return PermissionStatusDenied;
        case CBManagerAuthorizationNotDetermined:
            return PermissionStatusNotDetermined;
        }
    }
    // Before iOS 13, Bluetooth permissions are not required
    return PermissionStatusGranted;
}

void PermissionHelperIOS::openPermissionSettings()
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
}


void PermissionHelperIOS::requestPermission(PermissionHelper::Permission permission)
{
    if (permission == PermissionHelper::PermissionBluetooth) {
        // Instantiating a Bluetooth manager just trigger the popup...
        // This will leak, but would be only once...
        BluetoothManagerDelegate *delegate = [[BluetoothManagerDelegate alloc] init];
        m_bluetoothManager = [[CBCentralManager alloc] initWithDelegate:delegate queue:nil];
    }
}


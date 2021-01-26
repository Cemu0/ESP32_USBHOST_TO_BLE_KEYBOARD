#include "BleConnectionStatus.h"

BleConnectionStatus::BleConnectionStatus(void) {
}

void BleConnectionStatus::onConnect(BLEServer* pServer , esp_ble_gatts_cb_param_t *param)
{
  char remoteAddress[18];

  sprintf(
    remoteAddress,
    "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
    param->connect.remote_bda[0],
    param->connect.remote_bda[1],
    param->connect.remote_bda[2],
    param->connect.remote_bda[3],
    param->connect.remote_bda[4],
    param->connect.remote_bda[5]
  );

  ESP_LOGE(LOG_TAG, "myServerCallback onConnect, MAC: %s", remoteAddress);

  this->connected = true;
  BLE2902* desc = (BLE2902*)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);

  desc = (BLE2902*)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
}

void BleConnectionStatus::onDisconnect(BLEServer* pServer)
{
  this->connected = false;
  BLE2902* desc = (BLE2902*)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);

  desc = (BLE2902*)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);

}

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>
#include "esp_pm.h"

// === Pin Configuration ===
#define KEY_1   D1
#define KEY_2   D3
#define KEY_3   D5
#define KEY_4   D6
#define KEY_5   D7
#define KEY_6   D8
#define KEY_7   D9
#define LED_PIN LED_BUILTIN  // Onboard LED of Xiao ESP32-C6

BLEHIDDevice* hid;
BLECharacteristic* inputReport;
BLECharacteristic* outputReport;

// HID Report Descriptor for 7 buttons + X/Y axes
static const uint8_t reportDescriptor[] = {
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,        // USAGE (Gamepad)
  0xA1, 0x01,        // COLLECTION (Application)
  // Buttons
  0x05, 0x09,        // USAGE_PAGE (Button)
  0x19, 0x01,        // USAGE_MINIMUM (Button 1)
  0x29, 0x07,        // USAGE_MAXIMUM (Button 7)
  0x15, 0x00,        // LOGICAL_MINIMUM (0)
  0x25, 0x01,        // LOGICAL_MAXIMUM (1)
  0x75, 0x01,        // REPORT_SIZE (1)
  0x95, 0x07,        // REPORT_COUNT (7)
  0x81, 0x02,        // INPUT (Data,Var,Abs)
  0x75, 0x01,        // REPORT_SIZE (1) padding
  0x95, 0x01,        // REPORT_COUNT (1)
  0x81, 0x03,        // INPUT (Cnst,Var,Abs)
  // X/Y axes
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x30,        // USAGE (X)
  0x09, 0x31,        // USAGE (Y)
  0x15, 0x81,        // LOGICAL_MINIMUM (-127)
  0x25, 0x7F,        // LOGICAL_MAXIMUM (127)
  0x75, 0x08,        // REPORT_SIZE (8)
  0x95, 0x02,        // REPORT_COUNT (2)
  0x81, 0x02,        // INPUT (Data,Var,Abs)
  0xC0               // END_COLLECTION
};

// Previous report to detect changes
static uint8_t prevReport[3] = {0, 0, 0};

void setup() {
  Serial.begin(115200);

  // Configure button pins
  pinMode(KEY_1, INPUT_PULLUP);
  pinMode(KEY_2, INPUT_PULLUP);
  pinMode(KEY_3, INPUT_PULLUP);
  pinMode(KEY_4, INPUT_PULLUP);
  pinMode(KEY_5, INPUT_PULLUP);
  pinMode(KEY_6, INPUT_PULLUP);
  pinMode(KEY_7, INPUT_PULLUP);

  // Configure onboard LED
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);  // LED off initially (dim control)

  // ESP32-C6 CPU frequency scaling (no light sleep)
  esp_pm_config_esp32c6_t pm_config = {
      .max_freq_mhz = 160,
      .min_freq_mhz = 80,
      .light_sleep_enable = false
  };
  esp_pm_configure(&pm_config);

  // BLE initialization
  BLEDevice::init("Alynx");
  delay(150); // Small delay to ensure BLE stack is ready

  BLEServer *pServer = BLEDevice::createServer();
  hid = new BLEHIDDevice(pServer);
  inputReport = hid->inputReport(1);  // Report ID
  outputReport = hid->outputReport(1);

  hid->manufacturer()->setValue("Hashim");
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  hid->hidInfo(0x00, 0x01);

  hid->reportMap((uint8_t*)reportDescriptor, sizeof(reportDescriptor));
  hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(hid->hidService()->getUUID());
  pAdvertising->start();

  Serial.println("BLE Gamepad 'Alynx' started, waiting for connection...");
}

void loop() {
  // *** ESSENTIAL FIX: Check if connected before processing ***
  if (BLEDevice::getServer()->getConnectedCount() == 0) {
    delay(500);  // Longer delay when not connected to save power
    return;
  }

  uint8_t report[3] = {0, 0, 0}; // 1 byte buttons + 2 bytes X/Y

  // Read buttons
  if (digitalRead(KEY_1) == LOW) report[0] |= 0x01;
  if (digitalRead(KEY_2) == LOW) report[0] |= 0x02;
  if (digitalRead(KEY_3) == LOW) report[0] |= 0x04;
  if (digitalRead(KEY_4) == LOW) report[0] |= 0x08;
  if (digitalRead(KEY_5) == LOW) report[0] |= 0x10;
  if (digitalRead(KEY_6) == LOW) report[0] |= 0x20;
  if (digitalRead(KEY_7) == LOW) report[0] |= 0x40;

  // Onboard LED dimmed at very dim when any button pressed
  if (report[0] != 0) analogWrite(LED_PIN, 10);  // very dim
  else analogWrite(LED_PIN, 0);                 // LED off

  // Only send report if changed
  if (memcmp(report, prevReport, sizeof(report)) != 0) {
    inputReport->setValue(report, sizeof(report));
    inputReport->notify();
    memcpy(prevReport, report, sizeof(report));
  }

  delay(50);  // 20Hz polling, power efficient
}

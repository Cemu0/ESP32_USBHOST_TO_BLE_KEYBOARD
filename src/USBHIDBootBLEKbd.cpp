#include <hidboot.h>
#include <usbhub.h>
#include <EEPROM.h>
#include <esp_bt.h>
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp32-hal-cpu.h"
#include <SPI.h>
#include <BleKeyboard.h>
BleKeyboard bleKeyboard;

#define MAX3421_RESET digitalWrite(16, 1);digitalWrite(16, 0);delay(10);digitalWrite(16, 1);
int baterryLevel()
{
  return map((4095.0/analogRead(34))*((4.2*100)/(30 + 100))*100, 3.3*100, 4.2*100, 0, 100);
}

unsigned long last_trigger = 0;

void changeID(); 

class KbdRptParser : public KeyboardReportParser
{
  public:
    void changeledstage(uint8_t lockLeds){
      kbdLockingKeys.bLeds = lockLeds;
    }
     uint8_t ledstage(){
      uint8_t lockLeds = kbdLockingKeys.bLeds;
      return lockLeds; 
    }
    uint8_t changeNumled(bool stage){
      kbdLockingKeys.kbdLeds.bmNumLock = (stage ? 1 : 0);
      uint8_t lockLeds = kbdLockingKeys.bLeds;
      return lockLeds; 
    }
    uint8_t changeScrollled(bool stage){
      kbdLockingKeys.kbdLeds.bmScrollLock = (stage ? 1 : 0);
      uint8_t lockLeds = kbdLockingKeys.bLeds;
      return lockLeds;
    }
  private:
    void PrintKey(uint8_t mod, uint8_t key);
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown	(uint8_t mod, uint8_t key);
    void OnKeyUp	(uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  switch (key)
  {
  case 101:
    bleKeyboard.press(KEY_MEDIA_WWW_HOME);
    break;
  default:
    bleKeyboard.press(194-58+key);
  }
  uint8_t c = OemToAscii(mod, key);
  if (c)
    Serial.print((char)c);
}
void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;
//KEY_LEFT_CTRL
  if(afterMod.bmLeftCtrl)
      bleKeyboard.press(KEY_LEFT_CTRL);
  else
      bleKeyboard.release(KEY_LEFT_CTRL);
//KEY_LEFT_SHIFT
  if(afterMod.bmLeftShift)
      bleKeyboard.press(KEY_LEFT_SHIFT);
  else
      bleKeyboard.release(KEY_LEFT_SHIFT);
//KEY_LEFT_ALT
  if(afterMod.bmLeftAlt)
      bleKeyboard.press(KEY_LEFT_ALT);
  else
      bleKeyboard.release(KEY_LEFT_ALT);
//KEY_LEFT_GUI
    if(afterMod.bmLeftGUI)
        bleKeyboard.press(KEY_LEFT_GUI);
    else
        bleKeyboard.release(KEY_LEFT_GUI);
//KEY_RIGHT_CTRL
    if(afterMod.bmRightCtrl)
        bleKeyboard.press(KEY_RIGHT_CTRL);
    else
        bleKeyboard.release(KEY_RIGHT_CTRL);
// KEY_RIGHT_SHIFT
    if(afterMod.bmRightShift)
        bleKeyboard.press(KEY_RIGHT_SHIFT);
    else
        bleKeyboard.release(KEY_RIGHT_SHIFT);
// KEY_RIGHT_ALT
    if(afterMod.bmRightAlt)
        bleKeyboard.press(KEY_RIGHT_ALT);
    else
        bleKeyboard.release(KEY_RIGHT_ALT);
// KEY_RIGHT_GUI
    if(afterMod.bmRightGUI)
        bleKeyboard.press(KEY_RIGHT_GUI);
    else
        bleKeyboard.release(KEY_RIGHT_GUI);
  last_trigger = millis();
}
void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  {
      switch (key)
      {
      case 71:
        bleKeyboard.release(194-58+key);
        changeID();
        break;
      case 101:
        bleKeyboard.release(KEY_MEDIA_WWW_HOME); // LOL 
        break;
      default:
        bleKeyboard.release(194-58+key);
      }

  }
  last_trigger = millis(); // RESET TIMER
}
void KbdRptParser::OnKeyPressed(uint8_t key)
{
  Serial.print("ASCII: ");
  Serial.println((char)key);
};

//is this even work?
RTC_DATA_ATTR  int Initaled = 0;
USB     Usb;
USBHub     Hub(&Usb);
// USBHub     Hub1(&Usb);
HIDBoot < USB_HID_PROTOCOL_KEYBOARD > HidComposite(&Usb);
HIDBoot < USB_HID_PROTOCOL_KEYBOARD > HidKeyboard(&Usb);
KbdRptParser KbdPrs;

uint8_t led = 0;
static bool changeLed = false;
unsigned long timer,leng = 5000;

const int maxdevice = 3;
uint8_t MACAddress[maxdevice][6] = 
{
  {0x35, 0xAF, 0xA4, 0x07, 0x0B, 0x66},
  {0x31, 0xAE, 0xAA, 0x47, 0x0D, 0x61},
  {0x31, 0xAE, 0xAC, 0x42, 0x0A, 0x31},
  // ...
};

void KbdLedCb(KbdLeds *kbls)
{
  if(KbdPrs.ledstage() != *(uint8_t *) kbls)
  {
      KbdPrs.changeledstage(*(uint8_t *) kbls);
      changeLed = true;
  }
}

void changeID()
{
    Serial.println("changing MAC...");
    int deviceChose = EEPROM.read(0);
    deviceChose++;
    if(deviceChose >= maxdevice)
        EEPROM.write(0,0);
    else
        EEPROM.write(0,deviceChose);
    EEPROM.commit();
        //lmao
    esp_sleep_enable_timer_wakeup(1);
    esp_deep_sleep_start(); 
}

void turnoff()
{
    for(int i = 0;i<10;i++)
    {
        uint8_t ldc = KbdPrs.changeScrollled(i%2);
        HidComposite.SetReport(0, 0, 2, 0, 1, &ldc);
        Usb.Task();
        delay(200);
    }
    Serial.println("STOP"); 
    MAX3421_RESET // reset for no good reason
    esp_deep_sleep_start();
    
}

int first_blink = 5; 
bool updated = false;
bool turningoff = false;

void updateTimers()
{
  if(changeLed)
  {
    uint8_t ldc = KbdPrs.ledstage();
    HidComposite.SetReport(0, 0, 2, 0, 1,&ldc);
  }

  // sleep after 10 min
  if(millis() - last_trigger > 10UL*60*1000 || turningoff)
  {
    turnoff();
  }

  if( millis() - last_trigger > 5000)
  {
    if(!updated)
    {
      Serial.print("raw val: ");
      Serial.println(analogRead(34));
      Serial.print("Baterry vol: ");
      Serial.println((4095.0/analogRead(34))*((4.2*100)/(30 + 100)));
      Serial.print("Baterry lev: ");
      Serial.println(baterryLevel());
      updated = true;
    }
    bleKeyboard.setBatteryLevel(baterryLevel());
  }

  if(first_blink > 0)
    leng = 500;
  else if(!bleKeyboard.isConnected())
    leng = 100;
  else
    leng = 1000;

  if(millis() - timer > 50 && leng>900)
  {
      if(HidComposite.isReady())
      {
        uint8_t ldc = KbdPrs.changeScrollled(false);
        HidComposite.SetReport(0, 0, 2, 0, 1, &ldc);
      }
  }

  if(millis() - timer > leng)
  {
    if(led==0)led = 1;
    else led = 0;
      if(HidComposite.isReady())
      {
        first_blink--;
        uint8_t ldc = KbdPrs.changeScrollled(led);
        HidComposite.SetReport(0, 0, 2, 0, 1, &ldc);
      }

    timer = millis();

    if(bleKeyboard.isConnected() && millis()>20000)
    {
      if(getCpuFrequencyMhz()>100)
      {
        setCpuFrequencyMhz(80); //Set CPU clock when connected
        //lower power 
        esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT,ESP_PWR_LVL_N12); 
      }
    }
  }
}

void keyboardSys(void * parameter)
{
  Serial.println("Start kbd");
  
//idk but have to restart twice
  if(!Initaled)
  {
    for (size_t i = 0; i < 2; i++)
    {
      MAX3421_RESET // reset
      if (Usb.Init() == -1)
        Serial.println("OSC did not start.");
      vTaskDelay(200);
      HidComposite.SetReportParser(0, &KbdPrs);
      HidKeyboard.SetReportParser(0, &KbdPrs);
      for(int j = 0;j<100;j++)
        Usb.Task();
    }
    Initaled = true;
    Serial.println("Started");
  }
  else{
    MAX3421_RESET // still reset
    if (Usb.Init() == -1)
      Serial.println("OSC did not start.");
    vTaskDelay(200);
    HidComposite.SetReportParser(0, &KbdPrs);
    HidKeyboard.SetReportParser(0, &KbdPrs);
    // Usb.Task();
    Serial.println("Already Started");
  }
  while(1){
    Usb.Task();
    updateTimers();
    // vTaskDelay(100);
  }
}

void setup()
{
  //if u want to disable the keyboard but ... not work
  pinMode(27,OUTPUT);
  digitalWrite(27,HIGH);
  //led status
  pinMode(2,OUTPUT);
  //baterry level with 30k & 10k 
  pinMode(34,INPUT); // baterry level
  pinMode(16,OUTPUT);  // MAX3421E RESET pin
  MAX3421_RESET // reset
  digitalWrite(2,HIGH);
  pinMode(0,INPUT);  // Pause Break pin  
  xTaskCreate(keyboardSys, "usb_server", 10000,NULL, 5, NULL);
  
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,0); //1 = Low to High, 0 = High to Low. Pin pulled HIGH

  attachInterrupt(digitalPinToInterrupt(0),[]{turningoff = true;},FALLING);

  last_trigger = millis();
  setCpuFrequencyMhz(240); //Set CPU clock when start
  EEPROM.begin(4);
  Serial.begin(115200);

// xTaskCreate(keyboardSys, "usb_server", 10000,NULL, 5, NULL);

  int deviceChose = EEPROM.read(0);
  esp_base_mac_addr_set(&MACAddress[deviceChose][0]);

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT,ESP_PWR_LVL_P9); 
  Serial.println("Starting BLE");
  bleKeyboard.begin();
  bleKeyboard.releaseAll();
  bleKeyboard.setBatteryLevel(baterryLevel());
  bleKeyboard.setLedChangeCallBack(KbdLedCb);
  digitalWrite(2,LOW);
  last_trigger = millis();
}

void loop()
{
}
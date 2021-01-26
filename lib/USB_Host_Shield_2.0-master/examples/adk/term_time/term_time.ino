#include <adk.h>
#include <usbhub.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;

ADK adk(&Usb, "Circuits@Home, ltd.",
              "USB Host Shield",
              "Arduino Terminal for Android",
              "1.0",
              "http://www.circuitsathome.com",
              "0000000000000001");

void setup()
{
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("\r\nADK demo start");

  if (Usb.Init() == -1) {
    Serial.println("OSCOKIRQ failed to assert");
    while (1); //halt
  }//if (Usb.Init() == -1...
}

void loop()
{
  uint8_t buf[ 12 ] = { 0 }; //buffer to convert unsigned long to ASCII
  const char* sec_ela = " seconds elapsed\r";
  uint8_t rcode;

  Usb.Task();
  if ( adk.isReady() == false ) {
    return;
  }

  ultoa((uint32_t)millis() / 1000, (char *)buf, 10 );

  rcode = adk.SndData( strlen((char *)buf), buf );
  if (rcode && rcode != hrNAK) {
    Serial.print(F("\r\nData send: "));
    Serial.print(rcode, HEX);
  }
  rcode = adk.SndData( strlen( sec_ela), (uint8_t *)sec_ela );
  if (rcode && rcode != hrNAK) {
    Serial.print(F("\r\nData send: "));
    Serial.print(rcode, HEX);
  }

  delay( 1000 );
}

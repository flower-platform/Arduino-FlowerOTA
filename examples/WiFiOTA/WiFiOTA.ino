/* license-start
 * 
 * Copyright (C) 2016 Crispico Resonate, <http://www.crispico.com/>.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details, at <http://www.gnu.org/licenses/>.
 * 
 * license-end
 */
 
#include <FlowerOTA.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUDP.h>

FlowerOTA ota;

WiFiUDP udp;

WiFiClient client;

// CHANGE ME: Uncomment if using secure dispatcher (and comment the line above)
// WiFiSSLClient client;

// CHANGE ME: Put an IP address compatible with your LAN
IPAddress ip(192, 168, 1, 251);

void setup() {
  Serial.begin(115200);

  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  
  // CHANGE ME: Put WiFi params compatible with your LAN
  char* ssid = "my-wifi-ssid";     
  char* pass = "my-wifi-pass";  
  
  WiFi.config(ip);
  int status = WL_IDLE_STATUS;
  while ( status != WL_CONNECTED) {
    Serial.print("Conntecting to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(3000);
  }
  Serial.println("Connected");

  // CHANGE ME: if uploading via LAN, paste the "server signature" from the dialog. If uploading via secure dispatcher, put the signature that you defined for the dispatcher.
  ota.begin(&udp, &client, "my-dispatcher-server-signature");
}

long deadline = 0;

uint8_t led;

/**
 * Trivial logic: makes the embedded LED blink
 */
void loop() {
  ota.loop();
  
  if (millis() > deadline) {
    Serial.println("loop");
    if (led == HIGH) {
      led = LOW;
    } else {
      led = HIGH;
    }
    digitalWrite(6, led);
    deadline = millis() + 500;
  }
}


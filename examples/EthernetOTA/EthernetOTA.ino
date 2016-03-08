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
#include <EthernetUDP.h>

FlowerOTA ota;
EthernetUDP udp;
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 100, 251);

void setup() {
  Serial.begin(115200);

  pinMode(SS, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(4, HIGH);
  

  Ethernet.begin(mac, ip);
  ota.begin(&udp, &client);

}

long deadline;
uint8_t led;

void loop() {
  ota.loop();
  
  if (millis() > deadline) {
    Serial.println("loop1");
    if (led == HIGH) {
      led = LOW;
    } else {
      led = HIGH;
    }
    deadline = millis() + 500;
  }
}



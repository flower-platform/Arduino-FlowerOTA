#include <WiFi101.h>
#include <WiFiUDP.h>
#include <FlowerOTA.h>
#include <AzureIoT.h>

// CHANGE ME: Put WiFi params compatible with your LAN
char ssid[] = "my-wifi-ssid"; //  your network SSID (name)
char pass[] = "my-wifi-pass";    // your network password (use for WPA, or use as key for WEP)

// CHANGE ME: Put an IP address compatible with your LAN
IPAddress ip(192, 168, 1, 251);
IPAddress mask(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);

// CHANGE ME: Use your actual IoT Hub connection string
const char* connectionString = "HostName=???.azure-devices.net;DeviceId=???;SharedAccessKey=???==";
int context = 0;

IOTHUB_MESSAGE_HANDLE messageHandle = NULL;
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

FlowerOTA ota;

// WiFiClient client;

// CHANGE ME: Uncomment if using secure dispatcher (and comment the line above)
WiFiSSLClient client;

WiFiUDP udp;
uint8_t relay1Pin = 7;
uint8_t relay2Pin = 8;
uint8_t value = HIGH;

IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback) {
  const char* buffer;
  size_t size;

  if (IoTHubMessage_GetByteArray(message, (const unsigned char**) &buffer, &size) != IOTHUB_MESSAGE_OK) {
    printf("unable to retrieve the message data\r\n");
    return IOTHUBMESSAGE_ACCEPTED;
  }

  const char* PREFIX_OTAUPLOAD = "OTAUPLOAD";
  if (strncmp(buffer, PREFIX_OTAUPLOAD, strlen(PREFIX_OTAUPLOAD)) == 0) { // perform update
      const char* url = buffer + 9; // skip "OTAUPLOAD"
      ota.getUpdate(url);
  } 

  return IOTHUBMESSAGE_ACCEPTED;
}

void setup() {
  Serial.begin(115200);
  
  // setup and connect to WiFi
  int status = WL_IDLE_STATUS;
  WiFi.config(ip, dns, gateway, mask);
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    if (status != WL_CONNECTED) {
      delay(10000);
    }
  }
  Serial.println("Connected to wifi");
  
  // CHANGE ME: if uploading via LAN, paste the "server signature" from the dialog. If uploading via secure dispatcher, put the signature that you defined for the dispatcher.
  ota.begin(&udp, &client, "my-dispatcher-server-signature");

  // setup relays
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  
  //setup IoT Hub
  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
  unsigned int timeout = 241000;
  IoTHubClient_LL_SetOption(iotHubClientHandle, "timeout", &timeout);
  unsigned int minimumPollingTime = 1; /*because it can poll "after 9 seconds" polls will happen effectively at ~10 seconds*/
  IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime);
  IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &context);

}

int deadline = 0;

void loop() {
  ota.loop();
  
  if (millis() > deadline) {
    // CHANGE ME: uncomment if using Azure IoT Hub
    IoTHubClient_LL_DoWork(iotHubClientHandle);

    digitalWrite(relay2Pin, value);
    value = ~value & 1;
    digitalWrite(relay1Pin, value);
    
    deadline = millis() + 500;
  }

}



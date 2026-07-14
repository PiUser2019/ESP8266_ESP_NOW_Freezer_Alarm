#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float alarmTemp = -6;

typedef struct {
  float temperature;
  bool alarm;
} Message;

Message msg;

// Receiver MAC address
uint8_t receiverMAC[] = {0x84,0xCC,0xA8,0xA5,0x34,0x4E};
// 84:cc:a8:a5:34:4e

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Send Status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

void setup()
{
  Serial.begin(115200);

  sensors.begin();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0)
  {
    Serial.println("ESPNow Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(receiverMAC,
                   ESP_NOW_ROLE_SLAVE,
                   1,
                   NULL,
                   0);
}

void loop()
{
  sensors.requestTemperatures();

  msg.temperature = sensors.getTempCByIndex(0);
  msg.alarm = (msg.temperature >= alarmTemp);

  Serial.print("Temp: ");
  Serial.print(msg.temperature);
  Serial.print(" Alarm: ");
  Serial.println(msg.alarm);

  esp_now_send(receiverMAC,
               (uint8_t *)&msg,
               sizeof(msg));

  delay(20000);
}
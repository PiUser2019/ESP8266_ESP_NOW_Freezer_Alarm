#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

#define BUZZER D5

typedef struct
{
  float temperature;
  bool alarm;
} Message;


Message incoming;

void OnDataRecv(uint8_t *mac,
                uint8_t *data,
                uint8_t len)
{
  memcpy(&incoming, data, sizeof(incoming));

  Serial.print("Temperature: ");
  Serial.println(incoming.temperature);
  Serial.println(incoming.alarm);

  if (incoming.alarm)
  {
    digitalWrite(BUZZER, HIGH);
  }
  else
  {
    digitalWrite(BUZZER, LOW);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0)
  {
    Serial.println("ESPNow Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}
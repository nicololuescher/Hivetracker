#include "utilities.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true

// set GSM PIN, if any
#define GSM_PIN "2827"

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "internet";
const char gprsPass[] = "guest";

// MQTT details
const char *broker = "mqtt.nicolo.info";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

const int oneWireBus = 19;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int LOADCELL_DOUT = 2;
const int LOADCELL_SCK = 18;

HX711 cell;

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();
}

boolean mqttConnect()
{
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  boolean status = mqtt.connect("GsmClientName", "guest", "gibbiX12345");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish("test", "GsmClientTest started");
  mqtt.subscribe("test");
  return mqtt.connected();
}


void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);

  delay(10);

  setupGSM();
  setupMQTT();
  setupTemp();
  setupWgt();
}

void loop()
{
  float wgt = getWgt();
  float temp = getTemp();
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected 2");
  }
  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    mqtt.publish("temp", ((String)temp).c_str());
    mqtt.publish("wgt", ((String)wgt).c_str());
    return;
  }

  SerialMon.println(getTemp());

  delay(10000);
}

void setupGSM() {

  setupModem();

  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

    // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected 1");
  }
}

void setupMQTT(){
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void setupTemp(){
  sensors.begin();
}

float getTemp(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

void setupWgt(){
  cell.begin(LOADCELL_DOUT, LOADCELL_SCK);
  cell.set_scale(-0.3468);
  //cell.tare();
}


float getWgt(){
  return cell.get_units(10);
}

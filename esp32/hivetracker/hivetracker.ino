#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include "HX711.h"

// Please select the corresponding model

#define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
// #define SIM800L_IP5306_VERSION_20200811

#include "utilities.h"

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
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN "2827"

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "internet";
const char gprsPass[] = "guest";

// MQTT
const char* broker = "mqtt.nicolo.info";
const char* mqttUser = "guest";
const char* mqttPassword = "";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

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

const int oneWireBus = 5;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int LOADCELL_DOUT_PIN_1 = 2;
const int LOADCELL_SCK_PIN_1 = 4;


HX711 cell1;

void setup() {
  pinMode(oneWireBus, INPUT);
  pinMode(LOADCELL_DOUT_PIN_1, INPUT);
  pinMode(LOADCELL_SCK_PIN_1, INPUT);
  // Set console baud rate
  SerialMon.begin(115200);
  delay(100);
  setupGSM();
  setupHX711();
  setupTemp();
  setupMQTT();
  SerialMon.println("Setup done!");
}

void loop() {
  sensors.requestTemperatures();
  float wgt = cell1.get_units(10);
  float temp = sensors.getTempCByIndex(0);
  SerialMon.print("Weight: ");
  SerialMon.print(wgt, 2);
  SerialMon.print(" Temperature: ");
  SerialMon.println(temp, 2);

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
    return;
  }
  delay(1000);
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Incomming message on topic ");
  SerialMon.println(topic);
  SerialMon.write(payload, len);
  SerialMon.println();
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.println(broker);

  boolean status = mqtt.connect("Stockwaage", mqttUser, mqttPassword);

  if (status == false) {
    SerialMon.println("Failed to connect to MQTT");
    return status;
  }

  SerialMon.println("MQTT connected!");
  mqtt.publish("outTopic","hello world");
//  mqtt.publish("hivetracker/data/temperature", ((String)temp).c_str());
//  mqtt.publish("hivetracker/data/weight", ((String)wgt).c_str());
  return mqtt.connected();
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
  //modem.init();
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
    SerialMon.println(F(" [fail]"));
    SerialMon.println(F("************************"));
    SerialMon.println(F(" Is your sim card locked?"));
    SerialMon.println(F(" Do you have a good signal?"));
    SerialMon.println(F(" Is antenna attached?"));
    SerialMon.println(F(" Does the SIM card work with your phone?"));
    SerialMon.println(F("************************"));
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print("Connecting to ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  IPAddress local = modem.localIP();
  SerialMon.print("Local IP: ");
  SerialMon.println(local);
}

void setupHX711() {
  SerialMon.println("Setting up loadcell");
  cell1.begin(LOADCELL_DOUT_PIN_1, LOADCELL_SCK_PIN_1);
  SerialMon.println("Set scale");
  cell1.set_scale(-5.18);
  SerialMon.println("Tare");
  cell1.tare();
}

void setupTemp() {
  delay(10);
  SerialMon.println("Setting up temperature sensor");
  sensors.begin();
}

void setupMQTT() {
  SerialMon.println("Setting up MQTT");
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

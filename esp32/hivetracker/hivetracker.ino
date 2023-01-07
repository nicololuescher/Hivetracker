#include "utilities.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"
#include <PubSubClient.h>

// modem model
#define TINY_GSM_MODEM_SIM800
// must be included AFTER modem definition!!
#include <TinyGsmClient.h>

// serial debugger
#define SerialMon Serial

// serial for at commands
#define SerialAT Serial1

// debug console for gsm client
#define TINY_GSM_DEBUG SerialMon

// connect via GPRS
#define TINY_GSM_USE_GPRS true

// sim pin
#define GSM_PIN "2827"

// gprs credentials for digitec iot
const char apn[] = "internet";
const char gprsUser[] = "internet";
const char gprsPass[] = "guest";

// mqtt broker
const char *broker = "mqtt.nicolo.info";
const char *mqttUser = "guest";
const char *mqttPassword = "gibbiX12345";

// configuration
const int baudRate = 115200;
const int timeout = 60; // in seconds
const float scaleFactor = -3.3707142857142857142857142857143;
const int numerOfMeasurements = 10;

// temperature sensor setup
const int oneWireBus = 19;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// load cell setup
const int LOADCELL_DOUT = 2;
const int LOADCELL_SCK = 18;
HX711 cell;

// create gprs modem
TinyGsm modem(SerialAT);

// create client from modem
TinyGsmClient client(modem);

// pass client to mqtt
PubSubClient mqtt(client);

// connect to mqtt
boolean mqttConnect()
{
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  boolean status = mqtt.connect("GsmClientName", mqttUser, mqttPassword, "hivetracker/events", 0, 1, "Scale Timeout");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish("hivetracker/events", "Scale Connected", true);
  return mqtt.connected();
}

void setup()
{
  // Set console baud rate
  SerialMon.begin(baudRate);

  delay(10);

  // initialize all components
  setupGSM();
  setupMQTT();
  setupTemp();
  setupWgt();

    // get measurement
  float wgt = getWgt();
  float temp = getTemp();

  // reconnect gprs if disconnected
  if(!modem.isGprsConnected()){
    connectGPRS();  
  }

  // reconnect mqtt if disconnected
  if (!mqtt.connected()) {
    SerialMon.println("Connecting to MQTT broker");

    while(!mqttConnect()){
      delay(10000);
    }
  }

  // publish measurement
  mqtt.publish("hivetracker/data/temperature", ((String)temp).c_str(), true);
  mqtt.publish("hivetracker/data/weight", ((String)wgt).c_str(), true);

  SerialMon.print("Publishing Temperature: ");
  SerialMon.println(temp);
  SerialMon.print("Publishing Weight: ");
  SerialMon.println(wgt);

  mqtt.publish("hivetracker/events", "Scale Disconnected", true);

  //making sure data has time to be published
  delay(500);

  mqtt.disconnect();
  delay(500);

  SerialMon.print("Sleeping for ");
  SerialMon.print(timeout);
  SerialMon.println(" seconds");

  // set wakeup time
  esp_sleep_enable_timer_wakeup(timeout * 1000 * 1000); // seconds * millis * nanos
  // flush serial monitor
  SerialMon.flush();
  // go to sleep
  esp_deep_sleep_start();
}

void loop()
{}

void setupGSM() {
  SerialMon.println("Setting up GSM");
  setupModem();
  SerialMon.println("Wait...");

  // set GSM module baud rate and UART pins
  SerialAT.begin(baudRate, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(6000);

  // initialize modem
  SerialMon.println("Initializing modem...");
  modem.restart();

  // print out modem info
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // unlock sim
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  // wait for network
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    setupGSM();
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }
  connectGPRS();
}

// connect to GPRS
void connectGPRS(){
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  // retry connection after 10 seconds if unable to connect
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    connectGPRS();
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }
}

// setup mqtt
void setupMQTT(){
  mqtt.setServer(broker, 1883);
}

// setup temperature
void setupTemp(){
  sensors.begin();
}

// read temperature
float getTemp(){
  // prepare data
  sensors.requestTemperatures();

  // read from index 0
  return sensors.getTempCByIndex(0);
}

// setup weight
void setupWgt(){
  // start cell with given ports
  cell.begin(LOADCELL_DOUT, LOADCELL_SCK);
  // configure scale
  cell.set_scale(scaleFactor);
}

// read weight
float getWgt(){
  // read weight n times and return average
  return cell.get_units(numerOfMeasurements);
}

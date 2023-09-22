/*******************************************************************************
  Release 2 for external
  Replaces the DHT11 and DHT22 sensor with a BME280
 *******************************************************************************/

#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_ADS1015.h>
#include <SoftwareSerial.h>
#include <Ticker.h>
#include <Adafruit_BME280.h>
#include "MedianFilterLib2.h"

//#define SEALEVELPRESSURE_HPA (1013.25) For BME280 altitude estimation
#define FWREVISION 6
#define BAUD_RATE 9600
#define TX 25
#define RX 26
#define TX_INTERVAL 60      //LoRa reansmitt in seconds
#define SERIAL_INTERVAL 5   //Local serial print interval
#define FS (TX_INTERVAL/SERIAL_INTERVAL)
#define FSpms (TX_INTERVAL/(SERIAL_INTERVAL + 1))

unsigned long now = 0;
unsigned long previousMillis = 0;
unsigned long acqInterval = 1000 * SERIAL_INTERVAL; //sample data every 5 seconds

Ticker secondTick;


//Regular sensors
MedianFilter2<float> tempMedian(FS);
MedianFilter2<float> humMedian(FS);
MedianFilter2<float> pressMedian(FS);
MedianFilter2<uint16_t> vocMedian(FS);
MedianFilter2<uint16_t> co2Median(FS);
MedianFilter2<uint16_t> pidMedian(FS);

//PMS sensor
MedianFilter2<uint16_t> pm1_0s_Median(FSpms);  //P1.0 standard
MedianFilter2<uint16_t> pm2_5s_Median(FSpms);  //P2.5 standard
MedianFilter2<uint16_t> pm5_0s_Median(FSpms);  //P5.0 standard
MedianFilter2<uint16_t> pm10_0s_Median(FSpms); //P10.0 standard
MedianFilter2<uint16_t> pm1_0e_Median(FSpms);  //P1.0 env
MedianFilter2<uint16_t> pm2_5e_Median(FSpms);  //P2.5 env
MedianFilter2<uint16_t> pm5_0e_Median(FSpms);  //P5.0 env
MedianFilter2<uint16_t> pm10_0e_Median(FSpms); //P10.0 env
MedianFilter2<uint16_t> pc0_3c_Median(FSpms);   //All particles > 0.3um per 0.1L air
MedianFilter2<uint16_t> pc0_5c_Median(FSpms);  //All particles > 0.5um per 0.1L air
MedianFilter2<uint16_t> pc1_0c_Median(FSpms);  //All particles > 1.0um per 0.1L air
MedianFilter2<uint16_t> pc2_5c_Median(FSpms);  //All particles > 2.5um per 0.1L air
MedianFilter2<uint16_t> pc5_0c_Median(FSpms);  //All particles > 5.0um per 0.1L air
MedianFilter2<uint16_t> pc10_0c_Median(FSpms); //All particles > 10.0um per 0.1L air


volatile uint8_t watchdogCount = 0; //Declare as volatile

void ISRwatchdog() {
  watchdogCount ++;

  //2 seconds will fire WDT
  if (watchdogCount == 2) {
    Serial.println("Watch dog set, about to reset device");
    esp_restart();
  }
}

//###############################################
// ADC setup
// usd for ...
//###############################################

// Adafruit_ADS1015 ads;  //12bit version
Adafruit_ADS1115 ads;     // THIS is the 16 bit component!
Adafruit_BME280 bme;      // I2C





SoftwareSerial pmsSerial;

//###############################################
// LoRa module setup
// usd for sensor communication with gateway
//###############################################


// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t payload[48];
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).


// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 5,                       // chip select
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,                       // reset pin
  .dio = {2, 4, LMIC_UNUSED_PIN}, // assumes external jumpers [feather_lora_jumper]
  // DIO1 is on JP1-1: is io1 - we connect to GPO6
  // DIO1 is on JP5-3: is D2 - we connect to GPO5
};

uint16_t pSent = 0;

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

  int count = 0;

void setup() {
  
  secondTick.attach(1, ISRwatchdog);
  watchdogCount = 0;


  payload[0] = FWREVISION; //1st payload byte

  while (!Serial); // wait for Serial to be initialized
  Serial.begin(115200);
  lora_config.update();

  delay(100);
  Serial.println(F("Starting..........."));


  pmsSerial.begin(BAUD_RATE, SWSERIAL_8N1, RX, TX, false, 256);
  delay(500); //Ensure PMS is fully powered up
  Serial.println(PSTR("\nSoftware serial test started"));
  byte send_data[8] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70}; //For active mode
  //byte send_data[1] = {0x42};
  pmsSerial.write(send_data, 7);

  // Temp/humidity/pressure and ADc initialisation
  bme.begin(0x76);
  ads.begin();
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  //LoRaConfig lora_config;


  LMIC_setSession (0x13, lora_config.get_devaddr(), lora_config.get_nwkskey(), lora_config.get_appskey());


#if defined(CFG_eu868)
    // Set up the channels used by the Things Network, which corresponds
    // to the defaults of most gateways. Without this, only three base
    // channels from the LoRaWAN specification are used, which certainly
    // works, so it is good for debugging, but can overload those
    // frequencies, so be sure to configure the full frequency range of
    // your network here (unless your network autoconfigures them).
    // Setting up channels should happen after LMIC_setSession, as that
    // configures the minimal channel set. The LMIC doesn't let you change
    // the three basic settings, but we show them here.
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B
    // devices' ping slots. LMIC does not have an easy way to define set this
    // frequency and support for class B is spotty and untested, so this
    // frequency is not configured here.
    #elif defined(CFG_us915) || defined(CFG_au915)
    // NA-US and AU channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    LMIC_selectSubBand(1);
    #elif defined(CFG_as923)
    // Set up the channels used in your country. Only two are defined by default,
    // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
    // LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    // LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);

    // ... extra definitions for channels 2..n here
    #elif defined(CFG_kr920)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    // LMIC_setupChannel(0, 922100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(1, 922300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(2, 922500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

    // ... extra definitions for channels 3..n here.
    #elif defined(CFG_in866)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    // LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
    // LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

    // ... extra definitions for channels 3..n here.
    #else
    # error Region not supported
    #endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF9, 14);
  //LMIC_setAdrMode(1);

  // Start job
  do_send(&sendjob);
}


void loop() {
  unsigned long currentMillis = millis();
  

  //Record each sensor input every second
  //Will put this into a bigger tx array later
  if (currentMillis - previousMillis > acqInterval) {
    previousMillis = currentMillis;
    count++;
    getTHP(count);
    getVOC();
    getPMS();
    getCO2();

  }

  //while(1); // WDT test
  os_runloop_once(); // timingis critical according to the sketch this is based on. We should remove any print statements when ready.
  watchdogCount = 0; // Resets watchdog
}

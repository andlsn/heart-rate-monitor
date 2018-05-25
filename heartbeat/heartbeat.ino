#include <SPI.h>
#include <SPBTLE_RF.h>
#include <sensor_service.h>

#define PIN_BLE_SPI_MOSI   (11)
#define PIN_BLE_SPI_MISO   (12)
#define PIN_BLE_SPI_SCK    (3)

#define PIN_BLE_SPI_nCS    (A1)
#define PIN_BLE_SPI_RESET  (7)
#define PIN_BLE_SPI_IRQ    (A0)

#define PIN_BLE_LED    (0xFF)

#define SerialPort Serial

// Configure BTLE_SPI
SPIClass BTLE_SPI(PIN_BLE_SPI_MOSI, PIN_BLE_SPI_MISO, PIN_BLE_SPI_SCK);

// Configure BTLE pins
SPBTLERFClass BTLE(&BTLE_SPI, PIN_BLE_SPI_nCS, PIN_BLE_SPI_IRQ, PIN_BLE_SPI_RESET, PIN_BLE_LED);

const char *name = "BlueNRG";
uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x03};
//end bluetooth

#define PIN 2

#define MAX 2000
#define MIN -1
#define SHORT_PAUSE 2000
#define LONG_PAUSE 20000

#define MIN_MAX_SIZE 3
#define IBIS_SIZE 10
#define IBI_THRESHOLD 3000

#define NOISE_THRESHOLD 15

int Signal;

unsigned long pause = SHORT_PAUSE;

int minValues[MIN_MAX_SIZE];
int maxValues[MIN_MAX_SIZE];

unsigned long lastTime = 0;
int threshold = 0;
int goodValues = 0;
bool goodThreshold = false;

bool detected = false;

int IBIs[IBIS_SIZE];
unsigned long previousTimeIBI = 0;
unsigned long lastTimeIBI = 0;
int i = 0;

int IBI = 0;
int BPM = 0;

int noise = 0;

int previousIBI = 0;
int editIBI = 1;

void setup() {
  int ret;

  Serial.begin(9600);

  if (BTLE.begin() == SPBTLERF_ERROR) {
    Serial.println("Bluetooth module configuration error!");
    while (1);
  }

  if (SensorService.begin(name, SERVER_BDADDR)) {
    Serial.println("Sensor service configuration error!");
    while (1);
  }

  ret = SensorService.Add_Environmental_Sensor_Service();

  if (ret == BLE_STATUS_SUCCESS)
    Serial.println("Environmental Sensor service added successfully.");
  else
    Serial.println("Error while adding Environmental Sensor service.");

  if (SensorService.isConnected() == FALSE) {
    SensorService.setConnectable();
  }

  resetMinMax();

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  BTLE.update();

  Signal = analogRead(PIN);
  printSignal();

  setMinMax();
  setNewThreshold();

  if (goodThreshold) {
    detectHeartBeat();
  }

  delay(20);
}

void setNewThreshold() {
  if (millis() - lastTime >= pause) {
    lastTime = millis();

    if (SensorService.isConnected() == FALSE) {
      SensorService.setConnectable();
    }

    int meanMin = meanMinMax(minValues, MAX);
    int meanMax = meanMinMax(maxValues, MIN);

    if (meanMin + 30 < meanMax && meanMin >= 300) {
      goodThreshold = true;
      threshold = meanMax - 25;

      printThreshold(threshold, meanMin, meanMax);

    } else {
      goodValues = 0; //reset IBIs
      i = 0; //reset IBIs
      goodThreshold = false;
      previousTimeIBI = 0;
      BPM = 0;
      printSensorError(meanMin, meanMax);
    }

    pause = BPM > 0 ? LONG_PAUSE : SHORT_PAUSE;

    resetMinMax();
  }
}

void detectHeartBeat() {
  if (Signal >= threshold) {
    if (!detected) {
      detected = true;

      printHeartBeat();

      if (getIBI()) {
        digitalWrite(LED_BUILTIN, HIGH);
        getBPM(IBI);
      }

    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    noise = 0;
  } else {
    if (noise > NOISE_THRESHOLD) {
      detected = false;
      noise = 0;
    }
    noise++;
    digitalWrite(LED_BUILTIN, LOW);
  }
}

bool getIBI() {
  lastTimeIBI = millis();

  bool accept = false;

  if (previousTimeIBI > 0) {
    previousIBI = IBI;
    IBI = lastTimeIBI - previousTimeIBI;

    if (IBI == previousIBI) {
      IBI += editIBI;
      editIBI *= -1;
    }

    accept = IBI <= IBI_THRESHOLD;

    if (accept) {
      printIBI(IBI);

      if (SensorService.isConnected() == TRUE) {
        SensorService.Humidity_Update(IBI);
      }

    } else {
      printDiscardedIBI(IBI);
    }
  }

  previousTimeIBI = lastTimeIBI;

  return accept;
}

void getBPM(int IBI) {
  goodValues++;

  IBIs[i] = IBI;
  i = i == (IBIS_SIZE - 1) ? 0 : (i + 1);

  printIBIs();

  if (goodValues >= IBIS_SIZE) {
    BPM = 60000 / meanIBI();
    printBPM(BPM);
  }
}

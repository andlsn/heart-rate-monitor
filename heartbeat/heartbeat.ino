#define PIN 2

#define MAX 2000
#define MIN -1
#define SHORT_PAUSE 2000
#define LONG_PAUSE 20000

#define MIN_MAX_SIZE 3
#define IBIS_SIZE 10

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
unsigned long previousIBI = 0;
unsigned long lastIBI = 0;
int i = 0;

int IBI = 0;
int BPM = 0;

int noise = 0;

void setup() {
  resetMinMaxValues();

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Signal = analogRead(PIN);
  printSignal();

  setMinMaxValues();
  setNewThreshold();

  if (goodThreshold) {
    detectHeartBeat();
  }

  delay(20);
}

void setNewThreshold() {
  if (millis() - lastTime >= pause) {
    lastTime = millis();

    int meanMin = meanMinMaxValues(minValues, MAX);
    int meanMax = meanMinMaxValues(maxValues, MIN);

    if (meanMin + 30 < meanMax && meanMin >= 300) {
      goodThreshold = true;
      threshold = meanMax - 25;

      printThreshold(threshold, meanMin, meanMax);

    } else {
      goodValues = 0; //reset IBIs
      i = 0; //reset IBIs
      goodThreshold = false;
      BPM = 0;
      printSensorError(meanMin, meanMax);
    }

    pause = BPM > 0 ? LONG_PAUSE : SHORT_PAUSE;

    resetMinMaxValues();
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
  lastIBI = millis();

  bool accept = false;

  if (previousIBI > 0) {
    IBI = lastIBI - previousIBI;

    accept = IBI <= 1500;

    if (accept) {
      printIBI(IBI);
    } else {
      printDiscardedIBI(IBI);
    }
  }

  previousIBI = lastIBI;

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
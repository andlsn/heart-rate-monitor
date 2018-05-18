#define PIN 2

#define MAX 2000
#define MIN -1

#define MIN_MAX_SIZE 3
#define IBIS_SIZE 10

int Signal;

unsigned long pause = 2000;

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

void setup() {
  resetMinMaxValues();

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Signal = analogRead(PIN);

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
  } else {
    detected = false;
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

void setMinMaxValues() {
  for (int j = 0; j < MIN_MAX_SIZE; j++) {
    if (Signal > maxValues[j]) {
      maxValues[j] = Signal;
      break;
    }
  }

  for (int j = 0; j < MIN_MAX_SIZE; j++) {
    if (Signal < minValues[j]) {
      minValues[j] = Signal;
      break;
    }
  }
}

void resetMinMaxValues() {
  for (int i = 0; i < MIN_MAX_SIZE; i++) {
    minValues[i] = MAX;
    maxValues[i] = MIN;
  }
}

int meanMinMaxValues(int a[], int discard) {
  int sum = 0;
  int len = 0;
  for (int j = 0; j < MIN_MAX_SIZE; j++) {
    if (a[j] != discard) {
      sum += a[j];
      len++;
    }
  }
  return sum / len;
}

int meanIBI() {
  int sum = 0;
  for (int j = 0; j < IBIS_SIZE; j++) sum += IBIs[j];
  return sum / IBIS_SIZE;
}
#define LEVEL INFO

#define TRACE 0
#define DEBUG 1
#define INFO 2
#define OFF 3

void printSignal() {
  if (!log(TRACE)) return;

  Serial.println(Signal);
}

void printThreshold(int threshold, int meanMin, int meanMax) {
  if (!log(INFO)) return;

  Serial.print("New threshold: ");
  Serial.print(threshold);
  Serial.print(" - (");
  Serial.print(meanMin);
  Serial.print("/");
  Serial.print(meanMax);
  Serial.println(")");
}

void printSensorError(int meanMin, int meanMax) {
  if (!log(INFO)) return;

  Serial.print("The sensor is not properly placed - (");
  Serial.print(meanMin);
  Serial.print("/");
  Serial.print(meanMax);
  Serial.println(")");
}

void printHeartBeat() {
  if (!log(DEBUG)) return;

  static int n = 0;

  Serial.print("New heartbeat - ");
  Serial.println(n);
  n++;
}

void printIBI(int IBI) {
  if (!log(OFF)) return;

  Serial.println("❤️ !!Heartbeat detected!! ❤️");
  Serial.print("IBI: ");
  Serial.println(IBI);
}

void printDiscardedIBI(int IBI) {
  if (!log(DEBUG)) return;

  Serial.print("DISCARD IBI: ");
  Serial.println(IBI);
}

void printBPM(int BPM) {
  if (!log(OFF)) return;

  Serial.print("BPM: ");
  Serial.println(BPM);
}

void printIBIs() {
  if (!log(DEBUG)) return;

  Serial.print(" -> [");
  for (int j = 0; j < IBIS_SIZE - 1; j++) {
    Serial.print(IBIs[j]);
    Serial.print(", ");
  }
  Serial.print(IBIs[IBIS_SIZE - 1]);
  Serial.print("]");
  Serial.println();
}

bool log(int l) {
  return l >= LEVEL;
}

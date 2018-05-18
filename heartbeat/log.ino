void printThreshold(int threshold, int meanMin, int meanMax) {
  Serial.print("New threshold: ");
  Serial.print(threshold);
  Serial.print(" - (");
  Serial.print(meanMin);
  Serial.print("/");
  Serial.print(meanMax);
  Serial.println(")");
}

void printSensorError(int meanMin, int meanMax) {
  Serial.print("The sensor is not properly placed - (");
  Serial.print(meanMin);
  Serial.print("/");
  Serial.print(meanMax);
  Serial.println(")");
}

void printHeartBeat() {
  static int n = 0;

  Serial.print("New heartbeat - ");
  Serial.println(n);
  n++;
}

void printIBI(int IBI) {
  Serial.println("❤️ !!Heartbeat detected!! ❤️");
  Serial.print("IBI: ");
  Serial.println(IBI);
}

void printBPM(int BPM) {
  Serial.print("BPM: ");
  Serial.println(BPM);
}

void printIBIs() {
  Serial.print(" -> [");
  for (int j = 0; j < IBIS_SIZE - 1; j++) {
    Serial.print(IBIs[j]);
    Serial.print(", ");
  }
  Serial.print(IBIs[IBIS_SIZE - 1]);
  Serial.print("]");
  Serial.println();
}
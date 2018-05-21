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
  for (int j = 0; j < MIN_MAX_SIZE; j++) {
    minValues[j] = MAX;
    maxValues[j] = MIN;
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
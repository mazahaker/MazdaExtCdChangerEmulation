uint8_t getBit() {
  unsigned long time = micros();
  while (digitalRead(2) == LOW) {
  }
  if (micros() - time < 1000) {
    return 0;
  }
  return 1;
}

uint32_t receive() {
  long t1 = micros();
  uint32_t res = 0;
  long curTime = micros();
  while (micros() - curTime < 3250) {
    if (digitalRead(M_IN) == LOW) {
      res = res << 1 | getBit();
      curTime = micros();
    }
  }
  long t2 = micros();
  if (debug) {
    Serial.print("R: ");
    Serial.print(t1);
    Serial.print(" - ");
    Serial.print(t2);
    Serial.print(" = ");
    Serial.print(t2 - t1);
    Serial.print(" RES ");
    Serial.println(String(res, HEX));
  }
  needReceive = false;
  return res;
}

void sendZero()
{
  digitalWrite(M_OUT, HIGH);
  delayMicroseconds(600);
  digitalWrite(M_OUT, LOW);
  delayMicroseconds(2400);
}

void sendOne()
{
  digitalWrite(M_OUT, HIGH);
  delayMicroseconds(1800);
  digitalWrite(M_OUT, LOW);
  delayMicroseconds(1200);
}

inline unsigned char toHex(char ch) {
  if (ch >= 'A') {
    return ch - 'A' + 0xA & 0x0F;
  } else {
    return ch - '0' & 0x0F;
  }
}

void writeHexBitWise(uint8_t message) {
  for (int8_t i = 3; i > -1; i--) {
    uint8_t output = ((message & (1 << i) ) >> i);
    if (output == 1) {
      sendOne();
    } else {
      sendZero();
    }
  }
}

void send(const char message[], int size, int delayValue) {
  if (debug) {
    Serial.print("S: ");
    Serial.println(message);
  }
  delay(delayValue);
  uint8_t controlSum = 0;
  for (int i = 0; i < size; i++) {
    uint8_t output = toHex(message[i]);
    controlSum = controlSum ^ output;
    writeHexBitWise(output);
  }
  controlSum += 1;
  writeHexBitWise(controlSum);
}

void send(const char message[], int size) {
  send(message, size, 90);
}

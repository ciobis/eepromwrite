
static const uint8_t ADDR_0 = 2;
static const uint8_t ADDR_1 = 3;
static const uint8_t ADDR_2 = 4;
static const uint8_t ADDR_3 = 5;
static const uint8_t ADDR_4 = 6;
static const uint8_t ADDR_5 = 7;
static const uint8_t ADDR_6 = 8;
static const uint8_t ADDR_7 = 9;
static const uint8_t ADDR_8 = 10;
static const uint8_t ADDR_9 = 11;
static const uint8_t ADDR_10 = 12;

static const uint8_t IO_0 = ADDR_0;
static const uint8_t IO_1 = ADDR_1;
static const uint8_t IO_2 = ADDR_2;
static const uint8_t IO_3 = ADDR_3;
static const uint8_t IO_4 = ADDR_4;
static const uint8_t IO_5 = ADDR_5;
static const uint8_t IO_6 = ADDR_6;
static const uint8_t IO_7 = ADDR_7;

static const uint8_t LATCH_ADDRESS = 13;
static const uint8_t LATCH_ENABLED = A0; //active low
static const uint8_t EEPROM_WRITE = A1; //active low
static const uint8_t EEPROM_READ = A2; //active low

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ADDR_0, OUTPUT);
  pinMode(ADDR_1, OUTPUT);
  pinMode(ADDR_2, OUTPUT);
  pinMode(ADDR_3, OUTPUT);
  pinMode(ADDR_4, OUTPUT);
  pinMode(ADDR_5, OUTPUT);
  pinMode(ADDR_6, OUTPUT);
  pinMode(ADDR_7, OUTPUT);
  pinMode(ADDR_8, OUTPUT);
  pinMode(ADDR_9, OUTPUT);
  pinMode(ADDR_10, OUTPUT);
  
  pinMode(IO_0, OUTPUT);
  pinMode(IO_1, OUTPUT);
  pinMode(IO_2, OUTPUT);
  pinMode(IO_3, OUTPUT);
  pinMode(IO_4, OUTPUT);
  pinMode(IO_5, OUTPUT);
  pinMode(IO_6, OUTPUT);
  pinMode(IO_7, OUTPUT);
  
  pinMode(LATCH_ADDRESS, OUTPUT);
  pinMode(LATCH_ENABLED, OUTPUT);
  pinMode(EEPROM_WRITE, OUTPUT);
  pinMode(EEPROM_READ, OUTPUT);

  digitalWrite(LATCH_ADDRESS, LOW);
  digitalWrite(LATCH_ENABLED, HIGH);
  digitalWrite(EEPROM_WRITE, HIGH);
  digitalWrite(EEPROM_READ, HIGH);
  
  Serial.begin(9600);
  Serial.setTimeout(2147483647);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  int handshake = Serial.parseInt();
  Serial.println("OK");
}

void loop() {
  int function = Serial.parseInt();
  if (function == 1) {
    int address = Serial.parseInt();
    int value = Serial.parseInt();
    writeValue(address, value);

    Serial.print(address, DEC);
    Serial.print(":");
    Serial.println(value, DEC);
  } else if (function == 2) {
    int address = Serial.parseInt();
    setAddress(address);
    int valueAtAddress = readValue();

    Serial.println(valueAtAddress, DEC);
  }
}

void writeValue(int address, int value) {
  setAddress(address);
  setValue(value);
  
  performWrite(nthBit(value, 7));
}

int readValue() {
  digitalWrite(EEPROM_READ, LOW);
  pinMode(ADDR_0, INPUT);
  pinMode(ADDR_1, INPUT);
  pinMode(ADDR_2, INPUT);
  pinMode(ADDR_3, INPUT);
  pinMode(ADDR_4, INPUT);
  pinMode(ADDR_5, INPUT);
  pinMode(ADDR_6, INPUT);
  pinMode(ADDR_7, INPUT);
  
  int result = 0;
  for (int i = 9; i >= 2; i--) {
    result = result << 1;
    result = result | digitalRead(i);
  }

  digitalWrite(EEPROM_READ, HIGH);
  pinMode(ADDR_0, OUTPUT);
  pinMode(ADDR_1, OUTPUT);
  pinMode(ADDR_2, OUTPUT);
  pinMode(ADDR_3, OUTPUT);
  pinMode(ADDR_4, OUTPUT);
  pinMode(ADDR_5, OUTPUT);
  pinMode(ADDR_6, OUTPUT);
  pinMode(ADDR_7, OUTPUT);


  return result;
}

void performWrite(bool lastBitValue) {
  digitalWrite(EEPROM_WRITE, LOW);
  delayMicroseconds(1);
  digitalWrite(EEPROM_WRITE, HIGH);

  pinMode(IO_7, INPUT);
  digitalWrite(EEPROM_READ, LOW);

  while(digitalRead(IO_7) != lastBitValue) {
    //wait until IO_7 == last bit value
  }

  digitalWrite(EEPROM_READ, HIGH);
  pinMode(IO_7, OUTPUT);
}

void setValue(int value) {
  digitalWrite(IO_0, nthBit(value, 0));
  digitalWrite(IO_1, nthBit(value, 1));
  digitalWrite(IO_2, nthBit(value, 2));
  digitalWrite(IO_3, nthBit(value, 3));
  digitalWrite(IO_4, nthBit(value, 4));
  digitalWrite(IO_5, nthBit(value, 5));
  digitalWrite(IO_6, nthBit(value, 6));
  digitalWrite(IO_7, nthBit(value, 7));
}

void setAddress(int address) {
  digitalWrite(ADDR_0, nthBit(address, 0));
  digitalWrite(ADDR_1, nthBit(address, 1));
  digitalWrite(ADDR_2, nthBit(address, 2));
  digitalWrite(ADDR_3, nthBit(address, 3));
  digitalWrite(ADDR_4, nthBit(address, 4));
  digitalWrite(ADDR_5, nthBit(address, 5));
  digitalWrite(ADDR_6, nthBit(address, 6));
  digitalWrite(ADDR_7, nthBit(address, 7));
  digitalWrite(ADDR_8, nthBit(address, 8));
  digitalWrite(ADDR_9, nthBit(address, 9));
  digitalWrite(ADDR_10, nthBit(address, 10));

  //first 8 bits are buffered to registers so i need to latch the data there
  digitalWrite(LATCH_ENABLED, LOW);
  delay(1);
  digitalWrite(LATCH_ADDRESS, HIGH);
  delay(1);
  digitalWrite(LATCH_ADDRESS, LOW);
  digitalWrite(LATCH_ENABLED, HIGH);
}

int nthBit(int value, int n) {
  return value & (1 << n);
}

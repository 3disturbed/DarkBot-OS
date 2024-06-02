//DarkBot RoboHat Teensy4.1 Firmware Alpha
// Propery to 3disturbed

#include <Servo.h>
#include <SD.h>
#include <EEPROM.h>

#define NUM_SERVOS 10
#define NUM_MOTORS 4
#define NUM_ADCS 8
#define LOG_FILE "log.txt"

Servo servos[NUM_SERVOS];
int motorInverted[NUM_MOTORS] = {0};  // 0 = not inverted, 1 = inverted
int servoInverted[NUM_SERVOS] = {0};  // 0 = not inverted, 1 = inverted
int servoSoftLimitMin[NUM_SERVOS] = {0};
int servoSoftLimitMax[NUM_SERVOS] = {180};

// Motor control pins (using DRV8833)
int motorPins[NUM_MOTORS][2] = {
  {2, 3},   // Motor 1
  {4, 5},   // Motor 2
  {6, 9},   // Motor 3
  {10, 11}  // Motor 4
};

// ADC pins (including battery monitor)
int adcPins[NUM_ADCS] = {16, 17, 18, 19, 20, 21, 22, 23};

// Servo control pins
int servoPins[NUM_SERVOS] = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26};

// Initialize SD card
void initSD() {
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  File logFile = SD.open(LOG_FILE, FILE_WRITE);
  if (!logFile) {
    Serial.println("Failed to open log file!");
    return;
  }
  logFile.close();
}

// Write to SD card
void logToSD(const String& message) {
  File logFile = SD.open(LOG_FILE, FILE_WRITE);
  if (logFile) {
    logFile.println(message);
    logFile.close();
  }
}

// Remove old log entries if the SD card is near full
void manageSDLogs() {
  // Implementation for removing old entries
  // This can be implemented as per specific requirements
}

// Save configurations to EEPROM
void saveConfig() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    EEPROM.update(i * 2, servoInverted[i]);
    EEPROM.update(i * 2 + 1, servoSoftLimitMin[i]);
    EEPROM.update(i * 2 + 2, servoSoftLimitMax[i]);
  }
  for (int i = 0; i < NUM_MOTORS; i++) {
    EEPROM.update(NUM_SERVOS * 2 + i, motorInverted[i]);
  }
}

// Load configurations from EEPROM
void loadConfig() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servoInverted[i] = EEPROM.read(i * 2);
    servoSoftLimitMin[i] = EEPROM.read(i * 2 + 1);
    servoSoftLimitMax[i] = EEPROM.read(i * 2 + 2);
  }
  for (int i = 0; i < NUM_MOTORS; i++) {
    motorInverted[i] = EEPROM.read(NUM_SERVOS * 2 + i);
  }
}

void setup() {
  Serial.begin(115200);     // Serial for debugging
  Serial1.begin(9600);      // UART1
  Serial2.begin(9600);      // UART2

  loadConfig();
  initSD();

  // Initialize motor control pins
  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(motorPins[i][0], OUTPUT);
    pinMode(motorPins[i][1], OUTPUT);
  }

  // Attach servos to pins
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }

  // Initialize ADC pins
  for (int i = 0; i < NUM_ADCS; i++) {
    pinMode(adcPins[i], INPUT);
  }
}

void loop() {
  manageSDLogs();
  // Check for commands
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    processCommand(command);
  }
}

void processCommand(String command) {
  if (command.startsWith("pinMode")) {
    int pin, state;
    sscanf(command.c_str(), "pinMode(%d,%d)", &pin, &state);
    if (pin < 40) {
      pinMode(pin, state == 0 ? OUTPUT : (state == 1 ? INPUT : INPUT_ANALOG));
      logToSD("pinMode(" + String(pin) + "," + String(state) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("Servo")) {
    int index, position;
    sscanf(command.c_str(), "Servo(%d,%d)", &index, &position);
    if (index < NUM_SERVOS) {
      position = constrain(position, servoSoftLimitMin[index], servoSoftLimitMax[index]);
      servos[index].write(servoInverted[index] ? 180 - position : position);
      logToSD("Servo(" + String(index) + "," + String(position) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("ServoInvert")) {
    int index;
    sscanf(command.c_str(), "ServoInvert(%d)", &index);
    if (index < NUM_SERVOS) {
      servoInverted[index] = !servoInverted[index];
      saveConfig();
      logToSD("ServoInvert(" + String(index) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("ServoSoftLimitMin")) {
    int index, limit;
    sscanf(command.c_str(), "ServoSoftLimitMin(%d,%d)", &index, &limit);
    if (index < NUM_SERVOS) {
      servoSoftLimitMin[index] = limit;
      saveConfig();
      logToSD("ServoSoftLimitMin(" + String(index) + "," + String(limit) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("ServoSoftLimitMax")) {
    int index, limit;
    sscanf(command.c_str(), "ServoSoftLimitMax(%d,%d)", &index, &limit);
    if (index < NUM_SERVOS) {
      servoSoftLimitMax[index] = limit;
      saveConfig();
      logToSD("ServoSoftLimitMax(" + String(index) + "," + String(limit) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("Motor")) {
    int index, speed;
    sscanf(command.c_str(), "Motor(%d,%d)", &index, &speed);
    if (index < NUM_MOTORS) {
      bool invert = motorInverted[index];
      digitalWrite(motorPins[index][0], invert ? speed < 0 : speed > 0);
      analogWrite(motorPins[index][1], abs(speed));
      logToSD("Motor(" + String(index) + "," + String(speed) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  } else if (command.startsWith("MotorSetInverted")) {
    int index;
    sscanf(command.c_str(), "MotorSetInverted(%d)", &index);
    if (index < NUM_MOTORS) {
      motorInverted[index] = !motorInverted[index];
      saveConfig();
      logToSD("MotorSetInverted(" + String(index) + ")");
    } else {
      Serial2.println(command); // Forward to next device
    }
  }
}

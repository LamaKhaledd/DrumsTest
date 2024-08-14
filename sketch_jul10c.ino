#include <Wire.h>
#include <SparkFun_BNO080_Arduino_Library.h>

#define DEBOUNCE_DELAY 50 
#define BUTTON1 6 
#define BUTTON2 7 

BNO080 imu1;
BNO080 imu2;

float yawOffset1 = 0;
float yawOffset2 = 0;

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;

int previousCommand = -1;
float normalizeYaw(float yaw) {
    yaw = fmod(yaw, 360.0); // Use floating point modulo to wrap yaw
    if (yaw < 0) {
        yaw += 360.0; // Ensure yaw is positive
    }
    return yaw;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Check if the IMU1 is connected properly
  if (!imu1.begin()) {
    Serial.println("IMU1 (BNO080) not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }
  Serial.println("IMU1 (BNO080) detected!");

  // Check if the IMU2 is connected properly
  if (!imu2.begin()) {
    Serial.println("IMU2 (BNO080) not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }
  Serial.println("IMU2 (BNO080) detected!");

  // Enable rotation vector and linear acceleration data for both IMUs
  imu1.enableRotationVector(10); // Rotation vector at 10ms interval
  imu1.enableLinearAccelerometer(10); // Linear acceleration at 10ms interval
  imu2.enableRotationVector(10); // Rotation vector at 10ms interval
  imu2.enableLinearAccelerometer(10); // Linear acceleration at 10ms interval

  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
}

void loop() {
  int command = -1;

  // Check for new data from IMU1
  if (imu1.dataAvailable()) {
    float quatI = imu1.getQuatI();
    float quatJ = imu1.getQuatJ();
    float quatK = imu1.getQuatK();
    float quatReal = imu1.getQuatReal();

    float yaw1 = atan2(2.0 * (quatReal * quatK + quatI * quatJ), 1.0 - 2.0 * (quatJ * quatJ + quatK * quatK)) * 180.0 / PI;
    float pitch1 = asin(2.0 * (quatReal * quatJ - quatK * quatI)) * 180.0 / PI;
    float roll1 = atan2(2.0 * (quatReal * quatI + quatJ * quatK), 1.0 - 2.0 * (quatI * quatI + quatJ * quatJ)) * 180.0 / PI;

    yaw1 = normalizeYaw(yaw1 - yawOffset1); // Adjust yaw by its offset and normalize

    float linAccelY = imu1.getLinAccelY();

    if (yaw1 >= 20 && yaw1 <= 120) {
      if (linAccelY < -2.5) {
        command = 0;
      }
    } else if (yaw1 >= 340 || yaw1 <= 20) {
      if (linAccelY < -2.5) {
        if (pitch1 > 50) {
          command = 5;
        } else {
          command = 3;
        }
      }
    } else if (yaw1 >= 305 && yaw1 <= 340) {
      if (linAccelY < -2.5) {
        if (pitch1 > 50) {
          command = 6;
        } else {
          command = 4;
        }
      }
    } else if (yaw1 >= 200 && yaw1 <= 305) {
      if (linAccelY < -2.5) {
        if (pitch1 > 30) {
          command = 6;
        } else {
          command = 7;
        }
      }
    }
  }

  if (imu2.dataAvailable()) {
    float quatI = imu2.getQuatI();
    float quatJ = imu2.getQuatJ();
    float quatK = imu2.getQuatK();
    float quatReal = imu2.getQuatReal();

    float yaw2 = atan2(2.0 * (quatReal * quatK + quatI * quatJ), 1.0 - 2.0 * (quatJ * quatJ + quatK * quatK)) * 180.0 / PI;
    float pitch2 = asin(2.0 * (quatReal * quatJ - quatK * quatI)) * 180.0 / PI;
    float roll2 = atan2(2.0 * (quatReal * quatI + quatJ * quatK), 1.0 - 2.0 * (quatI * quatI + quatJ * quatJ)) * 180.0 / PI;

    yaw2 = normalizeYaw(yaw2 - yawOffset2); 

    float linAccelY = imu2.getLinAccelY();

    if (yaw2 >= 350 || yaw2 <= 100) {
      if (linAccelY < -2.5) {
        if (pitch2 > 30) {
          command = 1;
        } else {
          command = 0;
        }
      }
    } else if (yaw2 >= 325 && yaw2 <= 350) {
      if (linAccelY < -2.5) {
        if (pitch2 > 50) {
          command = 5;
        } else {
          command = 3;
        }
      }
    } else if (yaw2 >= 300 && yaw2 <= 325) {
      if (linAccelY < -2.5) {
        if (pitch2 > 50) {
          command = 6;
        } else {
          command = 4;
        }
      }
    } else if (yaw2 >= 200 && yaw2 <= 300) {
      if (linAccelY < -2.5) {
        if (pitch2 > 30) {
          command = 6;
        } else {
          command = 7;
        }
      }
    }
  }

  if (command != -1 && command != previousCommand) {
    Serial.println(command);
    previousCommand = command;
  }

  if (digitalRead(BUTTON1) == HIGH) {
    if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {
      Serial.println("2");
      lastDebounceTime1 = millis();
    }
  }

  if (digitalRead(BUTTON2) == HIGH) {
    if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {
      yawOffset1 = imu1.getYaw();
      yawOffset2 = imu2.getYaw();
      lastDebounceTime2 = millis();
    }
  }

  delay(10);
}

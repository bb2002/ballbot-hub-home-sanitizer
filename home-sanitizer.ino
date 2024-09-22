#define DISTA_TRIG  2
#define DISTA_ECHO  3
#define LIGHT_ANLG  A0
#define RELAY_UVCL  7
#define BUZZER_WR   8

#define DOOR_CLOSE_RATE 4000
#define LIGHT_OFF_RATE  25

// 문을 닫고, 불을 끄면 몇초 뒤 살균 시작?
#define UVC_RUNNING_DELAY 30

// 살균을 몇 초 동안 수행?
#define UVC_STOP_DEALY    600


void setup() {
  Serial.begin(9600);

  // Setup Distance Sensor
  pinMode(DISTA_TRIG, OUTPUT);
  pinMode(DISTA_ECHO, INPUT);

  // Setup Relay
  pinMode(RELAY_UVCL, OUTPUT);
  digitalWrite(RELAY_UVCL, HIGH);
}

bool checkIsDoorClosed() {
  digitalWrite(DISTA_TRIG, HIGH);
  delay(10);
  digitalWrite(DISTA_TRIG, LOW);
  float distance = pulseIn(DISTA_ECHO, HIGH);

  return distance < DOOR_CLOSE_RATE;
}

bool checkIsLightOff() {
  int light = map(analogRead(A0), 0, 1023, 255, 0);
  return light < LIGHT_OFF_RATE;
}

void writeBuzzer() {
  tone(BUZZER_WR, 1000);
  delay(250);
  noTone(BUZZER_WR);
}

void loop() {
  digitalWrite(RELAY_UVCL, HIGH);

  bool isDoorClosed = checkIsDoorClosed();
  bool isLightOff = checkIsLightOff();

  if (isDoorClosed && isLightOff) {
    int uvcAfterStart = UVC_RUNNING_DELAY;
    while (uvcAfterStart >= 0) {
      delay(700);
      if (!checkIsDoorClosed() || !checkIsLightOff()) {
        return;
      }

      writeBuzzer();
      uvcAfterStart -= 1;
    }

    float uvcAfterStop = UVC_STOP_DEALY;
    while (uvcAfterStop >= 0) {
      digitalWrite(RELAY_UVCL, LOW);
      if (!checkIsDoorClosed()) {
        return;
      }

      delay(190);
      uvcAfterStop -= 0.2;
    }
    
    digitalWrite(RELAY_UVCL, HIGH);
    writeBuzzer();
    writeBuzzer();

    while (checkIsDoorClosed() && checkIsLightOff()) {
      // 살균 종료, 문이 열리거나 불이 켜질때 까지 대기
      delay(500);
    }
  }
}

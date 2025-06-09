#include <Servo.h>
#include <LiquidCrystal.h>

// LCD Pins
const int Rs = 12, en = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(Rs, en, D4, D5, D6, D7);

// Sensors & Servo
#define trig_in 6
#define echo_in 13
#define trig_out 7
#define echo_out 8
#define servopin 9
#define maxdistance 18

Servo myservo;
const int total_slots = 4;
int available_slots = 4;

void setup() {
  pinMode(trig_in, OUTPUT);
  pinMode(echo_in, INPUT);
  pinMode(trig_out, OUTPUT);
  pinMode(echo_out, INPUT);

  myservo.attach(servopin);
  lcd.begin(16, 2);
  displayDefaultScreen();
  myservo.write(0);
  Serial.begin(9600);
}

void loop() {
  float dist_in = readDistance(trig_in, echo_in);
  float dist_out = readDistance(trig_out, echo_out);

  // ====== دخول ======
  if (dist_in <= maxdistance) {
    // لو الجراج مليان
    if (available_slots == 0) {
      while (readDistance(trig_in, echo_in) <= maxdistance) {
        showTemporaryMessage("Parking Full");
        delay(500);
      }
      displayDefaultScreen();
      return;
    }

    // فيه أماكن → نفتح الباب ونستنى نشوف لو العربية دخلت
    openGate();
    delay(500);
    dist_out = readDistance(trig_out, echo_out);
    if (dist_out <= maxdistance) {
      available_slots--;
      showTemporaryMessage("Car entered");
    } else {
      showTemporaryMessage("No entry");
    }
    delay(2000);
    closeGate();
    displayDefaultScreen();
  }

  // ====== خروج ======
  else if (dist_out <= maxdistance) {
    // لو مفيش عربيات جوه
    if (available_slots == total_slots) {
      while (readDistance(trig_out, echo_out) <= maxdistance) {
        showTemporaryMessage("Garage Empty");
        delay(500);
      }
      displayDefaultScreen();
      return;
    }

    // فيه عربيات جوه → نفتح الباب ونشوف لو خرجت
    openGate();
    delay(500);
    dist_in = readDistance(trig_in, echo_in);
    if (dist_in <= maxdistance) {
      available_slots++;
      showTemporaryMessage("Car exited");
    } else {
      showTemporaryMessage("No exit");
    }
    delay(2000);
    closeGate();
    displayDefaultScreen();
  }

  delay(200);
}

// ========== Helper Functions ==========

float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration ? (duration * 0.0343) / 2 : maxdistance + 10;
}

void openGate() {
  myservo.write(90);
  delay(500);
}

void closeGate() {
  myservo.write(0);
  delay(500);
}

void displayDefaultScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Slots:");
  lcd.print(total_slots);
  lcd.setCursor(0, 1);
  lcd.print("Available:");
  lcd.print(available_slots);
}

void showTemporaryMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

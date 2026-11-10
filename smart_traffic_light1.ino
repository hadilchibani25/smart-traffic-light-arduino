

// =================================
// 1. DECLARATION DES VARIABLES
// =================================

// LEDs
int blueLED = 5;
int greenLED = 12;
int yellowLED = 8;
int redLED = 6;

// Button
int button = 2;

int buttonState = HIGH;
int lastButtonState = HIGH;

bool systemOn = false;

// Timing
unsigned long previousMillis = 0;
unsigned long lastDebounceTime = 0;
unsigned long previousSensorMillis = 0;

const unsigned long debounceDelay = 50;
const unsigned long sensorInterval = 300;

// Traffic Light states
int trafficState = 0;

// Ultrasonic Sensor
int trigPin = 10;
int echoPin = 11;

long duration;
float distance;

// Traffic Light durations
const unsigned long greenDuration = 5000;
const unsigned long greenVehicleDuration = 8000;
const unsigned long yellowDuration = 2000;
const unsigned long redDuration = 5000;


// =================================
// 2. CONFIGURATION
// =================================

void setup() {

  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  pinMode(button, INPUT_PULLUP);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

}


// =================================
// 3. PROGRAMME PRINCIPAL
// =================================

void loop() {

  // Lecture du bouton
  int reading = digitalRead(button);

  // Detection d'un changement
  if (reading != lastButtonState) {

    lastDebounceTime = millis();
    lastButtonState = reading;

  }

  // Debouncing
  if (millis() - lastDebounceTime >= debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      // Nouvelle pression
      if (buttonState == LOW) {

        systemOn = !systemOn;

        if (systemOn == true) {

          trafficState = 0;
          previousMillis = millis();

          Serial.println("SYSTEME ON");
          Serial.println("GREEN ON");

        }

        else {

          Serial.println("SYSTEME OFF");

        }

      }

    }

  }


  // =================================
  // 4. CAPTEUR DE DISTANCE
  // =================================

  unsigned long currentMillis = millis();

  if (currentMillis - previousSensorMillis >= sensorInterval) {

    previousSensorMillis = currentMillis;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 30000);

    if (duration > 0) {

      distance = duration * 0.034 / 2;

      Serial.print("Distance : ");
      Serial.print(distance);
      Serial.println(" cm");

    }

    else {

      distance = 0;
      Serial.println("Aucune mesure");

    }

  }


  // =================================
  // 5. DETECTION D'UNE VOITURE
  // =================================

  unsigned long activeGreenDuration = greenDuration;

  if (distance > 0 && distance < 20) {

    activeGreenDuration = greenVehicleDuration;

  }


  // =================================
  // 6. GESTION DU TEMPS
  // =================================

  if (systemOn == true) {

    if (trafficState == 0 &&
        currentMillis - previousMillis >= activeGreenDuration) {

      trafficState = 1;
      previousMillis = currentMillis;

      Serial.println("YELLOW ON");

    }

    else if (trafficState == 1 &&
             currentMillis - previousMillis >= yellowDuration) {

      trafficState = 2;
      previousMillis = currentMillis;

      Serial.println("RED ON");

    }

    else if (trafficState == 2 &&
             currentMillis - previousMillis >= redDuration) {

      trafficState = 0;
      previousMillis = currentMillis;

      Serial.println("GREEN ON");

    }

  }


  // =================================
  // 7. COMMANDE DES LEDs
  // =================================

  digitalWrite(blueLED, systemOn);

  digitalWrite(greenLED, systemOn && trafficState == 0);

  digitalWrite(yellowLED, systemOn && trafficState == 1);

  digitalWrite(redLED, systemOn && trafficState == 2);

}

#include <PS4Controller.h>

// Definisi Motor
#define STOP 0
#define CW 1
#define CCW 2
#define MOTOR_1 0
#define MOTOR_2 1

// PIN
#define RPWM_1 4
#define LPWM_1 2
#define RPWM_2 14
#define LPWM_2 12
#define LED_ACT 21

// KECEPATAN
#define MAX_SPEED 255
#define NORMAL_SPEED 150
#define ATTACK_SPEED 200
#define EDGE_ESCAPE_SPEED 180
#define TURN_SPEED 120
#define ANALOG_MAX 127 // Maksimal nilai dari analog stick (range -127 hingga 127)

// Variabel untuk kecepatan bertahap (hanya untuk thumb stick)
short currentGradualSpeed = 0; // Kecepatan motor yang sedang diterapkan saat ini untuk mode gradual
int targetGradualSpeed = 0;    // Target kecepatan yang ingin dicapai secara bertahap

const int SPEED_INCREMENT = 5;  // Penambahan kecepatan per iterasi (semakin kecil semakin halus)
const int SPEED_DECREMENT = 10; // Pengurangan kecepatan saat berhenti atau arah berubah

// Variabel untuk menyimpan status arah motor saat ini
unsigned short motorStatus_1 = STOP;
unsigned short motorStatus_2 = STOP;

void motorGo(uint8_t motor, uint8_t direction, uint8_t speed) {
  if (motor == MOTOR_1) {
    if (direction == CW) {
      analogWrite(LPWM_1, speed);
      analogWrite(RPWM_1, 0);
    } else if (direction == CCW) {
      analogWrite(LPWM_1, 0);
      analogWrite(RPWM_1, speed);
    } else {
      analogWrite(LPWM_1, 0);
      analogWrite(RPWM_1, 0);
    }
  } else if (motor == MOTOR_2) {
    if (direction == CW) {
      analogWrite(LPWM_2, speed);
      analogWrite(RPWM_2, 0);
    } else if (direction == CCW) {
      analogWrite(LPWM_2, 0);
      analogWrite(RPWM_2, speed);
    } else {
      analogWrite(LPWM_2, 0);
      analogWrite(RPWM_2, 0);
    }
  }
}

// --- Fungsi untuk Mengatur Kecepatan Gradual (Hanya Dipanggil oleh Thumb Stick) ---
void updateGradualSpeed() {
  if (currentGradualSpeed < targetGradualSpeed) {
    currentGradualSpeed += SPEED_INCREMENT;
    if (currentGradualSpeed > targetGradualSpeed) {
      currentGradualSpeed = targetGradualSpeed;
    }
  } else if (currentGradualSpeed > targetGradualSpeed) {
    currentGradualSpeed -= SPEED_DECREMENT;
    if (currentGradualSpeed < 0) {
      currentGradualSpeed = 0;
    }
  }
}

// --- Fungsi Pergerakan untuk Thumb Stick (Menggunakan Gradual Speed) ---
void gradualForward() {
  motorStatus_1 = CCW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
}

void gradualReverse() {
  motorStatus_1 = CW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
}

void gradualLeft() {
  motorStatus_1 = CCW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed / 2);
}

void gradualRight() {
  motorStatus_1 = CW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed / 2);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
}

void gradualRotateLeft() {
  motorStatus_1 = CCW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
}

void gradualRotateRight() {
  motorStatus_1 = CW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
  motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
}

// --- Fungsi Pergerakan Langsung (Tanpa Gradual Speed) ---
// Digunakan untuk D-Pad, L1, R1, dan tombol lainnya yang tidak gradual
void directForward(int speed) {
  motorStatus_1 = CCW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, speed);
  motorGo(MOTOR_2, motorStatus_2, speed);
}

void directReverse(int speed) {
  motorStatus_1 = CW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, speed);
  motorGo(MOTOR_2, motorStatus_2, speed);
}

void directLeft(int speed) {
  motorStatus_1 = CCW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, speed);
  motorGo(MOTOR_2, motorStatus_2, speed / 2);
}

void directRight(int speed) {
  motorStatus_1 = CW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, speed / 2);
  motorGo(MOTOR_2, motorStatus_2, speed);
}

void directRotateLeft(int speed) {
  motorStatus_1 = CCW;
  motorStatus_2 = CW;
  motorGo(MOTOR_1, motorStatus_1, speed);
  motorGo(MOTOR_2, motorStatus_2, speed);
}

void directRotateRight(int speed) {
  motorStatus_1 = CW;
  motorStatus_2 = CCW;
  motorGo(MOTOR_1, motorStatus_1, speed);
  motorGo(MOTOR_2, motorStatus_2, speed);
}

// --- Fungsi Stop Universal ---
// Ini akan menghentikan motor secara gradual jika sebelumnya bergerak dengan gradual,
// atau langsung berhenti jika sedang bergerak direct.
void StopMotors() {
  // Jika sedang dalam mode gradual (targetGradualSpeed > 0 atau currentGradualSpeed > 0),
  // maka proses deselerasi gradual.
  if (targetGradualSpeed > 0 || currentGradualSpeed > 0) {
    targetGradualSpeed = 0; // Set target ke 0
    updateGradualSpeed(); // Panggil update untuk melambatkan
    motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
    motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
    if (currentGradualSpeed == 0) { // Pastikan benar-benar mati jika sudah 0
      motorGo(MOTOR_1, STOP, 0);
      motorGo(MOTOR_2, STOP, 0);
    }
  } else {
    // Jika tidak dalam mode gradual, langsung berhenti
    motorGo(MOTOR_1, STOP, 0);
    motorGo(MOTOR_2, STOP, 0);
  }
}

void setup() {
  Serial.begin(115200);
  PS4.begin("a0:dd:6c:0f:05:52"); // Ganti dengan MAC address PS4 controller Anda
  pinMode(LED_ACT, OUTPUT);
  StopMotors(); // Pastikan motor berhenti saat startup
}

void loop() {
  if (PS4.isConnected()) {
    digitalWrite(LED_ACT, HIGH);

    int leftStickX = PS4.LStick.x();
    int leftStickY = PS4.LStick.y();

    int joystickThreshold = 10;
    bool stickActive = false; // Flag khusus untuk menandai apakah thumb stick aktif
    bool anyControlActive = false; // Flag untuk menandai apakah ada kontrol yang sedang aktif (dari mana pun)

    // --- Kontrol Thumb Stick (dengan Gradual Speed) ---
    if (abs(leftStickY) > joystickThreshold) { // Maju/Mundur
      targetGradualSpeed = map(abs(leftStickY), joystickThreshold, ANALOG_MAX, NORMAL_SPEED, MAX_SPEED);
      updateGradualSpeed(); // Perbarui kecepatan gradual
      if (leftStickY > 0) {
        gradualForward();
      } else {
        gradualReverse();
      }
      stickActive = true;
      anyControlActive = true;
    } else if (abs(leftStickX) > joystickThreshold) { // Belok Kiri/Kanan
      targetGradualSpeed = map(abs(leftStickX), joystickThreshold, ANALOG_MAX, TURN_SPEED, MAX_SPEED);
      updateGradualSpeed(); // Perbarui kecepatan gradual
      if (leftStickX > 0) {
        gradualRight();
      } else {
        gradualLeft();
      }
      stickActive = true;
      anyControlActive = true;
    }

    // Jika thumb stick baru saja dilepas atau tidak aktif, pastikan robot melambat
    if (!stickActive && currentGradualSpeed > 0) {
      targetGradualSpeed = 0; // Target kecepatan gradual menjadi 0
      updateGradualSpeed(); // Lakukan proses melambat
      // Terus panggil motorGo dengan currentGradualSpeed yang berkurang
      motorGo(MOTOR_1, motorStatus_1, currentGradualSpeed);
      motorGo(MOTOR_2, motorStatus_2, currentGradualSpeed);
      if (currentGradualSpeed == 0) { // Jika sudah mencapai 0, benar-benar set ke STOP
        motorGo(MOTOR_1, STOP, 0);
        motorGo(MOTOR_2, STOP, 0);
      }
      anyControlActive = true; // Anggap ini sebagai aksi melambat
    }

    // --- Kontrol D-Pad (tanpa Gradual Speed) ---
    // Hanya aktif jika thumb stick TIDAK aktif (tidak digerakkan dan tidak dalam proses melambat)
    if (!anyControlActive) {
      if (PS4.Up()) {
        directForward(NORMAL_SPEED);
        anyControlActive = true;
      } else if (PS4.Down()) {
        directReverse(NORMAL_SPEED);
        anyControlActive = true;
      } else if (PS4.Left()) { // D-Pad Left
        directLeft(NORMAL_SPEED); // Contoh kecepatan untuk D-Pad belok
        anyControlActive = true;
      } else if (PS4.Right()) { // D-Pad Right
        directRight(NORMAL_SPEED); // Contoh kecepatan untuk D-Pad belok
        anyControlActive = true;
      }
    }

    // --- Kontrol L1/R1 (tanpa Gradual Speed) ---
    // Hanya aktif jika thumb stick dan D-Pad TIDAK aktif
    if (!anyControlActive) {
      if (PS4.L1()) {
        directRotateLeft(MAX_SPEED); // Rotasi cepat langsung
        anyControlActive = true;
      } else if (PS4.R1()) {
        directRotateRight(MAX_SPEED); // Rotasi cepat langsung
        anyControlActive = true;
      }
    }

    // --- Kontrol Tombol Lain (Cross, Circle, Square, Triangle - tanpa Gradual Speed) ---
    // Hanya aktif jika thumb stick, D-Pad, L1/R1 TIDAK aktif
    if (!anyControlActive) {
      if (PS4.Cross()) {
        directForward(ATTACK_SPEED);
        anyControlActive = true;
      } else if (PS4.Circle()) {
        directForward(NORMAL_SPEED);
        anyControlActive = true;
      } else if (PS4.Square()) {
        directRotateRight(TURN_SPEED);
        anyControlActive = true;
      } else if (PS4.Triangle()) {
        directReverse(NORMAL_SPEED);
        anyControlActive = true;
      }
    }

    // --- Jika Tidak Ada Kontrol yang Aktif, Hentikan Motor ---
    // Ini juga akan melambatkan jika sebelumnya bergerak dengan gradual
    if (!anyControlActive) {
      StopMotors();
    }

  } else {
    // Jika koneksi terputus, hentikan motor dan matikan LED
    digitalWrite(LED_ACT, LOW);
    StopMotors();
  }

  delay(10);
}
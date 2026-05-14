#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>

/* ===== CONFIGURATION ===== */
const char* ssid = "";
const char* password = "";
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

#define RELAY_PIN 18
#define SAKLAR_PIN 21

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/* ===== GLOBAL STATE ===== */
bool lampuStatus = false;
bool lastScheduleState = false;
String startTime = "17:45";
String endTime   = "03:30";

/* ===== UTILITY FUNCTIONS ===== */
String waktuLengkap() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  if (!t || t->tm_year < 120) return "--";
  const char* hari[] = {"Minggu","Senin","Selasa","Rabu","Kamis","Jumat","Sabtu"};
  char buf[64];
  sprintf(buf,"%s, %02d/%02d/%04d %02d:%02d WIB",
          hari[t->tm_wday], t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
          t->tm_hour, t->tm_min);
  return String(buf);
}

bool inSchedule(int h, int m) {
  int now = h * 60 + m;
  int s = startTime.substring(0,2).toInt() * 60 + startTime.substring(3,5).toInt();
  int e = endTime.substring(0,2).toInt() * 60 + endTime.substring(3,5).toInt();
  if (s <= e) return now >= s && now < e;
  return now >= s || now < e;
}

/* ===== CORE CONTROL FUNCTION ===== */
void setLamp(bool on, String mode) {
  // STEP 1: PRIORITAS HARDWARE (LANGSUNG EKSEKUSI)
  lampuStatus = on;
  digitalWrite(RELAY_PIN, on ? HIGH : LOW); 
  Serial.printf(">>> ACTION: %s [%s]\n", on ? "ON" : "OFF", mode.c_str());

  // STEP 2: JEDA UNTUK STABILITAS LISTRIK
  delay(50); 

  // STEP 3: SYNC CLOUD (ATOMIC UPDATE)
  if (Firebase.ready()) {
    FirebaseJson data;
    data.set("status", lampuStatus);
    data.set("mode", mode);
    data.set("time", waktuLengkap());
    
    FirebaseJson event;
    event.set("mode", mode);
    event.set("text", lampuStatus ? "Light ON" : "Light OFF");
    data.set("last_event", event);

    // Update sekaligus agar Termux tidak kirim notif double
    Firebase.setJSON(fbdo, "/lampu", data);
    
    // Kirim Log History
    FirebaseJson log;
    log.set("event", lampuStatus ? "Light ON" : "Light OFF");
    log.set("mode", mode);
    log.set("time", waktuLengkap());
    Firebase.pushJSON(fbdo, "/logs", log);
  }
}

void checkPhysicalSwitch() {
  static bool lastBtnState = HIGH;
  static unsigned long lastBtnTime = 0;
  bool currentBtnReading = digitalRead(SAKLAR_PIN);

  if (currentBtnReading != lastBtnState) {
    lastBtnTime = millis();
  }

  if ((millis() - lastBtnTime) > 500) {
    static bool confirmedBtnState = HIGH;
    if (currentBtnReading != confirmedBtnState) {
      confirmedBtnState = currentBtnReading;
      if (confirmedBtnState == LOW) { // Saklar ditekan
        setLamp(!lampuStatus, "MANUAL_SWITCH");
      }
    }
  }
  lastBtnState = currentBtnReading;
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Start awal mati
  pinMode(SAKLAR_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  
  // Timeout 15 detik agar sistem tidak macet jika WiFi mati
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 30) { 
    delay(500); Serial.print("."); 
    timeout++;
  }

  configTime(7 * 3600, 0, "pool.ntp.org", "id.pool.ntp.org");
  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Ambil data awal dari Firebase
  if (Firebase.getString(fbdo, "/settings/startTime")) startTime = fbdo.stringData();
  if (Firebase.getString(fbdo, "/settings/endTime")) endTime = fbdo.stringData();
  if (Firebase.getBool(fbdo, "/lampu/status")) {
    lampuStatus = fbdo.boolData();
    digitalWrite(RELAY_PIN, lampuStatus ? HIGH : LOW);
  }

  Firebase.setString(fbdo, "/command/value", "NONE");
  Serial.println("\n--- SYSTEM ONLINE ---");
}

void loop() {
  // Selalu cek saklar fisik (Prioritas Utama)
  checkPhysicalSwitch();

  if (!Firebase.ready()) return;

  // 1. Sync Jadwal (Setiap 60 detik)
  static unsigned long lastSync = 0;
  if (millis() - lastSync > 60000) {
    if (Firebase.getString(fbdo, "/settings/startTime")) startTime = fbdo.stringData();
    if (Firebase.getString(fbdo, "/settings/endTime")) endTime = fbdo.stringData();
    lastSync = millis();
  }

  // 2. Cek Command Aplikasi (Setiap 1 detik)
  static unsigned long lastCmdCheck = 0;
  if (millis() - lastCmdCheck > 1000) {
    if (Firebase.getString(fbdo, "/command/value")) {
      String cmd = fbdo.stringData();
      if (cmd != "NONE") {
        Firebase.setString(fbdo, "/command/value", "NONE");
        if (cmd == "ON" && !lampuStatus) setLamp(true, "MANUAL_APP");
        else if (cmd == "OFF" && lampuStatus) setLamp(false, "MANUAL_APP");
      }
    }
    lastCmdCheck = millis();
  }

  // 3. Logika Jadwal Otomatis
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  if (t && t->tm_year >= 120) {
    bool currSchedule = inSchedule(t->tm_hour, t->tm_min);
    if (currSchedule != lastScheduleState) {
      lastScheduleState = currSchedule;
      setLamp(currSchedule, "AUTO");
    }
  }
  
  delay(50); 
}

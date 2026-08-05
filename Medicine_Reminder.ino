#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

//==========================
// WiFi Credentials
//==========================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

//==========================
// Telegram Configuration
//==========================
const char* BOT_TOKEN = "8671933731:AAHskMjNnANB4tD37Ii1ZAJ4Jfp8XZDnNQo";
const char* CHAT_ID = "7172814043";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

//==========================
// NTP Configuration
//==========================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;      // India = UTC +5:30
const int daylightOffset_sec = 0;

//==========================
// LCD
//==========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

//==========================
// Preferences
//==========================
Preferences preferences;
//==========================
// Medicine Reminder Times
//==========================

String dose1Time;
String dose2Time;
String dose3Time;
//==========================
// Reminder Variables
//==========================
bool reminderActive = false;
bool doseTaken = false;
String currentDose = "";

int lastReminderMinute = -1;
//==========================
// Missed Dose Variables
//==========================
unsigned long reminderStartMillis = 0;
const unsigned long REMINDER_TIMEOUT = 120000;   // 2 minutes
//==========================
// Medicine Statistics
//==========================
int totalTaken = 0;
int totalMissed = 0;
//==========================
// Weekly Report Variables
//==========================
bool weeklyReportSent = false;

//==========================
// Pin Definitions
//==========================
#define BUZZER_PIN         26
#define RED_LED            25
#define GREEN_LED          33

#define MEDICINE_BUTTON    27
#define SOS_BUTTON         32

//==========================
// Function Prototypes
//==========================
void connectWiFi();
void initTime();
void displayClock();
void loadReminderTimes();
void printReminderTimes();
void checkMedicineReminder();
void startReminder(String doseName);
void stopReminder();
void checkMissedDose();
void sendTelegramMessage(String message);
void loadStatistics();
void saveStatistics();
void checkWeeklyReport();
void checkSOSButton();
//==========================
// Setup
//==========================
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(MEDICINE_BUTTON, INPUT_PULLUP);
  pinMode(SOS_BUTTON, INPUT_PULLUP);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Medicine");
  lcd.setCursor(0,1);
  lcd.print("Reminder");

  delay(2000);

  connectWiFi();
  initTime();
  loadReminderTimes();
  loadStatistics();
  printReminderTimes();
}

//==========================
// Loop
//==========================
  void loop()
{
  displayClock();

  checkMedicineReminder();
  checkMissedDose();
  checkSOSButton();
  checkWeeklyReport();

  if (reminderActive)
  {
    digitalWrite(RED_LED, HIGH);

    digitalWrite(BUZZER_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_PIN, LOW);
    delay(250);

    if (digitalRead(MEDICINE_BUTTON) == LOW)
    {
      stopReminder();
    }
  }
  else
  {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

//==========================
// WiFi Connection
//==========================
void connectWiFi()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  secured_client.setInsecure();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  delay(1500);
}

//==========================
// Initialize NTP
//==========================
void initTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;

  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Waiting for NTP...");
    delay(1000);
  }

  Serial.println("Time Synchronized");
}

//==========================
// Display Time
//==========================
void displayClock()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
    return;

  char line1[17];
  char line2[17];

  strftime(line1, sizeof(line1), "%d/%m/%Y", &timeinfo);
  strftime(line2, sizeof(line2), "%H:%M:%S", &timeinfo);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(line1);

  lcd.setCursor(0,1);
  lcd.print(line2);

  Serial.println(line2);
}
void loadReminderTimes()
{
  preferences.begin("medicine", false);

  dose1Time = preferences.getString("dose1", "");
  dose2Time = preferences.getString("dose2", "");
  dose3Time = preferences.getString("dose3", "");

  if (dose1Time == "")
  {
    dose1Time = "09:00";
    dose2Time = "13:00";
    dose3Time = "20:00";

    preferences.putString("dose1", dose1Time);
    preferences.putString("dose2", dose2Time);
    preferences.putString("dose3", dose3Time);

    Serial.println("Default reminder times saved.");
  }

  preferences.end();
}

void printReminderTimes()
{
  Serial.println();
  Serial.println("========== Medicine Schedule ==========");

  Serial.print("Dose 1 : ");
  Serial.println(dose1Time);

  Serial.print("Dose 2 : ");
  Serial.println(dose2Time);

  Serial.print("Dose 3 : ");
  Serial.println(dose3Time);

  Serial.println("=======================================");
}
//==========================
// Load Statistics
//==========================
void loadStatistics()
{
  preferences.begin("medicine", false);

  totalTaken = preferences.getInt("taken", 0);
  totalMissed = preferences.getInt("missed", 0);

  preferences.end();

  Serial.println();
  Serial.println("===== Statistics =====");
  Serial.print("Taken : ");
  Serial.println(totalTaken);

  Serial.print("Missed: ");
  Serial.println(totalMissed);
  Serial.println("======================");
}

//==========================
// Save Statistics
//==========================
void saveStatistics()
{
  preferences.begin("medicine", false);

  preferences.putInt("taken", totalTaken);
  preferences.putInt("missed", totalMissed);

  preferences.end();
}
//==========================
// Check Reminder Time
//==========================
void checkMedicineReminder()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
    return;

  char currentTime[6];

  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);

  if (timeinfo.tm_min == lastReminderMinute)
    return;

  if (String(currentTime) == dose1Time)
  {
    startReminder("Dose 1");
    lastReminderMinute = timeinfo.tm_min;
  }

  else if (String(currentTime) == dose2Time)
  {
    startReminder("Dose 2");
    lastReminderMinute = timeinfo.tm_min;
  }

  else if (String(currentTime) == dose3Time)
  {
    startReminder("Dose 3");
    lastReminderMinute = timeinfo.tm_min;
  }
}

//==========================
// Start Reminder
//==========================
void startReminder(String doseName)
{
  reminderActive = true;
  doseTaken = false;
  currentDose = doseName;
  reminderStartMillis = millis();

  Serial.println();
  Serial.println("************************");
  Serial.print("Medicine Time : ");
  Serial.println(doseName);
  Serial.println("************************");
  sendTelegramMessage(
  "💊 Medicine Reminder\n\n"
  "Please take your medicine.\n"
  "Current Dose: " + doseName
);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Take Medicine");

  lcd.setCursor(0,1);
  lcd.print(doseName);
}

//==========================
// Stop Reminder
//==========================
void stopReminder()
{
  reminderActive = false;
  doseTaken = true;

  totalTaken++;
  saveStatistics();

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);

  Serial.print(currentDose);
  Serial.println(" Taken");
  sendTelegramMessage(
  "✅ Medicine Taken\n\n" +
  currentDose +
  " has been taken successfully."
);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Medicine");
  lcd.setCursor(0,1);
  lcd.print("Taken");

  delay(2000);
}
//==========================
// Check Missed Dose
//==========================
void checkMissedDose()
{
  if (!reminderActive)
    return;

  if (millis() - reminderStartMillis >= REMINDER_TIMEOUT)
  {
    reminderActive = false;

    totalMissed++;
    saveStatistics();

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RED_LED, LOW);

    Serial.println();
    Serial.print(currentDose);
    Serial.println(" MISSED");
    sendTelegramMessage(
  "❌ Dose Missed\n\n" +
  currentDose +
  " was not taken within the allowed time."
);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dose Missed");

    lcd.setCursor(0, 1);
    lcd.print(currentDose);

    delay(3000);
  }
}
//==========================
// Check SOS Button
//==========================
void checkSOSButton()
{
  static bool lastButtonState = HIGH;

  bool currentButtonState = digitalRead(SOS_BUTTON);

  if (lastButtonState == HIGH && currentButtonState == LOW)
  {
    Serial.println("SOS Button Pressed");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SOS ALERT");

    lcd.setCursor(0, 1);
    lcd.print("Sending Msg");

    sendTelegramMessage(
      "🆘 EMERGENCY ALERT\n\n"
      "The SOS button was pressed.\n"
      "Please check the patient immediately."
    );

    delay(1000);
  }

  lastButtonState = currentButtonState;
}
//==========================
// Send Telegram Message
//==========================
void sendTelegramMessage(String message)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    bot.sendMessage(CHAT_ID, message, "");
  }
}
//==========================
// Weekly Telegram Report
//==========================
void checkWeeklyReport()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
    return;

  // Sunday = 0
  if (timeinfo.tm_wday == 0 &&
      timeinfo.tm_hour == 20 &&
      timeinfo.tm_min == 0 &&
      !weeklyReportSent)
  {
    int total = totalTaken + totalMissed;

    float percentage = 0;

    if (total > 0)
      percentage = (totalTaken * 100.0) / total;

    String report =
      "📊 WEEKLY MEDICINE REPORT\n\n";

    report += "✅ Taken : ";
    report += String(totalTaken);

    report += "\n❌ Missed : ";
    report += String(totalMissed);

    report += "\n📅 Total : ";
    report += String(total);

    report += "\n📈 Success : ";
    report += String(percentage,1);
    report += "%";

    sendTelegramMessage(report);

    totalTaken = 0;
    totalMissed = 0;

    saveStatistics();

    weeklyReportSent = true;

    Serial.println("Weekly Report Sent");
  }

  if (timeinfo.tm_wday != 0)
  {
    weeklyReportSent = false;
  }
}
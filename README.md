# IoT-Based Smart Medicine Reminder System using ESP32

## 📖 Overview

The IoT-Based Smart Medicine Reminder System is designed to help patients take their medicines on time. It uses an ESP32 microcontroller with Wi-Fi connectivity, NTP time synchronization, and Telegram Bot notifications to provide real-time medicine reminders and emergency alerts.

This project improves medication adherence by reminding patients at scheduled times, detecting missed doses, and notifying caregivers remotely.

## ✨ Features

- 💊 Three medicine reminders per day
- 🌐 Wi-Fi connectivity
- 🕒 NTP time synchronization (No RTC required)
- 📟 16×2 I2C LCD display
- 🔔 Buzzer reminder
- 🔴 Red LED reminder indication
- 🟢 Green LED medicine taken indication
- 🔘 Medicine confirmation button
- 🆘 SOS emergency button
- 📩 Telegram notifications
- ❌ Missed dose detection
- 💾 Flash memory storage using Preferences
- 📊 Weekly medicine report

## 🛠 Hardware Components

- ESP32 DevKit V1
- 16×2 I2C LCD Display
- Active Buzzer
- Red LED
- Green LED
- Push Button (Medicine)
- Push Button (SOS)
- Wi-Fi Connection

## 💻 Software Used

- Arduino IDE
- ESP32 Board Package
- Telegram Bot API
- NTP Server
- ESP32 Preferences Library

## ⚙️ Working

1. ESP32 connects to Wi-Fi.
2. The current time is synchronized using the NTP server.
3. The LCD displays the current date and time.
4. At the scheduled medicine time:
   - LCD shows the reminder.
   - Buzzer sounds.
   - Red LED turns ON.
   - Telegram reminder is sent.
5. After taking the medicine, the user presses the medicine button.
6. Green LED turns ON and a confirmation message is sent through Telegram.
7. If the medicine is not taken within the timeout period, the system marks it as a missed dose and sends a missed-dose alert.
8. Pressing the SOS button sends an emergency notification to the caregiver.
9. Every week, the ESP32 sends a weekly medicine report.

## 📂 Project Structure

Final-IoT-Project/
│── Medicine_Reminder.ino
│── README.md
│── Circuit_Diagram.png
│── Block_Diagram.png
│── Project_Report.pdf
│── images/
│── screenshots/

## 🚀 Applications

- Home Healthcare
- Hospitals
- Clinics
- Elderly Care
- Patient Monitoring

## 🔮 Future Enhancements

- Mobile Application
- AI-Based Medicine Prediction
- Voice Assistant
- Smartwatch Notifications
- Cloud Dashboard
- Automatic Pill Dispenser

## 👩‍💻 Author

**Rubadharshini S**

Electronics and Communication Engineering (ECE)

Arjun College of Technology

Tamil Nadu, India

## 📜 License

This project is developed for educational and academic purposes.

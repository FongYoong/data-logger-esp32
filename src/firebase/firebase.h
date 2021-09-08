#ifndef FIREBASE_H
#define FIREBASE_H

#include "Arduino.h"
#include <FirebaseESP32.h>

// Firebase Login Credentials
#define USER_EMAIL "logger@logger.com"
#define USER_PASSWORD "logger"
#define API_KEY "AIzaSyCx6udX1tu3Fxr-jKnYHR4_KN-dL_utugU"
#define DATABASE_URL "https://data-logger-abda2-default-rtdb.asia-southeast1.firebasedatabase.app"

extern FirebaseAuth auth; // The user UID can be obtained from auth.token.uid
extern FirebaseConfig config; // FirebaseConfig data for config data

#define UPDATE_FIREBASE_CONFIG_INTERVAL 2000 // Milliseconds // Interval between updating pending config changes
extern bool pendingConfigFirebaseUpdate;
extern bool demoMode;
extern bool enableLogging;
extern unsigned long logInterval;
extern float temperatureLimit;

void firebaseSetup();
void addLogtoFirebase(float temperatureValue, unsigned long custom_timestamp = 0);
bool updateConfigFirebase();
void configStreamCallback(StreamData data);
void configStreamTimeoutCallback(bool timeout);

#endif
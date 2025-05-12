// Arduino Timezone Library Copyright (C) 2018 by Jack Christensen and
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino Timezone Library example sketch.
// Self-adjusting clock for one time zone.
// TimeChangeRules can be hard-coded or read from EEPROM, see comments.
// Jack Christensen Mar 2012

#include <Timezone.h> // https://github.com/JChristensen/Timezone

#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwmH = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwmM = Adafruit_PWMServoDriver(0x40);

int servoFrequency = 50;                                                                                                                                                                                                                                    // Set servo operating frequency
int segmentHOn[14] = {385, 375, 385, 375, 382, 375, 354, 367, 375, 385, 375, 368, 371, 375};                                                                                                                                                                // On positions for each Hour servo
int segmentMOn[14] = {382, 395, 378, 315, 375, 340, 345, 380, 385, 365, 290, 365, 315, 365};                                                                                                                                                                // On positions for each Minute servo
int segmentHOff[14] = {200, 200, 550, 480, 200, 520, 200, 200, 200, 480, 550, 200, 515, 200};                                                                                                                                                               // Off positions for each Hour servo
int segmentMOff[14] = {200, 200, 550, 440, 200, 480, 200, 200, 200, 550, 450, 200, 430, 200};                                                                                                                                                               // Off positions for each Minute servo
int digits[10][7] = {{1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 1}, {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1}, {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 0, 1, 1}}; // Position values for each digit

int midOffset = 100; // Amount by which adjacent segments to the middle move away when required

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240}; // Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};  // Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

// If TimeChangeRules are already stored in EEPROM, comment out the three
// lines above and uncomment the line below.
// Timezone myTZ(100);       // assumes rules stored at EEPROM address 100

TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev

int hourTens = 0;
int hourUnits = 0;
int minuteTens = 0;
int minuteUnits = 0;
int horas = 0;
int minutos = 0;
int prevHourTens = 0;
int prevHourUnits = 0;
int prevMinuteTens = 0;
int prevMinuteUnits = 0;

void updateMid() // Function to move the middle segements and adjacent ones out of the way
{
    if (digits[minuteUnits][6] != digits[prevMinuteUnits][6]) // Move adjacent segments for Minute units
    {
        if (digits[prevMinuteUnits][1] == 1)
            pwmM.setPWM(1, 0, segmentMOn[1] - midOffset);
        if (digits[prevMinuteUnits][6] == 1)
            pwmM.setPWM(5, 0, segmentMOn[5] + midOffset);
    }
    delay(100);                      // Delay allows adjacent segments to move before moving middle
    if (digits[minuteUnits][6] == 1) // Move Minute units middle segment if required
        pwmM.setPWM(6, 0, segmentMOn[6]);
    else
        pwmM.setPWM(6, 0, segmentMOff[6]);
    if (digits[minuteTens][6] != digits[prevMinuteTens][6]) // Move adjacent segments for Minute tens
    {
        if (digits[prevMinuteTens][1] == 1)
            pwmM.setPWM(8, 0, segmentMOn[8] - midOffset);
        if (digits[prevMinuteTens][6] == 1)
            pwmM.setPWM(12, 0, segmentMOn[12] + midOffset);
    }
    delay(100);                     // Delay allows adjacent segments to move before moving middle
    if (digits[minuteTens][6] == 1) // Move Minute tens middle segment if required
        pwmM.setPWM(13, 0, segmentMOn[13]);
    else
        pwmM.setPWM(13, 0, segmentMOff[13]);
    if (digits[hourUnits][6] != digits[prevHourUnits][6]) // Move adjacent segments for Hour units
    {
        if (digits[prevHourUnits][1] == 1)
            pwmH.setPWM(1, 0, segmentHOn[1] - midOffset);
        if (digits[prevHourUnits][6] == 1)
            pwmH.setPWM(5, 0, segmentHOn[5] + midOffset);
    }
    delay(100);                    // Delay allows adjacent segments to move before moving middle
    if (digits[hourUnits][6] == 1) // Move Hour units middle segment if required
        pwmH.setPWM(6, 0, segmentHOn[6]);
    else
        pwmH.setPWM(6, 0, segmentHOff[6]);
    if (digits[hourTens][6] != digits[prevHourTens][6]) // Move adjacent segments for Hour tens
    {
        if (digits[prevHourTens][1] == 1)
            pwmH.setPWM(8, 0, segmentHOn[8] - midOffset);
        if (digits[prevHourTens][6] == 1)
            pwmH.setPWM(12, 0, segmentHOn[12] + midOffset);
    }
    delay(100);                   // Delay allows adjacent segments to move before moving middle
    if (digits[hourTens][6] == 1) // Move Hour tens middle segment if required
        pwmH.setPWM(13, 0, segmentHOn[13]);
    else
        pwmH.setPWM(13, 0, segmentHOff[13]);
}

void updateDisplay() // Function to update the displayed time
{
    updateMid();                 // Move the segments out of the way of the middle segment and then move the middle segments
    for (int i = 0; i <= 5; i++) // Move the remaining segments
    {
        if (digits[hourTens][i] == 1) // Update the hour tens
            pwmH.setPWM(i + 7, 0, segmentHOn[i + 7]);
        else
            pwmH.setPWM(i + 7, 0, segmentHOff[i + 7]);
        delay(10);
        if (digits[hourUnits][i] == 1) // Update the hour units
            pwmH.setPWM(i, 0, segmentHOn[i]);
        else
            pwmH.setPWM(i, 0, segmentHOff[i]);
        delay(10);
        if (digits[minuteTens][i] == 1) // Update the minute tens
            pwmM.setPWM(i + 7, 0, segmentMOn[i + 7]);
        else
            pwmM.setPWM(i + 7, 0, segmentMOff[i + 7]);
        delay(10);
        if (digits[minuteUnits][i] == 1) // Update the minute units
            pwmM.setPWM(i, 0, segmentMOn[i]);
        else
            pwmM.setPWM(i, 0, segmentMOff[i]);
        delay(10);
    }
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
    char buf[32];
    char m[4]; // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
            hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.println(buf);
}

// Function to return the compile date and time as a time_t value
time_t compileTime()
{
    const time_t FUDGE(10); // fudge factor to allow for compile time (seconds, YMMV)
    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[4], *m;
    tmElements_t tm;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    time_t t = makeTime(tm);
    return t + FUDGE; // add fudge factor to allow for compile time
}

void setup()
{
    Serial.begin(115200);
    setTime(myTZ.toUTC(compileTime()));
    // setTime(01, 55, 00, 11, 3, 2012);        //another way to set the time (hr,min,sec,day,mnth,yr)

    pwmH.begin(); // Start each board
    pwmM.begin();
    pwmH.setOscillatorFrequency(27000000); // Set the PWM oscillator frequency, used for fine calibration
    pwmM.setOscillatorFrequency(27000000);
    pwmH.setPWMFreq(servoFrequency); // Set the servo operating frequency
    pwmM.setPWMFreq(servoFrequency);
    // myRTC.setDS1302Time(00, 10, 16, 5, 8, 4, 2020);        //Only required once to reset the clock time
    for (int i = 0; i <= 13; i++) // Set all of the servos to on or up (88:88 displayed)
    {
        pwmH.setPWM(i, 0, segmentHOn[i]);
        delay(10);
        pwmM.setPWM(i, 0, segmentMOn[i]);
        delay(10);
    }
    delay(2000);
}

void loop()
{
    time_t utc = now();
    time_t local = myTZ.toLocal(utc, &tcr);
    Serial.println();
    hourTens = hour(local) / 10;
    hourUnits = hour(local) % 10;
    minuteTens = minute(local) / 10;
    minuteUnits = minute(local) % 10;
    Serial.print("Decenas de hora: ");
    Serial.println(hourTens);
    Serial.print("Unidades de hora: ");
    Serial.println(hourUnits);
    Serial.print("Decenas de minuto: ");
    Serial.println(minuteTens);
    Serial.print("Unidades de minuto: ");
    Serial.println(minuteUnits);
    if (minuteUnits != prevMinuteTens)
    {
        updateDisplay(); // Update the display if the time has changed
        prevHourTens = hourTens;
        prevHourUnits = hourUnits;
        prevMinuteTens = minuteTens;
        prevMinuteUnits = minuteUnits;
    }
    // printDateTime(utc, "UTC");
    // printDateTime(local, tcr -> abbrev);
}

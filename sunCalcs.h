#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <time.h>
#include <string>

using namespace std;


//====== Begin Function Prototypes ====================================================================================

bool IsNight(double lat, double lng, tm* dateTime, double timezone);

double FractionalYear(tm* dateTime); //returns how far we are through the datetime as a decimal
double FractionalYear(); //returns how far we are through the CURRENT year as a decimal

int DayOfYear(int, int, int); //returns day of year for the GIVEN date
int DayOfYear(); //returns day of year for the CURRENT date

bool IsLeapYear(); //returns if the CURRENT year is a leap year
bool IsLeapYear(int year); //returns if the given year is a leap year

tm* CurrentDateTime(); //Returns current date time object
int CurrentYear(); //Returns the current 4-digit year

double EqTime(tm* dateTime); //Returns the "Equation of Time"
double SolarDeclination(tm* dateTime); //Returns the Solar Declination
double TimeOffset(tm* dateTime, double longitude, double timezone);
double TrueSolarTime(tm* dateTime, double timeOffset, double timezone);
double SolarHourAngle(tm* dateTime, double lng, double timezone);
double SolarHourAngle_SunriseSunset(double lattitude, tm* dateTime); //Gives the solar hour angle at the given lattitude 
double Sunrise(double lat, double lon, tm* dateTime, int timezone);
double Sunset(double lat, double lon, tm* dateTime, int timezone);
double SolarNoon(double lon, tm* dateTime, int timezone);

string min_to_time(double min); //takes the time of day in minutes and returns a 24-hour time string


//====== End Function Prototypes ====================================================================================


bool IsNight(double lat, double lng, tm* dateTime, double timezone) {
	double time_min = dateTime->tm_hour * 60 + dateTime->tm_min + (double)dateTime->tm_sec / 60; //current time of day in minutes
	double sunrise_min = Sunrise(lat, lng, dateTime, timezone);
	double sunset_min = Sunset(lat, lng, dateTime, timezone);

	return (time_min < sunrise_min || time_min > sunset_min);
}

double FractionalYear() {
	return FractionalYear(CurrentDateTime());
}

double FractionalYear(tm* dateTime) {
	bool isLeapYear = IsLeapYear(dateTime->tm_year + 1900);
	int dayOfYear = dateTime->tm_yday + 1;
	int hour = dateTime->tm_hour;

	if (isLeapYear) {
		return (M_PI * 2 / 366) * (dayOfYear - 1 + ((hour - 12) / 24));
	}
	else {
		return (M_PI * 2 / 365) * (dayOfYear - 1 + ((hour - 12) / 24));
	}
}

double EqTime(tm* dateTime) {
	double fracYear = FractionalYear(dateTime);
	
	return 229.18 * (
			0.000075 + 
			0.001868 * cos(fracYear) -
			0.032077 * sin(fracYear) -
			0.014615 * cos(2 * fracYear) -
			0.040849 * sin(2 * fracYear)
		);
}

double SolarDeclination(tm* dateTime) {
	double fracYear = FractionalYear(dateTime);

	return 0.006918 - 
	0.399912 * cos(fracYear) + 
	0.070257 * sin(fracYear) - 
	0.006758*cos(2 * fracYear) + 
	0.000907 * sin(2 * fracYear) - 
	0.002697 * cos(3 * fracYear) + 
	0.00148 * sin (3 * fracYear);
}

double TimeOffset(tm* dateTime, double lng, double timezone) {
	return EqTime(dateTime) + (4 * lng) - (60 * timezone);
}

double TrueSolarTime(tm* dateTime, double lng, double timezone) {
	double fracYear = FractionalYear(dateTime);
	int hour = dateTime->tm_hour;
	int minute = dateTime->tm_min;
	int second = dateTime->tm_sec;
	double timeOffset = TimeOffset(dateTime, lng, timezone);

	return (hour * 60) + (minute) + (second / 60) + timeOffset;
}

double SolarHourAngle(tm* dateTime, double lng, double timezone) {
	double trueSolarTime = TrueSolarTime(dateTime, lng, timezone);
	return (trueSolarTime / 4) - 180;
}

double SolarHourAngle_SunriseSunset(double lat, tm* dateTime) {
	double a = 90.833 * M_PI / 180; //zenith angle of sunrise/sunset, adjusting for atmospheric refraction
	double lat_rad = lat * M_PI / 180;
	double decl = SolarDeclination(dateTime);
	return acos((cos(a)/(cos(lat_rad) * cos(decl))) - (tan(lat_rad) * tan(decl)));
}

double Sunrise(double lat, double lng, tm* dateTime, int timezone) {
	double fracYear = FractionalYear(dateTime);
	double ha = SolarHourAngle_SunriseSunset(lat, dateTime) * 180 / M_PI;
	double eqT = EqTime(dateTime);
	return 720 - 4 * (lng + ha) - eqT + (timezone * 60);
}

double Sunset(double lat, double lng, tm* dateTime, int timezone) {
	double fracYear = FractionalYear(dateTime);
	double ha = SolarHourAngle_SunriseSunset(lat, dateTime) * 180 / M_PI;
	double eqT = EqTime(dateTime);
	return 720 - 4 * (lng - ha) - eqT + (timezone * 60);
}

double SolarNoon(double lng, tm* dateTime, int timezone) {
	double eqTime = EqTime(dateTime);
	return 720 - 4 * lng - eqTime + (timezone * 60);
}

int DayOfYear() {
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);

	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
	int year = aTime->tm_year + 1900; // Year is # years since 1900

	return DayOfYear(day, month, year);
}


//Returns if the current year is leap year
bool IsLeapYear() {
	return IsLeapYear(CurrentYear());
}

//Returns if the given year is leap year
bool IsLeapYear(int year) {
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}
 
// Function to return the day number of the year for the given date
int DayOfYear(int day, int month, int year)
{ 
	int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // If current year is a leap year and the date
    // given is after the 28th of February then
    // it must include the 29th February
    if (month > 2 && year % 4 == 0
        && (year % 100 != 0 || year % 400 == 0)) {
        ++day;
    }
 
    // Add the days in the previous months
    while (month-- > 1) {
        day = day + days[month];
    }
    return day;
}

int CurrentYear() {
	return CurrentDateTime()->tm_year;
}

tm* CurrentDateTime() {
	time_t rawtime;
	time ( &rawtime );
	return localtime ( &rawtime );
}

string min_to_time(double full_min) {
	int hr = full_min / 60;
	int min = full_min - (hr * 60);
	int sec = (full_min - (int)full_min) * 60;

	string hr_str = hr < 10 ? "0" + to_string(hr) : to_string(hr);
	string min_str = min < 10 ? "0" + to_string(min) : to_string(min);
	string sec_str = sec < 10 ? "0" + to_string(sec) : to_string(sec);

	return hr_str + ":" + min_str + ":" + sec_str;
}
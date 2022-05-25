#include "sunCalcs.h"

using namespace std;

int main() {
	double lat = 44;
	double lng = -93;
	double tz = -5;

	tm* time_local = CurrentDateTime();
	double time_minutes = time_local->tm_hour * 60 + time_local->tm_min + (double)time_local->tm_sec / 60; //local time in minutes
	
	cout << "Sunrise: " << min_to_time(Sunrise(lat, lng, time_local, tz)) << endl;
	cout << "Solar Noon: " << min_to_time(SolarNoon(lng, time_local, tz)) << endl;
	cout << "Sunset: " << min_to_time(Sunset(lat, lng, time_local, tz)) << endl;
	cout << "Current Time: " << min_to_time(time_minutes) << endl;
	cout << "Is Night? " << (IsNight(lat, lng, time_local, tz) ? "True" : "False") << endl;

    return 0;
}
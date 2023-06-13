/** @file logger.cpp
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @brief Vehicle Data logger functions
 */

#include "logger.h"
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include "dlgps.h"
#include "loggermqtt.h"

#if SENSEHAT == 1
#include "sensehat.h"
SenseHat Sh;
#endif

// Global Objects

/** @brief Initialize data logger, currently prints header
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @param void
 *  @return int
 */
int DlInitialization(void) {
	// fprintf(stdout, "\nData Logger Initialization\n");
  return 1;
}

/** @brief Gets the serial number from the raspi from /proc/cpuinfo
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @param void
 *  @return uint64_t 'serial'
 */
uint64_t DlGetSerial(void) {
  static uint64_t serial = 0;
  FILE *fp;
  char buf[SYSINFOBUFSZ];
  char searchstring[] = SEARCHSTR;
  fp = fopen("/proc/cpuinfo", "r");
  if (fp != NULL) {
    while (fgets(buf, sizeof(buf), fp) != NULL) {
      if (!strncasecmp(searchstring, buf, strlen(searchstring))) {
        sscanf(buf + strlen(searchstring), "%Lx", &serial);
      }
    }
    fclose(fp);
  }
  if (serial == 0) {
    system("uname -a");
    system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
    fp = fopen("stamp.txt", "r");
    if (fp != NULL) {
      while (fgets(buf, sizeof(buf), fp) != NULL) {
        sscanf(buf, "%Lx", &serial);
      }
      fclose(fp);
    }
  }
  return serial;
}

/** @brief Prints the logger info to the console
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @param struct reading_s
 *  @return void
 */
void DlDisplayLoggerReadings(reading_s dreads) {
  fprintf(stdout, "\nUnit:%lu \t", DlGetSerial());
  fprintf(stdout, " %s", ctime(&dreads.rtime));
  fprintf(stdout, "T: %0.1fC \t", dreads.temperature);
  fprintf(stdout, "H: %0.0f%% \t", dreads.humidity);
  fprintf(stdout, "P: %0.1f KPa\n", dreads.pressure);
  fprintf(stdout, "Xa: %f g\t", dreads.xa);
  fprintf(stdout, "Ya: %f g\t", dreads.ya);
  fprintf(stdout, "Za: %f g\n", dreads.za);
	fprintf(stdout, "Pitch: %f \t", dreads.pitch);
	fprintf(stdout, "Roll: %f \t", dreads.roll);
	fprintf(stdout, "Yaw: %f \t \n", dreads.yaw);
	fprintf(stdout, "Xm: %f \t", dreads.xm);
	fprintf(stdout, "Ym: %f \t", dreads.ym);
	fprintf(stdout, "Zm: %f \t \n", dreads.zm);
  fprintf(stdout, "Latitude: %f \t", dreads.latitude);
  fprintf(stdout, "Longitude: %f \t", dreads.longitude);
  fprintf(stdout, "Altitude: %f \n", dreads.altitude);
  fprintf(stdout, "Speed: %f \t", dreads.speed);
  fprintf(stdout, "Heading: %f \n", dreads.heading);
}

/** @brief Gets the reading results from the system, and adds items (system
 * time, currently default data)
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @param void
 *  @return struct reading_s
 */
reading_s DlGetLoggerReadings(void) {
  reading_s creads;
	loc_t gpsdata;
	gpsdata = {0};
  creads.rtime = time(NULL);
#if SENSEHAT == 1
  usleep(IMUDELAY);
  creads.temperature = Sh.GetTemperature();
  creads.humidity = Sh.GetHumidity();
  creads.pressure = Sh.GetPressure();
  Sh.GetAcceleration(creads.xa, creads.ya, creads.za);
  Sh.GetOrientation(creads.pitch, creads.roll, creads.yaw);
  Sh.GetMagnetism(creads.xm, creads.ym, creads.zm);
#else
  creads.temperature = DTEMP;
  creads.humidity = DHUMID;
	creads.pressure = DPRESS;
  creads.xa = DXA;
  creads.ya = DYA;
  creads.za = DZA;
  creads.pitch = DPITCH;
  creads.roll = DROLL;
  creads.yaw = DYAW;
  creads.xm = DXM;
  creads.ym = DYM;
	creads.zm = DZM;
#endif
#if GPSDEVICE == 1
	DlGpsInit();
	gpsdata = DlGpsLocation();
	creads.latitude = gpsdata.latitude;
	creads.longitude = gpsdata.longitude;
	creads.altitude = gpsdata.altitude;
	creads.heading = gpsdata.course;
	creads.speed = gpsdata.speed;
#else
  creads.latitude = DLAT;
  creads.longitude = DLONG;
  creads.altitude = DALT;
  creads.speed = DSPEED;
  creads.heading = DHEADING;
#endif
  return creads;
}

/** @brief Saves logger data to a .csv and a .json
 *  @author Robert Miller
 *  @date 23Jan2022
 *  @param struct reading_s creads
 *  @return int
 */
int DlSaveLoggerData(reading_s creads) {
	FILE *fp;
	char ltime[TIMESTRSZ];
	char jsondata[PAYLOADSTRSZ];
	fp = fopen("loggerdata.csv", "a");
	if (fp == NULL) {
		return 0;
	}
	strncpy(ltime, ctime(&creads.rtime), TIMESTRSZ);
	ltime[3] = ',';
	ltime[7] = ',';
  ltime[10] = ',';
  ltime[19] = ',';
	fprintf(fp, "%.24s,%3.1f,%3.0f,%3.1f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", ltime, creads.temperature, creads.humidity, creads.pressure, creads.xa, creads.ya, creads.za, creads.pitch, creads.yaw, creads.roll, creads.xm, creads.ym, creads.zm, creads.latitude, creads.longitude, creads.altitude, creads.speed, creads.heading);
	fclose(fp);
	sprintf(jsondata, "{\"temperature\":%-3.1f,\"humidity\":%-3.0f,\"pressure\":%-3.1f,\"xa\":%-f,\"ya\":%-f,\"za\":%-f,\
\"pitch\":%-f,\"roll\":%-f,\"yaw\":%-f,\"xm\":%-f,\"ym\":%-f,\"zm\":%-f,\"latitude\":%-f,\"longitude\":%-f,\"altitude\":%-f,\"speed\":%-f,\"heading\":%-f,\"active\": true}", creads.temperature, creads.humidity, creads.pressure, creads.pitch, creads.yaw, creads.roll, creads.xm, creads.ym, creads.zm, creads.latitude, creads.longitude, creads.altitude, creads.speed, creads.heading);
	fp = fopen("loggerdata.json", "a");
	if (fp == NULL) {
		return -1;
	}
	fprintf(fp, "%s", jsondata);
	fclose(fp);

	int rc = DlPublishLoggerData(jsondata);

  return rc;
}

/** @brief Displays the Humber logo on the SenseHat Screen
 *  @author Robert Miller
 *  @date 06Feb2022
 *  @param void
 *  @return void
 */
void DlDisplayLogo(void) {
	uint16_t logo[8][8] = {	HB,HB,HB,HB,HB,HB,HB,HB,
			HB,HB,HW,HB,HB,HW,HB,HY,
			HB,HB,HW,HB,HB,HW,HY,HY,
			HB,HB,HW,HB,HB,HW,HY,HY,
			HB,HB,HW,HW,HW,HW,HY,HY,
			HB,HB,HW,HY,HY,HW,HY,HY,
			HB,HY,HW,HY,HY,HW,HY,HY,
			HY,HY,HY,HY,HY,HY,HY,HY,
	};
	Sh.WipeScreen();
	Sh.ViewPattern(logo);
}

/** @brief Updates a group of yellow pixels on the sensehat screen
 *  @author Robert Miller
 *  @date 06Feb2022
 *  @param float xa, float ya from readings struct
 *  @return void
 */
void DlUpdateLevel(float xa, float ya) {
	int x, y;
	Sh.WipeScreen();
	y = (int) (xa * -30.0 + 4);
    x = (int) (ya * -30.0 + 4);
	// constrain between 0 -> 6
	if (x < 0) {
		x = 0;
	} else if (x > 6) {
		x = 6;
	}
	if (y < 0) {
		y = 0;
	} else if (y > 6) {
		y = 6;
	}
    Sh.LightPixel(x, y, HY);
    Sh.LightPixel(x+1, y, HY);
    Sh.LightPixel(x, y+1, HY);
    Sh.LightPixel(x+1, y+1, HY);
}

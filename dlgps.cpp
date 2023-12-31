/** @file dlgps.cpp
 *  @brief Data logger gps Functions
 */
#include "dlgps.h"
#include "nmea.h"
#include "serial.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <gps.h>
#include <unistd.h>

#if SIMGPS
// Global GPS Data File Pointer
FILE * fpgps = NULL;
#endif

/** @brief Initializes GPS Module
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param None
 *  @return void
 */
extern void DlGpsInit(void)
{
#if SIMGPS
    fpgps = fopen("gpstestdata.txt","r");
    if(fpgps == NULL) { fprintf(stdout,"Unable to open gps test data file\n"); }
#else
	// Serial GPS device or GPSD server
	serial_init();
	serial_config();
#endif
}

/** @brief Turns on GPS Module
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param None
 *  @return void
 */
extern void DlGpsOn(void)
{
    //Write on
}

/** @brief Compute the GPS location using decimal scale
 *  @author Robert Miller, from lab code
 *  @date 4APR2022
 *  @param void
 *  @return coord loc_t data structure
 */
loc_t DlGpsLocation(void)
{
    loc_t cloc = {0.0};
    char buffer[GPSDATASZ] = {0};
#if GPSDSERVER
	struct gps_data_t gps_data;

	if ((gps_open("localhost", "2947", &gps_data)) == -1)
	{
		fprintf(stdout,"code: %d, reason: %s\n", errno, gps_errstr(errno));
		return cloc;
	}
	(void)gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
    // Wait for data available.
    while (gps_waiting(&gps_data, 5000000))
    {
        // will not block because we know data is available.
        if (-1 == gps_read(&gps_data))
        {
            printf("Read error.  Bye, bye\n");
            break;
        }
        if (MODE_SET != (MODE_SET & gps_data.set))
        {
            // did not even get mode, nothing to see here
            continue;
        }
        break;

    }
    if (std::isfinite(gps_data.fix.latitude) &&
        std::isfinite( gps_data.fix.longitude))
    {
		cloc.latitude = gps_data.fix.latitude;
		cloc.longitude = gps_data.fix.longitude;
		cloc.altitude = gps_data.fix.altitude;
		cloc.speed = gps_data.fix.speed;
		cloc.course = gps_data.fix.track;
		cloc.utc = gps_data.fix.time;
    }
#else
   uint8_t status = _EMPTY;
    gpgga_t gpgga;
    gprmc_t gprmc;

    while(status != _COMPLETED)
    {
#if SIMGPS
        fgets(buffer,NMEAMSGSZ,fpgps);
        if(feof(fpgps)) { rewind(fpgps); }
#else
        serial_readln(buffer,GPSDATASZ);
#endif
        switch (nmea_get_message_type(buffer))
	{
            case NMEA_GPGGA:
                nmea_parse_gpgga(buffer, &gpgga);
				cloc.utc = gpgga.utc;
                DlGpsConvertDegToDec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);
                cloc.latitude = gpgga.latitude;
                cloc.longitude = gpgga.longitude;
                cloc.altitude = gpgga.altitude;
				status |= NMEA_GPGGA;
				break;
            case NMEA_GPRMC:
                nmea_parse_gprmc(buffer, &gprmc);
				cloc.speed = gprmc.speed;
                cloc.course = gprmc.course;
				cloc.date = gprmc.date;
                status |= NMEA_GPRMC;
 				break;
        }
    }
#endif
    return cloc;
}


/** @brief Turns Off GPS Module
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param None
 *  @return void
 */
extern void DlGpsOff(void)
{
#if SIMGPS
	fclose(fpgps);
#endif
//    serial_close();
}

/** @brief Convert lat e lon to decimals (from deg)
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param latitude double *
 *  @param ns char
 *  @param longitude double *
 *  @param we char
 *  @return void
 */
void DlGpsConvertDegToDec(double *latitude, char ns,  double *longitude, char we)
{
    double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
    double lon = (we == 'E') ? *longitude : -1 * (*longitude);

    *latitude = DlGpsDegDec(lat);
    *longitude = DlGpsDegDec(lon);
}

/** @brief Convert GPS points to decimal
 *  @author Paul Moggach
 *  @date 25MAR2019
 *  @param deg_point double
 *  @return double
 */
double DlGpsDegDec(double deg_point)
{
    double ddeg;
    double sec = modf(deg_point, &ddeg)*60;
    int deg = (int)(ddeg/100);
    int min = (int)(deg_point-(deg*100));

    double absmlat = round(min * 1000000.);
    double absslat = round(sec * 1000000.);
    double absdlat = round(deg * 1000000.);

    return round(absdlat + (absmlat/60) + (absslat/3600)) /1000000;
}

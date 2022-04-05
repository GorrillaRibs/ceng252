/** @file vdl.cpp Serial: 1a2b3c4d
 *  @author Robert Miller
 *  @date 16Jan2022
 *  @brief Vehicle Data Logger main function
 */
#include <unistd.h>
#include "vdl.h"
#include "logger.h"
#include "stdafx.h"
#include <ofArduino.h>
#include "dlfirmata.h"

ArduinoFirmata ard;

/** @brief Vehicle Data Logger main function
 *  @author Robert Miller
 *  @date 30Mar2022
 *  @param void
 *  @return int program status
 */
int main(void)
{
  int tc = 0;
  reading_s reads = {0};
  DlInitialization();
	DlDisplayLogo();
	sleep(3);
  if (!ard.connect("ttyACM0", 57600)) { // Serial Port and Baud Rate
    fprintf(stdout, "\nFailed to connect to arduino!");
    return -1;
  }
  // Need to do this to init the pins, get the firmware version, and  call
  // setupArduino. Will stay in update loop looking for signal. When it arrives
  // Setup will be called and we can start processing.
  ard.sendReset();
  ard.sendProtocolVersionRequest();
  ard.sendFirmwareVersionRequest();

  ard.setupArduino(1);

  while (1) {
		ard.update();
		ard.sendDigital(WHITELED, 1);
    reads = DlGetLoggerReadings();
		if (ard.getDigital(LSWITCH) == 1) {
			ard.sendDigital(BEEPER, 1);
		} else {
			ard.sendDigital(REDLED, 1);
		}
    usleep(SLEEPTIME);
    ard.sendDigital(WHITELED, 0);
    ard.sendDigital(REDLED, 0);
    ard.sendDigital(BEEPER, 0);
    DlUpdateLevel(reads.xa, reads.ya);
		if (tc == LOGCOUNT) {
			DlDisplayLoggerReadings(reads);
			DlSaveLoggerData(reads);
			tc =  0;
    } else {
			usleep(SLEEPTIME);
			tc++;
		}
  }
}

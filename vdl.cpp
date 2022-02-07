/** @file vdl.cpp Serial: 1a2b3c4d
 *  @author Robert Miller
 *  @date 16Jan2022
 *  @brief Vehicle Data Logger main function
 */
#include <unistd.h>
#include "vdl.h"
#include "logger.h"

/** @brief Vehicle Data Logger main function
 *  @author Robert Miller
 *  @date 16Jan2022
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
  while (1) {
    reads = DlGetLoggerReadings();
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

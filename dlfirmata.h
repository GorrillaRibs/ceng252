#ifndef DLFIRMATA_H_INCLUDED
#define DLFIRMATA_H_INCLUDED
/** @file dlfirmata.h
 *  @brief Firmata interface constants, structures, function prototypes
 *  @author Paul Moggach
 *  @date 31DEC2020
 */

#include <ofArduino.h>

// Constants
#define TC_ID 1
#define CF_ID 2
#define GRIP_ID 3
#define LSWITCH 3   // Arduino digital pin 3
#define BEEPER 5    // Arduino digital pin 5
#define CSERVO 6    // Arduino digital pin 6
#define WHITELED 7  // Arduino digital pin 7
#define REDLED  8   // Arduino digital pin 8
#define JOYX 0      // Arduino analog pin 0
#define JOYY 1      // Arduino analog pin 1

class ArduinoFirmata:public ofArduino
{
protected:
	boost::signals2::connection m_EInitializedConnection;
	boost::signals2::connection m_EDigitalPinChanged;
	boost::signals2::connection m_EAnalogPinChanged;

	void digitalPinChanged(const int & pinNum);
	void analogPinChanged(const int & pinNum);

public:
	ArduinoFirmata(void);
	virtual ~ArduinoFirmata(void);

	bool m_bSetupArduino;

	void setupArduino(const int & version);
};

#endif // #ifndef DLFIRMATA_H_INCLUDED

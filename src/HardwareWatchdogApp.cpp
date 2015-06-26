#include "cinder/app/AppBasic.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 0.25

// We'll create a new Cinder Application by deriving from the BasicApp class
class SerialApp : public AppBasic {
public:
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void setup();
	void keyDown(KeyEvent event);

	Serial serial;
	string text;

	double lastSentTime;
	double timeout;
	bool init;
	bool isAck;
};

void SerialApp::setup()
{
	// print the devices
	const vector<Serial::Device> &devices(Serial::getDevices());
	for (vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt) {
		console() << "Device: " << deviceIt->getName() << endl;
	}

	try {
		Serial::Device dev = Serial::findDeviceByNameContains("Arduino");
		serial = Serial(dev, 9600);
	}
	catch (...) {
		try{
			Serial::Device dev = Serial::findDeviceByNameContains("Serial");
			serial = Serial(dev, 9600);
		}
		catch (...){
			console() << "There was an error initializing the serial device!" << std::endl;
			exit(-1);
		}
		
	}
	timeout = 2.0;
	init = true;
}

void SerialApp::update()
{
	if (init){
		serial.writeByte('S');
		lastSentTime = getElapsedSeconds() - timeout;
		init = false;
		isAck = false;
	}
	else {
		if (getElapsedSeconds() - lastSentTime > timeout) {
			console() << isAck << endl;
			if (!isAck){
				if (serial.getNumBytesAvailable() > 0){
					if (serial.readChar() == 'A'){
						serial.writeByte('O');
						isAck = true;
						serial.flush();
					}
					else{
						serial.writeByte('S');
					}
				}
			}
			else{
				serial.writeByte('A');
			}
			lastSentTime = getElapsedSeconds();
		}
	}
}

void SerialApp::keyDown(KeyEvent event){

}


void SerialApp::draw()
{
	gl::clear(ColorA(0, 0, 0));
	gl::drawStringCentered(text, getWindowCenter(), ColorA(1, 1, 1), Font("TAHOMA", 100.0f));
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(SerialApp, RendererGl)

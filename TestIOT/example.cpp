#include <iostream>
using namespace std;
#include "mraa.hpp"


using namespace mraa;


int main(void) {
	cout << "Hello IOT2000." << endl;

	Gpio* d_pin = NULL;
	d_pin = new mraa::Gpio(13, true, false);
	d_pin->dir(mraa::DIR_OUT);

	for (;;) {
			d_pin->write(1);
			sleep(1);
			d_pin->write(0);
			sleep(1);
	}
	return 0;
}


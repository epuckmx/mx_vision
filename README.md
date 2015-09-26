## mx vision library for e-puck

Just make sure to NOT define MX_DEV when compiling the e-puck source code.  The macro MX_DEV is used for desktop development.

If you want to run the tests in a desktop just run

    $ gcc -D MX_DEV -o test main.c mx_vision.c
    $ ./test

### Usage

Import ```mx_vision.h``` into your code. Call ```mx_vision_init()``` after ```e_init_port()```. Then, in your main loop call ```mx_vision_see()``` to perform a seach using the data received in the camera. Example:

    #include "e_init_port.h"
    #include "mx_vision.h"

    int main(void) {
    	e_init_port();
    	mx_vision_init();
    	while (1) {
    		mx_vision_see();
    		// redsDetected = number of red objects, available in red struct
    		// greensDetected = number of green objects, available in green struct
    		// bluesDetected = number of blue objects, available in blue struct
    	}
    	return 0;
    }

Variables ```redsDetected```, ```greensDetected``` and ```bluesDetected``` contains respectively the amount of red, green and blue objects detected (up to 1 currently). You can access the distance and direction of the objects detected using the variables ```red```, ```green``` and ```blue``` which are of type Object:

    struct Object {
    	char dis; // absolute distance in cm
    	char dir; // relative direction in sexageximal degrees using left hand rule
    };

Refer to ```mx_vision.h``` for more information.
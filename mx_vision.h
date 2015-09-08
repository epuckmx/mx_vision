#ifndef MX_VISION_H
#define	MX_VISION_H

#include <stdbool.h>

#define WIDTH 40 // max width 640
#define HEIGHT 40 // max height 480
#define ZOOM 8 // 1, 2, 4, 8 or 16
#define BUFFER_SIZE (WIDTH * HEIGHT * 2) // we are working on RGB_565
#define PIXELS (WIDTH * HEIGHT) // pixels in camera
#define RED_THRESHOLD 40 // used for binary filter
#define GREEN_THRESHOLD 40 // used for binary filter
#define BLUE_THRESHOLD 40 // used for binary filter
#define MIN_WIDTH 5
#define MIN_HEIGHT 5

struct Object {
	char x;
	char y;
	char w;
	char h;
};

extern struct Object redObject;
extern struct Object greenObject;
extern struct Object blueObject;
extern bool redDetected;
extern bool greenDetected;
extern bool blueDetected;

#ifdef MX_DEV
	void init(int test);
#else
	void init(void);
	void initRand(void);
#endif
void see(void);

#endif
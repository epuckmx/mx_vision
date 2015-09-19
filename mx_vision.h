#ifndef MX_VISION_H
#define	MX_VISION_H

#define WIDTH 40 // max width 640
#define HEIGHT 40 // max height 480
#define ZOOM 8 // 1, 2, 4, 8 or 16
#define BUFFER_SIZE (WIDTH * HEIGHT * 2) // we are working on RGB_565
#define PIXELS (WIDTH * HEIGHT) // pixels in camera
#define RED_THRESHOLD 40 // used for binary filter (0 - 255)
#define GREEN_THRESHOLD 40 // used for binary filter (0 - 255)
#define BLUE_THRESHOLD 40 // used for binary filter (0 - 255)
#define MIN_WIDTH 5
#define MIN_HEIGHT 5

struct Object {
	char dis;
	char dir;
};

extern struct Object red;
extern struct Object green;
extern struct Object blue;
extern char redsDetected;
extern char greensDetected;
extern char bluesDetected;

#ifdef MX_DEV
	void mx_init(int test);
#else
	void mx_init(void);
	void mx_initRand(void);
#endif
void mx_see(void);

#endif
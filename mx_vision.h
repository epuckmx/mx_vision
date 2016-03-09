#ifndef MX_VISION_H
#define	MX_VISION_H

#define WIDTH 40 // max width 640
#define HEIGHT 40 // max height 480
#define ZOOM 8 // 1, 2, 4, 8 or 16
#define BUFFER_SIZE (WIDTH * HEIGHT * 2) // we are working on RGB_565
#define PIXELS (WIDTH * HEIGHT) // pixels in camera
#define RED_THRESHOLD 30 // used for binary filter (0 - 255)
#define GREEN_THRESHOLD 30 // used for binary filter (0 - 255)
#define BLUE_THRESHOLD 30 // used for binary filter (0 - 255)
#define MIN_WIDTH 3
#define MIN_HEIGHT 3
#define MAX_OBJECTS 4
#define RATIO_THRESHOLD 1.1
#define DIR_DELTA 3
#define DIS_DELTA 3
#define SINGLE 0
#define MULTIPLE 1
#define BLUE_MODE SINGLE

struct Object {
    int dis;
    int dir;
    int w;
    int h;
};

extern struct Object reds[MAX_OBJECTS];
extern struct Object blues[MAX_OBJECTS];
extern struct Object greens[MAX_OBJECTS];

extern int redsDetected;
extern int greensDetected;
extern int bluesDetected;

void mx_vision_init(void);
#ifdef MX_DEV
    void mx_vision_set(unsigned char *);
    void mx_vision_see(unsigned char *);
#else
    void mx_vision_see(void);
#endif

#endif

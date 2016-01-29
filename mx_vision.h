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
#define MAX_OBJECTS 10
#define RATIO_THRESHOLD 1.1

struct Object {
    int dis;
    int dir;
};

extern struct Object red;
extern struct Object green;
extern struct Object blue;

extern struct Object reds[MAX_OBJECTS];

extern int redsDetected;
extern int greensDetected;
extern int bluesDetected;

void mx_vision_init(void);
#ifdef MX_DEV
    void getColorR(int x, int y, unsigned char *r);
    void setColorR(int x, int y, unsigned char r);
    void getColorG(int x, int y, unsigned char *g);
    void getColorB(int x, int y, unsigned char *b);
    void mx_vision_set(unsigned char *image);
    void mx_vision_see(unsigned char *image);
#else
    void mx_vision_see(void);
#endif

#endif

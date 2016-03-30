#include "mx_vision.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef MX_DEV
    #include <stdio.h>
#else
    #include "e_poxxxx.h"
#endif

unsigned char buffer[BUFFER_SIZE];

struct Rect {
    int x;
    int y;
    int w;
    int h;
};

struct Object reds[MAX_OBJECTS];
struct Object blues[MAX_OBJECTS];

struct Object red;
struct Object green;
struct Object blue;

int redsDetected = 0;
int greensDetected = 0;
int bluesDetected = 0;

int prevRedsDetected = 0;
int prevBluesDetected = 0;

struct Object redsPrediction[4][MAX_OBJECTS];

unsigned char backup_ra[HEIGHT];
unsigned char backup_rb[HEIGHT];
unsigned char backup_ga[HEIGHT];
unsigned char backup_gb[HEIGHT];
unsigned char backup_ba[HEIGHT];
unsigned char backup_bb[HEIGHT];

// utils

int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

// R

void getColorR(int x, int y, unsigned char *r) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    *r = h & 0xF8;
}

void setColorR(int x, int y , unsigned char r) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (h & 0x07);
}

// G

void getColorG(int x, int y, unsigned char *g) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
}

void setColorG(int x, int y, unsigned char g) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    buffer[2 * (WIDTH * y + x)] = (h & 0xF8) | ((g >> 5));
    buffer[2 * (WIDTH * y + x) + 1] = ((g << 3) & 0xE0) | (l & 0x1F);
}

// B

void getColorB(int x, int y, unsigned char *b) {
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    *b = (l & 0x1F) << 3;
}

void setColorB(int x, int y, unsigned char b) {
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    buffer[2 * (WIDTH * y + x) + 1] = (l & 0xE0) | (b >> 3);
}

// RG

void getColorsRG(int x, int y, unsigned char* r, unsigned char* g) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
}

void setColorsRG(int x, int y, unsigned char r, unsigned char g) {
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (g >> 5);
    unsigned char byte = buffer[2 * (WIDTH * y + x) + 1] & 0x1F;
    buffer[2 * (WIDTH * y + x) + 1] = (g << 5) | byte;
}

// RGB

void getColorsRGB(int x, int y, unsigned char *r, unsigned char *g, unsigned char *b) {
    unsigned char h = buffer[2 * (WIDTH * y + x)];
    unsigned char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
    *b = (l & 0x1F) << 3;
}

void setColorsRGB(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (g >> 5);
    buffer[2 * (WIDTH * y + x) + 1] = ((g & 0x1F) << 5) | (b >> 3);
}

void medianFilter(void) {
    int i, j, k, l;
    unsigned char r, g, b;
    unsigned char rs[9], gs[9], bs[9];
    for (j = 0; j < HEIGHT; ++j) {
        getColorsRGB(0, j, &r, &g, &b);
        backup_ra[j] = r;
        backup_ga[j] = g;
        backup_ba[j] = b;
    }
    for (i = 0; i < WIDTH; ++i) {
        if (i == 0 || i == WIDTH - 1) continue;
        for (j = 0; j < HEIGHT; ++j) {
            getColorsRGB(i, j, &r, &g, &b);
            backup_rb[j] = r;
            backup_gb[j] = g;
            backup_bb[j] = b;
        }
        for (j = 0; j < HEIGHT; ++j) {
            if (j == 0  || j == HEIGHT - 1) continue;
            rs[0] = backup_ra[j - 1]; // i - 1
            gs[0] = backup_ga[j - 1]; // i - 1
            bs[0] = backup_ba[j - 1]; // i - 1
            getColorsRGB(i, j - 1, &r, &g, &b);
            rs[1] = r;
            gs[1] = g;
            bs[1] = b;
            getColorsRGB(i + 1, j - 1, &r, &g, &b);
            rs[2] = r;
            gs[2] = g;
            bs[2] = b;
            rs[3] = backup_ra[j]; // i - 1
            gs[3] = backup_ga[j]; // i - 1
            bs[3] = backup_ba[j]; // i - 1
            getColorsRGB(i, j, &r, &g, &b);
            rs[4] = r;
            gs[4] = g;
            bs[4] = b;
            getColorsRGB(i + 1, j, &r, &g, &b);
            rs[5] = r;
            gs[5] = g;
            bs[5] = b;
            rs[6] = backup_ra[j + 1]; // i - 1
            gs[6] = backup_ga[j + 1]; // i - 1
            bs[6] = backup_ba[j + 1]; // i - 1
            getColorsRGB(i, j + 1, &r, &g, &b);
            rs[7] = r;
            gs[7] = g;
            bs[7] = g;
            getColorsRGB(i + 1, j + 1, &r, &g, &b);
            rs[8] = r;
            gs[8] = g;
            bs[8] = b;
            for (k = 0; k < 9; ++k) {
                for (l = 0; l < (9 - k - 1); ++l) {
                    char t;
                    if (rs[l + 1] < rs[l]) {
                        t = rs[l + 1];
                        rs[l + 1] = rs[l];
                        rs[l] = t;
                    }
                    if (gs[l + 1] < gs[l]) {
                        t = gs[l + 1];
                        gs[l + 1] = gs[l];
                        gs[l] = t;
                    }
                    if (bs[l + 1] < bs[l]) {
                    	t = bs[l + 1];
                    	bs[l + 1] = bs[l];
                    	bs[l] = t;
                    }
                }
            }
            setColorsRGB(i, j, rs[4], gs[4], bs[4]);
        }
        memcpy(&backup_ra, &backup_rb, HEIGHT);
        memcpy(&backup_ga, &backup_gb, HEIGHT);
        memcpy(&backup_ba, &backup_bb, HEIGHT);
    }
}

void binaryFilter(void) {
    int i, j;
    unsigned char r, g, b;
    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            getColorsRGB(i, j, &r, &g, &b);
            int rb = (r - g) + (r - b);
            int gb = (g - r) + (g - b);
            int bb = (b - r) + (b - g);
            if (rb > RED_THRESHOLD) {
                rb = 32;
            } else {
                rb = 0;
            }
            if (gb > GREEN_THRESHOLD) {
                gb = 32;
            } else {
                gb = 0;
            }
            if (bb > BLUE_THRESHOLD) {
            	bb = 32;
            } else {
            	bb = 0;
            }
            setColorsRGB(i, j, rb, gb, bb);
        }
    }
}

void granularityFilter(void) {
    int i, j;
    unsigned char r, g, b, rs, gs, bs, rn, gn, bn;
    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            getColorsRGB(i, j, &r, &g, &b);
            backup_gb[j] = g;
            backup_rb[j] = r;
            backup_bb[j] = b;
        }
        for (j = 0; j < HEIGHT; ++j) {
            if (i == 0 || i == WIDTH - 1 || j == 0 || j == HEIGHT - 1) {
                setColorsRGB(i, j, 0, 0, 0);
                continue;
            }
            rs = 0;
            gs = 0;
            bs = 0;
            rn = 0;
            gn = 0;
            bn = 0;
            if (backup_rb[j - 1] > 0) rs++;
            if (backup_gb[j - 1] > 0) gs++;
            if (backup_bb[j - 1] > 0) bs++;
            if (backup_ra[j] > 0) rs++;
            if (backup_ga[j] > 0) gs++;
            if (backup_ba[j] > 0) bs++;
            if (backup_rb[j] > 0) rs++;
            if (backup_gb[j] > 0) gs++;
            if (backup_bb[j] > 0) bs++;
            getColorsRGB(i + 1, j, &r, &g, &b);
            if (r > 0) rs++;
            if (g > 0) gs++;
            if (b > 0) bs++;
            if (backup_rb[j + 1] > 0) rs++;
            if (backup_gb[j + 1] > 0) gs++;
            if (backup_bb[j + 1] > 0) bs++;
            if (rs > 3) rn = 32;
            if (gs > 3) gn = 32;
            if (bs > 3) bn = 32;
            setColorsRGB(i, j, rn, gn, bn);
        }
        memcpy(&backup_ra, &backup_rb, HEIGHT);
        memcpy(&backup_ga, &backup_gb, HEIGHT);
        memcpy(&backup_ba, &backup_bb, HEIGHT);
    }
}

void lookForRedEdges(int ix, int iy, int* hu, int* wr, int* hd, int* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    unsigned char r;
    getColorR(ix, iy, &r);
    unsigned char rr = r;
    while (rr > 0) {
        (*hu)--;
        if (*hu > 0) {
            getColorR(ix, *hu, &rr);
            if (rr == 0) {
                (*hu)++;
                break;
            }
        } else {
            (*hu)++;
            break;
        }
    }
    rr = r;
    while (rr > 0) {
        (*wr)++;
        if (*wr < WIDTH - 1) {
            getColorR(*wr, iy, &rr);
            if (rr == 0) {
                (*wr)--;
                break;
            }
        } else {
            (*wr)--;
            break;
        }
    }
    rr = r;
    while (rr > 0) {
        (*hd)++;
        if (*hd < HEIGHT - 1) {
            getColorR(ix, *hd, &rr);
            if (rr == 0) {
                (*hd)--;
                break;
            }
        } else {
            (*hd)--;
            break;
        }
    }
    rr = r;
    while (rr > 0) {
        (*wl)--;
        if (*wl > 0) {
            getColorR(*wl, iy, &rr);
            if (rr == 0) {
                (*wl)++;
                break;
            }
        } else {
            (*wl)++;
            break;
        }
    }
}

void lookForGreenEdges(int ix, int iy, int* hu, int* wr, int* hd, int* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    unsigned char g;
    getColorG(ix, iy, &g);
    unsigned char gr = g;
    while (gr > 0) {
        (*hu)--;
        if (*hu > 0) {
            getColorG(ix, *hu, &gr);
            if (gr == 0) {
                (*hu)++;
                break;
            }
        } else {
            (*hu)++;
            break;
        }
    }
    gr = g;
    while (gr > 0) {
        (*wr)++;
        if (*wr < WIDTH - 1) {
            getColorG(*wr, iy, &gr);
            if (gr == 0) {
                (*wr)--;
                break;
            }
        } else {
            (*wr)--;
            break;
        }
    }
    gr = g;
    while (gr > 0) {
        (*hd)++;
        if (*hd < HEIGHT - 1) {
            getColorG(ix, *hd, &gr);
            if (gr == 0) {
                (*hd)--;
                break;
            }
        } else {
            (*hd)--;
            break;
        }
    }
    gr = g;
    while (gr > 0) {
        (*wl)--;
        if (*wl > 0) {
            getColorG(*wl, iy, &gr);
            if (gr == 0) {
                (*wl)++;
                break;
            }
        } else {
            (*wl)++;
            break;
        }
    }
}

void lookForBlueEdges(int ix, int iy, int* hu, int* wr, int* hd, int* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    unsigned char b;
    getColorB(ix, iy, &b);
    unsigned char br = b;
    while (br > 0) {
        (*hu)--;
        if (*hu > 0) {
            getColorB(ix, *hu, &br);
            if (br == 0) {
                (*hu)++;
                break;
            }
        } else {
            (*hu)++;
            break;
        }
    }
    br = b;
    while (br > 0) {
        (*wr)++;
        if (*wr < WIDTH - 1) {
            getColorB(*wr, iy, &br);
            if (br == 0) {
                (*wr)--;
                break;
            }
        } else {
            (*wr)--;
            break;
        }
    }
    br = b;
    while (br > 0) {
        (*hd)++;
        if (*hd < HEIGHT - 1) {
            getColorB(ix, *hd, &br);
            if (br == 0) {
                (*hd)--;
                break;
            }
        } else {
            (*hd)--;
            break;
        }
    }
    br = b;
    while (br > 0) {
        (*wl)--;
        if (*wl > 0) {
            getColorB(*wl, iy, &br);
            if (br == 0) {
                (*wl)++;
                break;
            }
        } else {
            (*wl)++;
            break;
        }
    }
}

void clearRectRed(int x, int y, int w, int h) {
    int i, j;
    for (i = x; i < x + w; ++i) {
        for (j = y; j < y + h; ++j) {
            setColorR(i, j, 0);
        }
    }
}

void clearRectBlue(int x, int y, int w, int h) {
    int i, j;
    for (i = x; i < x + w; ++i) {
        for (j = y; j < y + h; ++j) {
            setColorB(i, j, 0);
        }
    }
}

void detectRedObjects() {
    redsDetected = 0;
    int iteration = 0;
    while (redsDetected < MAX_OBJECTS && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        int ix = index % WIDTH;
        int iy = index / WIDTH;
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        unsigned char r;
        getColorR(ix, iy, &r);
        if (r > 0) {
            int area = 0;
            int newArea = 1;
            struct Rect rect = (struct Rect) {0, 0, 0, 0};
            struct Rect newRect = (struct Rect) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                int hu = iy, wr = ix, hd = iy, wl = ix;
                lookForRedEdges(ix, iy, &hu, &wr, &hd, &wl);
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                double ratio = (double)rect.w / rect.h;
                if (ratio < RATIO_THRESHOLD) {
                    int w = rect.w;
                    rect.w = (int)(1.33 * rect.h);
                    if (rect.x < WIDTH / 2) {
                        rect.x -= (rect.w - w);
                    }
                }
                reds[redsDetected].dis = 550 / rect.w;
                reds[redsDetected].dir = (rect.x + rect.w / 2 - WIDTH / 2) * 1.5;
                reds[redsDetected].x = rect.x;
                reds[redsDetected].w = rect.w;
                reds[redsDetected].h = rect.h;
                redsDetected++;
                iteration = 0;
                clearRectRed(rect.x, rect.y, rect.w, rect.h);
            }
        }
        iteration++;
    }
}

void detectBlueObjects() {
    bluesDetected = 0;
    int iteration = 0;
    while (bluesDetected < MAX_OBJECTS && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        int ix = index % WIDTH;
        int iy = index / WIDTH;
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        unsigned char b;
        getColorB(ix, iy, &b);
        if (b > 0) {
            int area = 0;
            int newArea = 1;
            struct Rect rect = (struct Rect) {0, 0, 0, 0};
            struct Rect newRect = (struct Rect) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                int hu = iy, wr = ix, hd = iy, wl = ix;
                lookForBlueEdges(ix, iy, &hu, &wr, &hd, &wl);
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                double ratio = (double)rect.w / rect.h;
                if (ratio < RATIO_THRESHOLD) {
                    int w = rect.w;
                    rect.w = (int)(1.33 * rect.h);
                    if (rect.x < WIDTH / 2) {
                        rect.x -= (rect.w - w);
                    }
                }
                blues[bluesDetected].dis = 550 / rect.w;
                blues[bluesDetected].dir = (rect.x + rect.w / 2 - WIDTH / 2) * 1.5;
                blues[bluesDetected].x = rect.x;
                blues[bluesDetected].y = rect.y;
                blues[bluesDetected].w = rect.w;
                blues[bluesDetected].h = rect.h;
                bluesDetected++;
                iteration = 0;
                clearRectBlue(rect.x, rect.y, rect.w, rect.h);
            }
        }
        iteration++;
    }
}

void getMaxBlueObject() {
    if (BLUE_MODE == SINGLE) {
        if (bluesDetected > 1) {
            int min = 0;
            int dist = blues[0].dis;
            int i = 0;
            for (i = 0; i < bluesDetected; ++i) {
                if (blues[i].dis < dist) {
                    min = i;
                    dist = blues[i].dis;
                }
            }
            if (min != 0) {
                blues[0] = blues[min];
            }
            bluesDetected = 1;
        }
    }
}

void mx_vision_init_cycle() {
    int i;
    /*for (i = 0; i < 256; i++) {
        fits[i] = 0;
    }*/
    for (i = 0; i < redsDetected; i++) {
        redsPrediction[0][i] = reds[i]; // no movement
        redsPrediction[1][i] = reds[i];
        redsPrediction[1][i].dir -= DIR_DELTA; // to the left
        redsPrediction[1][i].x -= DIR_DELTA;
        redsPrediction[2][i] = reds[i];
        redsPrediction[2][i].dir += DIR_DELTA; // to the right
        redsPrediction[2][i].x += DIR_DELTA;
        redsPrediction[3][i] = reds[i];
        redsPrediction[3][i].dis += DIS_DELTA; // moving far
    }
    prevRedsDetected = redsDetected;
    prevBluesDetected = bluesDetected;
    redsDetected = 0;
    bluesDetected = 0;
}

// from is the prev Object
// to is the new Object
// return a percentage of "to" into "from"
int computeArea(struct Object from, struct Object to) {
    int left = max(from.x, to.x);
    int right = min(from.x + from.w, to.x + to.w);
    int top = max(from.y, to.y);
    int bottom = min(from.y + from.h, to.y + to.h);
    if (left < right && top < bottom) {
        int toArea = 100 * to.w * to.h;
        int intersection = (right - left) * (bottom - top);
        return toArea / intersection;
    } else {
        return 0;
    }
}

void mx_vision_after_cycle() {
    int i = 0, j, k;
    int objects[4];
    int maxFit = 0, maxIndex = 0;
    for (i = 0; i < 256; i++) {
        // compute fit for this setup
        objects[0] = i & 3;
        objects[1] = (i & 12) >> 2;
        objects[2] = (i & 48) >> 4;
        objects[3] = (i & 192) >> 6;
        int newFit = 0;
        for (j = 0; j < prevRedsDetected; j++) {
            for (k = 0; k < redsDetected; k++) {
                int fit = computeArea(redsPrediction[objects[j]][j], reds[k]);
                newFit = newFit + fit;
            }
        }
        //printf("Fit %d at setup %d\n", newFit, i);
        //fit[i] = newFit;
        if (newFit > maxFit) {
            maxFit = newFit;
            maxIndex = i;
        }
    }
    printf("Max fit %d at index %d\n", maxFit, maxIndex);
    // take decision
}

#ifdef MX_DEV
    void mx_vision_init(void) {
        time_t s = time(0);
        srand((unsigned)s);
    }

    void mx_vision_set(unsigned char *image) {
        memcpy(buffer, image, BUFFER_SIZE);
    }

    void mx_vision_see(unsigned char *image) {
        mx_vision_init_cycle();
        memcpy(buffer, image, BUFFER_SIZE);
        medianFilter();
        binaryFilter();
        granularityFilter();
        detectRedObjects();
        detectBlueObjects();
        getMaxBlueObject();
        mx_vision_after_cycle();
    }
#else
    void mx_vision_init(void) {
        e_poxxxx_init_cam();
        e_poxxxx_config_cam(0, 160, WIDTH * ZOOM, HEIGHT * ZOOM, ZOOM, ZOOM, RGB_565_MODE);
        e_poxxxx_write_cam_registers();
    }

    void mx_vision_see(void) {
        mx_vision_init_cycle();
        e_poxxxx_launch_capture(buffer);
        while (!e_poxxxx_is_img_ready());
        medianFilter();
        binaryFilter();
        granularityFilter();
        detectRedObjects();
        detectBlueObjects();
        getMaxBlueObject();
        mx_vision_after_cycle();
    }
#endif

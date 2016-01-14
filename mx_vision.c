#include "mx_vision.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef MX_DEV
    #include <stdio.h>
#else
    #include "e_poxxxx.h"
#endif

char buffer[BUFFER_SIZE];

struct Rect {
    int x;
    int y;
    int w;
    int h;
};

struct Rect redObject = (struct Rect) {0, 0, 0, 0};
struct Rect greenObject = (struct Rect) {0, 0, 0, 0};
struct Rect blueObject = (struct Rect) {0, 0, 0, 0};

struct Object red;
struct Object green;
struct Object blue;

int redsDetected = 0;
int greensDetected = 0;
int bluesDetected = 0;

char backup_ra[HEIGHT];
char backup_rb[HEIGHT];
char backup_ga[HEIGHT];
char backup_gb[HEIGHT];
char backup_ba[HEIGHT];
char backup_bb[HEIGHT];

// R

void getColorR(char x, char y, char *r) {
    char h = buffer[2 * (WIDTH * y + x)];
    *r = h & 0xF8;
}

// G

void getColorG(char x, char y, char *g) {
    char h = buffer[2 * (WIDTH * y + x)];
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
}

// B

void getColorB(char x, char y, char *b) {
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *b = (l & 0x1F) << 3;
}

// RG

void getColorsRG(char x, char y, char* r, char* g) {
    char h = buffer[2 * (WIDTH * y + x)];
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
}

void setColorsRG(char x, char y, char r, char g) {
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (g >> 5);
    char byte = buffer[2 * (WIDTH * y + x) + 1] & 0x1F;
    buffer[2 * (WIDTH * y + x) + 1] = (g << 5) | byte;
}

// RGB

void getColorsRGB(char x, char y, char* r, char* g, char* b) {
    char h = buffer[2 * (WIDTH * y + x)];
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
    *b = (l & 0x1F) << 3;
}

void setColorsRGB(char x, char y, char r, char g, char b) {
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (g >> 5);
    buffer[2 * (WIDTH * y + x) + 1] = ((g & 0x1F) << 5) | (b >> 3);
}

void medianFilter(void) {
    char i, j, k, l, r, g, b;
    char rs[9], gs[9], bs[9];
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
    char i, j, r, g, b;
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
    char i, j, r, g, b, rs, gs, bs, rn, gn, bn;
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

void lookForRedEdges(char ix, char iy, char* hu, char* wr, char* hd, char* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    char r;
    getColorR(ix, iy, &r);
    char rr = r;
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

void lookForGreenEdges(char ix, char iy, char* hu, char* wr, char* hd, char* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    char g;
    getColorG(ix, iy, &g);
    char gr = g;
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

void lookForBlueEdges(char ix, char iy, char* hu, char* wr, char* hd, char* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    char b;
    getColorB(ix, iy, &b);
    char br = b;
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

void detectChannelRed(void) {
    char objectDetected = 0;
    int iteration = 0;
    while (objectDetected == 0 && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        char ix = (char)(index % WIDTH);
        char iy = (char)(index / WIDTH);
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        char r, g;
        getColorsRG(ix, iy, &r, &g);
        if (r > 0) {
            int area = 0;
            int newArea = 1;
            struct Rect rect = (struct Rect) {0, 0, 0, 0};
            struct Rect newRect = (struct Rect) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                char hu = iy, wr = ix, hd = iy, wl = ix;
                lookForRedEdges(ix, iy, &hu, &wr, &hd, &wl);
                #ifdef MX_DEV
                    printf("Rect red %d %d %d %d\n", hu, wr, hd, wl);
                #endif
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                objectDetected = 1;
                redObject = rect;
                redsDetected = 1;
                red.dis = 550 / rect.w;
                red.dir = (rect.x + rect.w / 2 - 40) * -1.5;
                #ifdef MX_DEV
                    printf("Red object\n");
                #endif
            }
        }
        iteration++;
    }
}

void detectChannelGreen(void) {
    char objectDetected = 0;
    int iteration = 0;
    while (objectDetected == 0 && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        char ix = (char)(index % WIDTH);
        char iy = (char)(index / WIDTH);
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        char r, g;
        getColorsRG(ix, iy, &r, &g);
        if (g > 0) {
            int area = 0;
            int newArea = 1;
            struct Rect rect = (struct Rect) {0, 0, 0, 0};
            struct Rect newRect = (struct Rect) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                char hu = iy, wr = ix, hd = iy, wl = ix;
                lookForGreenEdges(ix, iy, &hu, &wr, &hd, &wl);
                #ifdef MX_DEV
                	printf("Rect green %d %d %d %d\n", hu, wr, hd, wl);
                #endif
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                objectDetected = 1;
                greenObject = rect;
                greensDetected = 1;
                green.dis = 550 / rect.w;
                green.dir = (rect.x + rect.w / 2 - 40) * -1.5;
                #ifdef MX_DEV
                	printf("Green object\n");
                #endif
            }
        }
        iteration++;
    }
}

void detectChannelBlue(void) {
    char objectDetected = 0;
    int iteration = 0;
    while (objectDetected == 0 && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        char ix = (char)(index % WIDTH);
        char iy = (char)(index / WIDTH);
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        char b;
        getColorB(ix, iy, &b);
        if (b > 0) {
            int area = 0;
            int newArea = 1;
            struct Rect rect = (struct Rect) {0, 0, 0, 0};
            struct Rect newRect = (struct Rect) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                char hu = iy, wr = ix, hd = iy, wl = ix;
                lookForBlueEdges(ix, iy, &hu, &wr, &hd, &wl);
                #ifdef MX_DEV
                    printf("Rect blue %d %d %d %d\n", hu, wr, hd, wl);
                #endif
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                objectDetected = 1;
                blueObject = rect;
                bluesDetected = 1;
                blue.dis = 550 / rect.w;
                blue.dir = (rect.x + rect.w / 2 - 40) * -1.5;
                #ifdef MX_DEV
                    printf("Blue object\n");
                #endif
            }
        }
        iteration++;
    }
}

#ifdef MX_DEV
    void mx_vision_init(int test) {
        time_t s = time(0);
        srand((unsigned)s);
    }
#else
    void mx_vision_init(void) {
        e_poxxxx_init_cam();
        e_poxxxx_config_cam(0, 160, WIDTH * ZOOM, HEIGHT * ZOOM, ZOOM, ZOOM, RGB_565_MODE);
        e_poxxxx_write_cam_registers();
    }
#endif

#ifdef MX_DEV
    void mx_vision_see(char[BUFFER_SIZE] image) {
        redsDetected = 0;
        greensDetected = 0;
        bluesDetected = 0;
        memcpy(&buffer, &image, BUFFER_SIZE);
        medianFilter();
        binaryFilter();
        granularityFilter();
        detectChannelRed();
        detectChannelBlue();
    }
#else
    void mx_vision_see(void) {
        redsDetected = 0;
        greensDetected = 0;
        bluesDetected = 0;
        e_poxxxx_launch_capture(buffer);
        while (!e_poxxxx_is_img_ready());
        medianFilter();
        binaryFilter();
        granularityFilter();
        detectChannelRed();
        detectChannelGreen();
        detectChannelBlue();
    }
#endif

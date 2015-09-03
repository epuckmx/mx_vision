#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "e_poxxxx.h"
#include "mx_vision.h"

char buffer[BUFFER_SIZE];

struct Object redObject = (struct Object) {0, 0, 0, 0};
struct Object greenObject = (struct Object) {0, 0, 0, 0};
bool redDetected = false;
bool greenDetected = false;

char backup_ga[HEIGHT];
char backup_gb[HEIGHT];
char backup_ra[HEIGHT];
char backup_rb[HEIGHT];

void getColors(char x, char y, char* r, char* g) {
    char h = buffer[2 * (WIDTH * y + x)];
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
}

void getColorsRgb(char x, char y, char* r, char* g, char* b) {
    char h = buffer[2 * (WIDTH * y + x)];
    char l = buffer[2 * (WIDTH * y + x) + 1];
    *r = h & 0xF8;
    *g = ((h & 0x07) << 5) | ((l & 0xE0) >> 3);
    *b = (l & 0x1F) << 3;
}

void setColors(char x, char y, char r, char g) {
    buffer[2 * (WIDTH * y + x)] = (r & 0xF8) | (g >> 5);
    char byte = buffer[2 * (WIDTH * y + x) + 1] & 0x1F;
    buffer[2 * (WIDTH * y + x) + 1] = (g << 5) | byte;
}

void medianFilter(void) {
    char i, j, k, l, r, g;
    char gs[9], rs[9];
    for (j = 0; j < HEIGHT; ++j) {
        getColors(0, j, &r, &g);
        backup_ga[j] = g;
        backup_ra[j] = r;
    }
    for (i = 0; i < WIDTH; ++i) {
        if (i == 0 || i == WIDTH - 1) continue;
        for (j = 0; j < HEIGHT; ++j) {
            getColors(i, j, &r, &g);
            backup_gb[j] = g;
            backup_rb[j] = r;
        }
        for (j = 0; j < HEIGHT; ++j) {
            if (j == 0  || j == HEIGHT - 1) continue;
            gs[0] = backup_ga[j - 1]; // i - 1
            rs[0] = backup_ra[j - 1]; // i - 1
            getColors(i, j - 1, &r, &g);
            gs[1] = g;
            rs[1] = r;
            getColors(i + 1, j - 1, &r, &g);
            gs[2] = g;
            rs[2] = r;
            gs[3] = backup_ga[j]; // i - 1
            rs[3] = backup_ra[j]; // i - 1
            getColors(i, j, &r, &g);
            gs[4] = g;
            rs[4] = r;
            getColors(i + 1, j, &r, &g);
            gs[5] = g;
            rs[5] = r;
            gs[6] = backup_ga[j + 1]; // i - 1
            rs[6] = backup_ra[j + 1]; // i - 1
            getColors(i, j + 1, &r, &g);
            gs[7] = g;
            rs[7] = r;
            getColors(i + 1, j + 1, &r, &g);
            gs[8] = g;
            rs[8] = r;
            for (k = 0; k < 9; ++k) {
                for (l = 0; l < (9 - k - 1); ++l) {
                    char t;
                    if (gs[l + 1] < gs[l]) {
                        t = gs[l + 1];
                        gs[l + 1] = gs[l];
                        gs[l] = t;
                    }
                    if (rs[l + 1] < rs[l]) {
                        t = rs[l + 1];
                        rs[l + 1] = rs[l];
                        rs[l] = t;
                    }
                }
            }
            setColors(i, j, rs[4], gs[4]);
        }
        memcpy(&backup_ga, &backup_gb, HEIGHT);
        memcpy(&backup_ra, &backup_rb, HEIGHT);
    }
}

void binaryFilter(void) {
    char i, j, r, g, b;
    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            getColorsRgb(i, j, &r, &g, &b);
            char rb = (r - g) + (r - b);
            char gb = (g - r) + (g - b);
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
            setColors(i, j, rb, gb);
        }
    }
}

void cleanBinaryFilter(void) {
    char i, j, r, g, rs, gs, rn, gn;
    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            getColors(i, j, &r, &g);
            backup_gb[j] = g;
            backup_rb[j] = r;
        }
        for (j = 0; j < HEIGHT; ++j) {    
            if (i == 0 || i == WIDTH - 1 || j == 0 || j == HEIGHT - 1) {
                setColors(i, j, 0, 0);
                continue;
            }
            rs = 0;
            gs = 0;
            rn = 0;
            gn = 0;
            if (backup_rb[j - 1] > 0) rs++;
            if (backup_gb[j - 1] > 0) gs++;
            if (backup_ra[j] > 0) rs++;
            if (backup_ga[j] > 0) gs++;
            if (backup_rb[j] > 0) rs++;
            if (backup_gb[j] > 0) gs++;
            getColors(i + 1, j, &r, &g);
            if (r > 0) rs++;
            if (g > 0) gs++;
            if (backup_rb[j + 1] > 0) rs++;
            if (backup_gb[j + 1] > 0) gs++;
            if (rs > 3) rn = 32;
            if (gs > 3) gn = 32;
            setColors(i, j, rn, gn);
        }
        memcpy(&backup_ga, &backup_gb, HEIGHT);
        memcpy(&backup_ra, &backup_rb, HEIGHT);
    }
}

void lookForRedEdges(char ix, char iy, char* hu, char* wr, char* hd, char* wl) {
    *hu = iy;
    *wr = ix;
    *hd = iy;
    *wl = ix;
    char r, g;
    getColors(ix, iy, &r, &g);
    char rr = r, gr = g;
    while (rr > 0) {
        (*hu)--;
        if (*hu > 0) {
            getColors(ix, *hu, &rr, &gr);
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
            getColors(*wr, iy, &rr, &gr);
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
            getColors(ix, *hd, &rr, &gr);
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
            getColors(*wl, iy, &rr, &gr);
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
    char r, g;
    getColors(ix, iy, &r, &g);
    char rr = r, gr = g;
    while (gr > 0) {
        (*hu)--;
        if (*hu > 0) {
            getColors(ix, *hu, &rr, &gr);
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
            getColors(*wr, iy, &rr, &gr);
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
            getColors(ix, *hd, &rr, &gr);
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
            getColors(*wl, iy, &rr, &gr);
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

void detectChannelRed(void) {
    bool objectDetected = false;
    int iteration = 0;
    while (!objectDetected && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        char ix = (char)(index % WIDTH);
        char iy = (char)(index / HEIGHT);
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        char r, g, b;
        getColors(ix, iy, &r, &g);
        if (r > 0) {
            int area = 0;
            int newArea = 1;
            struct Object rect = (struct Object) {0, 0, 0, 0};
            struct Object newRect = (struct Object) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                char hu = iy, wr = ix, hd = iy, wl = ix;
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
                objectDetected = true;
                redObject = rect;
                redDetected = true;
            }
        }
        iteration++;
    }
}

void detectChannelGreen(void) {
    bool objectDetected = false;
    int iteration = 0;
    while (!objectDetected && iteration < PIXELS / 2) {
        int index = rand() % PIXELS;
        char ix = (char)(index % WIDTH);
        char iy = (char)(index / HEIGHT);
        if (ix == 0 || ix == WIDTH - 1 || iy == 0 || iy == HEIGHT - 1) continue;
        char r, g;
        getColors(ix, iy, &r, &g);
        if (g > 0) {
            int area = 0;
            int newArea = 1;
            struct Object rect = (struct Object) {0, 0, 0, 0};
            struct Object newRect = (struct Object) {0, 0, 1, 1};
            while (newArea > area) {
                area = newArea;
                rect = newRect;
                char hu = iy, wr = ix, hd = iy, wl = ix;
                lookForGreenEdges(ix, iy, &hu, &wr, &hd, &wl);
                newRect.x = wl;
                newRect.y = hu;
                newRect.w = wr - wl + 1;
                newRect.h = hd - hu + 1;
                newArea = newRect.w * newRect.h;
                ix = newRect.x + newRect.w / 2;
                iy = newRect.y + newRect.h / 2;
            }
            if (rect.w >= MIN_WIDTH && rect.h >= MIN_HEIGHT) {
                objectDetected = true;
                greenObject = rect;
                greenDetected = true;
            }
        }
        iteration++;
    }
}

void init(void) {
	e_poxxxx_init_cam();
    e_poxxxx_config_cam(0, 160, WIDTH * ZOOM, HEIGHT * ZOOM, ZOOM, ZOOM, RGB_565_MODE);
    e_poxxxx_write_cam_registers();
}

void initRand(void) {
    time_t s = time(0);
    srand((unsigned)s);
    init();
}

void see(void) {
	redDetected = false;
    greenDetected = false;
    e_poxxxx_launch_capture(buffer);
    while (!e_poxxxx_is_img_ready());
    medianFilter();
    binaryFilter();
    cleanBinaryFilter();
    detectChannelRed();
}
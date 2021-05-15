#include "defs.h"

/// These work for my setup, they most likely will differ for yours

#define TOUCHPAD_INPUT_ID "12"

#define SXMAX 1236
#define SYMAX 896

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080

static volatile sig_atomic_t stop = 0;

static void interrupt_handler(int sig) {
    stop = 1;
}

typedef struct input_event input_event;

int die() {
    exit(EXIT_FAILURE);
}

int32_t test_grab(int32_t fd) {
    int rc;
    rc = ioctl(fd, EVIOCGRAB, (void *) 1);

    if (rc == 0) {
        ioctl(fd, EVIOCGRAB, (void *) 0);
    }
    return rc;
}

struct finger {
    uint32_t x;
    uint32_t y;
};

struct finger fingers[6];

uint32_t gscr(uint32_t len, uint32_t max, uint32_t val) {
    return (uint32_t) ((float) len * ((float)val / (float)max));
}

int32_t start_abs_touchpad(int32_t fd) {
    Display *display = XOpenDisplay(NULL);
    Window window = XRootWindow(display, 0);
    XSelectInput(display, window, KeyReleaseMask);

    if (display == NULL) {
        fputs("Could not open display!", stderr);
        die();
    }
    input_event ev[64];
    int32_t i, rd;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    int32_t finger_current = 0;
    uint32_t screen_x, screen_y;

    while (!stop) {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop) { break; }
        rd = read(fd, ev, sizeof(ev));
        if (rd < (int) sizeof(input_event)) {
            printf("expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
            perror("\nevtest: error reading");
            return 1;
        }
        for (i = 0; i < rd / sizeof(input_event); ++i) {
            uint32_t type, code;
            type = ev[i].type;
            code = ev[i].code;
            if (code != MSC_TIMESTAMP && code != SYN_REPORT) {
                if (type == EV_ABS) {
                    switch (code) {
                        case ABS_MT_SLOT:
                            finger_current = ev[i].value;
                            break;

                        case ABS_MT_TRACKING_ID:
                            finger_current = 0;
                            break;

                        case ABS_MT_POSITION_X:
                            fingers[finger_current].x = ev[i].value;
                            break;

                        case ABS_MT_POSITION_Y:
                            fingers[finger_current].y = ev[i].value;
                            break;

                        default:
                            break;
                    }

                }
            }
        }

        screen_x = gscr(MONITOR_WIDTH, SXMAX, fingers[0].x);
        screen_y = gscr(MONITOR_HEIGHT, SYMAX, fingers[0].y);

        XWarpPointer(display, None, window, 0, 0, 0, 0, screen_x, screen_y);
        XSync(display, False);
    }

    ioctl(fd, EVIOCGRAB, (void *) 0);
    XCloseDisplay(display);
    return EXIT_SUCCESS;
}

int32_t do_capture() {
    const char *filename = "/dev/input/event" TOUCHPAD_INPUT_ID;
    int32_t fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        perror("abstouch");
        if (errno == EACCES && getuid() != 0) {
            fprintf(stderr, "You do not have access to %s. Try "
                            "running as root instead.\n",
                    filename);
        }
        die();
    }
    if (!isatty(fileno(stdout))) {
        setbuf(stdout, NULL);
    }
    if (test_grab(fd)) {
        fprintf(stderr, "The device is grabbed by another proccess");
    }

    signal(SIGINT, interrupt_handler);
    signal(SIGTERM, interrupt_handler);

    return start_abs_touchpad(fd);

}

int main(int argc, char **argv) {
    return do_capture();
}

#include <SDL2/SDL.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_POINTS 500

typedef struct {
    int x, y;
} Point;

Point points[MAX_POINTS];
int pointCount = 0;

int init_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 1;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return fd;
}

void draw_plot(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green line

    for (int i = 1; i < pointCount; i++) {
        SDL_RenderDrawLine(renderer,
                           points[i - 1].x, HEIGHT - points[i - 1].y,
                           points[i].x, HEIGHT - points[i].y);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    int fd = init_serial("/dev/ttyUSB0");
    if (fd < 0) return 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Serial Plotter",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    char buffer[100];
    int buf_index = 0;
    memset(buffer, 0, sizeof(buffer));

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        // Handle window close
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
        }

        char ch;
        if (read(fd, &ch, 1) > 0) {
            if (ch == '\n' || ch == '\r') {
                buffer[buf_index] = '\0';
                buf_index = 0;

                int t, y;
                if (sscanf(buffer, "%d,%d", &t, &y) == 2) {
                    if (pointCount < MAX_POINTS) {
                        points[pointCount].x = t % WIDTH;
                        points[pointCount].y = y % HEIGHT;
                        pointCount++;
                    } else {
                        memmove(points, points + 1, sizeof(Point) * (MAX_POINTS - 1));
                        points[MAX_POINTS - 1].x = t % WIDTH;
                        points[MAX_POINTS - 1].y = y % HEIGHT;
                    }
                    draw_plot(renderer);
                }
            } else if (buf_index < sizeof(buffer) - 1) {
                buffer[buf_index++] = ch;
            }
        }

        SDL_Delay(10);
    }

    close(fd);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

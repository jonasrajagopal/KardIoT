#include <SDL2/SDL.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 2400
#define HEIGHT 600
#define MAX_POINTS 200

typedef struct {
    int x, y;
} Point;

typedef struct {
    float x, y;
} PointSmooth;

Point points[MAX_POINTS];
float baselines[MAX_POINTS];
PointSmooth points_smooth[MAX_POINTS];
int pointCount = 0;

void draw_plot(SDL_Renderer *renderer) {
    if (pointCount < 2) return;

    // Find min and max Y
    int minY = points[0].y;
    int maxY = points[0].y;
    for (int i = 1; i < pointCount; i++) {
        if (points[i].y < minY) minY = points[i].y;
        if (points[i].y > maxY) maxY = points[i].y;
    }

    int yRange = maxY - minY;
    if (yRange == 0) yRange = 1;  // Avoid division by zero

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green line

    int avg = 2;
    int baseline = avg*40;
    for (int i=0; i<pointCount; i++) {
        baselines[i] = minY + yRange/2;
    }
    for (int i=baseline-1; i<pointCount-baseline; i++) {
        baselines[i] = 0;
        for (int j = -1*baseline ; j < baseline ; j++) {
            baselines[i]  += points[i+j].y;
        }
        baselines[i] /= 2.0*baseline;
    }

    for (int i=avg; i<pointCount-avg; i++) {
        points_smooth[i].x = 0;
        points_smooth[i].y = 0;
        for (int j = -1*avg ; j < avg ; j ++) {
            points_smooth[i].x  += points[i+j].x;
            points_smooth[i].y  += points[i+j].y;
        }
        points_smooth[i].x /= 2.0*avg;
        points_smooth[i].y /= 2.0*avg;

        points_smooth[i].y += -1.0*baselines[i] + minY + yRange/2;
    }

    // int minYS = points_smooth[0].y;
    // int maxYS = points_smooth[0].y;
    // for (int i = 1; i < pointCount; i++) {
    //     if (points_smooth[i].y < minY) minYS = points_smooth[i].y;
    //     if (points_smooth[i].y > maxY) maxYS = points_smooth[i].y;
    // }

    // int yRangeS = maxYS - minYS;
    // if (yRangeS == 0) yRangeS = 1;  // Avoid division by zero

    for (int i = 1; i < pointCount; i++) {
        SDL_SetRenderDrawColor(renderer, 255,0, 0, 50); // Red line
        int x1 = points[i - 1].x;
        int y1 = HEIGHT - ((points[i - 1].y - minY) * HEIGHT / yRange);
        int x2 = points[i].x;
        int y2 = HEIGHT - ((points[i].y - minY) * HEIGHT / yRange);
        if (x2 < x1) continue;
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green line
        if (i > avg) {
            x1 = (int)points_smooth[i - 1].x;
            y1 = HEIGHT - ((points_smooth[i - 1].y - minY) * HEIGHT / (yRange*1.0));
            x2 = (int)points_smooth[i].x;
            y2 = HEIGHT - ((points_smooth[i].y - minY) * HEIGHT / (yRange*1.0));
            if (x2 < x1) continue;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }

    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./data_plot_in [output file]\n");
        return -1;
    }

    // int fd = init_serial("/dev/ttyUSB0");
    int fd = 0;
    // int fd = init_serial(argv[1]);
    FILE *outf = fopen(argv[1],"w");

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
    
    int start = 0;

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
                    if (t==0) start = 1;
                    if (start == 0) continue;
                    if (pointCount < MAX_POINTS) {
                        points[pointCount].x = t % WIDTH;
                        points[pointCount].y = y % HEIGHT;
                        pointCount++;
                    } else {
                        memmove(points, points + 1, sizeof(Point) * (MAX_POINTS - 1));
                        points[MAX_POINTS - 1].x = t % WIDTH;
                        points[MAX_POINTS - 1].y = y % HEIGHT;
                    }
                    printf("%d,%d\n",t,y);
                    fprintf(outf, "%d,%d\n",t,y);
                    // if (t%(WIDTH>>6) < 3)
                        draw_plot(renderer);
                }
            } else if (buf_index < sizeof(buffer) - 1) {
                buffer[buf_index++] = ch;
            }
        }

        // SDL_Delay(1);
    }

    close(fd);
    fclose(outf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

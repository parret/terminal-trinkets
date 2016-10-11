#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

struct winsize w;
mutex mtx;

void safe_draw(string item, int x, int y){
    mtx.lock();
    cout << "\033[" << y << ";" << x << "H" << item; fflush(stdout);
    mtx.unlock();
}

string rand_speck() {
    switch (rand() % 6) {
        case 0: return ",\'";
        case 1: return ",";
        case 2: return "\'";
        case 3: return ";";
        case 4: return "..";
        case 5: return "\',";
        default: return "OOPS";
    }
}

class Drop {
private:
    int x, y, lastX, lastY;

public:
    Drop() { x = y = 0; }
    Drop(int x, int y) {
        this->x = x;
        this->y = y;
        this->lastX = x;
        this->lastY = y;
    }

    int getX(){ return x; }
    int getY(){ return y; }

    void draw() { safe_draw("o", x, y); }
    void undraw() { safe_draw(" ", x, y); }
    void undrawLast() { safe_draw(" ", lastX, lastY); }
    void stepDraw() {
        safe_draw("   ", lastX - 1, lastY);
        safe_draw("o", x, y);
    }

    void computeNext() {
        const int LAST_X = x, LAST_Y = y;

        if (y != lastY) {// drop is moving
            if (rand() % 20 > 0) { // drop continues moving
                y++;
                if (rand() % 5 == 0) { // chance of right drift
                    x++;
                }
            }
        }

        else { // drop is still
            if (rand() % 100 == 0) { // drop breaks out of still
                y++;
                if (rand() % 5 == 0) { // drop drifts right
                    x++;
                }
            }
        }

        lastX = LAST_X;
        lastY = LAST_Y;
    }
};

void *NewDrop(void *threadid) {
    Drop drop((rand() % w.ws_col), (rand() % w.ws_row + 5) - 5);
    while (drop.getY() < w.ws_row + 1 && drop.getX() < w.ws_col) {
        drop.stepDraw();
        drop.computeNext();
        usleep(20000);
    }
    pthread_exit(NULL);
}

void *Speckle(void *threadid) {
    while (true) {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        safe_draw(rand_speck(), rand() % w.ws_col, rand() % w.ws_row + 1);
        usleep(rand() % 10000);
    }
}

int main() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    srand((unsigned) time(0));

    pthread_t t; // rain is chaotic, so we don't need to worry about tracking threads
    pthread_create(&t, NULL, Speckle, (void *) 0);
    while (true) {
        pthread_create(&t, NULL, NewDrop, (void *) 0);
        usleep(rand() % 500000 + 1000);
    }
}

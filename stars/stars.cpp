#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

struct winsize w;

int main(int argv, char* argc[]){
  int density;
  if (argv < 2) {
    density = 1000;
  } else {
    density = atoi(argc[1]);
  }
  srand((unsigned)time(0));
  int j;
  // cout << "\033[2J"; fflush(stdout);
  while (true) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    cout << "\033[" << (rand() % w.ws_row) << ";" << (rand()% w.ws_col) << "H"; fflush(stdout);
    j = (rand() % density);
    if (j == 0) {
      cout << "#";
    }
    else if (j <= 5) {
      cout << "o";
    }
    else if (j <= 14) {
      cout << "*";
    }
    else if (j <= 59) {
      cout << ".";
    } else {
      cout << " ";
    }
    fflush(stdout);
    usleep(1000);
  }
}

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

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
    cout << "\033[" << (rand()%45) << ";" << (rand()%150) << "H"; fflush(stdout);
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

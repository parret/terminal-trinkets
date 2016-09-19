#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

// THIS CODE IS DISGUSTING, YE BE WARNED
mutex mtx;

class Drop {
private:
  int lastX;
  int lastY;
  int x;
  int y;
  
public:
  Drop(){
    x = 0;
    y = 0;
  }

  Drop(int x, int y){
    this->x = x;
    this->y = y;
    this->lastX = x;
    this->lastY = y;
  }

  int getX(){
    return x;
  }

  int getY(){
    return y;
  }

  int getLastX(){
    return lastX;
  }

  int getLastY(){
    return lastY;
  }

  void computeNext(){
    const int LAST_X = x;
    const int LAST_Y = y;

    // drop is moving
    if(y != lastY){
      // drop continues moving
      if(rand()%20 > 0){
	y++;
	if(rand()%5 == 0){
	  x++;
	}
      }
    }
    // drop is still
    else {
      // drop breaks out of still
      if(rand()%100 == 0){
	y++;
	if(rand()%5 == 0){
	  x++;
	}
      }
    }
    lastX = LAST_X;
    lastY = LAST_Y;
  }
 
  void draw(){
    mtx.lock();
    cout << "\033[" << y << ";" << x << "H" << "o"; fflush(stdout);
    mtx.unlock();
  }

  void undrawLast(){
    mtx.lock();
    cout << "\033[" << lastY << ";" << lastX << "H" << " "; fflush(stdout);
    mtx.unlock();
  }

  void undraw(){
    mtx.lock();
    cout << "\033[" << y << ";" << x << "H" << " "; fflush(stdout);
    mtx.unlock();
  }

  void stepDraw(){
    mtx.lock();
    cout << "\033[" << lastY << ";" << lastX-1 << "H" << "   " << "\033[" << y << ";" << x << "H" << "o"; fflush(stdout);
    mtx.unlock();
  }
};

void *NewDrop(void *threadid){  
  Drop drop((rand() % 300), rand()%100);
  while (drop.getY() < 100){
    drop.stepDraw();
    drop.computeNext();
    usleep(20000);
  }
  //drop.undraw();
  pthread_exit(NULL);
}

void *Speckle(void *threadid){
  string speck = "";
  while(true){
    switch(rand()%6){
    case 0:
      speck = ",\'";
      break;
    case 1:
      speck = ",";
      break;
    case 2:
      speck = "\'";
      break;
    case 3:
      speck = ";";
      break;
    case 4:
      speck = "..";
      break;
    case 5:
      speck = "\',";
      break;
    default:
      speck = "OOPS";
      break;
    }
    mtx.lock();
    cout << "\033[" << rand()%100 << ";" << rand()%300 << "H" << speck; fflush(stdout);
    mtx.unlock();
    usleep(rand()%10000);
  }
}

int main(){
  srand((unsigned)time(0));
  int rc;
  
  pthread_t threads[100000];
  rc = pthread_create(&threads[0], NULL, Speckle, (void *)0);
  for (int i = 1; i < 100000; i++){
    rc = pthread_create(&threads[i], NULL, NewDrop, (void *)i);
    if(rc){
      exit(-1);
    }
    usleep(rand()%500000+1000);
  }
  pthread_exit(NULL);
}

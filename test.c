
#include <stdio.h>
#include <time.h>

int main () {
  time_t rawtime;
  struct tm * timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf ("Current local time and date: %s", asctime(timeinfo));
  
  return 0;
}
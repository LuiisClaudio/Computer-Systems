#include <stdio.h>
#include <unistd.h>

int main(void)
{
  int i=0;
  while(i<50) {
    printf("dura bastante\n");
    sleep(1);
    i++;
  }
  
  return 0;
}

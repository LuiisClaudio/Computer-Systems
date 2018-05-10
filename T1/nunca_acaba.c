#include <stdio.h>
#include <unistd.h>

int main(void)
{
  while(1) {
    printf("teste nunca acaba\n");
    sleep(1);
  }
  
  return 0;
}

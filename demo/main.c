#include <stdio.h>
#include <string.h>

char *str1 = "DEADBEEF";
char *str2 = " apple pie\n";

int main() {
  int x = 1;
	int y = 69;
	printf("hello world %d\n", (x+y)*(x+y));
  char* newStr = strdup(str1);
	strcat(newStr, str2);
	printf("%s", newStr);

  return 0;
}

#include <string.h>
#include <stdio.h>
#include "utils.h"

// check every '\\' in str and unescape "\\\\" "\\n" "\\e"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        printf("%s\n\n%c\n\n", str, *str);
        switch(str[1]) {
            case '\\':
                memmove(str, str+1, strlen(str));
                break;
            case 'e':
                memmove(str, str+1, strlen(str));
                *str = '\e';
                break;
            case 'n':
                memmove(str, str+1, strlen(str));
                *str = '\n';
                break;
        }
        ++str;
    }
}

int max(const int *nums, unsigned const int lenght) {
    if(!lenght) return 0;
    int max_num = nums[0];

    for(unsigned int i = 1; i < lenght; ++i) {
        if(nums[i] > max_num)
            max_num = nums[i];
    }

    return max_num;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    printf("error: not enough memory\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

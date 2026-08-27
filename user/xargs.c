#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

static void run(char *command, char **args)
{
  int pid = fork();
  if(pid < 0){
    fprintf(2, "xargs: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    exec(command, args);
    fprintf(2, "xargs: exec %s failed\n", command);
    exit(1);
  }
  wait(0);
}

int main(int argc, char *argv[])
{
  char line[512];
  char *args[MAXARG];
  int base = 0;

  if(argc < 2){
    fprintf(2, "usage: xargs command [arguments ...]\n");
    exit(1);
  }
  while(base < argc - 1 && base < MAXARG - 1){
    args[base] = argv[base + 1];
    base++;
  }

  int length = 0;
  char c;
  while(read(0, &c, 1) == 1){
    if(c != '\n'){
      if(length < sizeof(line) - 1)
        line[length++] = c;
      continue;
    }

    line[length] = 0;
    int count = base;
    char *p = line;
    while(*p != 0 && count < MAXARG - 1){
      while(*p == ' ' || *p == '\t')
        p++;
      if(*p == 0)
        break;
      args[count++] = p;
      while(*p != 0 && *p != ' ' && *p != '\t')
        p++;
      if(*p != 0)
        *p++ = 0;
    }
    args[count] = 0;
    if(count > base)
      run(argv[1], args);
    length = 0;
  }

  if(length > 0){
    line[length] = 0;
    int count = base;
    char *p = line;
    while(*p != 0 && count < MAXARG - 1){
      while(*p == ' ' || *p == '\t')
        p++;
      if(*p == 0)
        break;
      args[count++] = p;
      while(*p != 0 && *p != ' ' && *p != '\t')
        p++;
      if(*p != 0)
        *p++ = 0;
    }
    args[count] = 0;
    if(count > base)
      run(argv[1], args);
  }

  exit(0);
}

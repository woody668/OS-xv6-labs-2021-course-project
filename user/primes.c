#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
sieve(int input)
{
  int prime;
  if(read(input, &prime, sizeof(prime)) != sizeof(prime)){
    close(input);
    exit(0);
  }

  printf("prime %d\n", prime);

  int output[2];
  if(pipe(output) < 0){
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    close(input);
    close(output[1]);
    sieve(output[0]);
  }

  close(output[0]);
  int number;
  while(read(input, &number, sizeof(number)) == sizeof(number)){
    if(number % prime != 0 &&
       write(output[1], &number, sizeof(number)) != sizeof(number)){
      fprintf(2, "primes: write failed\n");
      exit(1);
    }
  }
  close(input);
  close(output[1]);
  wait(0);
  exit(0);
}

int
main(void)
{
  int input[2];
  if(pipe(input) < 0){
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    close(input[1]);
    sieve(input[0]);
  }

  close(input[0]);
  for(int number = 2; number <= 35; number++){
    if(write(input[1], &number, sizeof(number)) != sizeof(number)){
      fprintf(2, "primes: write failed\n");
      exit(1);
    }
  }
  close(input[1]);
  wait(0);
  exit(0);
}

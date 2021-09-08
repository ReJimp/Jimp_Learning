// Created by Jimp on 2021/9/9.
#include "include/apue.h"

int main()
{
  int c;

  while((c = getc(stdin)) != EOF) {
    if(putc(c, stdout) == EOF)
      err_sys("output error");
  }

  if(ferror(stdin))
    err_sys("input error");

  exit(0);
}
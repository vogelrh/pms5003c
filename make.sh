echo 'Compiling...'

cc -Wall -Wno-unused-but-set-variable -Wno-unused-variable -static \
  -std=c99 -pedantic \
  ./example.c \
  ./pms5003.c \
  -o example
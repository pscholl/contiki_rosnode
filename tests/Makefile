VPATH=../src/contiki_rosnode:../src
CFLAGS+=-I../src -std=gnu99 -O0 -g
OBJECTS=$(wildcard ../src/*.c)
HEADERS=$(wildcard ../src/**/*.h)

#TestMsgDecode: TestMsgDecode.o $(OBJECTS:.c=.o)
Benchmark: Test.o TestComplex.o
TestMsgDecode: Test.o TestComplex.o

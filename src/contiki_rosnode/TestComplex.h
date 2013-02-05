#ifndef _ROS_CONTIKI_ROSNODE_TESTCOMPLEX_H_
# define _ROS_CONTIKI_ROSNODE_TESTCOMPLEX_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <contiki_rosnode/Test.h>


#define TestComplex_md5 ("ac366b76ffe0cd23d789ef457600b38f")
#define TestComplex_rostype ("contiki_rosnode/TestComplex")

typedef struct TestComplex {
  int8_t some_number;
  Test_t test_static[2];
  Test_t **test_dynamic;
  int8_t another_number;
  char *aString;
  char* sStrings[2];
  char* *dStrings;

} TestComplex_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct TestComplex_packed {
  int8_t some_number;
  Test_t_packed test_static[2];
  Test_t **test_dynamic;
  int8_t another_number;
  char *aString;
  char* sStrings[2];
  char* *dStrings;

} __attribute__((__packed__)) TestComplex_t_packed;

TestComplex_t *
TestComplex_deserialize_size(char *buf, char *to, size_t *n);

TestComplex_t *
TestComplex_deserialize(char *buf, size_t n);

size_t
TestComplex_serialize(TestComplex_t *obj, char *buf, size_t n);

#endif

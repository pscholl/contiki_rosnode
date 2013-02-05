#ifndef _ROS_CONTIKI_ROSNODE_TEST_H_
# define _ROS_CONTIKI_ROSNODE_TEST_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Test_md5 ("87c6715226f6ac2cf369418c10409ae0")
#define Test_rostype ("contiki_rosnode/Test")



typedef struct Test {
  char *from_node;
  char *to_node;
  uint8_t quality[8];
  uint32_t a;
  uint32_t b;
  uint16_t c;
  uint8_t d;
  int16_t *i16arr;
  int32_t x;

} Test_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Test_packed {
  char *from_node;
  char *to_node;
  uint8_t quality[8];
  uint32_t a;
  uint32_t b;
  uint16_t c;
  uint8_t d;
  int16_t *i16arr;
  int32_t x;

} __attribute__((__packed__)) Test_t_packed;

Test_t *
Test_deserialize_size(char *buf, char *to, size_t *n);

Test_t *
Test_deserialize(char *buf, size_t n);

size_t
Test_serialize(Test_t *obj, char *buf, size_t n);

#endif

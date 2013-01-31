#ifndef _ROS_STD_MSGS_INT8_H_
# define _ROS_STD_MSGS_INT8_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Int8_md5 ("27ffa0c9c4b8fb8492252bcad9e5c57b")
#define Int8_rostype ("std_msgs/Int8")

typedef struct __attribute__((__packed__)) Int8 {
  int8_t data;

} Int8_t;


Int8_t *
Int8_deserialize_size(char *buf, char *to, size_t *n);

Int8_t *
Int8_deserialize(char *buf, size_t n);

size_t
Int8_serialize(Int8_t *obj, char *buf, size_t n);

#endif

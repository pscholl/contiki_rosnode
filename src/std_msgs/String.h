#ifndef _ROS_STD_MSGS_STRING_H_
# define _ROS_STD_MSGS_STRING_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define String_md5 ("992ce8a1687cec8c8bd883ec73ca41d1")
#define String_rostype ("std_msgs/String")

typedef struct __attribute__((__packed__)) String {
  char *data;

} String_t;


String_t *
String_deserialize_size(char *buf, char *to, size_t *n);

String_t *
String_deserialize(char *buf, size_t n);

size_t
String_serialize(String_t *obj, char *buf, size_t n);

#endif

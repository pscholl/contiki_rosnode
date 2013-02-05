#ifndef _ROS_STD_MSGS_INT16MULTIARRAY_H_
# define _ROS_STD_MSGS_INT16MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Int16MultiArray_md5 ("d9338d7f523fcb692fae9d0a0e9f067c")
#define Int16MultiArray_rostype ("std_msgs/Int16MultiArray")



typedef struct Int16MultiArray {
  MultiArrayLayout_t layout;
  int16_t *data;

} Int16MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int16MultiArray_packed {
  MultiArrayLayout_t layout;
  int16_t *data;

} __attribute__((__packed__)) Int16MultiArray_t_packed;

Int16MultiArray_t *
Int16MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Int16MultiArray_t *
Int16MultiArray_deserialize(char *buf, size_t n);

size_t
Int16MultiArray_serialize(Int16MultiArray_t *obj, char *buf, size_t n);

#endif

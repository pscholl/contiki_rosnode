#ifndef _ROS_STD_MSGS_INT8MULTIARRAY_H_
# define _ROS_STD_MSGS_INT8MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Int8MultiArray_md5 ("d7c1af35a1b4781bbe79e03dd94b7c13")
#define Int8MultiArray_rostype ("std_msgs/Int8MultiArray")



typedef struct Int8MultiArray {
  MultiArrayLayout_t layout;
  int8_t *data;

} Int8MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int8MultiArray_packed {
  MultiArrayLayout_t layout;
  int8_t *data;

} __attribute__((__packed__)) Int8MultiArray_t_packed;

Int8MultiArray_t *
Int8MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Int8MultiArray_t *
Int8MultiArray_deserialize(char *buf, size_t n);

size_t
Int8MultiArray_serialize(Int8MultiArray_t *obj, char *buf, size_t n);

#endif

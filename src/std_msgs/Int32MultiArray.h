#ifndef _ROS_STD_MSGS_INT32MULTIARRAY_H_
# define _ROS_STD_MSGS_INT32MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Int32MultiArray_md5 ("1d99f79f8b325b44fee908053e9c945b")
#define Int32MultiArray_rostype ("std_msgs/Int32MultiArray")



typedef struct Int32MultiArray {
  MultiArrayLayout_t layout;
  int32_t *data;

} Int32MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int32MultiArray_packed {
  MultiArrayLayout_t layout;
  int32_t *data;

} __attribute__((__packed__)) Int32MultiArray_t_packed;

Int32MultiArray_t *
Int32MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Int32MultiArray_t *
Int32MultiArray_deserialize(char *buf, size_t n);

size_t
Int32MultiArray_serialize(Int32MultiArray_t *obj, char *buf, size_t n);

#endif

#ifndef _ROS_STD_MSGS_UINT32MULTIARRAY_H_
# define _ROS_STD_MSGS_UINT32MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define UInt32MultiArray_md5 ("4d6a180abc9be191b96a7eda6c8a233d")
#define UInt32MultiArray_rostype ("std_msgs/UInt32MultiArray")

typedef struct __attribute__((__packed__)) UInt32MultiArray {
  MultiArrayLayout_t layout;
  uint32_t *data;

} UInt32MultiArray_t;


UInt32MultiArray_t *
UInt32MultiArray_deserialize_size(char *buf, char *to, size_t *n);

UInt32MultiArray_t *
UInt32MultiArray_deserialize(char *buf, size_t n);

size_t
UInt32MultiArray_serialize(UInt32MultiArray_t *obj, char *buf, size_t n);

#endif

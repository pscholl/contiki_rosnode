#ifndef _ROS_STD_MSGS_UINT64MULTIARRAY_H_
# define _ROS_STD_MSGS_UINT64MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define UInt64MultiArray_md5 ("6088f127afb1d6c72927aa1247e945af")
#define UInt64MultiArray_rostype ("std_msgs/UInt64MultiArray")



typedef struct UInt64MultiArray {
  MultiArrayLayout_t layout;
  uint64_t *data;

} UInt64MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt64MultiArray_packed {
  MultiArrayLayout_t layout;
  uint64_t *data;

} __attribute__((__packed__)) UInt64MultiArray_t_packed;

UInt64MultiArray_t *
UInt64MultiArray_deserialize_size(char *buf, char *to, size_t *n);

UInt64MultiArray_t *
UInt64MultiArray_deserialize(char *buf, size_t n);

size_t
UInt64MultiArray_serialize(UInt64MultiArray_t *obj, char *buf, size_t n);

#endif

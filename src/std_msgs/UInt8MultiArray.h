#ifndef _ROS_STD_MSGS_UINT8MULTIARRAY_H_
# define _ROS_STD_MSGS_UINT8MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define UInt8MultiArray_md5 ("82373f1612381bb6ee473b5cd6f5d89c")
#define UInt8MultiArray_rostype ("std_msgs/UInt8MultiArray")



typedef struct UInt8MultiArray {
  MultiArrayLayout_t layout;
  uint8_t *data;

} UInt8MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt8MultiArray_packed {
  MultiArrayLayout_t layout;
  uint8_t *data;

} __attribute__((__packed__)) UInt8MultiArray_t_packed;

UInt8MultiArray_t *
UInt8MultiArray_deserialize_size(char *buf, char *to, size_t *n);

UInt8MultiArray_t *
UInt8MultiArray_deserialize(char *buf, size_t n);

size_t
UInt8MultiArray_serialize(UInt8MultiArray_t *obj, char *buf, size_t n);

#endif

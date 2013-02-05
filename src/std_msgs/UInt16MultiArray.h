#ifndef _ROS_STD_MSGS_UINT16MULTIARRAY_H_
# define _ROS_STD_MSGS_UINT16MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define UInt16MultiArray_md5 ("52f264f1c973c4b73790d384c6cb4484")
#define UInt16MultiArray_rostype ("std_msgs/UInt16MultiArray")



typedef struct UInt16MultiArray {
  MultiArrayLayout_t layout;
  uint16_t *data;

} UInt16MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt16MultiArray_packed {
  MultiArrayLayout_t layout;
  uint16_t *data;

} __attribute__((__packed__)) UInt16MultiArray_t_packed;

UInt16MultiArray_t *
UInt16MultiArray_deserialize_size(char *buf, char *to, size_t *n);

UInt16MultiArray_t *
UInt16MultiArray_deserialize(char *buf, size_t n);

size_t
UInt16MultiArray_serialize(UInt16MultiArray_t *obj, char *buf, size_t n);

#endif

#ifndef _ROS_STD_MSGS_BYTEMULTIARRAY_H_
# define _ROS_STD_MSGS_BYTEMULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define ByteMultiArray_md5 ("70ea476cbcfd65ac2f68f3cda1e891fe")
#define ByteMultiArray_rostype ("std_msgs/ByteMultiArray")



typedef struct ByteMultiArray {
  MultiArrayLayout_t layout;
  int8_t *data;

} ByteMultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct ByteMultiArray_packed {
  MultiArrayLayout_t layout;
  int8_t *data;

} __attribute__((__packed__)) ByteMultiArray_t_packed;

ByteMultiArray_t *
ByteMultiArray_deserialize_size(char *buf, char *to, size_t *n);

ByteMultiArray_t *
ByteMultiArray_deserialize(char *buf, size_t n);

size_t
ByteMultiArray_serialize(ByteMultiArray_t *obj, char *buf, size_t n);

#endif

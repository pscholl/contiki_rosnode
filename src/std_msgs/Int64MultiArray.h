#ifndef _ROS_STD_MSGS_INT64MULTIARRAY_H_
# define _ROS_STD_MSGS_INT64MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Int64MultiArray_md5 ("54865aa6c65be0448113a2afc6a49270")
#define Int64MultiArray_rostype ("std_msgs/Int64MultiArray")



typedef struct Int64MultiArray {
  MultiArrayLayout_t layout;
  int64_t *data;

} Int64MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int64MultiArray_packed {
  MultiArrayLayout_t layout;
  int64_t *data;

} __attribute__((__packed__)) Int64MultiArray_t_packed;

Int64MultiArray_t *
Int64MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Int64MultiArray_t *
Int64MultiArray_deserialize(char *buf, size_t n);

size_t
Int64MultiArray_serialize(Int64MultiArray_t *obj, char *buf, size_t n);

#endif

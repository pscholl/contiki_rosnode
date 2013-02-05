#ifndef _ROS_STD_MSGS_FLOAT64MULTIARRAY_H_
# define _ROS_STD_MSGS_FLOAT64MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Float64MultiArray_md5 ("4b7d974086d4060e7db4613a7e6c3ba4")
#define Float64MultiArray_rostype ("std_msgs/Float64MultiArray")



typedef struct Float64MultiArray {
  MultiArrayLayout_t layout;
  float *data;

} Float64MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Float64MultiArray_packed {
  MultiArrayLayout_t layout;
  float *data;

} __attribute__((__packed__)) Float64MultiArray_t_packed;

Float64MultiArray_t *
Float64MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Float64MultiArray_t *
Float64MultiArray_deserialize(char *buf, size_t n);

size_t
Float64MultiArray_serialize(Float64MultiArray_t *obj, char *buf, size_t n);

#endif

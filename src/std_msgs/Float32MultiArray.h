#ifndef _ROS_STD_MSGS_FLOAT32MULTIARRAY_H_
# define _ROS_STD_MSGS_FLOAT32MULTIARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayLayout.h>


#define Float32MultiArray_md5 ("6a40e0ffa6a17a503ac3f8616991b1f6")
#define Float32MultiArray_rostype ("std_msgs/Float32MultiArray")



typedef struct Float32MultiArray {
  MultiArrayLayout_t layout;
  float *data;

} Float32MultiArray_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Float32MultiArray_packed {
  MultiArrayLayout_t layout;
  float *data;

} __attribute__((__packed__)) Float32MultiArray_t_packed;

Float32MultiArray_t *
Float32MultiArray_deserialize_size(char *buf, char *to, size_t *n);

Float32MultiArray_t *
Float32MultiArray_deserialize(char *buf, size_t n);

size_t
Float32MultiArray_serialize(Float32MultiArray_t *obj, char *buf, size_t n);

#endif

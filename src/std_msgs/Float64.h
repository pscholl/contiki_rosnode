#ifndef _ROS_STD_MSGS_FLOAT64_H_
# define _ROS_STD_MSGS_FLOAT64_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Float64_md5 ("fdb28210bfa9d7c91146260178d9a584")
#define Float64_rostype ("std_msgs/Float64")



typedef struct Float64 {
  float data;

} Float64_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Float64_packed {
  float data;

} __attribute__((__packed__)) Float64_t_packed;

Float64_t *
Float64_deserialize_size(char *buf, char *to, size_t *n);

Float64_t *
Float64_deserialize(char *buf, size_t n);

size_t
Float64_serialize(Float64_t *obj, char *buf, size_t n);

#endif

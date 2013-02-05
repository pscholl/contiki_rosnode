#ifndef _ROS_STD_MSGS_FLOAT32_H_
# define _ROS_STD_MSGS_FLOAT32_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Float32_md5 ("73fcbf46b49191e672908e50842a83d4")
#define Float32_rostype ("std_msgs/Float32")



typedef struct Float32 {
  float data;

} Float32_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Float32_packed {
  float data;

} __attribute__((__packed__)) Float32_t_packed;

Float32_t *
Float32_deserialize_size(char *buf, char *to, size_t *n);

Float32_t *
Float32_deserialize(char *buf, size_t n);

size_t
Float32_serialize(Float32_t *obj, char *buf, size_t n);

#endif

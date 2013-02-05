#ifndef _ROS_STD_MSGS_INT64_H_
# define _ROS_STD_MSGS_INT64_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Int64_md5 ("34add168574510e6e17f5d23ecc077ef")
#define Int64_rostype ("std_msgs/Int64")



typedef struct Int64 {
  int64_t data;

} Int64_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int64_packed {
  int64_t data;

} __attribute__((__packed__)) Int64_t_packed;

Int64_t *
Int64_deserialize_size(char *buf, char *to, size_t *n);

Int64_t *
Int64_deserialize(char *buf, size_t n);

size_t
Int64_serialize(Int64_t *obj, char *buf, size_t n);

#endif

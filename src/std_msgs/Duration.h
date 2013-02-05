#ifndef _ROS_STD_MSGS_DURATION_H_
# define _ROS_STD_MSGS_DURATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Duration_md5 ("3e286caf4241d664e55f3ad380e2ae46")
#define Duration_rostype ("std_msgs/Duration")

typedef struct Duration {
  int32_t secs;
  int32_t nanosecs;

} Duration_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Duration_packed {
  int32_t secs;
  int32_t nanosecs;

} __attribute__((__packed__)) Duration_t_packed;

Duration_t *
Duration_deserialize_size(char *buf, char *to, size_t *n);

Duration_t *
Duration_deserialize(char *buf, size_t n);

size_t
Duration_serialize(Duration_t *obj, char *buf, size_t n);

#endif

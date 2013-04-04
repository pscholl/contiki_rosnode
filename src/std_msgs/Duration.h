#ifndef _ROS_STD_MSGS_DURATION_H_
# define _ROS_STD_MSGS_DURATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define duration_md5 ("3e286caf4241d664e55f3ad380e2ae46")
#define duration_rostype ("std_msgs/duration")



typedef struct duration {
  int32_t secs;
  int32_t nanosecs;

} duration_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct duration_packed {
  int32_t secs;
  int32_t nanosecs;

} __attribute__((__packed__)) duration_t_packed;

duration_t *
duration_deserialize_size(char *buf, char *to, size_t *n);

duration_t *
duration_deserialize(char *buf, size_t n);

size_t
duration_serialize(duration_t *obj, char *buf, size_t n);

#endif

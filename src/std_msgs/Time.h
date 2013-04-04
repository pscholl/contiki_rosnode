#ifndef _ROS_STD_MSGS_TIME_H_
# define _ROS_STD_MSGS_TIME_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define time_md5 ("cd7166c74c552c311fbcc2fe5a7bc289")
#define time_rostype ("std_msgs/time")



typedef struct time {
  int32_t secs;
  int32_t nanosecs;

} time_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct time_packed {
  int32_t secs;
  int32_t nanosecs;

} __attribute__((__packed__)) time_t_packed;

time_t *
time_deserialize_size(char *buf, char *to, size_t *n);

time_t *
time_deserialize(char *buf, size_t n);

size_t
time_serialize(time_t *obj, char *buf, size_t n);

#endif

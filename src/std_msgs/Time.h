#ifndef _ROS_STD_MSGS_TIME_H_
# define _ROS_STD_MSGS_TIME_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Time_md5 ("cd7166c74c552c311fbcc2fe5a7bc289")
#define Time_rostype ("std_msgs/Time")

typedef struct Time {
  int32_t secs;
  int32_t nanosecs;

} Time_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Time_packed {
  int32_t secs;
  int32_t nanosecs;

} __attribute__((__packed__)) Time_t_packed;

Time_t *
Time_deserialize_size(char *buf, char *to, size_t *n);

Time_t *
Time_deserialize(char *buf, size_t n);

size_t
Time_serialize(Time_t *obj, char *buf, size_t n);

#endif

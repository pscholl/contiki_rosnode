#ifndef _ROS_ROSSERIAL_MSGS_LOG_H_
# define _ROS_ROSSERIAL_MSGS_LOG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Log_md5 ("7170d5aec999754ba0d9f762bf49b913")
#define Log_rostype ("rosserial_msgs/Log")

typedef struct Log {
  uint8_t level;
  char *msg;

} Log_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Log_packed {
  uint8_t level;
  char *msg;

} __attribute__((__packed__)) Log_t_packed;

Log_t *
Log_deserialize_size(char *buf, char *to, size_t *n);

Log_t *
Log_deserialize(char *buf, size_t n);

size_t
Log_serialize(Log_t *obj, char *buf, size_t n);

#endif

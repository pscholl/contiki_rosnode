#ifndef _ROS_STD_MSGS_INT16_H_
# define _ROS_STD_MSGS_INT16_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Int16_md5 ("8524586e34fbd7cb1c08c5f5f1ca0e57")
#define Int16_rostype ("std_msgs/Int16")



typedef struct Int16 {
  int16_t data;

} Int16_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int16_packed {
  int16_t data;

} __attribute__((__packed__)) Int16_t_packed;

Int16_t *
Int16_deserialize_size(char *buf, char *to, size_t *n);

Int16_t *
Int16_deserialize(char *buf, size_t n);

size_t
Int16_serialize(Int16_t *obj, char *buf, size_t n);

#endif

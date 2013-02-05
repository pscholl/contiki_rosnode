#ifndef _ROS_STD_MSGS_INT32_H_
# define _ROS_STD_MSGS_INT32_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Int32_md5 ("da5909fbe378aeaf85e547e830cc1bb7")
#define Int32_rostype ("std_msgs/Int32")



typedef struct Int32 {
  int32_t data;

} Int32_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Int32_packed {
  int32_t data;

} __attribute__((__packed__)) Int32_t_packed;

Int32_t *
Int32_deserialize_size(char *buf, char *to, size_t *n);

Int32_t *
Int32_deserialize(char *buf, size_t n);

size_t
Int32_serialize(Int32_t *obj, char *buf, size_t n);

#endif

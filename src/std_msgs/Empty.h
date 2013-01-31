#ifndef _ROS_STD_MSGS_EMPTY_H_
# define _ROS_STD_MSGS_EMPTY_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Empty_md5 ("d41d8cd98f00b204e9800998ecf8427e")
#define Empty_rostype ("std_msgs/Empty")

typedef struct __attribute__((__packed__)) Empty {

} Empty_t;


Empty_t *
Empty_deserialize_size(char *buf, char *to, size_t *n);

Empty_t *
Empty_deserialize(char *buf, size_t n);

size_t
Empty_serialize(Empty_t *obj, char *buf, size_t n);

#endif

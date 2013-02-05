#ifndef _ROS_STD_MSGS_UINT32_H_
# define _ROS_STD_MSGS_UINT32_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define UInt32_md5 ("304a39449588c7f8ce2df6e8001c5fce")
#define UInt32_rostype ("std_msgs/UInt32")



typedef struct UInt32 {
  uint32_t data;

} UInt32_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt32_packed {
  uint32_t data;

} __attribute__((__packed__)) UInt32_t_packed;

UInt32_t *
UInt32_deserialize_size(char *buf, char *to, size_t *n);

UInt32_t *
UInt32_deserialize(char *buf, size_t n);

size_t
UInt32_serialize(UInt32_t *obj, char *buf, size_t n);

#endif

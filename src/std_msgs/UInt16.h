#ifndef _ROS_STD_MSGS_UINT16_H_
# define _ROS_STD_MSGS_UINT16_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define UInt16_md5 ("1df79edf208b629fe6b81923a544552d")
#define UInt16_rostype ("std_msgs/UInt16")



typedef struct UInt16 {
  uint16_t data;

} UInt16_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt16_packed {
  uint16_t data;

} __attribute__((__packed__)) UInt16_t_packed;

UInt16_t *
UInt16_deserialize_size(char *buf, char *to, size_t *n);

UInt16_t *
UInt16_deserialize(char *buf, size_t n);

size_t
UInt16_serialize(UInt16_t *obj, char *buf, size_t n);

#endif

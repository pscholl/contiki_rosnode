#ifndef _ROS_STD_MSGS_UINT64_H_
# define _ROS_STD_MSGS_UINT64_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define UInt64_md5 ("1b2a79973e8bf53d7b53acb71299cb57")
#define UInt64_rostype ("std_msgs/UInt64")



typedef struct UInt64 {
  uint64_t data;

} UInt64_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt64_packed {
  uint64_t data;

} __attribute__((__packed__)) UInt64_t_packed;

UInt64_t *
UInt64_deserialize_size(char *buf, char *to, size_t *n);

UInt64_t *
UInt64_deserialize(char *buf, size_t n);

size_t
UInt64_serialize(UInt64_t *obj, char *buf, size_t n);

#endif

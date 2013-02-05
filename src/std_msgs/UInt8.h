#ifndef _ROS_STD_MSGS_UINT8_H_
# define _ROS_STD_MSGS_UINT8_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define UInt8_md5 ("7c8164229e7d2c17eb95e9231617fdee")
#define UInt8_rostype ("std_msgs/UInt8")



typedef struct UInt8 {
  uint8_t data;

} UInt8_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct UInt8_packed {
  uint8_t data;

} __attribute__((__packed__)) UInt8_t_packed;

UInt8_t *
UInt8_deserialize_size(char *buf, char *to, size_t *n);

UInt8_t *
UInt8_deserialize(char *buf, size_t n);

size_t
UInt8_serialize(UInt8_t *obj, char *buf, size_t n);

#endif

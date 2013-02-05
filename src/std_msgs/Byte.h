#ifndef _ROS_STD_MSGS_BYTE_H_
# define _ROS_STD_MSGS_BYTE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Byte_md5 ("ad736a2e8818154c487bb80fe42ce43b")
#define Byte_rostype ("std_msgs/Byte")



typedef struct Byte {
  int8_t data;

} Byte_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Byte_packed {
  int8_t data;

} __attribute__((__packed__)) Byte_t_packed;

Byte_t *
Byte_deserialize_size(char *buf, char *to, size_t *n);

Byte_t *
Byte_deserialize(char *buf, size_t n);

size_t
Byte_serialize(Byte_t *obj, char *buf, size_t n);

#endif

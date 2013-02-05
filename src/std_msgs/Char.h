#ifndef _ROS_STD_MSGS_CHAR_H_
# define _ROS_STD_MSGS_CHAR_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Char_md5 ("1bf77f25acecdedba0e224b162199717")
#define Char_rostype ("std_msgs/Char")



typedef struct Char {
  uint8_t data;

} Char_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Char_packed {
  uint8_t data;

} __attribute__((__packed__)) Char_t_packed;

Char_t *
Char_deserialize_size(char *buf, char *to, size_t *n);

Char_t *
Char_deserialize(char *buf, size_t n);

size_t
Char_serialize(Char_t *obj, char *buf, size_t n);

#endif

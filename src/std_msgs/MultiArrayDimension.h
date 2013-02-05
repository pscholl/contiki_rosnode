#ifndef _ROS_STD_MSGS_MULTIARRAYDIMENSION_H_
# define _ROS_STD_MSGS_MULTIARRAYDIMENSION_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define MultiArrayDimension_md5 ("4cd0c83a8683deae40ecdac60e53bfa8")
#define MultiArrayDimension_rostype ("std_msgs/MultiArrayDimension")



typedef struct MultiArrayDimension {
  char *label;
  uint32_t size;
  uint32_t stride;

} MultiArrayDimension_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct MultiArrayDimension_packed {
  char *label;
  uint32_t size;
  uint32_t stride;

} __attribute__((__packed__)) MultiArrayDimension_t_packed;

MultiArrayDimension_t *
MultiArrayDimension_deserialize_size(char *buf, char *to, size_t *n);

MultiArrayDimension_t *
MultiArrayDimension_deserialize(char *buf, size_t n);

size_t
MultiArrayDimension_serialize(MultiArrayDimension_t *obj, char *buf, size_t n);

#endif

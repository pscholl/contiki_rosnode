#ifndef _ROS_STD_MSGS_MULTIARRAYLAYOUT_H_
# define _ROS_STD_MSGS_MULTIARRAYLAYOUT_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/MultiArrayDimension.h>


#define MultiArrayLayout_md5 ("0fed2a11c13e11c5571b4e2a995a91a3")
#define MultiArrayLayout_rostype ("std_msgs/MultiArrayLayout")



typedef struct MultiArrayLayout {
  MultiArrayDimension_t **dim;
  uint32_t data_offset;

} MultiArrayLayout_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct MultiArrayLayout_packed {
  MultiArrayDimension_t **dim;
  uint32_t data_offset;

} __attribute__((__packed__)) MultiArrayLayout_t_packed;

MultiArrayLayout_t *
MultiArrayLayout_deserialize_size(char *buf, char *to, size_t *n);

MultiArrayLayout_t *
MultiArrayLayout_deserialize(char *buf, size_t n);

size_t
MultiArrayLayout_serialize(MultiArrayLayout_t *obj, char *buf, size_t n);

#endif

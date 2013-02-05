#ifndef _ROS_STD_MSGS_HEADER_H_
# define _ROS_STD_MSGS_HEADER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <std_msgs/Time.h>


#define Header_md5 ("2176decaecbce78abc3b96ef049fabed")
#define Header_rostype ("std_msgs/Header")



typedef struct Header {
  uint32_t seq;
  time_t stamp;
  char *frame_id;

} Header_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct Header_packed {
  uint32_t seq;
  time_t stamp;
  char *frame_id;

} __attribute__((__packed__)) Header_t_packed;

Header_t *
Header_deserialize_size(char *buf, char *to, size_t *n);

Header_t *
Header_deserialize(char *buf, size_t n);

size_t
Header_serialize(Header_t *obj, char *buf, size_t n);

#endif

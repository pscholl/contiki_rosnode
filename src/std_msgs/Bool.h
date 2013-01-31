#ifndef _ROS_STD_MSGS_BOOL_H_
# define _ROS_STD_MSGS_BOOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define Bool_md5 ("8b94c1b53db61fb6aed406028ad6332a")
#define Bool_rostype ("std_msgs/Bool")

typedef struct __attribute__((__packed__)) Bool {
  bool data;

} Bool_t;


Bool_t *
Bool_deserialize_size(char *buf, char *to, size_t *n);

Bool_t *
Bool_deserialize(char *buf, size_t n);

size_t
Bool_serialize(Bool_t *obj, char *buf, size_t n);

#endif

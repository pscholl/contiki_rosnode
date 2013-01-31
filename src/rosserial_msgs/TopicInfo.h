#ifndef _ROS_ROSSERIAL_MSGS_TOPICINFO_H_
# define _ROS_ROSSERIAL_MSGS_TOPICINFO_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define TopicInfo_md5 ("63aa5e8f1bdd6f35c69fe1a1b9d28e9f")
#define TopicInfo_rostype ("rosserial_msgs/TopicInfo")

typedef struct __attribute__((__packed__)) TopicInfo {
  uint16_t topic_id;
  char *topic_name;
  char *message_type;
  char *md5sum;
  int32_t buffer_size;
} TopicInfo_t;


TopicInfo_t *
TopicInfo_deserialize_size(char *buf, char *to, size_t *n);

TopicInfo_t *
TopicInfo_deserialize(char *buf, size_t n);

size_t
TopicInfo_serialize(TopicInfo_t *obj, char *buf, size_t n);

#endif

#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "rosserial_msgs/TopicInfo.h"

TopicInfo_t*
TopicInfo_deserialize(char *buf, size_t n) {
  return TopicInfo_deserialize_size(buf,NULL,&n);
}

TopicInfo_t*
TopicInfo_deserialize_size(char *buf, char *to, size_t *n)
{
  TopicInfo_t  obj_header, *obj=&obj_header;

  uint32_t grow_len = 0,
           tmp;

  char *save_ptr=buf,
       *var_ptr=NULL;

  // deserialize in three steps:
  //  1. copy the header information onto the stack
  //  2. memmove var-len members at the end of buf
  //  3. memcpy the header back into place
  // return a pointer to the readable structure in buf

  if (*n < sizeof(*obj))
    return NULL; // check if header fits

  obj->topic_id = ROS_READ16(buf); buf+=2;
  obj->topic_name = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  obj->message_type = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  obj->md5sum = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  obj->buffer_size = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(TopicInfo_t)-sizeof(TopicInfo_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(TopicInfo_t)-sizeof(TopicInfo_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(TopicInfo_t)-sizeof(TopicInfo_t_packed));
  else          var_ptr = to;

  buf -= sizeof(int32_t);
  tmp = ROS_READ32(obj->md5sum);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->md5sum = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  tmp = ROS_READ32(obj->message_type);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->message_type = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  tmp = ROS_READ32(obj->topic_name);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->topic_name = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  buf -= sizeof(uint16_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(TopicInfo_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (TopicInfo_t*) save_ptr;
}

size_t
TopicInfo_serialize(TopicInfo_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE16(buf,obj->topic_id); buf+=2;
  tmp = obj->topic_name==NULL ? 0 : strlen(obj->topic_name);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->topic_name, tmp);
  buf += tmp;
  tmp = obj->message_type==NULL ? 0 : strlen(obj->message_type);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->message_type, tmp);
  buf += tmp;
  tmp = obj->md5sum==NULL ? 0 : strlen(obj->md5sum);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->md5sum, tmp);
  buf += tmp;
  ROS_WRITE32(buf,obj->buffer_size); buf+=4;


  return buf-save_ptr;
}

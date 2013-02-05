#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/Time.h"


Time_t*
Time_deserialize(char *buf, size_t n) {
  return Time_deserialize_size(buf,NULL,&n);
}

Time_t*
Time_deserialize_size(char *buf, char *to, size_t *n)
{
  Time_t  obj_header, *obj=&obj_header;

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

  obj->secs = ROS_READ32(buf); buf+=4;
  obj->nanosecs = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr)+grow_len)
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr)+grow_len;

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(Time_t)-sizeof(Time_t_packed));
  else          var_ptr = to;

  buf -= sizeof(int32_t);
  buf -= sizeof(int32_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(Time_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (Time_t*) save_ptr;
}

size_t
Time_serialize(Time_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE32(buf,obj->secs); buf+=4;
  ROS_WRITE32(buf,obj->nanosecs); buf+=4;


  return buf-save_ptr;
}

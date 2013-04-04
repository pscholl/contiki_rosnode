#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "rosserial_msgs/Log.h"

Log_t*
Log_deserialize(char *buf, size_t n) {
  return Log_deserialize_size(buf,NULL,&n);
}

Log_t*
Log_deserialize_size(char *buf, char *to, size_t *n)
{
  Log_t  obj_header, *obj=&obj_header;

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

  obj->level = ROS_READ8(buf); buf+=1;
  obj->msg = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(Log_t)-sizeof(Log_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(Log_t)-sizeof(Log_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(Log_t)-sizeof(Log_t_packed));
  else          var_ptr = to;

  tmp = ROS_READ32(obj->msg);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->msg = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  buf -= sizeof(uint8_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(Log_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (Log_t*) save_ptr;
}

size_t
Log_serialize(Log_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE8(buf,obj->level); buf+=1;
  tmp = obj->msg==NULL ? 0 : strlen(obj->msg);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->msg, tmp);
  buf += tmp;


  return buf-save_ptr;
}

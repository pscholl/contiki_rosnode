#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/MultiArrayDimension.h"

MultiArrayDimension_t*
MultiArrayDimension_deserialize(char *buf, size_t n) {
  return MultiArrayDimension_deserialize_size(buf,NULL,&n);
}

MultiArrayDimension_t*
MultiArrayDimension_deserialize_size(char *buf, char *to, size_t *n)
{
  MultiArrayDimension_t  obj_header, *obj=&obj_header;

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

  obj->label = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  obj->size = ROS_READ32(buf); buf+=4;
  obj->stride = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(MultiArrayDimension_t)-sizeof(MultiArrayDimension_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(MultiArrayDimension_t)-sizeof(MultiArrayDimension_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(MultiArrayDimension_t)-sizeof(MultiArrayDimension_t_packed));
  else          var_ptr = to;

  buf -= sizeof(uint32_t);
  buf -= sizeof(uint32_t);
  tmp = ROS_READ32(obj->label);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->label = (char*) (var_ptr);
  buf -= sizeof(uint32_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(MultiArrayDimension_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (MultiArrayDimension_t*) save_ptr;
}

size_t
MultiArrayDimension_serialize(MultiArrayDimension_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  tmp = obj->label==NULL ? 0 : strlen(obj->label);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->label, tmp);
  buf += tmp;
  ROS_WRITE32(buf,obj->size); buf+=4;
  ROS_WRITE32(buf,obj->stride); buf+=4;


  return buf-save_ptr;
}

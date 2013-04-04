#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/Int64.h"

Int64_t*
Int64_deserialize(char *buf, size_t n) {
  return Int64_deserialize_size(buf,NULL,&n);
}

Int64_t*
Int64_deserialize_size(char *buf, char *to, size_t *n)
{
  Int64_t  obj_header, *obj=&obj_header;

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

  obj->data = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(Int64_t)-sizeof(Int64_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(Int64_t)-sizeof(Int64_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(Int64_t)-sizeof(Int64_t_packed));
  else          var_ptr = to;

  buf -= sizeof(int64_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(Int64_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (Int64_t*) save_ptr;
}

size_t
Int64_serialize(Int64_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE32(buf,obj->data); buf+=4;


  return buf-save_ptr;
}

#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/String.h"

String_t*
String_deserialize(char *buf, size_t n) {
  return String_deserialize_size(buf,NULL,&n);
}

String_t*
String_deserialize_size(char *buf, char *to, size_t *n)
{
  String_t  obj_header, *obj=&obj_header;

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

  obj->data = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(String_t)-sizeof(String_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(String_t)-sizeof(String_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(String_t)-sizeof(String_t_packed));
  else          var_ptr = to;

  tmp = ROS_READ32(obj->data);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->data = (char*) (var_ptr);
  buf -= sizeof(uint32_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(String_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (String_t*) save_ptr;
}

size_t
String_serialize(String_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  tmp = obj->data==NULL ? 0 : strlen(obj->data);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->data, tmp);
  buf += tmp;


  return buf-save_ptr;
}

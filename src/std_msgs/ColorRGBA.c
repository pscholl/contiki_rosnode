#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/ColorRGBA.h"

ColorRGBA_t*
ColorRGBA_deserialize(char *buf, size_t n) {
  return ColorRGBA_deserialize_size(buf,NULL,&n);
}

ColorRGBA_t*
ColorRGBA_deserialize_size(char *buf, char *to, size_t *n)
{
  ColorRGBA_t  obj_header, *obj=&obj_header;

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

  obj->r = ROS_READ32(buf); buf+=4;
  obj->g = ROS_READ32(buf); buf+=4;
  obj->b = ROS_READ32(buf); buf+=4;
  obj->a = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(ColorRGBA_t)-sizeof(ColorRGBA_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(ColorRGBA_t)-sizeof(ColorRGBA_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(ColorRGBA_t)-sizeof(ColorRGBA_t_packed));
  else          var_ptr = to;

  buf -= sizeof(float);
  buf -= sizeof(float);
  buf -= sizeof(float);
  buf -= sizeof(float);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(ColorRGBA_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (ColorRGBA_t*) save_ptr;
}

size_t
ColorRGBA_serialize(ColorRGBA_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE32(buf,obj->r); buf+=4;
  ROS_WRITE32(buf,obj->g); buf+=4;
  ROS_WRITE32(buf,obj->b); buf+=4;
  ROS_WRITE32(buf,obj->a); buf+=4;


  return buf-save_ptr;
}

#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "contiki_rosnode/Test.h"

Test_t*
Test_deserialize(char *buf, size_t n) {
  return Test_deserialize_size(buf,NULL,&n);
}

Test_t*
Test_deserialize_size(char *buf, char *to, size_t *n)
{
  Test_t  obj_header, *obj=&obj_header;

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

  obj->from_node = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  obj->to_node = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  for (size_t i=0; i<8; i++) {
    obj->quality[i] = ROS_READ8(buf); buf+=1;
  }
  obj->a = ROS_READ32(buf); buf+=4;
  obj->b = ROS_READ32(buf); buf+=4;
  obj->c = ROS_READ16(buf); buf+=2;
  obj->d = ROS_READ8(buf); buf+=1;
  tmp = ROS_READ32(buf);
  grow_len += sizeof(uint32_t);
  buf += sizeof(uint32_t);
  obj->i16arr = (int16_t*) buf;
  for (size_t i=0; i<tmp; i++) {
    buf += 2;
  }
  obj->x = ROS_READ32(buf); buf+=4;


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(Test_t)-sizeof(Test_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(Test_t)-sizeof(Test_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(Test_t)-sizeof(Test_t_packed));
  else          var_ptr = to;

  buf -= sizeof(int32_t);
  for (size_t i=roslen(obj->i16arr); i-->0;) {
    buf -= sizeof(int16_t);
    var_ptr -= sizeof(int16_t);
    *((int16_t*) (var_ptr)) = ROS_READ16(buf);
  }
  buf -= sizeof(uint32_t);
  *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->i16arr); // store arrlen
  obj->i16arr = (int16_t*) (var_ptr);
  var_ptr -= sizeof(uint32_t);
  buf -= sizeof(uint8_t);
  buf -= sizeof(uint16_t);
  buf -= sizeof(uint32_t);
  buf -= sizeof(uint32_t);
  buf -= sizeof(uint8_t)*8;
  tmp = ROS_READ32(obj->to_node);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->to_node = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  tmp = ROS_READ32(obj->from_node);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->from_node = (char*) (var_ptr);
  buf -= sizeof(uint32_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(Test_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (Test_t*) save_ptr;
}

size_t
Test_serialize(Test_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  tmp = obj->from_node==NULL ? 0 : strlen(obj->from_node);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->from_node, tmp);
  buf += tmp;
  tmp = obj->to_node==NULL ? 0 : strlen(obj->to_node);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->to_node, tmp);
  buf += tmp;
  for (size_t i=0; i<8; i++) {
    ROS_WRITE8(buf,obj->quality[i]); buf+=1;
  }
  ROS_WRITE32(buf,obj->a); buf+=4;
  ROS_WRITE32(buf,obj->b); buf+=4;
  ROS_WRITE16(buf,obj->c); buf+=2;
  ROS_WRITE8(buf,obj->d); buf+=1;
  tmp = obj->i16arr==NULL ? 0 : roslen(obj->i16arr);
  ROS_WRITE32(buf, tmp); buf+=4;
  for (size_t i=0; i<tmp; i++) {
    ROS_WRITE16(buf,obj->i16arr[i]); buf+=2;
  }
  ROS_WRITE32(buf,obj->x); buf+=4;


  return buf-save_ptr;
}

#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "std_msgs/UInt64MultiArray.h"

UInt64MultiArray_t*
UInt64MultiArray_deserialize(char *buf, size_t n) {
  return UInt64MultiArray_deserialize_size(buf,NULL,&n);
}

UInt64MultiArray_t*
UInt64MultiArray_deserialize_size(char *buf, char *to, size_t *n)
{
  UInt64MultiArray_t  obj_header, *obj=&obj_header;

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

    tmp = ROS_READ32(buf);
    grow_len += sizeof(uint32_t);
    buf += sizeof(uint32_t);
    /* allocate pointer space on stack */
    obj->layout.dim = (MultiArrayDimension_t**) alloca(sizeof(MultiArrayDimension_t*)*tmp + sizeof(uint32_t));
    *((uint32_t*)obj->layout.dim) = tmp;
    obj->layout.dim = (MultiArrayDimension_t**) (((uint32_t*) obj->layout.dim) + 1);
    for (size_t j=0; j<tmp; j++) {
      grow_len += sizeof(MultiArrayDimension_t) - sizeof(MultiArrayDimension_t_packed);
      obj->layout.dim[j] = (MultiArrayDimension_t*)buf;
      grow_len += sizeof(MultiArrayDimension_t*); // allocate storage space
      buf += ROS_READ32(buf) + sizeof(uint32_t);
      grow_len += 1;
      buf += sizeof(uint32_t);
      buf += sizeof(uint32_t);
    }
    obj->layout.data_offset = ROS_READ32(buf); buf+=4;
  tmp = ROS_READ32(buf);
  grow_len += sizeof(uint32_t);
  buf += sizeof(uint32_t);
  obj->data = (uint64_t*) buf;
  for (size_t i=0; i<tmp; i++) {
    buf += 4;
  }


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(UInt64MultiArray_t)-sizeof(UInt64MultiArray_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(UInt64MultiArray_t)-sizeof(UInt64MultiArray_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(UInt64MultiArray_t)-sizeof(UInt64MultiArray_t_packed));
  else          var_ptr = to;

  for (size_t i=roslen(obj->data); i-->0;) {
    buf -= sizeof(uint64_t);
    var_ptr -= sizeof(uint64_t);
    *((uint64_t*) (var_ptr)) = ROS_READ32(buf);
  }
  buf -= sizeof(uint32_t);
  *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->data); // store arrlen
  obj->data = (uint64_t*) (var_ptr);
  var_ptr -= sizeof(uint32_t);
    buf -= sizeof(uint32_t);
    for (size_t j=roslen(obj->layout.dim); j-->0;) {
      uint32_t m = *n;
      buf = (uint8_t*) obj->layout.dim[j];
      var_ptr = (uint8_t*) MultiArrayDimension_deserialize_size(buf,var_ptr,&m);
      obj->layout.dim[j] = (MultiArrayDimension_t*) var_ptr;
    }
    buf -= sizeof(uint32_t);
    var_ptr -= sizeof(MultiArrayDimension_t*) * roslen(obj->layout.dim);
    memcpy(var_ptr,obj->layout.dim,roslen(obj->layout.dim)*sizeof(MultiArrayDimension_t*));
    *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->layout.dim); // store arrlen
    obj->layout.dim = (MultiArrayDimension_t**) (var_ptr);
    var_ptr -= sizeof(uint32_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(UInt64MultiArray_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (UInt64MultiArray_t*) save_ptr;
}

size_t
UInt64MultiArray_serialize(UInt64MultiArray_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  buf += MultiArrayLayout_serialize(&obj->layout,buf,buf-save_ptr);
  tmp = obj->data==NULL ? 0 : roslen(obj->data);
  ROS_WRITE32(buf, tmp); buf+=4;
  for (size_t i=0; i<tmp; i++) {
    ROS_WRITE32(buf,obj->data[i]); buf+=4;
  }


  return buf-save_ptr;
}

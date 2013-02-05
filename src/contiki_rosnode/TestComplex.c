#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "contiki_rosnode/TestComplex.h"


TestComplex_t*
TestComplex_deserialize(char *buf, size_t n) {
  return TestComplex_deserialize_size(buf,NULL,&n);
}

TestComplex_t*
TestComplex_deserialize_size(char *buf, char *to, size_t *n)
{
  TestComplex_t  obj_header, *obj=&obj_header;

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

  obj->some_number = ROS_READ8(buf); buf+=1;
  for (size_t i=0; i<2; i++) {
      obj->test_static[i].from_node = buf;
      buf += ROS_READ32(buf) + sizeof(uint32_t);
      grow_len += 1;
      obj->test_static[i].to_node = buf;
      buf += ROS_READ32(buf) + sizeof(uint32_t);
      grow_len += 1;
      for (size_t j=0; j<8; j++) {
        obj->test_static[i].quality[j] = ROS_READ8(buf); buf+=1;
      }
      obj->test_static[i].a = ROS_READ32(buf); buf+=4;
      obj->test_static[i].b = ROS_READ32(buf); buf+=4;
      obj->test_static[i].c = ROS_READ16(buf); buf+=2;
      obj->test_static[i].d = ROS_READ8(buf); buf+=1;
      tmp = ROS_READ32(buf);
      grow_len += sizeof(uint32_t);
      buf += sizeof(uint32_t);
      obj->test_static[i].i16arr = (int16_t*) buf;
      for (size_t j=0; j<tmp; j++) {
        buf += 2;
      }
      obj->test_static[i].x = ROS_READ32(buf); buf+=4;
  }
  tmp = ROS_READ32(buf);
  grow_len += sizeof(uint32_t);
  buf += sizeof(uint32_t);
  /* allocate pointer space on stack */
  obj->test_dynamic = (Test_t**) alloca(sizeof(Test_t*)*tmp + sizeof(uint32_t));
  *((uint32_t*)obj->test_dynamic) = tmp;
  obj->test_dynamic = (Test_t**) (((uint32_t*) obj->test_dynamic) + 1);
  for (size_t i=0; i<tmp; i++) {
    grow_len += sizeof(Test_t) - sizeof(Test_t_packed);
    obj->test_dynamic[i] = (Test_t*)buf;
    grow_len += sizeof(Test_t*); // allocate storage space
    buf += ROS_READ32(buf) + sizeof(uint32_t);
    grow_len += 1;
    buf += ROS_READ32(buf) + sizeof(uint32_t);
    grow_len += 1;
    buf += 8 * sizeof(uint8_t);
    buf += sizeof(uint32_t);
    buf += sizeof(uint32_t);
    buf += sizeof(uint16_t);
    buf += sizeof(uint8_t);
    buf += ROS_READ32(buf) * sizeof(int16_t) + sizeof(uint32_t);
    grow_len += 4;
    buf += sizeof(int32_t);
  }
  obj->another_number = ROS_READ8(buf); buf+=1;
  obj->aString = buf;
  buf += ROS_READ32(buf) + sizeof(uint32_t);
  grow_len += 1;
  for (size_t i=0; i<2; i++) {
    obj->sStrings[i] = buf;
    buf += ROS_READ32(buf) + sizeof(uint32_t);
    grow_len += 1;
  }
  tmp = ROS_READ32(buf);
  grow_len += sizeof(uint32_t);
  buf += sizeof(uint32_t);
  /* allocate pointer space on stack */
  obj->dStrings = (char**) alloca(sizeof(char**)*tmp + sizeof(uint32_t));
  *((uint32_t*)obj->dStrings) = tmp;
  obj->dStrings = (char**) (((uint32_t*) obj->dStrings) + 1);
  for (size_t i=0; i<tmp; i++) {
    obj->dStrings[i] = buf;
    buf += ROS_READ32(buf) + sizeof(uint32_t);
    grow_len += 1;
  }


  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr) + grow_len + (sizeof(TestComplex_t)-sizeof(TestComplex_t_packed)))
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr) + grow_len + (sizeof(TestComplex_t)-sizeof(TestComplex_t_packed));

  if (to==NULL) var_ptr = buf + grow_len + (sizeof(TestComplex_t)-sizeof(TestComplex_t_packed));
  else          var_ptr = to;

  for (size_t i=roslen(obj->dStrings); i-->0;) {
    tmp = ROS_READ32(obj->dStrings[i]);
    buf -= tmp;
    var_ptr -= 1;
    *var_ptr = '\0';
    var_ptr -= tmp;
    memmove(var_ptr,buf,tmp);
    obj->dStrings[i] = (char*) (var_ptr);
    buf -= sizeof(uint32_t);
  }
  buf -= sizeof(uint32_t);
  var_ptr -= sizeof(char**) * roslen(obj->dStrings);
  memcpy(var_ptr,obj->dStrings,roslen(obj->dStrings)*sizeof(char**));
  *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->dStrings); // store arrlen
  obj->dStrings = (char**) (var_ptr);
  var_ptr -= sizeof(uint32_t);
  for (size_t i=2; i-->0;) {
    tmp = ROS_READ32(obj->sStrings[i]);
    buf -= tmp;
    var_ptr -= 1;
    *var_ptr = '\0';
    var_ptr -= tmp;
    memmove(var_ptr,buf,tmp);
    obj->sStrings[i] = (char*) (var_ptr);
    buf -= sizeof(uint32_t);
  }
  tmp = ROS_READ32(obj->aString);
  buf -= tmp;
  var_ptr -= 1;
  *var_ptr = '\0';
  var_ptr -= tmp;
  memmove(var_ptr,buf,tmp);
  obj->aString = (char*) (var_ptr);
  buf -= sizeof(uint32_t);
  buf -= sizeof(int8_t);
  for (size_t i=roslen(obj->test_dynamic); i-->0;) {
    uint32_t m = *n;
    buf = (uint8_t*) obj->test_dynamic[i];
    var_ptr = (uint8_t*) Test_deserialize_size(buf,var_ptr,&m);
    obj->test_dynamic[i] = (Test_t*) var_ptr;
  }
  buf -= sizeof(uint32_t);
  var_ptr -= sizeof(Test_t*) * roslen(obj->test_dynamic);
  memcpy(var_ptr,obj->test_dynamic,roslen(obj->test_dynamic)*sizeof(Test_t*));
  *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->test_dynamic); // store arrlen
  obj->test_dynamic = (Test_t**) (var_ptr);
  var_ptr -= sizeof(uint32_t);
  for (size_t i=2; i-->0;) {
      buf -= sizeof(int32_t);
      for (size_t j=roslen(obj->test_static[i].i16arr); j-->0;) {
        buf -= sizeof(int16_t);
        var_ptr -= sizeof(int16_t);
        *((int16_t*) (var_ptr)) = ROS_READ16(buf);
      }
      buf -= sizeof(uint32_t);
      *((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->test_static[i].i16arr); // store arrlen
      obj->test_static[i].i16arr = (int16_t*) (var_ptr);
      var_ptr -= sizeof(uint32_t);
      buf -= sizeof(uint8_t);
      buf -= sizeof(uint16_t);
      buf -= sizeof(uint32_t);
      buf -= sizeof(uint32_t);
      buf -= sizeof(uint8_t)*8;
      tmp = ROS_READ32(obj->test_static[i].to_node);
      buf -= tmp;
      var_ptr -= 1;
      *var_ptr = '\0';
      var_ptr -= tmp;
      memmove(var_ptr,buf,tmp);
      obj->test_static[i].to_node = (char*) (var_ptr);
      buf -= sizeof(uint32_t);
      tmp = ROS_READ32(obj->test_static[i].from_node);
      buf -= tmp;
      var_ptr -= 1;
      *var_ptr = '\0';
      var_ptr -= tmp;
      memmove(var_ptr,buf,tmp);
      obj->test_static[i].from_node = (char*) (var_ptr);
      buf -= sizeof(uint32_t);
  }
  buf -= sizeof(int8_t);


  if (to!=NULL)
      save_ptr = var_ptr - sizeof(TestComplex_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return (TestComplex_t*) save_ptr;
}

size_t
TestComplex_serialize(TestComplex_t *obj, char *buf, size_t n)
{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

  ROS_WRITE8(buf,obj->some_number); buf+=1;
  for (size_t i=0; i<2; i++) {
    buf += Test_serialize(&obj->test_static[i],buf,buf-save_ptr);
  }
  tmp = obj->test_dynamic==NULL ? 0 : roslen(obj->test_dynamic);
  ROS_WRITE32(buf, tmp); buf+=4;
  for (size_t i=0; i<tmp; i++) {
    buf += Test_serialize(obj->test_dynamic[i],buf,buf-save_ptr);
  }
  ROS_WRITE8(buf,obj->another_number); buf+=1;
  tmp = obj->aString==NULL ? 0 : strlen(obj->aString);
  ROS_WRITE32(buf, tmp); buf+=4;
  memcpy(buf, obj->aString, tmp);
  buf += tmp;
  for (size_t i=0; i<2; i++) {
    tmp = obj->sStrings[i]==NULL ? 0 : strlen(obj->sStrings[i]);
    ROS_WRITE32(buf, tmp); buf+=4;
    memcpy(buf, obj->sStrings[i], tmp);
    buf += tmp;
  }
  tmp = obj->dStrings==NULL ? 0 : roslen(obj->dStrings);
  ROS_WRITE32(buf, tmp); buf+=4;
  for (size_t i=0; i<tmp; i++) {
    tmp = obj->dStrings[i]==NULL ? 0 : strlen(obj->dStrings[i]);
    ROS_WRITE32(buf, tmp); buf+=4;
    memcpy(buf, obj->dStrings[i], tmp);
    buf += tmp;
  }


  return buf-save_ptr;
}

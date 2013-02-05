#ifndef _ROS_STD_MSGS_COLORRGBA_H_
# define _ROS_STD_MSGS_COLORRGBA_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define ColorRGBA_md5 ("a29a96539573343b1310c73607334b00")
#define ColorRGBA_rostype ("std_msgs/ColorRGBA")



typedef struct ColorRGBA {
  float r;
  float g;
  float b;
  float a;

} ColorRGBA_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct ColorRGBA_packed {
  float r;
  float g;
  float b;
  float a;

} __attribute__((__packed__)) ColorRGBA_t_packed;

ColorRGBA_t *
ColorRGBA_deserialize_size(char *buf, char *to, size_t *n);

ColorRGBA_t *
ColorRGBA_deserialize(char *buf, size_t n);

size_t
ColorRGBA_serialize(ColorRGBA_t *obj, char *buf, size_t n);

#endif

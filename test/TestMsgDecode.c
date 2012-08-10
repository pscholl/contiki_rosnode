/*
 * Copyright (c) 2012
 * Philipp Scholl, TU Darmstadt, Germany.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither the name of the Universitaet Karlsruhe (TH) nor the names
 *    of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * theory OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s): Philipp Scholl <scholl@ess.tu-darmstadt.de>
 */

#include <challenge_msgs/Test.h>
#include <ros.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void TestSimpleDeserial()
{
  uint8_t raw[] = {
    sizeof("abcd")-1,0,0,0, 'a','b','c','d',
    sizeof("efgh")-1,0,0,0, 'e','f','g','h',
    127,126,125,124,123,122,121,120,
    0xff,0,0,0,
    0xfe,0,0,0,
    0xde,0xad,
    0xbe,
    0x02,0,0,0, 0xde,0xad,0xbe,0xef,
    0x03,0,0,0,
    0,0,    // two additional bytes for unpacking strings
    0,0,0,0 // and four for the array
  };

  // test buffer boundary checking for object header
  assert (Test_deserialize(raw, sizeof(Test_t)-1)==NULL);
  // test buffer boundary checking for string growing
  assert (Test_deserialize(raw, sizeof(raw)-2)==NULL);

  // this should work!
  Test_t *obj = Test_deserialize(raw, sizeof(raw));
  assert (obj != NULL);

  assert (strlen(obj->from_node)==4);
  assert (strlen(obj->to_node)==4);
  assert (strncmp(obj->from_node,"abcd",4)==0);
  assert (strncmp(obj->to_node,"efgh",4)==0);
  assert (obj->quality[0] == 127);
  assert (obj->quality[1] == 126);
  assert (obj->quality[2] == 125);
  assert (obj->quality[3] == 124);
  assert (obj->quality[4] == 123);
  assert (obj->quality[5] == 122);
  assert (obj->quality[6] == 121);
  assert (obj->quality[7] == 120);
  assert (obj->a == 0xff);
  assert (obj->b == 0xfe);
  assert (obj->c == 0xadde);
  assert (obj->d == 0xbe);
  assert (roslen(obj->i16arr)==2);
  assert (obj->i16arr[0] == (int16_t) 0xadde);
  assert (obj->i16arr[1] == (int16_t) 0xefbe);
  assert (obj->x == 0x3);
}


void TestSimpleSerial()
{
  int16_t *arr = rosarr_n(int16_t,2),
           i,n;

  Test_t obj = {
    .from_node = "from_node_str",
    .to_node   = "to_node_str",
    .quality   = {1,2,3,4,5,6,7,8},
    .a = 0xdeadbeef,
    .b = 0xbeefdead,
    .c = 0xabcd,
    .d = 0x01,
    .i16arr = arr,
    .x = 32
  };

  uint8_t raw[] = {
    sizeof("from_node_str")-1,0,0,0, 'f','r','o','m','_','n','o','d','e','_','s','t','r',
    sizeof("to_node_str")-1,0,0,0, 't','o','_','n','o','d','e','_','s','t','r',
    1,2,3,4,5,6,7,8,
    0xef,0xbe,0xad,0xde,
    0xad,0xde,0xef,0xbe,
    0xcd,0xab,
    0x01,
    2,0,0,0, 1,0, 2,0,
    32,0,0,0,
  }, buf[200];


  for (i=0; i<2; i++)
    arr[i]=i+1;

  n=Test_serialize(&obj, buf, sizeof(buf));

  assert (n==sizeof(raw));

  for (i=0; i<n; i++) {
    if (buf[i]!=raw[i]) {
      printf ("buf[%d]=0x%x != 0x%x=raw[%d]\n",i,buf[i],raw[i],i);
    }
  }

  return;
}

void TestSimpleRoundtrip()
{
  uint8_t buf[200];

  Test_t obj = {
    .from_node = "from_node_str",
    .to_node   = "to_node_str",
    .quality   = {1,2,3,4,5,6,7,8},
    .a = 0xdeadbeef,
    .b = 0xbeefdead,
    .c = 0xabcd,
    .d = 0x01,
    .i16arr = rosarr(int16_t, 4,5,6,7,8,9,10),
    .x = 32
  };

  size_t n,i;
  Test_t *ds;

  n  = Test_serialize(&obj,buf,sizeof(buf)),
  ds = Test_deserialize(buf,sizeof(buf));

  assert (ds != NULL);

  assert (strlen(ds->from_node) == strlen(obj.from_node));
  assert (strncmp(ds->from_node,obj.from_node,strlen(ds->from_node))==0);

  assert (strlen(ds->to_node) == strlen(obj.to_node));
  assert (strncmp(ds->to_node,obj.to_node,strlen(ds->to_node))==0);

  for (i=0; i<(sizeof(obj.quality)/sizeof(*obj.quality)); i++)
    assert(ds->quality[i] == obj.quality[i]);

  assert (ds->a == obj.a);
  assert (ds->b == obj.b);
  assert (ds->c == obj.c);
  assert (ds->d == obj.d);

  assert (roslen(ds->i16arr)==roslen(obj.i16arr));
  for (i=0; i<7; i++)
    assert (ds->i16arr[i] == obj.i16arr[i]);

  assert (ds->x == obj.x);
}

int main (int argc, char const* argv[])
{
  TestSimpleDeserial();
  TestSimpleSerial();
  TestSimpleRoundtrip();
  //TestSimpleSerialDeserial();
  //TestComplexDeserial();

  return 0;
}

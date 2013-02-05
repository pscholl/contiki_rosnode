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

#include <contiki_rosnode/Test.h>
#include <contiki_rosnode/TestComplex.h>
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
    0,0,0,0, // and four for the array
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

  // test buffer boundary checking for object header
  assert (Test_deserialize(raw, sizeof(Test_t)-1)==NULL);
  // test buffer boundary checking for string growing
  //assert (Test_deserialize(raw, sizeof(raw)-2)==NULL);

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

void TestComplexRoundtrip()
{
  TestComplex_t obj = {
    .some_number = 8,
    .test_static = { {
      .from_node = "abc",
      .to_node   = "cde",
      .quality   = {1,2,3,4,5,6,7,8},
      .a         = 0xdeadbeef,
      .b         = 0xbeefdead,
      .c         = 0xbeef,
      .d         = 0xcd,
      .i16arr    = rosarr(int16_t, 1),
      .x         = 0xdeadbeef }, {

      .from_node = "second",
      .to_node   = "one",
      .quality   = {1,2,3,4,5,6,7,8},
      .a         = 0xdeadbeef,
      .b         = 0xbeefdead,
      .c         = 0xbeef,
      .d         = 0xcd,
      .i16arr    = rosarr(int16_t, 1,2,3),
      .x         = 0xdeadbeef } },
    .another_number = 16,

    .test_dynamic = rosarr(Test_t*, &(Test_t) {
      .from_node = "third",
      .to_node   = "one",
      .quality   = {1,2,3,4,5,6,7,8},
      .a         = 0xdeadbeef,
      .b         = 0xbeefdead,
      .c         = 0xbeef,
      .d         = 0xcd,
      .i16arr    = rosarr(int16_t, 1),
    }, &(Test_t) {
      .from_node = "fourth",
      .to_node   = "one",
      .quality   = {8,7,6,5,4,3,2,1},
      .a         = 0xdeadbeef,
      .b         = 0xbeefdead,
      .c         = 0xbeef,
      .d         = 0xcd,
      .i16arr    = rosarr(int16_t, 1,-12,12),
    }),

    .aString     = "mylittlestring",
    .sStrings    = { "numberone", "numbertwo"},
    .dStrings    = rosarr(char*, "abc", "cdejkl", ""),
  };

  TestComplex_t *ds;
  uint8_t buf[500];
  size_t i,j;

  i = TestComplex_serialize(&obj,buf,sizeof(buf));
  //printf("%d\n",i);
  assert(i==263);

  ds = TestComplex_deserialize(buf,sizeof(buf));
  assert (ds!=NULL);

  assert (ds->some_number == obj.some_number);

  assert (strlen(ds->test_static[0].from_node) == strlen(obj.test_static[0].from_node));
  assert (strncmp(ds->test_static[0].from_node,obj.test_static[0].from_node,strlen(ds->test_static[0].from_node))==0);
  assert (strlen(ds->test_static[0].to_node) == strlen(obj.test_static[0].to_node));
  assert (strncmp(ds->test_static[0].to_node,obj.test_static[0].to_node,strlen(ds->test_static[0].to_node))==0);
  for (i=0; i<8; i++)
    assert(ds->test_static[0].quality[i]==obj.test_static[0].quality[i]);
  assert (ds->test_static[0].a == obj.test_static[0].a);
  assert (ds->test_static[0].b == obj.test_static[0].b);
  assert (ds->test_static[0].c == obj.test_static[0].c);
  assert (ds->test_static[0].d == obj.test_static[0].d);
  assert (roslen(ds->test_static[0].i16arr) == roslen(obj.test_static[0].i16arr));
  for (i=0; i<roslen(ds->test_static[0].i16arr); i++)
    assert (ds->test_static[0].i16arr[i]==obj.test_static[0].i16arr[i]);
  assert (ds->test_static[0].x == obj.test_static[0].x);

  assert (strlen(ds->test_static[1].from_node) == strlen(obj.test_static[1].from_node));
  assert (strncmp(ds->test_static[1].from_node,obj.test_static[1].from_node,strlen(ds->test_static[1].from_node))==0);
  assert (strlen(ds->test_static[1].to_node) == strlen(obj.test_static[1].to_node));
  assert (strncmp(ds->test_static[1].to_node,obj.test_static[1].to_node,strlen(ds->test_static[1].to_node))==0);
  for (i=1; i<8; i++)
    assert(ds->test_static[1].quality[i]==obj.test_static[1].quality[i]);
  assert (ds->test_static[1].a == obj.test_static[1].a);
  assert (ds->test_static[1].b == obj.test_static[1].b);
  assert (ds->test_static[1].c == obj.test_static[1].c);
  assert (ds->test_static[1].d == obj.test_static[1].d);
  assert (roslen(ds->test_static[1].i16arr) == roslen(obj.test_static[1].i16arr));
  for (i=1; i<roslen(ds->test_static[1].i16arr); i++)
    assert (ds->test_static[1].i16arr[i]==obj.test_static[1].i16arr[i]);
  assert (ds->test_static[1].x == obj.test_static[1].x);

  //printf("%s %s\n",ds->sStrings[0],obj.sStrings[0]);
  assert (strlen(ds->sStrings[0]) == strlen(obj.sStrings[0]));
  assert (strncmp(ds->sStrings[0],obj.sStrings[0],strlen(ds->sStrings[0]))==0);
  assert (strlen(ds->sStrings[1]) == strlen(obj.sStrings[1]));
  assert (strncmp(ds->sStrings[1],obj.sStrings[1],strlen(ds->sStrings[1]))==0);

  assert (strlen(ds->dStrings[0]) == strlen(obj.dStrings[0]));
  assert (strncmp(ds->dStrings[0],obj.dStrings[0],strlen(ds->dStrings[0]))==0);
  assert (strlen(ds->dStrings[1]) == strlen(obj.dStrings[1]));
  assert (strncmp(ds->dStrings[1],obj.dStrings[1],strlen(ds->dStrings[1]))==0);
  assert (strlen(ds->dStrings[2]) == strlen(obj.dStrings[2]));
  assert (strncmp(ds->dStrings[2],obj.dStrings[2],strlen(ds->dStrings[2]))==0);

  assert (roslen(ds->test_dynamic)==roslen(obj.test_dynamic));
  assert (roslen(ds->test_dynamic)==2);

  assert (strlen(ds->test_dynamic[0]->from_node) == strlen(obj.test_dynamic[0]->from_node));
  assert (strncmp(ds->test_dynamic[0]->from_node,obj.test_dynamic[0]->from_node,strlen(ds->test_dynamic[0]->from_node))==0);
  assert (strlen(ds->test_dynamic[0]->to_node) == strlen(obj.test_dynamic[0]->to_node));
  assert (strncmp(ds->test_dynamic[0]->to_node,obj.test_dynamic[0]->to_node,strlen(ds->test_dynamic[0]->to_node))==0);
  for (i=0; i<8; i++)
    assert(ds->test_dynamic[0]->quality[i]==obj.test_dynamic[0]->quality[i]);
  assert (ds->test_dynamic[0]->a == obj.test_dynamic[0]->a);
  assert (ds->test_dynamic[0]->b == obj.test_dynamic[0]->b);
  assert (ds->test_dynamic[0]->c == obj.test_dynamic[0]->c);
  assert (ds->test_dynamic[0]->d == obj.test_dynamic[0]->d);
  assert (roslen(ds->test_dynamic[0]->i16arr) == roslen(obj.test_dynamic[0]->i16arr));
  for (i=0; i<roslen(ds->test_dynamic[0]->i16arr); i++)
    assert (ds->test_dynamic[0]->i16arr[i]==obj.test_dynamic[0]->i16arr[i]);
  assert (ds->test_dynamic[0]->x == obj.test_dynamic[0]->x);

  assert (strlen(ds->test_dynamic[1]->from_node) == strlen(obj.test_dynamic[1]->from_node));
  assert (strncmp(ds->test_dynamic[1]->from_node,obj.test_dynamic[1]->from_node,strlen(ds->test_dynamic[1]->from_node))==0);
  assert (strlen(ds->test_dynamic[1]->to_node) == strlen(obj.test_dynamic[1]->to_node));
  assert (strncmp(ds->test_dynamic[1]->to_node,obj.test_dynamic[1]->to_node,strlen(ds->test_dynamic[1]->to_node))==0);
  for (i=0; i<8; i++)
    assert(ds->test_dynamic[1]->quality[i]==obj.test_dynamic[1]->quality[i]);
  assert (ds->test_dynamic[1]->a == obj.test_dynamic[1]->a);
  assert (ds->test_dynamic[1]->b == obj.test_dynamic[1]->b);
  assert (ds->test_dynamic[1]->c == obj.test_dynamic[1]->c);
  assert (ds->test_dynamic[1]->d == obj.test_dynamic[1]->d);
  assert (roslen(ds->test_dynamic[1]->i16arr) == roslen(obj.test_dynamic[1]->i16arr));
  for (i=0; i<roslen(ds->test_dynamic[1]->i16arr); i++)
    assert (ds->test_dynamic[1]->i16arr[i]==obj.test_dynamic[1]->i16arr[i]);
  assert (ds->test_dynamic[1]->x == obj.test_dynamic[1]->x);
}

int main (int argc, char const* argv[])
{
  TestSimpleSerial();
  TestSimpleDeserial();
  TestSimpleRoundtrip();
  TestComplexRoundtrip();

  return 0;
}

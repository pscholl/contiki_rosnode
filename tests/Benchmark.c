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
#include <time.h>

double BenchmarkSimpleSerialize()
{
  uint8_t buf[40000];

  Test_t obj = {
    .from_node = "",
    .to_node   = "to_node_str",
    .quality   = {1,2,3,4,5,6,7,8},
    .a = 0xdeadbeef,
    .b = 0xbeefdead,
    .c = 0xabcd,
    .d = 0x01,
    .i16arr = rosarr(int16_t),
    .x = 32
  };

  size_t n,i,j,k;
  Test_t *ds;

  double volatile start = 0., accu  = 0.;
  for (j=0; j<2000; j+=100) {
    char str[j+1];
    memset(str, 'a', j);
    str[j-1] = '\0';
    obj.from_node = str;

    accu=0.;

    for (k=0; k<10; k++) {
      start=0.;

      for (i=0; i<(size_t) 1e6; i++) {
        start = clock() / (CLOCKS_PER_SEC / 1e9);
        n     = Test_serialize(&obj,buf,sizeof(buf));
        accu += (clock() / (CLOCKS_PER_SEC / 1e9)) - start;
      }
    }

    printf("j=%d accu=%f ns\n", j, (accu/1e6)/10.);
    fflush(stdout);
  }

  return 0;
}

void BenchmarkComplex()
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
      .i16arr    = rosarr(int16_t),
      .x         = 0xdeadbeef }, {

      .from_node = "second",
      .to_node   = "one",
      .quality   = {1,2,3,4,5,6,7,8},
      .a         = 0xdeadbeef,
      .b         = 0xbeefdead,
      .c         = 0xbeef,
      .d         = 0xcd,
      .i16arr    = rosarr(int16_t),
      .x         = 0xdeadbeef } },
    .another_number = 16,

    .test_dynamic = rosarr(Test_t*),

    .aString     = "mylittlestring",
    .sStrings    = { "numberone", "numbertwo"},
    .dStrings    = rosarr(char*),
  };

  TestComplex_t *ds;
  uint8_t buf[4000];
  size_t n,i,j,k;

  double volatile start = 0., accu  = 0.;
  char *str = "abcde";

  for (j=10; j<400; j+=20) {
    char *data[j];
    char **arr = ((struct {size_t len; char **data;}) {.len=j, .data=data}).data;
    obj.dStrings = arr;

    for (i=0; i<j; i++)
      obj.dStrings[i] = str;

    accu=0.;
    n = TestComplex_serialize(&obj,buf,sizeof(buf));

    for (k=0; k<10; k++) {
      start=0.;

      for (i=0; i<(size_t) 1e5; i++) {
        start = clock() / (CLOCKS_PER_SEC / 1e9);
        ds    = TestComplex_deserialize(buf,n);
        accu += (clock() / (CLOCKS_PER_SEC / 1e9)) - start;
      }
    }

    printf("j=%d accu=%f ns\n", j, (accu/1e5)/10.);
    fflush(stdout);
  }

}

int main (int argc, char const* argv[])
{
  size_t i;

  //BenchmarkSimpleSerialize();
  //BenchmarkComplexRoundtrip();
  BenchmarkComplex();

  return 0;
}

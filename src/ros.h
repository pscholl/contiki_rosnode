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
 * 3. Neither the name of the TU Darmstadt nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s): Philipp Scholl <scholl@ess.tu-darmstadt.de>
 */

#ifndef __ROS_H_
# define __ROS_H_

# define _rlen(type,args...) (sizeof((type[]){args})/sizeof(type))

# define rosarr(type,args...) ((struct {uint32_t len; type data[_rlen(type,args)];}) {.len=_rlen(type,args), .data={args} }).data
# define rosarr_n(type,n)     ({uint32_t *tmp = alloca(sizeof(uint32_t) + n*sizeof(type)); *tmp = n; tmp += 1;}) /* this is a gnu c extension called statement expression */
# define roslen(ptr)          (ptr==NULL ? 0 : (*(((uint32_t*) ptr)-1)))

/* it seems there is no fixed endianess of ROS message, so for now we assume
 * that data needs is transmitted in non-network-byte word (i.e. little endian
 */

# if  __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define ROS_READ64(ptr) (ptr)
#   define ROS_READ32(ptr) (*(uint32_t*) (ptr))
#   define ROS_READ16(ptr) (*(uint16_t*) (ptr))
#   define ROS_READ8(ptr)  (*(uint8_t*) (ptr))

//#   define ROS_WRITE64(buf,x) (buf)
//#   define ROS_WRITE32(buf,x) (*(uint32_t*) (buf)=x)
//#   define ROS_WRITE16(buf,x) (*(uint16_t*) (buf)=x)
//#   define ROS_WRITE8(buf,x)  (*(uint8_t*)  (buf)=x)

#   define ROS_WRITE64(buf,x) (buf)
#   define ROS_WRITE32(buf,x) (memcpy(buf,(void*) &x,sizeof(uint32_t)))
#   define ROS_WRITE16(buf,x) (memcpy(buf,(void*) &x,sizeof(uint16_t)))
#   define ROS_WRITE8(buf,x)  (memcpy(buf,(void*) &x,sizeof(uint8_t)))
# else
#  define ROS_HTONS(n)  (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#  define ROS_HTONL(n)  (((uint32_t)ROS_HTONS(n) << 16) | ROS_HTONS((uint32_t)(n) >> 16))
#  define ROS_HTONLL(n) (((uint64_t)ROS_HTONL(n) << 32) | ROS_HTONL((uint64_t)(n) >> 32))

#  define ROS_READ64(ptr) (ROS_HTONLL(*(uint64_t*) (ptr)))
#  define ROS_READ32(ptr) (ROS_HTONL(*(uint32_t*) (ptr)))
#  define ROS_READ16(ptr) (ROS_HTONS(*(uint16_t*) (ptr)))
#  define ROS_READ8(ptr)  (*(uint8_t*) (ptr))

#  define ROS_WRITE64(buf,x) (*(uint64_t*) (buf)=ROS_HTONLL(x))
#  define ROS_WRITE32(buf,x) (*(uint32_t*) (buf)=ROS_HTONL(x))
#  define ROS_WRITE16(buf,x) (*(uint32_t*) (buf)=ROS_HTONS(x))
#  define ROS_WRITE8(buf,x)  (*(uint8_t*)  (buf)=x)
# endif
#endif


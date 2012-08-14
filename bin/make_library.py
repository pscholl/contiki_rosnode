#!/usr/bin/env python

#####################################################################
# Software License Agreement (BSD License)
#
# Copyright (c) 2011, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Willow Garage, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

__author__ = "mferguson@willowgarage.com (Michael Ferguson)"

# XXX: generate some code to test the size of boolean types etc.

import roslib; roslib.load_manifest("rosserial_client")
import roslib.gentools, roslib.srvs, rospkg
import rospy,copy

import os, sys, subprocess, re, argparse

messages, services = {},{}

def type_to_var(ty):
    lookup = {
        1 : 'uint8_t',
        2 : 'uint16_t',
        4 : 'uint32_t',
        8 : 'uint64_t',
    }
    return lookup[ty]

#####################################################################
# Data Types

class EnumerationType(object):
    """ For data values. """

    def __init__(self, name, ty, value):
        self.name = name
        self.type = ty
        self.value = value

    def make_declaration(self):
        return 'enum { %s = %s };' % (self.name, self.value)

class PrimitiveDataType(object):
    """ Our datatype is a C/C++ primitive. """    

    def __init__(self, name, ty, bytes):
        self.name = name
        self.type = ty
        self.bytes = bytes

    def make_declaration(self):
        return '%s %s;' % (self.type, self.name)

    def deserialize(self):
        if self.bytes==8:   modifier = 'ROS_READ64'
        elif self.bytes==4: modifier = 'ROS_READ32'
        elif self.bytes==2: modifier = 'ROS_READ16'
        else:               modifier = 'ROS_READ8'

        return """obj->{self.name} = {modifier}(buf+var_len+offsetof(typeof(*obj),{self.name}));
""".format(**locals())

    def deserialize_arrayfixup(self):
        pass

    def serialize(self):
        if self.bytes==8:   modifier = 'ROS_WRITE64'
        elif self.bytes==4: modifier = 'ROS_WRITE32'
        elif self.bytes==2: modifier = 'ROS_WRITE16'
        else:               modifier = 'ROS_WRITE8'

        return """{modifier}(buf, obj->{self.name});
  buf += sizeof(obj->{self.name});
""".format(**locals())

class MessageDataType(PrimitiveDataType):
    """ For when our data type is another message. """
    def deserialize(self):
        mytype = self.type[:-2]
        return 'var_len += {mytype}_deserialize(&obj->{self.name},buf,n-var_len-sizeof(*obj));\n'.format(**locals())

    def serialize(self):
        mytype = self.type[:-2]
        return '{mytype}_serialize(&obj->{self.name},buf,n);\n'.format(**locals())

class Float64DataType(PrimitiveDataType):
    """ AVR C/C++ has no native 64-bit support, we automatically convert to 32-bit float. """

    def make_declaration(self):
        return 'double %s;' % self.name

    def deserialize(self):
        # TODO/XXX assume IEEE-754 on all machines
        return """memcpy(&obj->{self.name},buf+var_len+offsetof(typeof(*obj),{self.name}),sizeof(double);
""".format(**locals())

    def serialize(self):
        return """memcpy(buf,&obj->{self.name},sizeof(double));
  buf += sizeof(double);
""".format(**locals())

class StringDataType(PrimitiveDataType):
    """ Need to convert to signed char *. """

    def make_declaration(self):
        return 'union { char *%s; uint32_t %s_len; };' % (self.name, self.name)

    def serialize(self):
        return """tmp = strlen(obj->{self.name}); // do not serialize trailing '\\0'
  ROS_WRITE32(buf, tmp);
  buf += sizeof(uint32_t);
  memcpy(buf, obj->{self.name}, tmp);
  buf += tmp;
""".format(**locals())

    def deserialize(self):
        return """obj->{self.name}_len = ROS_READ32(buf+var_len+offsetof(typeof(*obj),{self.name}));
  var_len += obj->{self.name}_len;
""".format(**locals())

    def deserialize_arrayfixup(self):
        return """buf[sizeof(*obj)+grow_len-1] = '\\0';
  var_len-=obj->{self.name}_len;
  grow_len-=obj->{self.name}_len+1;
  memmove(buf+sizeof(*obj)+grow_len,
    buf+offsetof(typeof(*obj),{self.name})+var_len+sizeof(uint32_t),
    obj->{self.name}_len);
  obj->{self.name}=buf+grow_len+sizeof(*obj);

""".format(**locals())

class TimeDataType(PrimitiveDataType):
    def __init__(self, name, ty, bytes):
        self.sec = PrimitiveDataType(name+'.sec','uint32_t',4)
        self.nsec = PrimitiveDataType(name+'.nsec','uint32_t',4)
        self.bytes = self.sec.bytes + self.nsec.bytes
        PrimitiveDataType.__init__(self,name,ty,self.bytes)

    def make_declaration(self):
        return '%s %s;' % (self.type, self.name)

    def serialize(self):
        return self.sec.serialize() + self.nsec.serialize()

    def deserialize(self):
        return self.sec.deserialize() + self.nsec.deserialize()

class ArrayDataType(PrimitiveDataType):
    def __init__(self, name, ty, bytes, cls, array_size=None):
        PrimitiveDataType.__init__(self, name, ty, bytes)
        self.size = array_size
        self.cls = cls

        if self.bytes==8:   self.primitivetype = 'ROS_WRITE64'
        elif self.bytes==4: self.primitivetype = 'ROS_WRITE32'
        elif self.bytes==2: self.primitivetype = 'ROS_WRITE16'
        elif self.bytes==1: self.primitivetype = 'ROS_WRITE8'
        else: self.primitivetype = None

    def make_declaration(self):
        if self.size == None:
            return 'union { %s *%s; uint32_t %s_len; };' % (self.type, self.name, self.name)
        else:
            return '%s %s[%d];' % (self.type, self.name, self.size)

    def serialize(self):
        # XXX: need to fix that modifier stuff with a call
        # to the underlying datatype serialize method
        if self.size == None:
            code = """tmp = *(((uint32_t*) obj->{self.name})-1); // length = prefix of array!
  ROS_WRITE32(buf, tmp);
  buf += sizeof(uint32_t);
""".format(**locals())
        else:
            code = """tmp = {self.size};
""".format(**locals())

        if self.primitivetype is not None:
            code += """  for (i=0;i<tmp;i++) {{
    {self.primitivetype}(buf, obj->{self.name}[i]);
    buf += sizeof(*obj->{self.name});
  }}
""".format(**locals())
        else:
            thistype = self.type[:-2]
            code += """  for (i=0;i<tmp;i++)
    buf += {thistype}_serialize(&obj->{self.name}[i],buf,buf-save_ptr);
""".format(**locals())

        return code

    def deserialize(self):
        if self.size is None:
            return """obj->{self.name}_len = ROS_READ32(buf+var_len+offsetof(typeof(*obj),{self.name}));
  var_len += obj->{self.name}_len * sizeof(*obj->{self.name});
""".format(**locals())
        elif self.size is None and self.primitivetype is not None:
            return """for (i=0; i<{self.size}*sizeof(*obj->{self.name});i++)
  obj->{self.name}[i] = {self.primitivetype}(buf+i*sizeof(*obj->{self.name}));
"""
        else:
            print Message.lookup(self.name)
            return ""
            #raise Exception("meh")

    def deserialize_arrayfixup(self):
        # same here, memmove needs to be replaced with some endianess aware
        if self.size == None:
            return """var_len-=obj->{self.name}_len * sizeof(*obj->{self.name});
  grow_len-=obj->{self.name}_len * sizeof(*obj->{self.name});
  memmove(buf+sizeof(*obj)+grow_len,
    buf+offsetof(typeof(*obj),{self.name})+var_len+sizeof(uint32_t),
    obj->{self.name}_len*sizeof(*obj->{self.name}));
  grow_len-=sizeof(uint32_t);
  ROS_WRITE32(buf+sizeof(*obj)+grow_len,obj->{self.name}_len);
  obj->{self.name}=({self.type}*) (buf+grow_len+sizeof(*obj)+sizeof(uint32_t));
""".format(**locals())

ros_to_c_type = {
    'bool'    :   ('bool',              1, PrimitiveDataType, []),
    'int8'    :   ('int8_t',            1, PrimitiveDataType, []),
    'uint8'   :   ('uint8_t',           1, PrimitiveDataType, []),
    'int16'   :   ('int16_t',           2, PrimitiveDataType, []),
    'uint16'  :   ('uint16_t',          2, PrimitiveDataType, []),
    'int32'   :   ('int32_t',           4, PrimitiveDataType, []),
    'uint32'  :   ('uint32_t',          4, PrimitiveDataType, []),
    'int64'   :   ('int64_t',           4, PrimitiveDataType, []),
    'uint64'  :   ('uint64_t',          4, PrimitiveDataType, []),
    'float32' :   ('float',             4, PrimitiveDataType, []),
    'float64' :   ('float',             4, Float64DataType, []),
#    'time'    :   ('Time_t',            8, TimeDataType, ['time']),
#    'duration':   ('Duration_t',        8, TimeDataType, ['duration']),
    'time'    :   ('Time_t',            8, TimeDataType, []),
    'duration':   ('Duration_t',        8, TimeDataType, []),
    'string'  :   ('char*',             0, StringDataType, []),
    'Header'  :   ('Header_t',          0, MessageDataType, ['std_msgs/Header'])
}


#####################################################################
# Messages
class Message:
    """ Parses message definitions into something we can export. """
    def __init__(self, name, package, definition, md5):
        self.name = name            # name of message/class
        self.package = package      # package we reside in
        self.md5 = md5              # checksum
        self.includes = list()      # other files we must include
        self.data = list()          # data types for code generation
        self.enums = list()

        # parse definition
        for line in definition:
            # prep work
            line = line.strip().rstrip()
            value = None
            if line.find("#") > -1:
                line = line[0:line.find("#")]
            if line.find("=") > -1:
                try:
                    value = line[line.find("=")+1:]
                except:
                    value = '"' + line[line.find("=")+1:] + '"';
                line = line[0:line.find("=")]

            # find package/class name   
            line = line.replace("\t", " ")
            l = line.split(" ")
            while "" in l:
                l.remove("")
            if len(l) < 2:
                continue
            ty, name = l[0:2]
            if value != None:
                self.enums.append( EnumerationType(name, ty, value))
                continue

            try:
                type_package, type_name = ty.split("/")
            except:
                type_package = None
                type_name = ty
            type_array = False
            if type_name.find('[') > 0:
                type_array = True   
                try:
                    type_array_size = int(type_name[type_name.find('[')+1:type_name.find(']')])
                except:
                    type_array_size = None
                type_name = type_name[0:type_name.find('[')]

            # convert to C type if primitive, expand name otherwise
            try:
                code_type = ros_to_c_type[type_name][0]
                size = ros_to_c_type[type_name][1]
                cls = ros_to_c_type[type_name][2]
                for include in ros_to_c_type[type_name][3]:
                    if include not in self.includes:
                        self.includes.append(include)
            except:
                if type_package == None:
                    type_package = self.package
                if type_package+"/"+type_name not in self.includes:
                    self.includes.append(type_package+"/"+type_name)
                cls = MessageDataType
                # XXX: code_type = type_package + "::" + type_name
                code_type = type_name + "_t"
                size = 0
            if type_array:
                self.data.append( ArrayDataType(name, code_type, size, cls, type_array_size ) )
            else:
                self.data.append( cls(name, code_type, size) )

    def make_header(self, f):
        msg_inc  = "\n".join(['#include "%s.h"'%(h) for h in self.includes])
        msg_body = "\n".join(['  %s'%(d.make_declaration()) for d in self.data])

        f.write(
"""#ifndef _ROS_{self.package}_{self.name}_H_
# define _ROS_{self.package}_{self.name}_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
{msg_inc}

#define {self.name}_md5 ("{self.md5}")
#define {self.name}_rostype ("{self.package}/{self.name}")

typedef struct __attribute__((__packed__)) {self.name} {{
{msg_body}
}} {self.name}_t;

{self.name}_t *
{self.name}_deserialize(char *const buf, size_t n);

size_t
{self.name}_serialize({self.name}_t *obj, char *buf, size_t n);

#endif
""".format(**locals()))

    def make_cfile(self, f):
        serialize = "".join(['  %s'%d.serialize() for d in self.data])
        deserialize = "".join(['  %s'%d.deserialize() for d in self.data])
        deserialize_array = "".join(['  %s'%d.deserialize_arrayfixup() for d in reversed(self.data) if d.deserialize_arrayfixup() is not None])
        string_hack = len([x for x in self.data if type(x)==StringDataType])
        array_hack  = len([x for x in self.data if type(x)==ArrayDataType and x.size==None])*4
        f.write(
"""#include <stddef.h>
#include <ros_arch.h>
#include "{self.package}/{self.name}.h"

{self.name}_t*
{self.name}_deserialize(char *const buf, size_t n)
{{
  {self.name}_t  obj_header, *obj=&obj_header;
  uint32_t var_len = 0,
           grow_len = 0,
           tmp;

  // deserialize in three steps:
  //  1. copy the header information onto the stack
  //  2. memmove var-len members at the end of buf
  //  3. memcpy the header back into place
  // return a pointer to the readable structure in buf

  if (n < sizeof(*obj))
    return NULL; // check if header fits

{deserialize}

  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\\0' of strings. and prefix the length
  // of array for variable sized ones.
  grow_len = var_len + {string_hack} + {array_hack};

  if (n < sizeof(*obj)+grow_len)
      return NULL; // check if arrays+strings fit

{deserialize_array}

  // copy the header back in place.
  memcpy(buf,obj,sizeof(*obj));

  return ({self.name}_t*) buf;
}}

size_t
{self.name}_serialize({self.name}_t *obj, char *buf, size_t n)
{{
  char *save_ptr=buf;
  uint32_t tmp=0;
  size_t i=0;

{serialize}

  return buf-save_ptr;
}}
""".format(**locals()))

class Service:
    def __init__(self, name, package, definition, md5req, md5res):
        """ 
        @param name -  name of service
        @param package - name of service package
        @param definition - list of lines of  definition
        """

        self.name = name
        self.package = package

        sep_line = None
        sep = re.compile('---*')
        for i in range(0, len(definition)):
            if (None!= re.match(sep, definition[i]) ):
                sep_line = i
                break
        self.req_def = definition[0:sep_line]
        self.resp_def = definition[sep_line+1:]

        self.req = Message(name+"Request", package, self.req_def, md5req)
        self.resp = Message(name+"Response", package, self.resp_def, md5res)


    def make_cfile(self, f):
        pass

    def make_header(self, f):
        f.write('#ifndef _ROS_SERVICE_%s_h\n' % self.name)
        f.write('#define _ROS_SERVICE_%s_h\n' % self.name)

#####################################################################
# Make a Library
def MakeLibrary(package, output_path):
    print "Exporting " + package + "\n", 

    rp = rospkg.RosPack()
    pkg_dir = rp.get_path(package)

    sys.stdout.write('  Messages:')
    # find the messages in this package
    if os.path.exists(pkg_dir+"/msg"):
        sys.stdout.write('\n    ')
        for f in os.listdir(pkg_dir+"/msg"):
            if f.endswith(".msg"):
                file = pkg_dir + "/msg/" + f
                # add to list of messages
                print "%s," % f[0:-4],
                definition = open(file).readlines()
                md5sum = roslib.gentools.compute_md5(roslib.gentools.get_file_dependencies(file))
                mid = "%s_t"%(f[:-4])
                messages[mid] = Message(f[0:-4], package, definition, md5sum)
    print "\n"

    sys.stdout.write('  Services:')
    # find the services in this package
    if (os.path.exists(pkg_dir+"/srv/")):
        sys.stdout.write('\n    ')
        for f in os.listdir(pkg_dir+"/srv"):
            if f.endswith(".srv"):
                file = pkg_dir + "/srv/" + f
                # add to list of messages
                print "%s," % f[0:-4],
                definition, service = roslib.srvs.load_from_file(file)
                definition = open(file).readlines()
                md5req = roslib.gentools.compute_md5(roslib.gentools.get_dependencies(service.request, package))
                md5res = roslib.gentools.compute_md5(roslib.gentools.get_dependencies(service.response, package))
                sid = "%s_t"%(f[:-4])
                services[sid] = Service(f[0:-4], package, definition, md5req, md5res)
    print "\n"

    # generate for each message
    output_path = output_path + "/" + package
    for msg in messages.values():
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        header = open(output_path + "/" + msg.name + ".h", "w")
        cfile  = open(output_path + "/" + msg.name + ".c", "w")
        msg.make_header(header)
        msg.make_cfile(cfile)
        header.close()
        cfile.close()

if __name__=="__main__":
    parser = argparse.ArgumentParser(
            description='Generate C struct de-/serializers for ROS messages.')
    parser.add_argument('packages', metavar='PACKAGE', nargs='+',
            help='ROS package names to generate de-/serializers for')
    parser.add_argument('-v,', '--verbose', action='count', default=0,
            help='increase output verbosity')
    parser.add_argument('--path', nargs='?', default='.',
            help='path to write generated files to')
    args = parser.parse_args()

    # add all dependent packages
    rp = rospkg.RosPack()
    packages = set(args.packages)
    for p in set(args.packages):
        packages.update(set(rp.get_depends(p,implicit=True)))

    # create a lib for all found packages
    for p in packages:
        MakeLibrary(p, args.path)

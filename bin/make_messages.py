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

__author__ = "Philipp M. Scholl <scholl@ess.tu-darmstadt.de>"

#import roslib; roslib.load_manifest("rosserial_client")
import roslib.gentools, roslib.srvs, rospkg
import rospy,copy
import os, sys, subprocess, re, argparse, glob, copy
from StringIO import StringIO

#####################################################################
# Visitor from
# http://peter-hoffmann.com/2010/extrinsic-visitor-pattern-python-inheritance.html
class Visitor(object):
    def __init__(self,out=sys.stdout,indentation="  ",counter_var="i"):
        self.out = out
        self.indentation = indentation
        self.indent_level = 0

    def writeln(self,node,s=None):
        if s is not None:
            self.out.write(("{self.indent}"+s+"\n").format(**locals()))
        else:
            self.out.write("\n")

    @property
    def counter_var(self):
        return chr(len(self.stack)+ord('i')-1)

    @property
    def indent(self):
        return self.indentation * (len(self.stack) + self.indent_level)

    def dispatch_Array(self,node,recurse=None):
        if not hasattr(self, "stack"):
            self.stack = []

        node.name += "[%s]"%self.counter_var
        self.indent_level += 1

        if node.cls == PrimitiveDataType: self.visit_PrimitiveDataType(node)
        elif node.cls == StringDataType:  self.visit_StringDataType(node)
        elif node.cls == MessageDataType:
            if recurse == None:
                self.visit_MessageDataType(node)
            else:
                recurse(node)
        else:
            str = "don't know what todo with array of %s" % node.cls
            #XXX raise Exception(str)

        node.name = node.name[:-len("[i]")]
        self.indent_level -= 1

    def visit(self, node, *args, **kwargs):
        meth = None
        for cls in node.__class__.__mro__:
            meth_name = 'visit_'+cls.__name__
            meth = getattr(self, meth_name, None)
            if meth:
                break
        if not meth:
            meth = self.generic_visit
        return meth(node, *args, **kwargs)

    def generic_visit(self,node,*args,**kw):
        str = "visit for type %s not implemented in %s"%(node.__class__,self.__class__)
        raise Exception(str)

    def dfs(self, msg):
        """ depth-first search on message types. """
        if msg.__class__ != MessageDataType:
            msg = MessageDataType(msg.name, msg.type, 0)

        if not hasattr(self, "stack"):
            self.stack = []

        self.visit(msg)
        self.stack.append(msg)

        for d in Message.__messages__[msg.type[:-2]].data:
            if isinstance(d, MessageDataType):
                self.dfs(d)
            else:
                self.visit(d)

        self.stack = self.stack[:-1]
        if len(self.stack) == 0:
            if hasattr(self,"finalize"): self.finalize()
            return self

    def rdfs(self, msg):
        """ reversed depth-first search on message types. """
        if msg.__class__ != MessageDataType:
            msg = MessageDataType(msg.name, msg.type, 0)

        if not hasattr(self, "stack"):
            self.stack = []

        self.stack.append(msg)

        for d in reversed(Message.__messages__[msg.type[:-2]].data):
            if isinstance(d, MessageDataType): self.rdfs(d)
            else: self.visit(d)

        self.stack = self.stack[:-1]
        self.visit(msg)

        if len(self.stack) == 0:
            if hasattr(self,"finalize"): self.finalize()
            return self

    def onlyattr(self,msg):
        """ visit all attributes of msg. """
        self.stack = [msg]

        for d in msg.data:
            self.visit(d)

        if hasattr(self,"finalize"):
            self.finalize()
        return self

#####################################################################
# Data Types
class EnumerationType(object):
    def __init__(self, name, ty, value):
        self.name = name
        self.type = ty
        self.value = value

class PrimitiveDataType(object):
    def __init__(self, name, ty, bytes):
        self.name = name
        self.type = ty
        self.bytes = bytes

class MessageDataType(PrimitiveDataType):
    """ For when our data type is another message. """
    def __init__(self, name, ty, bytes):
        self.name = name
        self.type = ty
        self.bytes = bytes

class Float64DataType(PrimitiveDataType): pass

class StringDataType(PrimitiveDataType): pass

class StaticArray(PrimitiveDataType):
    def __init__(self, name, ty, bytes, cls, size):
        PrimitiveDataType.__init__(self, name, ty, bytes)
        self.size = size
        self.cls  = cls

class DynamicArray(PrimitiveDataType):
    def __init__(self, name, ty, bytes, cls, array_size=None):
        PrimitiveDataType.__init__(self, name, ty, bytes)
        self.size = array_size
        self.cls = cls

ros_to_c_type = {
    'bool'    :   ('bool',              1, PrimitiveDataType, []),
    'int8'    :   ('int8_t',            1, PrimitiveDataType, []),
    'byte'    :   ('int8_t',            1, PrimitiveDataType, []),
    'uint8'   :   ('uint8_t',           1, PrimitiveDataType, []),
    'char'    :   ('uint8_t',           1, PrimitiveDataType, []),
    'int16'   :   ('int16_t',           2, PrimitiveDataType, []),
    'uint16'  :   ('uint16_t',          2, PrimitiveDataType, []),
    'int32'   :   ('int32_t',           4, PrimitiveDataType, []),
    'uint32'  :   ('uint32_t',          4, PrimitiveDataType, []),
    'int64'   :   ('int64_t',           4, PrimitiveDataType, []),
    'uint64'  :   ('uint64_t',          4, PrimitiveDataType, []),
    'float32' :   ('float',             4, PrimitiveDataType, []),
    'float64' :   ('float',             4, Float64DataType, []),
    'string'  :   ('char*',             0, StringDataType, []),
#    'Header'  :   ('Header_t',          0, MessageDataType, ['std_msgs/Header'])
}

class PrettyPrinter(Visitor):
    def generic_visit(self,node,*args,**kw):
        self.out.write(('\t'*len(self.stack))+node.type+" "+node.name)
        self.out.write('\n')

    def visit_MessageDataType(self,node,*args,**kw):
        self.out.write(('\t'*len(self.stack))+node.name)
        self.out.write(':\n')

class Declaration(Visitor):
    def visit_MessageDataType(self,node):
        self.writeln(node, "{node.type} {node.name};")

    def visit_PrimitiveDataType(self,node):
        self.writeln(node, "{node.type} {node.name};")

    def visit_StringDataType(self,node):
        self.writeln(node, "char *{node.name};")

    def visit_StaticArray(self,node):
        self.writeln(node, "{node.type} {node.name}[{node.size}];")

    def visit_DynamicArray(self,node):
        if node.cls == MessageDataType:
            self.writeln(node, "{node.type} **{node.name};")
        else:
            self.writeln(node, "{node.type} *{node.name};")

class DeclarationPacked(Declaration):
    def visit_StaticArray(self,node):
        if node.cls == MessageDataType:
            self.writeln(node, "{node.type}_packed {node.name}[{node.size}];")
        else:
            Declaration.visit_StaticArray(self,node)

class Includes(Visitor):
    def __init__(self,out=sys.stdout):
        Visitor.__init__(self,out)
        self.set = set()

    def generic_visit(self,node):
        pass

    def finalize(self):
        self.out.write("".join(self.set))

    def add(self,node):
        self.set.add("#include <{node.package}/{node.name}.h>\n".format(**locals()))

    def visit_MessageDataType(self,node):
        node = Message.__messages__[node.type[:-2]]
        self.add(node)

    def visit_StaticArray(self,node):
        if node.cls == MessageDataType:
            node = Message.__messages__[node.type[:-2]]
            self.add(node)

    def visit_DynamicArray(self,node):
        if node.cls == MessageDataType:
            node = Message.__messages__[node.type[:-2]]
            self.add(node)

class Serialize(Visitor):
    def visit_PrimitiveDataType(self,node):
        if node.bytes == 0:
            str = "unknown size for %s"%node.name
            raise Exception(str)

        node.accessor = "ROS_WRITE%d"%(node.bytes*8)
        self.writeln(node,"{node.accessor}(buf,obj->{node.name}); buf+={node.bytes};")

    def visit_MessageDataType(self,node):
        node.mytype = node.type[:-2]
        if hasattr(self,"indynamicarray") and self.indynamicarray:
            self.writeln(node,"buf += {node.mytype}_serialize(obj->{node.name},buf,buf-save_ptr);")
        else:
            self.writeln(node,"buf += {node.mytype}_serialize(&obj->{node.name},buf,buf-save_ptr);")

    def visit_StaticArray(self,node):
        self.writeln(node,"for (size_t i=0; i<{node.size}; i++) {{")
        self.dispatch_Array(node)
        self.writeln(node,"}}")

    def visit_DynamicArray(self,node):
        self.writeln(node,"tmp = obj->{node.name}==NULL ? 0 : roslen(obj->{node.name});")
        self.writeln(node,"ROS_WRITE32(buf, tmp); buf+=4;")
        self.writeln(node,"for (size_t i=0; i<tmp; i++) {{")
        self.indynamicarray = True
        self.dispatch_Array(node)
        self.indynamicarray = False
        self.writeln(node,"}}")

    def visit_StringDataType(self,node):
        self.writeln(node,"tmp = obj->{node.name}==NULL ? 0 : strlen(obj->{node.name});")
        self.writeln(node,"ROS_WRITE32(buf, tmp); buf+=4;")
        self.writeln(node,"memcpy(buf, obj->{node.name}, tmp);")
        self.writeln(node,"buf += tmp;")

class DynamicArrayMessageRecurse(Visitor):
    def visit_PrimitiveDataType(self,node):
        self.writeln(node,"buf += sizeof({node.type});")

    def visit_MessageDataType(self,node):
        pass

    def visit_StaticArray(self,node):
        if node.cls == MessageDataType:
            self.dfs(node)
        else:
            self.writeln(node,"buf += {node.size} * sizeof({node.type});")

    def visit_DynamicArray(self,node):
        if node.cls == MessageDataType:
            self.writeln("grow_len += sizeof({node.type}); // for storing pointer")
            self.dfs(node)
        else:
            self.writeln(node,"buf += ROS_READ32(buf) * sizeof({node.type}) + sizeof(uint32_t);")
            self.writeln(node,"grow_len += 4;")

    def visit_StringDataType(self,node):
        self.writeln(node,"buf += ROS_READ32(buf) + sizeof(uint32_t);")
        self.writeln(node,"grow_len += 1;")

class Deserialize(Visitor):
    def visit_PrimitiveDataType(self,node):
        if node.bytes == 0:
            raise Exception("unknown size for %s"%node.name)

        node.accessor     = "ROS_READ%d"%(node.bytes*8)
        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])
        self.writeln(node,"obj->{node.indirection} = {node.accessor}(buf); buf+={node.bytes};")

    def visit_MessageDataType(self,node):
        # will be handled by the depth-first-search automatically,
        # this function is only here so the visit does not call
        # any other function.
        pass

    def visit_StaticArray(self,node):
        self.writeln(node,"for (size_t {self.counter_var}=0; {self.counter_var}<{node.size}; {self.counter_var}++) {{")
        self.dispatch_Array(node,recurse=self.dfs)
        self.writeln(node,"}}")

    def visit_DynamicArray(self,node):
        # put a pointer to the current array (not pointing to the length array but the data)
        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

        self.writeln(node,"tmp = ROS_READ32(buf);")
        self.writeln(node,"grow_len += sizeof(uint32_t);")
        self.writeln(node,"buf += sizeof(uint32_t);")

        if node.cls == MessageDataType or node.cls == StringDataType:
            self.writeln(node,"/* allocate pointer space on stack */");
            if node.cls == StringDataType:
                self.writeln(node,"obj->{node.indirection} = ({node.type}*) alloca(sizeof({node.type}*)*tmp + sizeof(uint32_t));")
                self.writeln(node,"*((uint32_t*)obj->{node.indirection}) = tmp;")
                self.writeln(node,"obj->{node.indirection} = ({node.type}*) (((uint32_t*) obj->{node.indirection}) + 1);")
            else:
                self.writeln(node,"obj->{node.indirection} = ({node.type}**) alloca(sizeof({node.type}*)*tmp + sizeof(uint32_t));")
                self.writeln(node,"*((uint32_t*)obj->{node.indirection}) = tmp;")
                self.writeln(node,"obj->{node.indirection} = ({node.type}**) (((uint32_t*) obj->{node.indirection}) + 1);")
        else:
            self.writeln(node,"obj->{node.indirection} = ({node.type}*) buf;")

        self.writeln(node,"for (size_t {self.counter_var}=0; {self.counter_var}<tmp; {self.counter_var}++) {{")
        node.name += "[%s]"%self.counter_var
        self.indent_level += 1

        if node.cls == PrimitiveDataType:
            self.writeln(node,"buf += {node.bytes};")
        elif node.cls == StringDataType:
            self.visit_StringDataType(node)
        elif node.cls == MessageDataType:
            self.writeln(node,"grow_len += sizeof({node.type}) - sizeof({node.type}_packed);")
            self.writeln(node,"obj->{node.indirection}[{self.counter_var}] = ({node.type}*)buf;")
            self.writeln(node,"grow_len += sizeof({node.type}*); // allocate storage space")
            DynamicArrayMessageRecurse(self.out,self.indent,self.counter_var).dfs(node)
        else:
            str = "don't know what todo with array of %s" % node.cls
            #XXX raise Exception(str)

        self.indent_level -= 1
        node.name = node.name[:-len("[i]")]
        self.writeln(node,"}}")

    def visit_StringDataType(self,node):
        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])
        self.writeln(node,"obj->{node.indirection} = buf;")
        self.writeln(node,"buf += ROS_READ32(buf) + sizeof(uint32_t);")
        self.writeln(node,"grow_len += 1;")

class DeserializeArray(Visitor):
    def visit_PrimitiveDataType(self,node):
        self.writeln(node,"buf -= sizeof({node.type});")

    def visit_MessageDataType(self,node):
        pass

    def visit_StaticArray(self,node):
        if node.cls == PrimitiveDataType:
            self.writeln(node,"buf -= sizeof({node.type})*{node.size};")
        elif node.cls == StringDataType:
            self.writeln(node,"for (size_t {self.counter_var}={node.size}; {self.counter_var}-->0;) {{")

            self.indent_level += 1
            node.name += "[%s]"%self.counter_var
            node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

            self.visit_StringDataType(node)

            self.indent_level -= 1
            self.writeln(node,"}}")
        elif node.cls == MessageDataType:
            self.writeln(node,"for (size_t {self.counter_var}={node.size}; {self.counter_var}-->0;) {{")

            self.indent_level += 1
            node.name += "[%s]"%self.counter_var
            node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

            self.rdfs(node)

            self.indent_level -= 1
            self.writeln(node,"}}")

    def visit_DynamicArray(self,node):
        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

        # load the length of the array into tmp and start reverse iteration
        self.writeln(node,"for (size_t {self.counter_var}=roslen(obj->{node.indirection}); {self.counter_var}-->0;) {{")

        self.indent_level += 1
        node.name += "[%s]"%self.counter_var
        node.accessor = "ROS_READ%d"%(node.bytes*8)

        if node.cls == PrimitiveDataType:
            self.writeln(node,"buf -= sizeof({node.type});")
            self.writeln(node,"var_ptr -= sizeof({node.type});")
            self.writeln(node,"*(({node.type}*) (var_ptr)) = {node.accessor}(buf);")
        elif node.cls == StringDataType:
            self.visit_StringDataType(node)
        elif node.cls == MessageDataType:
            node.mytype = node.type[:-2]
            self.writeln(node,"uint32_t m = *n;") # XXX
            self.writeln(node,"buf = (uint8_t*) obj->{node.indirection}[{self.counter_var}];")
            self.writeln(node,"var_ptr = (uint8_t*) {node.mytype}_deserialize_size(buf,var_ptr,&m);")
            self.writeln(node,"obj->{node.indirection}[{self.counter_var}] = ({node.type}*) var_ptr;")
        node.name = node.name[:-len("[i]")]

        self.indent_level -= 1
        self.writeln(node,"}}")

        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

        self.writeln(node,"buf -= sizeof(uint32_t);")

        if node.cls == MessageDataType or node.cls == StringDataType:
            # need to write the pointer for each struct/string into the buf 
            # (currently stored on the stack)
            self.writeln(node,"var_ptr -= sizeof({node.type}*) * roslen(obj->{node.indirection});")
            self.writeln(node,"memcpy(var_ptr,obj->{node.indirection},roslen(obj->{node.indirection})*sizeof({node.type}*));")

        self.writeln(node,"*((uint32_t*) (var_ptr-sizeof(uint32_t))) = roslen(obj->{node.indirection}); // store arrlen");
        if node.cls == MessageDataType:
            self.writeln(node,"obj->{node.indirection} = ({node.type}**) (var_ptr);")
        else:
            self.writeln(node,"obj->{node.indirection} = ({node.type}*) (var_ptr);")
        self.writeln(node,"var_ptr -= sizeof(uint32_t);")

    def visit_StringDataType(self,node):
        node.indirection = ".".join([n.name for n in self.stack[1:]+[node]])

        self.writeln(node,"tmp = ROS_READ32(obj->{node.indirection});")
        self.writeln(node,"buf -= tmp;")
        self.writeln(node,"var_ptr -= 1;")
        self.writeln(node,"*var_ptr = '\\0';")
        self.writeln(node,"var_ptr -= tmp;")
        self.writeln(node,"memmove(var_ptr,buf,tmp);")
        self.writeln(node,"obj->{node.indirection} = (char*) (var_ptr);")
        self.writeln(node,"buf -= sizeof(uint32_t);")

#####################################################################
# Messages
class Message(object):
    """ Parses message definitions into something we can export. """
    __messages__ = {}

    def __init__(self, file, package):
        self.name = os.path.basename(file)[:-4]
        self.type = os.path.basename(file)[:-4]+"_t"
        self.package = package
        self.md5 = roslib.gentools.compute_md5(roslib.gentools.get_file_dependencies(file))
        self.includes = list()
        self.data = list()
        self.enums = list()

        try:
            dup = Message.__messages__[self.name]
            raise Exception("Duplicate Message with name %s in %s and %s"%(self.name,self.package,dup.package))
        except KeyError:
            Message.__messages__[self.name] = self

        with open(file) as file:
            lines = file.readlines()

        if ("Time" in self.name and "std_msgs" in str(self.package)) or\
           ("Duration" in self.name and "std_msgs" in str(self.package)):
            lines = "int32 secs\nint32 nanosecs".split('\n')
            Message.__messages__[self.name.lower()] = self

        # parse definition
        for line in lines:
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
                    type_package = str(self.package)
                if type_package+"/"+type_name not in self.includes:
                    self.includes.append(type_package+"/"+type_name)
                cls = MessageDataType
                code_type = type_name + "_t"
                size = 0
            if type_array and type_array_size is None:
                self.data.append( DynamicArray(name, code_type, size, cls) )
            elif type_array and type_array_size is not None:
                self.data.append( StaticArray(name, code_type, size, cls, type_array_size) )
            else:
                self.data.append( cls(name, code_type, size) )

    def make_header(self, out=sys.stdout):
        includes = Includes(out=StringIO()).onlyattr(self).out.getvalue()
        declaration = Declaration(out=StringIO()).onlyattr(self).out.getvalue()
        declaration_packed = DeclarationPacked(out=StringIO()).onlyattr(self).out.getvalue()
        package,name = str(self.package).upper(),self.name.upper()

        out.write("""#ifndef _ROS_{package}_{name}_H_
# define _ROS_{package}_{name}_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
{includes}

#define {self.name}_md5 ("{self.md5}")
#define {self.name}_rostype ("{self.package}/{self.name}")

typedef struct {self.name} {{
{declaration}
}} {self.name}_t;

/* the packed declaration to calculate the proper offset later on */
typedef struct {self.name}_packed {{
{declaration_packed}
}} __attribute__((__packed__)) {self.name}_t_packed;

{self.name}_t *
{self.name}_deserialize_size(char *buf, char *to, size_t *n);

{self.name}_t *
{self.name}_deserialize(char *buf, size_t n);

size_t
{self.name}_serialize({self.name}_t *obj, char *buf, size_t n);

#endif
""".format(**locals()))

    def make_cfile(self, out=sys.stdout):
        serialize   = Serialize(out=StringIO()).onlyattr(self).out.getvalue()
        deserialize = Deserialize(out=StringIO()).dfs(self).out.getvalue()
        deserialize_array = DeserializeArray(out=StringIO()).rdfs(self).out.getvalue()

        out.write(
"""#include <stddef.h>
#include <ros.h>
#include <alloca.h>
#include "{self.package}/{self.name}.h"


{self.name}_t*
{self.name}_deserialize(char *buf, size_t n) {{
  return {self.name}_deserialize_size(buf,NULL,&n);
}}

{self.name}_t*
{self.name}_deserialize_size(char *buf, char *to, size_t *n)
{{
  {self.name}_t  obj_header, *obj=&obj_header;

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

{deserialize}

  // we grow the number of bytes we need beyond what is actually needed
  // by the message to encode the '\\0' of strings. and prefix the length
  // of array for variable sized ones.

  if (*n < (buf-save_ptr)+grow_len)
      return NULL; // check if arrays+strings fit
  *n = (buf-save_ptr)+grow_len;

  if (to==NULL) var_ptr = buf + grow_len + (sizeof({self.name}_t)-sizeof({self.name}_t_packed));
  else          var_ptr = to;

{deserialize_array}

  if (to!=NULL)
      save_ptr = var_ptr - sizeof({self.name}_t);

  // copy the header back in place.
  memcpy(save_ptr,obj,sizeof(*obj));

  return ({self.name}_t*) save_ptr;
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

class Library(object):
    def __init__(self, package, output_path):
        rp = rospkg.RosPack()
        pkg_dir = rp.get_path(package)

        self.path     = output_path
        self.package  = package
        self.messages = [Message(file, package=self) for file in glob.iglob(pkg_dir+'/msg/*.msg')]

        #self.services = [Service(file, package=self) for file in glob.iglob(pkg_dir+'/srv/*.srv')]
        #for file in glob.iglob(pkg_dir+'/srv/*.srv'):
        #    definition, service = roslib.srvs.load_from_file(file)
        #    definition = open(file).readlines()
        #    md5req = roslib.gentools.compute_md5(roslib.gentools.get_dependencies(service.request, package))
        #    md5res = roslib.gentools.compute_md5(roslib.gentools.get_dependencies(service.response, package))
        #    sid = "%s_t"%(os.path.basename(file)[:-4])
        #    services[sid] = Service(os.path.basename(file)[:-4], package, definition, md5req, md5res)

    def __str__(self):
        return self.package

    def generate(self):
        # generate for each message
        output_path = os.path.join(self.path, self.package)
        if not os.path.exists(output_path):
            os.makedirs(output_path)

        #for msg in self.messages:
        #    PrettyPrinter().rdfs(msg)
        #sys.exit(-1)

        for msg in self.messages:
            header = open(output_path + "/" + msg.name + ".h", "w")
            cfile  = open(output_path + "/" + msg.name + ".c", "w")
            #msg.make_header()
            #msg.make_cfile()
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
    rp,packages = rospkg.RosPack(), set(args.packages)
    for p in set(args.packages):
        packages.update(set(rp.get_depends(p,implicit=True)))

    # create a lib for all found packages
    libs = [Library(p,args.path) for p in packages]
    for l in libs: l.generate()


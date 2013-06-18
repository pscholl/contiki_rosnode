all: hello-subscriber

# contiki configuration, enable IPv6 mode and RPL-multihop routing
UIP_CONF_IPV6=1
UIP_CONF_RPL=1

# message definitions we need for ping-publisher
ROSTIKI_MSG=std_msgs rosserial_msgs

# paths to the contiki OS and rostiki middleware
CONTIKI=../../../contiki
ROSTIKI=../../

# ################################################################## #
# re-generate message de-/serializers if we are in a ROS environment #
# ################################################################## #

ifneq ($(shell which rospack),)
ROSTIKI_MSGS  =$(foreach MSG,$(ROSTIKI_MSG),$(foreach FILE,$(notdir $(wildcard $(shell rospack find $(MSG))/**/*.msg)),$(ROSTIKI)/src/$(MSG)/$(FILE)))
ROSTIKI_SRCS  =$(patsubst %.msg,%.c,$(ROSTIKI_MSGS))
ROSTIKI_INCS +=$(patsubst %.msg,%.h,$(ROSTIKI_MSGS))

$(ROSTIKI_SRCS) $(ROSTIKI_INCS): $(foreach MSG,$(ROSTIKI_MSG),$(wildcard $(shell rospack find $(MSG))/**/*.msg))
	$(ROSTIKI)/bin/make_messages.py --path $(ROSTIKI)/src $(ROSTIKI_MSG)

$(CONTIKI_SOURCEFILES) : $(ROSTIKI_INCS)
endif

# ###################################################### #
# add the de-/serializer and rostiki module to the build #
# ###################################################### #

CONTIKI_SOURCEFILES += rostiki.c $(notdir $(wildcard $(ROSTIKI)/src/**/*.c))
include $(CONTIKI)/Makefile.include
SOURCEDIRS += $(ROSTIKI)/src $(foreach DIR,$(ROSTIKI_MSG),$(ROSTIKI)/src/$(DIR)/)
vpath %.c $(ROSTIKI)/src $(foreach DIR,$(ROSTIKI_MSG),$(ROSTIKI)/src/$(DIR)/)

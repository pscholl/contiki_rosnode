all: hello-subscriber

UIP_CONF_IPV6=1
UIP_CONF_RPL=0

CONTIKI=../../contiki
ROSTIKI=../../src
ROSTIKI_MSG=std_msgs contiki_rosnode rosserial_msgs

CONTIKIDIRS += $(ROSTIKI)/ $(foreach DIR,$(ROSTIKI_MSG),$(ROSTIKI)/$(DIR)/)
CONTIKI_SOURCEFILES += $(notdir $(wildcard $(ROSTIKI)/**/*.c))

# generate the source file list for the required message
ifneq ($(shell which rospack),)
ROSTIKI_MSGS  =$(foreach MSG,$(ROSTIKI_MSG),$(foreach FILE,$(notdir $(wildcard $(shell rospack find $(MSG))/**/*.msg)),$(ROSTIKI)/$(MSG)/$(FILE)))
ROSTIKI_SRCS  =$(patsubst %.msg,%.c,$(ROSTIKI_MSGS))
ROSTIKI_INCS +=$(patsubst %.msg,%.h,$(ROSTIKI_MSGS))

$(ROSTIKI_SRCS) $(ROSTIKI_INCS):
	rosrun contiki_rosnode make_messages.py --path $(ROSTIKI) $(ROSTIKI_MSG)

$(CONTIKI_SOURCEFILES) : $(ROSTIKI_INCS)
endif

include $(CONTIKI)/Makefile.include

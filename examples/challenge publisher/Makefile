all: ping-publisher

UIP_CONF_IPV6=1
UIP_CONF_RPL=1

CONTIKI=../../../contiki
ROSTIKI=../../src
ROSTIKI_MSG=std_msgs rosserial_msgs challenge_msgs

CONTIKI_SOURCEFILES += rostiki.c

# generate the source file list for the required message
ifneq ($(shell which rospack),)
ROSTIKI_MSGS  =$(foreach MSG,$(ROSTIKI_MSG),$(foreach FILE,$(notdir $(wildcard $(shell rospack find $(MSG))/**/*.msg)),$(ROSTIKI)/$(MSG)/$(FILE)))
ROSTIKI_SRCS  =$(patsubst %.msg,%.c,$(ROSTIKI_MSGS))
ROSTIKI_INCS +=$(patsubst %.msg,%.h,$(ROSTIKI_MSGS))

$(ROSTIKI_SRCS) $(ROSTIKI_INCS): $(foreach MSG,$(ROSTIKI_MSG),$(wildcard $(shell rospack find $(MSG))/**/*.msg))
	$(ROSTIKI)/../bin/make_messages.py --path $(ROSTIKI) $(ROSTIKI_MSG)

$(CONTIKI_SOURCEFILES) : $(ROSTIKI_INCS)
endif

CONTIKI_SOURCEFILES += $(notdir $(wildcard $(ROSTIKI)/**/*.c))
include $(CONTIKI)/Makefile.include

SOURCEDIRS += $(ROSTIKI)
vpath %.c $(foreach DIR,$(ROSTIKI_MSG),$(ROSTIKI)/$(DIR)/)

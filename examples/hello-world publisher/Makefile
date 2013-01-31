all: hello-publisher

UIP_CONF_IPV6=1
UIP_CONF_RPL=0

CONTIKI=../../contiki
ROSTIKI=../../src

CONTIKIDIRS += $(ROSTIKI)/ $(filter %/, $(wildcard $(ROSTIKI)/*/))
#CONTIKI_SOURCEFILES += $(wildcard $(ROSTIKI)/**/*.c)
CONTIKI_SOURCEFILES += rostiki.c String.c TopicInfo.c Test.c TestComplex.c

include $(CONTIKI)/Makefile.include

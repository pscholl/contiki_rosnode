/**
 *   Subscribe to the "Hello_World_Topic" and print the received String.
 **/

#include <contiki.h>
#include <std_msgs/String.h>
#include <rostiki.h>

/* define a Contiki process and start it when the program starts */
PROCESS(hello_world_process,"ROS Hello World Publisher");
AUTOSTART_PROCESSES(&hello_world_process);

PROCESS_THREAD(hello_world_process, ev, data)
{
  /* 1. create a topic to subscribe on and set its anticipated type */
  static Topic_t hello_topic = ROS_TOPIC("Hello_World_Topic", String);

  PROCESS_BEGIN();

  /* 2. subscribe to the topic. */
  rostiki_sub(&hello_topic);

  while(1) {
    /* 3. wait until a message has been received. */
    PROCESS_YIELD_UNTIL(ev==hello_topic.event);

    /* 4. we can be sure that data points to a "String_t", otherwise no event
     *    would have been fired, so this cast is safe. */
    String_t *str = (String_t*) data;

    /* 5. print the string */
    puts(str->data);
  }

  PROCESS_END();
}


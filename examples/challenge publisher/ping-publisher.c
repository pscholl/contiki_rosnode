/**
 *   Publish a String message on the "Hello_World_Topic" topic
 **/

#include <contiki.h>
#include <std_msgs/String.h>
#include <rostiki.h>

/* define a Contiki process that will publish the ROS message and let it start
 * when the program starts. */
PROCESS(hello_world_process,"ROS Hello World Publisher");
AUTOSTART_PROCESSES(&hello_world_process);

PROCESS_THREAD(hello_world_process, ev, data)
{
  /* 1. create a topic to publish on and set its published type */
  static Topic_t hello_topic = ROS_TOPIC("Hello_World_Topic", String);

  /* 2. create the message to be published and fill with data. Note that this
   *    message can also be created temporarily on the stack, but Contiki
   *    processes do not so easily. */
  static String_t str = { .data = "Hello World!" };

  /* 3. create an event timer to fire every 0.5 seconds to send the
   *    ROS message. */
  static struct etimer periodic;

  PROCESS_BEGIN();

  /* 4. initialize the timer. */
  etimer_set(&periodic, CLOCK_SECOND/2);

  while(1) {
    /* 5. wait until the timer fired. */
    PROCESS_YIELD_UNTIL(ev==PROCESS_EVENT_TIMER);

    /* 6. publish the ROS message. */
    rostiki_pub(&hello_topic,&str);

    /* 7. restart timer */
    etimer_restart(&periodic);
  }

  PROCESS_END();
}


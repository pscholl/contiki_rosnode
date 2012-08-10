#include <contiki.h>
#include <stdio.h>
#include <ros.h>
#include <std_msgs/String.h>

PROCESS(rostiki_process, "Ros/Contiki Helper");
AUTOSTART_PROCESSES(&rostiki_process);

PROCESS_THREAD(rostiki_process,ev,data)
{
  static struct etimer et;
  static String_t msg = { "Hello World!" };

  PROCESS_BEGIN();

  ros_advertise("chatter", String_t);
  etimer_set(&et, CLOCK_SECOND/5);

  while (1) {
    PROCESS_YIELD_UNTIL(ev==PROCESS_EVENT_TIMER); etimer_restart(&et);
    ros_publish(msg);
  }

  PROCESS_END();
}

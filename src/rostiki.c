//
//  Created by Brahim E on 11.01.13.
//  Copyright (c) 2013 Brahim E. All rights reserved.
//  rostiki_publish.c
//  Studienarbeit
//
//  Created by Brahim E on 11.01.13.
//  Copyright (c) 2013 Brahim E. All rights reserved.
//

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"
#include "net/uip.h"

#include <string.h>
#include <std_msgs/String.h>
#include <rostiki.h>
#include <rosserial_msgs/TopicInfo.h>
#include <time.h>

#define SEND_INTERVAL_FAST  CLOCK_CONF_SECOND*4 //4
#define SEND_INTERVAL_SLOW  CLOCK_CONF_SECOND*16
static clock_time_t send_interval = SEND_INTERVAL_SLOW;

#define UIP_UDPAPPDATA_SIZE  (UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN)
#define UIP_UDPAPPDATA_PTR   (char*) &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN]

PROCESS(rostiki_process, "rostiki process");

static struct uip_udp_conn *client_conn;
static Topic_t* publisher_list  = NULL;
static Topic_t* subscriber_list = NULL;

static void print_local_addresses(void){
    int i;
    uint8_t state;

    PRINTF("Client IPv6 addresses: ");
    for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
        state = uip_ds6_if.addr_list[i].state;
        if(uip_ds6_if.addr_list[i].isused &&
           (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
            PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
            PRINTF("\n");
        }
    }
}

static uint16_t alloc_topic_id()
{
  static uint16_t id = 100; // XXX make dynamic for unsubscribe
  return id++;
}

void
uip_udp_packet_send_nocp(struct uip_udp_conn *c, int len)
{
  extern uint16_t uip_slen;

#if UIP_UDP
  uip_udp_conn = c;
  uip_slen = len;
  uip_process(UIP_UDP_SEND_CONN);
#if UIP_CONF_IPV6
  tcpip_ipv6_output();
#else
  if(uip_len > 0) {
    tcpip_output();
  }
#endif
  uip_slen = 0;
#endif /* UIP_UDP */
}


void rostiki_pub(Topic_t* topic, void* msg)
{
  rosserial_header *obj_rosserial_header = UIP_UDPAPPDATA_PTR;

  if (!process_is_running(&rostiki_process))
    process_start(&rostiki_process, NULL);

  if ( topic->topic_info.topic_id == 0) { // XXX reaplce with define
    if ( publisher_list == NULL ) {
      publisher_list = topic;
    } else {
      Topic_t *tail = publisher_list;
      while (tail->next != NULL)
        tail = tail->next;

      tail->next = topic;
    }

    topic->topic_info.topic_id = alloc_topic_id();
    send_interval = SEND_INTERVAL_FAST;
  }

  /* serialize and send message */
  obj_rosserial_header->topic_id = topic->topic_info.topic_id;
  obj_rosserial_header->sync_flag = 0xffff;
  obj_rosserial_header->msg_length = topic->p_serialize(msg,
      UIP_UDPAPPDATA_PTR+sizeof(*obj_rosserial_header), UIP_UDPAPPDATA_SIZE);

  uip_udp_packet_send_nocp(client_conn,
      sizeof(*obj_rosserial_header)+obj_rosserial_header->msg_length);
}

void rostiki_sub(Topic_t* topic)
{
  if (!process_is_running(&rostiki_process))
    process_start(&rostiki_process, NULL);

  if ( topic->topic_info.topic_id != 0) // XXX: reaplce with define
    return; // already subscribed

  if ( subscriber_list == NULL ) {
    subscriber_list = topic;
  } else {
    Topic_t *tail = subscriber_list;
    while (tail->next != NULL)
      tail = tail->next;

    tail->next = topic;
  }

  topic->topic_info.topic_id = alloc_topic_id();
  topic->event = process_alloc_event();
  topic->process_pt = PROCESS_CURRENT();
  send_interval = SEND_INTERVAL_FAST;
}

static void advertise_and_subscribe() {
  rosserial_header *obj_rosserial_header = UIP_UDPAPPDATA_PTR;
  Topic_t *topic;

  /* send advertisment for all published topics */
  for ( topic = publisher_list; topic; topic = topic->next ) {
    obj_rosserial_header->topic_id = ID_PUBLISHER;
    obj_rosserial_header->sync_flag = 0xffff;
    obj_rosserial_header->msg_length= TopicInfo_serialize(&(topic->topic_info),
        UIP_UDPAPPDATA_PTR+sizeof(*obj_rosserial_header), UIP_APPDATA_SIZE);

    uip_udp_packet_send_nocp(client_conn,
        sizeof(*obj_rosserial_header)+obj_rosserial_header->msg_length);
  }

  /* subscribe to all topics */
 for ( topic = subscriber_list; topic; topic = topic->next ) {
    obj_rosserial_header->topic_id = ID_SUBSCRIBER;
    obj_rosserial_header->sync_flag = 0xffff;
    obj_rosserial_header->msg_length= TopicInfo_serialize(&(topic->topic_info),
        UIP_UDPAPPDATA_PTR+sizeof(*obj_rosserial_header), UIP_APPDATA_SIZE);

    uip_udp_packet_send_nocp(client_conn,
        sizeof(*obj_rosserial_header)+obj_rosserial_header->msg_length);
  }
}

static void tcpip_handler() {
  if ( uip_newdata() ) {
    rosserial_header *obj_rosserial_header = (rosserial_header*) UIP_UDPAPPDATA_PTR;

    if (obj_rosserial_header->sync_flag != 0xFFFF) {
      printf("sync_flag not seen is %d\n", obj_rosserial_header->sync_flag);
      return;
    }

    if (obj_rosserial_header->topic_id == ID_PUBLISHER ||
        obj_rosserial_header->topic_id == ID_SUBSCRIBER) {
      /* XXX acknowledgement to our advertise/subscribe request */
      printf("got subscribed!\n");
      send_interval = SEND_INTERVAL_SLOW;
      return;
    }
    else if (obj_rosserial_header->topic_id >= 100) { // XXX need define
      void *msg; uint16_t msg_length;
      Topic_t *topic;
      for ( topic = subscriber_list; topic; topic = topic->next ) {
        if ( topic->topic_info.topic_id == obj_rosserial_header->topic_id )
          break;
      }

      if ( topic == NULL ) {
        printf("topic with id %d not found\n", obj_rosserial_header->topic_id);
        return;
      }

      msg = topic->p_deserialize(
          UIP_UDPAPPDATA_PTR + sizeof(*obj_rosserial_header), UIP_BUFSIZE );

      if ( msg == NULL ) {
        printf ("deserialization did not work (len:%d)\n", msg_length);
        return;
      }

      process_post_synch(topic->process_pt, topic->event, (void*) msg);
    }
    else {
      printf("msg with unknown topic_id=%d rx'ed\n", obj_rosserial_header->topic_id);
    }
  }
}

PROCESS_THREAD(rostiki_process, ev, data)
{
    static struct etimer et;
    uip_ipaddr_t ipaddr;

    PROCESS_BEGIN();

    PRINTF("UDP client process started\n");
    print_local_addresses();
    uip_ip6addr(&ipaddr,0xaaaa,0,0,0,0,0,0,0xff);

    /* new connection with remote host */
    client_conn = udp_new(&ipaddr, UIP_HTONS(3000), NULL);
    udp_bind(client_conn, UIP_HTONS(3001));
    etimer_set(&et, SEND_INTERVAL_SLOW);

    PRINTF("Created a connection with the server ");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF(" local/remote port %u/%u\n", UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

    while(1){
      PROCESS_YIELD();
      if ( ev == tcpip_event ) {
        tcpip_handler();
      }

      if ( ev == PROCESS_EVENT_TIMER ) {
        advertise_and_subscribe();
        etimer_set(&et, send_interval);
      }
    }

    PROCESS_END();
}


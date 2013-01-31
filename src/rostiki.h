//
//  rostiki.h
//  Studienarbeit
//
//  Created by Brahim E on 11.01.13.
//  Copyright (c) 2013 Brahim E. All rights reserved.
//


#ifndef Studienarbeit_rostiki_h
#define Studienarbeit_rostiki_h

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>
#include <std_msgs/String.h>
#include <rosserial_msgs/TopicInfo.h>
PROCESS_NAME(rostiki_process);

#define ID_PUBLISHER  0
#define ID_SUBSCRIBER 1

#define ROS_TOPIC(name,type)  {.topic_info = {.topic_id = 0, .topic_name = name, .message_type = type##_rostype, .md5sum = type##_md5, .buffer_size = 0}, .next = NULL, .p_deserialize_size = type##_deserialize_size, .p_deserialize = type##_deserialize,.p_serialize = type##_serialize}


typedef struct Topic_t {
    TopicInfo_t topic_info;
    struct Topic_t* next;

    void* (*p_deserialize_size)(char *buf, char *to, size_t *n);
    void* (*p_deserialize)(char *buf, size_t n);
    size_t (*p_serialize)(String_t *obj, char *buf, size_t n);

    /* Contiki event integration */
    process_event_t event;
    struct process *process_pt;
} Topic_t;


typedef struct rosserial_header{
    uint16_t sync_flag;
    uint16_t topic_id;
    uint16_t msg_length;
} __attribute__((packed)) rosserial_header; 

//Methoden
extern void rostiki_pub(Topic_t* topic, void* msg);
extern void rostiki_sub(Topic_t* topic_temp);


#endif


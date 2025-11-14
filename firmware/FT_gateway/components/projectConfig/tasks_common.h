/**
 * @file tasks_common.h
 * @brief 
 * @details
 * @date 16 de nov. de 2024
 * @author Luiz Carlos
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// Wifi application task
#define WIFI_APP_TASK_STACK_SIZE		4096
#define WIFI_APP_TASK_PRIORITY			5

// HTTP Server task
#define HTTP_SERVER_STACK_SIZE			8192
#define HTTP_SERVER_TASK_PRIORITY		4

// HTTP Server Monitor task
#define HTTP_SERVER_MONITOR_STACK_SIZE	4096
#define HTTP_SERVER_MONITOR_PRIORITY	3

// Main Loop Task
#define MAIN_LOOP_TASK_STACK_SIZE		1024
#define MAIN_LOOP_TASK_PRIORITY		    6

// NTP DateTime Task
#define NTP_DATE_TIME_TASK_STACK_SIZE	4096
#define NTP_DATE_TIME_TASK_PRIORITY     4

// Piepework Task
#define IRRIGATOR_MONITOR_STACK_SIZE	1024
#define IRRIGATOR_MONITOR_PRIORITY	    6

#endif /* MAIN_TASKS_COMMON_H_ */

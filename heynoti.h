/*
 * heynoti
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>,
 * Jaeho Lee <jaeho81.lee@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */



#ifndef __HEYNOTI_H__
#define __HEYNOTI_H__

/**
 * @addtogroup APPLICATION_FRAMEWORK
 * @{
 *
 * @defgroup   HEYNOTI HEY(ligHt Easy speedY) Notification
 * @brief      HEY(ligHt Easy speedY) Notification
 *
 * @section Header To use Them:
 * @code
 * #include <heynoti.h>
 * @endcode
 *
 * @section Properties
 * - Light, Easy and speedy notification (in other words,  HEY - ligHt Easy speedY)
 * - Convenient API
 * - Simple notification based on inotify
 *
 * @section example Simple Example
 * @code
#include <stdio.h>
#include <glib.h>
#include <heynoti.h>

void callback(void *data)
{
   printf("I got a testnoti\n");
}

int main(int argc, const char *argv[])
{
   int fd;
   GMainLoop *loop;

   if((fd = heynoti_init()) < 0) {
      fprintf(stderr, "heynoti_init FAIL\n");
      return -1;
   }else
      printf("heynoti_init OK\n");

   if(heynoti_subscribe(fd, "test_testnoti", callback, NULL))
      fprintf(stderr, "heynoti_subscribe FAIL\n");
   else
      printf("heynoti_subscribe OK\n");

    if(heynoti_attach_handler(fd))
       fprintf(stderr, "heynoti_attach_handler FAIL\n");
    else
       printf("heynoti_attach_handler OK\n");

   loop = g_main_loop_new(NULL, FALSE);
   g_main_loop_run(loop);

   heynoti_close(fd);
   g_main_loop_unref(loop);

   return 0;
}
 * @endcode
 */

/**
 * @addtogroup HEYNOTI
 * @{
 */

#include <sys/types.h>
#include <sys/inotify.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************
 * API for Notification                         *
 ************************************************/

/**
 * \par Description:
 * Initialize the notify service\n
 * Get file descriptor associated with a new inotify event queue.\n
 *
 * \par Purpose:
 * This API is used for initializing notify service.
 *
 * \par Typical use case:
 * If user want to initialize notify service, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \return Return Type (int) \n
 * - fd	- fild descriptor. \n
 * - -1	- fail to create file descriptor. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \par Related functions:
 * heynoti_close()
 *
 * \pre None
 * \post None
 * \see heynoti_close()
 * \remark	None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h> //Include heynoti.h file
 * ...
 *
 *	int fd = heynoti_init(); //Initialize heynoti
 *
 *	if (fd < 0) {
 *		printf("heynoti_init() failed\n");
 *		return;  //return if unavailable file descriptor is returned
 *	}
 *
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_init(void);

/**
 * \par Description:
 * Finalizes notification service\n
 * Close the file descriptor and remove handler related fd.\n
 *
 * \par Purpose:
 * This API is used for finalizing notify service.
 *
 * \par Typical use case:
 * If user want to finalize notify service, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param fd	[in]	file descriptor that is created by calling heynoti_ini().
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \par Related functions:
 * heynoti_init()
 *
 * \pre heynoti_init()
 * \post None
 * \see heynoti_init()
 * \remark	None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h> //Include heynoti.h file
 * ...
 *	int fd = heynoti_init(); //Initialize heynoti
 *
 *	if (fd < 0) {
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *
 *	heynoti_close(fd); //Finalize heynoti
 * ...
 * \endcode
 */
/*================================================================================================*/
void heynoti_close(int fd);

/**
 * \par Description:
 * Register a new notification callback function with noti name\n
 *
 * \par Purpose:
 * This API is used for registering a new notification callback function with noti name.
 *
 * \par Typical use case:
 * If user want to regist a new notification callback function with noti name, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	fd	[in]	notify file descriptor created by heynoti_init()
 * \param	noti	[in]	notification name
 * \param	cb	[in]	callback function pointer
 * \param	data	[in]	callback function data
 *
 * \return Return Type (int) \n
 * - 0	- success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre None
 * \post None
 * \see heynoti_unsubscribe()
 * \remark  None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	int fd;
 *	char sys_noti[20];
 *
 *	if((fd = heynoti_init()) < 0) //Initialize heynoti
 *	{
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti))) //Get system noti name
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *		heynoti_close(fd);  //Finalize heynoti and return if unavailable noti name is returned
 *		return;
 *	}
 *
 *	if((heynoti_subscribe(fd, sys_noti, callback, NULL))< 0)
 *	{
 *		fprintf(stderr, "heynoti_subscribe fail\n");
 *	}
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_subscribe(int fd, const char *noti, void (*cb)(void *), void *data);

/**
 * \par Description:
 * Unregister the notification callback function with noti name
 *
 * \par Purpose:
 * This API is used for unregistering a notification callback function with noti name.
 *
 * \par Typical use case:
 * If user want to unregist a notification callback function with noti name, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 * \pre heynoti_init()
 * \post None
 * \see heynoti_subscribe()
 * \remark  None
 *
 * \param	fd	[in]	notify file descriptor created by heynoti_init()
 * \param	noti	[in]	notification name
 * \param	cb	[in]	callback function pointer
 *
 * \return Return Type (int) \n
 * - 0	- success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	int fd;
 *	char sys_noti[20];
 *
 *	if((fd = heynoti_init()) < 0) //Initialize heynoti
 *	{
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *		heynoti_close(fd);  //Finalize heynoti and return if unavailable noti name is returned
 *		return;
 *	}
 *
 *	if((heynoti_subscribe(fd, sys_noti, callback, NULL))< 0)
 *	{
 *		fprintf(stderr, "heynoti_subscribe fail\n");
 *	}
 * 	
 *	if (heynoti_unsubscribe(fd, sys_noti, callback) < 0)
 *	{
 *		fprintf(stderr, "heynoti_unsubscribe fail\n");
 *	}
 *
 *	heynoti_close(fd);
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_unsubscribe(int fd, const char *noti, void (*cb)(void *));

/**
 * \par Description:
 * Send a notification
 *
 * \par Purpose:
 * This API is used for sending a notification
 *
 * \par Typical use case:
 * If user want to send a notification, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	noti	[in]	notification name
 *
 * \return Return Type (int) \n
 * - 0	- success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \par Related functions:
 * None
 * \pre None
 * \post None
 * \see None
 * \remark  None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	char sys_noti[20];
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *	}
 *
 *	if(heynoti_publish(sys_noti)) //Publish with notiname
 *		fprintf(stderr, "heynoti_publish fail\n");
 *	else
 *		fprintf(stderr, "heynoti_publish ok\n");
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_publish(const char *noti);

/**
 * \par Description:
 * Wait(block) for some notification of the file descriptor
 *
 * \par Purpose:
 * This API is used for waiting(block) for some notification of the file descriptor
 *
 * \par Typical use case:
 * If user want to waiting for some notification of the file descriptor, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	fd	[in]	notify file descriptor created by heynoti_init()
 *
 * \return Return Type (int) \n
 * - positive number - success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre heynoti_init()
 * \post None
 * \see None
 * \remark  None
 *
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	int fd;
 *	char sys_noti[20];
 *
 *	if((fd = heynoti_init()) < 0)
 *	{
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *	}
 *
 *	if((heynoti_subscribe(fd, sys_noti, callback, NULL))< 0)
 *	{
 *		fprintf(stderr, "heynoti_subscribe fail\n");
 *	}
 * ...
 *
 *	if (heynoti_poll_event(fd) < 0) //Polling and waiting for event 
 *	{
 *		fprintf(stderr, "heynoti_poll_event() failed");
 *	}
 *
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_poll_event(int fd);

/**
 * \par Description:
 * Activate whole heynoti callback which is added by heynoti_subscribe
 * Attach a fd handler to the default context of g_main_loop.\n
 * Notification is recognized by main loop
 *
 * \par Purpose:
 * This API is used for activating whole heynoti callback which is added by heynoti_subscribe.
 *
 * \par Typical use case:
 * If user want to activate whole heynoti callback which is added by heynoti_subscribe, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	fd	[in]	notify file descriptor created by heynoti_init()
 *
 * \return Return Type (int) \n
 * - 0 - success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre heynoti_init()
 * \post None
 * \see heynoti_init(), heynoti_detach_handler()
 * \remark	None
 *
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	int fd;
 *	char sys_noti[20];
 *
 *	if((fd = heynoti_init()) < 0)
 *	{
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *	}
 *
 *	if((heynoti_subscribe(fd, sys_noti, callback, NULL))< 0)
 *	{
 *		fprintf(stderr, "heynoti_subscribe fail\n");
 *	}
 *
 *	if(heynoti_attach_handler(fd)) //Attach handler to main loop
 *	{
 *		fprintf(stderr, "heynoti_attach_handler fail\n");
 *	}
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_attach_handler(int fd);

/**
 * \par Description:
 * Deactivate whole heynoti callback which is deleted by heynoti_unsubscribe
 * Detach a fd handler from the default context of g_main_loop.\n
 * Be detached the handler without calling heynoti_unsubscribe(), the event will reuse it that is attached before.
 *
 * \par Purpose:
 * This API is used for deactivating whole heynoti callback which is added by heynoti_subscribe.
 *
 * \par Typical use case:
 * If user want to deactivate whole heynoti callback which is added by heynoti_subscribe, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	fd	[in]	notify file descriptor created by heynoti_init()
 *
 * \return Return Type (int) \n
 * - 0 - success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre heynoti_init()
 * \post None
 * \see heynoti_attach_handler()
 * \remark	None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	int fd;
 *	char sys_noti[20];
 *
 *	if((fd = heynoti_init()) < 0)
 *	{
 *		fprintf(stderr, "heynoti_init fail, error_code: %d\n", fd);
 *		return; //return if unavailable file descriptor is returned
 *	}
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *	}
 *
 *	if((heynoti_subscribe(fd, sys_noti, callback, NULL))< 0)
 *	{
 *		fprintf(stderr, "heynoti_subscribe fail\n");
 *	}
 *	if(heynoti_attach_handler(fd)) //Attach handler to main loop
 *	{
 *		fprintf(stderr, "heynoti_attach_handler fail\n");
 *	}
 *	if(heynoti_detach_handler(fd) < 0) //Detach handler from main loop
 *	{
 *		fprintf(stderr, "heynoti_detach_handler() failed");
 *	}
 * ...
 * \endcode
 */
int heynoti_detach_handler(int fd);

/**
 * \par Description:
 * Make a name of system notification
 *
 * \par Purpose:
 * This API is used for making a name of system notification
 *
 * \par Typical use case:
 * If user want to make a name of system notification, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	name		[in]	notification name
 * \param	buf buffer	[out]	to get a system notification
 * \param	buf_size	[in]	The size of buffer should be "@p name + 6 or more"
 *
 * \return Return Type (int) \n
 * - 0	- success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre None
 * \post None
 * \see None
 * \remark  None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	char sys_noti[20];
 *
 *	if(heynoti_get_snoti_name("test_testnoti", sys_noti, sizeof(sys_noti)))
 *	{
 *		fprintf(stderr, "heynoti_get_snoti_name fail");
 *	}
 *
 *	if(heynoti_publish(sys_noti)) //Publish
 *		fprintf(stderr, "heynoti_publish fail\n");
 *	else
 *		fprintf(stderr, "heynoti_publish ok\n");
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_get_snoti_name(const char *name, char *buf, int buf_size);

/**
 * \par Description:
 * Make the name of process notification
 * <br>buf must be allocated.
 *
 * \par Purpose:
 * This API is used for making a name of process notification
 *
 * \par Typical use case:
 * If user want to make a name of process notification, he(or she) can use this API.
 *
 * \par Important notes:
 * None
 *
 * \param	pid		[in]	process id
 * \param	name		[in]	notification name
 * \param	buf buffer	[out]	to get a name of process notification
 * \param	buf_size	[in]	The size of buffer should be more than the sum of characters in pid, name and extra space(3 bytes)
 *
 * \return Return Type (int) \n
 * - 0	- success. \n
 * - -1	- fail. \n
 *
 * \par Prospective clients:
 * External Apps.
 *
 * \pre heynoti_init()
 * \post None
 * \see None
 * \remark	None
 * \par Sample code:
 * \code
 * ...
 * #include <heynoti.h>
 * ...
 *	char process_noti[25];
 *
 *	if(heynoti_get_pnoti_name(getpid(), "test_testnoti", process_noti, sizeof(process_noti));
 *	{
 *		fprintf(stderr, "heynoti_get_pnoti_name fail");
 *	}
 *
 *	if(heynoti_publish(process_noti)) //Publish
 *		fprintf(stderr, "heynoti_publish fail\n");
 *	else
 *		fprintf(stderr, "heynoti_publish ok\n");
 * ...
 * \endcode
 */
/*================================================================================================*/
int heynoti_get_pnoti_name(pid_t pid, const char *name, char *buf, int buf_size);


#ifdef __cplusplus
}
#endif

/**
 * @} @}
 */

#endif /* __HEYNOTI_H__ */

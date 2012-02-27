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


#include <stdio.h>
#include <stdlib.h>
#include <Ecore.h>

#include "heynoti.h"

#define NOTINAME "APP_LAUNCH_COMPLETED"

Ecore_Event_Handler *handler;

int cnt;

Eina_Bool exit_func(void *data, int ev_type, void *ev)
{
	Ecore_Event_Signal_Exit *e;

	e = (Ecore_Event_Signal_Exit *) ev;
	if (e->interrupt)
		printf("Exit: interrupt\n");
	else if (e->quit)
		printf("Exit: quit\n");
	else if (e->terminate)
		printf("Exit: terminate\n");

	ecore_main_loop_quit();

	return ECORE_CALLBACK_PASS_ON;
}

void callback(void *data)
{
	int fd = (int)data;
	cnt--;
	printf("%s: %d\n", __func__, cnt);

	if (cnt == 0) {
		heynoti_detach_handler(fd);
		printf("detach handler\n");
	}
}

int main(int argc, const char *argv[])
{
	int ret;
	int fd;
	char n[FILENAME_MAX];

	ecore_init();
	ecore_app_args_set(argc, argv);

	fd = heynoti_init();
	printf("Noti init: %d\n", fd);
	if (fd == -1)
		return -1;

	handler =
	    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_func, NULL);

	if (argv[1])
		ret = heynoti_get_snoti_name(argv[1], n, sizeof(n));
	else
		ret = heynoti_get_snoti_name(NOTINAME, n, sizeof(n));
	if (!ret) {
		ret = heynoti_subscribe(fd, n, callback, (void *)fd);
		printf("noti add: %d\n", ret);
	}
	if (ret == -1)
		goto err;

	cnt = 10;
	ret = heynoti_attach_handler(fd);
	printf("attach handler : %d\n", ret);
	if (ret == -1)
		goto err;

	ecore_main_loop_begin();
 err:
	heynoti_close(fd);
	ecore_shutdown();

	return 0;
}


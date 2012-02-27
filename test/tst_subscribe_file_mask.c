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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <Ecore.h>

#include "heynoti.h"

#define NOTI_DIRECT "/tmp/noti/direct"

int flag;
int fd;
Ecore_Event_Handler *handler;

#define NOTI_CB(n) callback##n

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

void in_access(void *data)
{
	printf("%s\n", __func__);
}

void in_modify(void *data)
{
	printf("%s\n", __func__);
}

void in_close_write(void *data)
{
	printf("%s\n", __func__);
}

void in_close_nowrite(void *data)
{
	printf("%s\n", __func__);
}

void in_open(void *data)
{
	printf("%s\n", __func__);
}

void term(int signo)
{
	flag = 1;
}

void step(int signo)
{
	static int step_cnt = 0;
	int r;

	printf("%s\n", __func__);

	switch (step_cnt) {
	case 0:
		r = heynoti_unsubscribe_file(fd, NOTI_DIRECT, in_access);
		printf("noti del: in_access : %d\n", r);
		break;
	case 1:
		r = heynoti_unsubscribe_file(fd, NOTI_DIRECT, in_modify);
		printf("noti del: in_modify : %d\n", r);
		break;
	case 2:
		r = heynoti_unsubscribe_file(fd, NOTI_DIRECT, in_close_write);
		printf("noti del: in_close_write : %d\n", r);
		break;
	case 3:
		r = heynoti_unsubscribe_file(fd, NOTI_DIRECT, in_close_nowrite);
		printf("noti del: in_close_nowrite : %d\n", r);
		break;
	case 4:
		r = heynoti_unsubscribe_file(fd, NOTI_DIRECT, in_open);
		printf("noti del: in_open : %d\n", r);
		break;
	}

	step_cnt++;
}

int main(int argc, const char *argv[])
{
	int ret;

	ecore_init();
	ecore_app_args_set(argc, argv);

	fd = heynoti_init();
	printf("Noti init: %d\n", fd);
	if (fd == -1)
		return -1;

	handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_func, NULL);

	ret = heynoti_subscribe_file(fd, NOTI_DIRECT, in_access, NULL, IN_ACCESS);
	printf("noti add: in_access : %d\n", ret);

	ret = heynoti_subscribe_file(fd, NOTI_DIRECT, in_modify, NULL, IN_MODIFY);
	printf("noti add: in_modify : %d\n", ret);

	ret = heynoti_subscribe_file(fd, NOTI_DIRECT, in_close_write, NULL, IN_CLOSE_WRITE);
	printf("noti add: in_close_write : %d\n", ret);

	ret = heynoti_subscribe_file(fd, NOTI_DIRECT, in_close_nowrite, NULL, IN_CLOSE_NOWRITE);
	printf("noti add: in_close_nowrite : %d\n", ret);

	ret = heynoti_subscribe_file(fd, NOTI_DIRECT, in_open, NULL, IN_OPEN);
	printf("noti add: in_open : %d\n", ret);

	ret = heynoti_attach_handler(fd);
	printf("add main: %d\n", ret);
	if (ret == -1)
		goto err;

	while (!flag) {
		sleep(1);
	}

	ecore_main_loop_begin();
 err:
	heynoti_close(fd);
	ecore_shutdown();

	return 0;
}


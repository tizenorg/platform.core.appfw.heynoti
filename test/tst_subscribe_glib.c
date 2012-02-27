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
#include <glib.h>

#include "heynoti.h"

#define NOTINAME "APP_LAUNCH_COMPLETED"

static GMainLoop *loop;
static int cnt;

void callback(void *data)
{
	int fd = (int)data;
	cnt--;
	printf("%s: %d\n", __func__, cnt);

	if (cnt == 0) {
		heynoti_detach_handler(fd);
		printf("detach handler\n");
		return;
	}
}

int main(int argc, const char *argv[])
{
	int r;
	int fd;
	char n[FILENAME_MAX];

	fd = heynoti_init();
	printf("Noti init: %d\n", fd);
	if (fd == -1)
		return -1;

	if (argv[1])
		r = heynoti_get_snoti_name(argv[1], n, sizeof(n));
	else
		r = heynoti_get_snoti_name(NOTINAME, n, sizeof(n));
	if (!r) {
		r = heynoti_subscribe(fd, n, callback, (void *)fd);
		printf("noti add: %d\n", r);
	}

	if (r == -1)
		goto err;

	cnt = 10;
	r = heynoti_attach_handler(fd);
	printf("attach handler : %d\n", r);
	if (r == -1)
		goto err;

	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);
 err:
	heynoti_close(fd);
	g_main_loop_unref(loop);

	return 0;
}


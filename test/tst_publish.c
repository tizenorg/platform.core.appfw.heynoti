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

#include "heynoti.h"

int main(int argc, char *argv[])
{
	if (argc > 1) {
		char p[FILENAME_MAX];

		if (!heynoti_get_snoti_name(argv[1], p, sizeof(p)))
			heynoti_publish(p);
		else
			printf("noti name error\n");
	} else {
		printf("usage) %s NOTI_NAME\n", argv[0]);
	}

	return 0;
}


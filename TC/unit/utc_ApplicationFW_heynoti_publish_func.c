/*
 *  heynoti
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
#include <tet_api.h>
#include <heynoti.h>

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_ApplicationFW_heynoti_publish_func_01(void);
static void utc_ApplicationFW_heynoti_publish_func_02(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{ utc_ApplicationFW_heynoti_publish_func_01, POSITIVE_TC_IDX },
	{ utc_ApplicationFW_heynoti_publish_func_02, NEGATIVE_TC_IDX },
	{ NULL, 0},
};

static void startup(void)
{
}

static void cleanup(void)
{
}

/**
 * @brief Positive test case of heynoti_publish()
 */
static void utc_ApplicationFW_heynoti_publish_func_01(void)
{
	int r = 0;
	char sys_noti[20];
	heynoti_get_snoti_name("noti", sys_noti, sizeof(sys_noti));
	r = heynoti_publish(sys_noti);

	if (r) {
		tet_infoline("heynoti_publish() failed in positive test case");
		tet_result(TET_FAIL);
		return;
	}
	tet_result(TET_PASS);
}

/**
 * @brief Negative test case of ug_init heynoti_publish()
 */
static void utc_ApplicationFW_heynoti_publish_func_02(void)
{
	int r = 0;


	r = heynoti_publish(NULL);

	if (!r) {
		tet_infoline("heynoti_publish() failed in negative test case");
		tet_result(TET_FAIL);
		return;
	}
	tet_result(TET_PASS);
}

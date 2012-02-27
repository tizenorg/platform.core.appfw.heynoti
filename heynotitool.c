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
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include "heynoti.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#define BUFSIZE		1024

const int SHARED_PERM = 0666;
const int USER_PERM = 0644;

const char *HEYNOTI_PREFIX = "/opt/share/noti";

static int is_app = FALSE;
static int perm = 0;
static int user_id = 5000;


static GOptionEntry entries[] = {
	{"application", 'a', 0, G_OPTION_ARG_NONE, &is_app,
	"allow for application", NULL},
	{NULL}
};


static void __print_help(const char *cmd)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "[Set heynoti key]\n");
	fprintf(stderr,
		"       %s set <KEY NAME>\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "       Ex) %s set heynoti_test\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "       <OPTIONS>\n");
	fprintf(stderr,
		"          -a : Allow application to publish the key.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "       Ex) %s set heynoti_test2 -a\n", cmd);
	fprintf(stderr, "\n");
//	fprintf(stderr, "       %s unset <KEY NAME>\n", cmd);
//	fprintf(stderr, "\n");
//	fprintf(stderr, "       Ex) %s unset heynoti_test\n", cmd);
//	fprintf(stderr, "\n");
}

static int __make_file_path(char *pszKey, char *pszBuf)
{
	snprintf(pszBuf, BUFSIZE, "%s/%s", HEYNOTI_PREFIX, pszKey);
	return 0;
}

int main(int argc, char **argv)
{
	char szFilePath[BUFSIZE] = { 0, };
	int fd;

	GError *error = NULL;
	GOptionContext *context;

	g_type_init();
	context = g_option_context_new("- vconf library tool");
	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_set_help_enabled(context, FALSE);
	g_option_context_set_ignore_unknown_options(context, TRUE);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("option parsing failed: %s\n", error->message);
		exit(1);
	}

	if (argc < 2) {
		__print_help(argv[0]);
		return 1;
	}

	if (!strncmp(argv[1], "set", 3)) {
		if (argc < 3) {
			__print_help(argv[0]);
			return 1;
		}


		/*  Start DAC  *************************************/
		if (0 != getuid()) {
			fprintf(stderr,
				"Error!\t Only root user can use '-g or -u' option\n");
			return -1;
		}

		if (__make_file_path(argv[2], szFilePath)) {
			fprintf(stderr, "Error!\t Bad prefix\n");
			return -1;
		}
		/*  End DAC  ***************************************/


		/*  Start File creation ********************************/
		if(is_app)
			perm = SHARED_PERM;
		else
			perm = USER_PERM;

		if ((fd = open(szFilePath, O_RDONLY)) == -1) {
			if ((fd = open(szFilePath, O_CREAT, 0644)) < 0) {
				return -1;
			}
			fchmod(fd, perm);
			close(fd);
		} else {
			fchmod(fd, perm);
			close(fd);
		}
		/*  End File creation **********************************/

	} else if (!strncmp(argv[1], "unset", 5)) {
		if (argv[2]) {
			if (__make_file_path(argv[2], szFilePath)) {
				fprintf(stderr, "Error!\t Bad key string\n");
				return -1;
			}

			if (remove(szFilePath)) {
				fprintf(stderr, "Error!\t fail to remove file\n");
				return -1;
			}
		}
		else
			__print_help(argv[0]);
	} else
		fprintf(stderr, "%s is a invalid command\n", argv[1]);
	return 0;
}


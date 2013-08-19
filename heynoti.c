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



#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/version.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>
#include <glib.h>
#include <sys/utsname.h>

#include "heynoti-internal.h"

#define AU_PREFIX_SYSNOTI "SYS"

#ifndef NOTI_ROOT
#  define NOTI_ROOT "/opt/share/noti"
#endif


#define NODAC_PERMISSION

struct noti_slot {
	int wd;
	void *cb_data;
	void (*cb) (void *);
	uint32_t mask;
};
typedef struct noti_slot nslot;

static int __make_noti_root(const char *p);
static int __make_noti_file(const char *p);
static inline int __make_noti_path(char *path, int size, const char *name);
static int __read_proc(const char *path, char *buf, int size);
static void __clear_nslot_list(GList *g_ns);
static struct noti_cont *__get_noti_cont(int fd);
static int __handle_callback(struct noti_cont *nc, int wd, uint32_t mask);
static int __handle_event(int fd);
static inline int __get_wd(int fd, const char *notipath);
static int __add_wd(struct noti_cont *nc, int wd, uint32_t mask,
		  const char *notipath);
static int __add_noti(int fd, const char *notipath, void (*cb) (void *),
		    void *data, uint32_t mask);
static int _del_noti(struct noti_cont *nc, int wd, void (*cb) (void *),
		     const char *notipath);
static int del_noti(int fd, const char *notipath, void (*cb) (void *));


int slot_comp(struct noti_slot *a, struct noti_slot *b)
{
	int r;

	r = a->wd - b->wd;
	if (r != 0)
		return r;

	r = (int)(a->cb - b->cb);
	return r;
}

typedef enum {
	H_NONE,
	H_SIGNAL,
	H_ECORE,
	H_GLIB,
} htype;

struct noti_cont {
	int fd;
	GList *g_ns;

	htype ht;

	int signo;
	struct sigaction oldact;

	void *handler;
};
typedef struct noti_cont ncont;

static const char *noti_root = NOTI_ROOT;
static GList *g_nc;

static int __make_noti_root(const char *p)
{
	int fd;

	if ((fd = open(p, O_RDONLY)) == -1 )
		return -1;

	close(fd);

	return 0;
}

static int __make_noti_file(const char *p)
{
	int fd;

	if ((fd = open(p, O_RDONLY)) == -1)
		return -1;

	close(fd);

	return 0;
}

static inline int __make_noti_path(char *path, int size, const char *name)
{
	return snprintf(path, size, "%s/%s", noti_root, name);
}

static int __read_proc(const char *path, char *buf, int size)
{
	int fd;
	int ret;

	if (buf == NULL || path == NULL) {
		UTIL_DBG("%s: path or buf is NULL\n", __func__);
		errno = EINVAL;
		return -1;
	}

	fd = open(path, O_RDONLY);
	util_retvm_if(fd == -1, -1, "File open error: %s", strerror(errno));

	memset(buf, 0x0, size);
	ret = read(fd, buf, size);
	close(fd);

	return ret;
}

#define PROC_VERSION "/proc/version"

static int __check_kern_ver()
{
	struct utsname nm;
	int ret;
	char buf[1024];
	int v1;
	int v2;
	int v3;

	ret = uname(&nm);
	if (ret == 0) {
		strncpy(buf, nm.release, sizeof(buf));

		ret = sscanf(buf, "%d.%d.%d", &v1, &v2, &v3);
	} else {
		__read_proc(PROC_VERSION, buf, sizeof(buf));

		ret = sscanf(buf, "Linux version %d.%d.%d", &v1, &v2, &v3);
	}

	if (ret == 3) {
		if(KERNEL_VERSION(v1, v2, v3) < KERNEL_VERSION(2, 6, 13)) {
			return -1;
		} else {
			return 0;
		}
	}

	return -1;
}

static void __clear_nslot_list(GList *g_ns)
{
	struct noti_slot *t;
	GList *it = NULL;

	for (it = g_ns; it != NULL; it = g_list_next(it)) {
		t = (struct noti_slot *)it->data;
		free(t);
	}

}

static struct noti_cont *__get_noti_cont(int fd)
{
	struct noti_cont *r = NULL;
	GList *it;

	for (it = g_nc; it != NULL; it = g_list_next(it)) {
		if (it->data) {
			r = (struct noti_cont *)it->data;
			if (r->fd == fd) {
				break;
			} else {
				r = NULL;
			}
		}
	}

	return r;
}

static int __handle_callback(struct noti_cont *nc, int wd, uint32_t mask)
{
	struct noti_slot *t;
	GList *it = NULL;

	if (!nc->g_ns)
		return 0;

	for (it = nc->g_ns; it != NULL; it = g_list_next(it)) {
		t = (struct noti_slot *)it->data;
		if (t->wd == wd) {
			if ((mask & t->mask) && t->cb) {
				t->cb(t->cb_data);
			}
		}
	}

	return 0;
}

static int __handle_event(int fd)
{
	int r;
	struct inotify_event ie;
	char name[FILENAME_MAX] = {0, };

	struct noti_cont *nc;

	nc = __get_noti_cont(fd);
	util_warn_if(nc == NULL, "Non-registered file descriptor");

	r = read(fd, &ie, sizeof(ie));

	while (r > 0) {
		if (nc)
			__handle_callback(nc, ie.wd, ie.mask);

		if(ie.len > SSIZE_MAX)
			return -1;

		if (ie.len > 0u) {
			r = read(fd, name, (ie.len > FILENAME_MAX) ? (size_t)FILENAME_MAX : (size_t) ie.len);
		}

		if(r > 0) {
			r = read(fd, &ie, sizeof(ie));
		}
	}

	return 0;
}

API int heynoti_poll_event(int fd)
{
	int r;
	struct noti_cont *nc;
	struct pollfd fds[1];

	nc = __get_noti_cont(fd);
	if (nc == NULL) {
		UTIL_ERR("Non-registered file descriptor : %d", fd);
		errno = EBADF;
		return -1;
	}

	if (nc->ht != H_NONE) {
		UTIL_ERR("Another handler already in progress");
		errno = EALREADY;
		return -1;
	}

	fds[0].fd = nc->fd;
	fds[0].events = POLLIN;

	r = poll(fds, 1, -1);
	util_retvm_if(r == -1, -1, "Error: poll : %s", strerror(errno));

	if (fds[0].revents & POLLIN)
		__handle_event(fd);

	return r;
}

static inline int __get_wd(int fd, const char *notipath)
{
	return inotify_add_watch(fd, notipath, IN_ACCESS);
}

static int __add_wd(struct noti_cont *nc, int wd, uint32_t mask,
		    const char *notipath)
{
	int r;
	uint32_t mask_all;
	struct noti_slot *t;
	GList *it;

	mask_all = 0;
	for (it = nc->g_ns; it != NULL; it = g_list_next(it)) {
		t = (struct noti_slot *)it->data;
		if (t->wd == wd) {
			mask_all |= t->mask;
		}
	}

	mask_all |= mask;

	r = inotify_add_watch(nc->fd, notipath, mask_all);
	return r;
}

static int __add_noti(int fd, const char *notipath, void (*cb) (void *),
		      void *data, uint32_t mask)
{
	int r;
	int wd;
	struct noti_cont *nc;
	struct noti_slot t, *n;
	struct noti_slot *f = NULL;
	GList *it;

	nc = __get_noti_cont(fd);
	if (nc == NULL) {
		UTIL_DBG("Bad file descriptor");
		errno = EBADF;
		return -1;
	}

	wd = __get_wd(fd, notipath);
	util_retvm_if(wd == -1, -1, "Error: add noti: %s", strerror(errno));

	for (it = nc->g_ns; it != NULL; it = g_list_next(it)) {
		if (it->data) {
			f = (struct noti_slot *)it->data;
			if (f->wd == wd && f->cb == cb) {
				break;
			} else {
				f = NULL;
			}
		}
	}

	if (f) {
		__add_wd(nc, wd, 0, notipath);
		errno = EALREADY;
		return -1;
	}

	r = __add_wd(nc, wd, mask, notipath);
	util_retvm_if(r == -1, -1, "Error: add noti: %s", strerror(errno));

	n = calloc(1, sizeof(nslot));
	util_retvm_if(n == NULL, -1, "Error: add noti: %s", strerror(errno));

	n->wd = wd;
	n->cb_data = data;
	n->cb = cb;
	n->mask = mask;
	nc->g_ns = g_list_append(nc->g_ns, (gpointer) n);

	return 0;
}

API int heynoti_subscribe(int fd, const char *noti, void (*cb) (void *),
			  void *data)
{
	char notipath[FILENAME_MAX];

	if (noti == NULL || cb == NULL) {
		UTIL_DBG("Error: add noti: Invalid input");
		errno = EINVAL;
		return -1;
	}

	__make_noti_path(notipath, sizeof(notipath), noti);
	UTIL_DBG("add watch: [%s]", notipath);

	return __add_noti(fd, notipath, cb, data, IN_CLOSE_WRITE | IN_DELETE);
}

static int _del_noti(struct noti_cont *nc, int wd, void (*cb) (void *),
		     const char *notipath)
{
	int r = 0;
	struct noti_slot *t;
	int n_del;
	int n_remain;
	GList *it;
	GList *it_next;

	n_del = 0;
	n_remain = 0;

	it = nc->g_ns;
	while (it) {
		it_next = it->next;

		if (it->data) {
			t = (struct noti_slot *)it->data;
			if (t->wd == wd) {
				if (cb == NULL || cb == t->cb) {
					nc->g_ns = g_list_remove(nc->g_ns, t);
					free(t);
					n_del++;
				} else {
					n_remain++;
				}
			}
		}

		it = it_next;
	}

	if (n_remain == 0)
		return inotify_rm_watch(nc->fd, wd);

	r = __add_wd(nc, wd, 0, notipath);

	if (n_del == 0) {
		UTIL_DBG("Error: nothing deleted");
		errno = ENOENT;
		return -1;
	}

	return r;
}

static int del_noti(int fd, const char *notipath, void (*cb) (void *))
{
	int wd;
	struct noti_cont *nc;

	nc = __get_noti_cont(fd);
	if (nc == NULL) {
		UTIL_DBG("Bad file descriptor");
		errno = EBADF;
		return -1;
	}

	/* get wd */
	wd = __get_wd(fd, notipath);
	util_retv_if(wd == -1, -1);

	return _del_noti(nc, wd, cb, notipath);
}

API int heynoti_unsubscribe(int fd, const char *noti, void (*cb) (void *))
{
	int r;
	char notipath[FILENAME_MAX];

	if (fd < 0 || noti == NULL) {
		errno = EINVAL;
		return -1;
	}

	__make_noti_path(notipath, sizeof(notipath), noti);
	UTIL_DBG("del watch: [%s]", notipath);

	r = del_noti(fd, notipath, cb);
	util_warn_if(r == -1, "Error: del [%s]: %s", noti, strerror(errno));

	return r;
}

API int heynoti_publish(const char *noti)
{
	int fd;
	char *path;
	char notipath[FILENAME_MAX];
	struct stat sb;

	if (noti == NULL) {
		UTIL_DBG("Error: send noti: Invalid input");
		errno = EINVAL;
		return -1;
	}

	path = strstr(noti, "/");
	if (path) {
		snprintf(notipath, sizeof(notipath), "%s", noti);
	} else {
		__make_noti_path(notipath, sizeof(notipath), noti);
	}
	UTIL_DBG("send noti: [%s]", notipath);

	fd = open(notipath, O_TRUNC | O_WRONLY);

	util_retvm_if(fd == -1, -1, "Error: send noti: %s", strerror(errno));

	/*
	fstat(fd, &sb);
	if(sb.st_uid != getuid())
		fchown(fd, getuid(), getgid());
	if(sb.st_mode & 0033)
		fchmod(fd, 0644);
	*/

	close(fd);
	return 0;
}

API int heynoti_init()
{
	int r;
	int fd;

	struct noti_cont *nc;

	if(__check_kern_ver() < 0) {
		UTIL_ERR("inotify requires kernel version >= 2.6.13 ");
		errno = EPERM;
		return -1;
	}

	fd = inotify_init();
	util_retvm_if(fd == -1, -1, "inotify init: %s", strerror(errno));

	r = fcntl(fd, F_SETFD, FD_CLOEXEC);
	util_retvm_if(r < 0, -1, "fcntl error : %s", strerror(errno));
	r = fcntl(fd, F_SETFL, O_NONBLOCK);
	util_retvm_if(r < 0, -1, "fcntl error : %s", strerror(errno));

	r = __make_noti_root(noti_root);
	if (r == -1) {
		UTIL_ERR("make noti root: %s : %s", noti_root, strerror(errno));
		close(fd);
		return -1;
	}

	nc = calloc(1, sizeof(struct noti_cont));
	if (nc == NULL) {
		close(fd);
		return -1;
	}

	nc->fd = fd;
	/*sglib_ncont_add(&nc_h, nc); */
	g_nc = g_list_append(g_nc, (gpointer) nc);

	return fd;
}

API int heynoti_get_pnoti_name(pid_t pid, const char *name, char *buf,
			       int buf_size)
{
	int ret;

	if (!name)
		return -1;

	ret = snprintf(buf, buf_size, ".%d_%s", pid, name);

	if (ret >= buf_size)
		return -1;
	else
		return 0;
}

API int heynoti_get_snoti_name(const char *name, char *buf, int buf_size)
{
	int ret;

	if (!name)
		return -1;

	ret = snprintf(buf, buf_size, ".%s_%s", AU_PREFIX_SYSNOTI, name);

	if (ret >= buf_size)
		return -1;
	else
		return 0;
}

gboolean gio_cb(GIOChannel *src, GIOCondition cond, gpointer data)
{
	int fd;

	/* need condition check?? */

	fd = g_io_channel_unix_get_fd(src);
	__handle_event(fd);

	return TRUE;
}

API int heynoti_attach_handler(int fd)
{
	int ret;
	struct noti_cont *nc = NULL;
	GSource *src;
	GIOChannel *gio;

	nc = __get_noti_cont(fd);
	if (nc == NULL) {
		UTIL_ERR("Non-registered file descriptor : %d", fd);
		errno = EBADF;
		return -1;
	}

	switch (nc->ht) {
	case H_NONE:
		/* do nothing */
		break;
	case H_GLIB:
		UTIL_ERR("glib based handler now in progress");
		errno = EINPROGRESS;
		break;
	default:
		UTIL_ERR("Another handler already in progress: %d", nc->ht);
		errno = EALREADY;
		break;
	}
	util_retv_if(nc->ht != H_NONE, -1);

	gio = g_io_channel_unix_new(fd);
	util_retvm_if(gio == NULL, -1, "Error: create a new GIOChannel");

	g_io_channel_set_flags(gio, G_IO_FLAG_NONBLOCK, NULL);

	src = g_io_create_watch(gio, G_IO_IN);
	g_source_set_callback(src, (GSourceFunc) gio_cb, NULL, NULL);
	ret = g_source_attach(src, NULL);
	g_io_channel_unref(gio);
	g_source_unref(src);
	if (!ret)
		return -1;

	nc->handler = src;
	nc->ht = H_GLIB;

	return 0;
}

API int heynoti_detach_handler(int fd)
{
	struct noti_cont *nc = NULL;

	nc = __get_noti_cont(fd);
	if (nc == NULL) {
		UTIL_ERR("Non-registered file descriptor : %d", fd);
		errno = EBADF;
		return -1;
	}

	switch (nc->ht) {
	case H_GLIB:
		g_source_destroy(nc->handler);
		nc->handler = NULL;
		nc->ht = H_NONE;
		break;
	default:
		/* do nothing ?? */
		break;
	}

	return 0;
}

API void heynoti_close(int fd)
{
	struct noti_cont *r = NULL;

	r = __get_noti_cont(fd);
	util_warn_if(r == NULL, "Non-registered file descriptor");

	if (r) {
		if (r->ht == H_GLIB)
			heynoti_detach_handler(fd);

		__clear_nslot_list(r->g_ns);
		g_list_free(r->g_ns);
		close(r->fd);

		g_nc = g_list_remove(g_nc, (gconstpointer) r);
		free(r);
	}
}


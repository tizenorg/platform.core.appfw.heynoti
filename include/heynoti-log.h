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




#ifndef __SAMSUNG_LINUX_UTIL_PRIVATE_LOG_H__
#define __SAMSUNG_LINUX_UTIL_PRIVATE_LOG_H__

/*
 * SYSLOG_INFO(), SYSLOG_ERR(), SYSLOG_DBG() are syslog() wrappers.
 * UTIL_PRT_INFO(), UTIL_PRT_ERR(), UTIL_PRT_DBG() are fprintf() wrappers.
 *
 * If SLP_DEBUG is not defined, UTIL_SYSLOG_DBG() and UTIL_PRT_DBG() is ignored.
 *
 * IF SLP_SYSLOG_OUT or SLP_DAEMON is defined,
 *   UTIL_INFO(), UTIL_ERR(), UTIL_DBG() are SYSLOG_XXX()
 * Otherwise,
 *   They are PRT_XXX()
 *
 *
 * util_warn_if(exrp, fmt, ...)
 *   If expr is true, The fmt string is printed using UTIL_ERR().
 *
 * util_ret_if(), util_retv_if(), util_retm_if(), util_retvm_if()
 *   If expr is true, current function return.
 *   Postfix 'v' means that it has a return value and 'm' means that 
 *   it has output message.
 *
 */

#include <stdio.h>

/*#define UTIL_SYSLOG_OUT */
#define UTIL_DLOG_OUT
#define UTILCORE_DEBUG

#ifdef UTIL_SYSLOG_OUT
#include <syslog.h>

#define __UTIL_LOG(prio, fmt, arg...) \
	   do { syslog(prio, fmt, ##arg); } while (0)
#define __UTIL_LOGD(prio, fmt, arg...) \
	   do { syslog(prio, "[%s:%d] "fmt"\n", __FILE__, __LINE__, ##arg); \
		} while (0)
#define UTIL_INFO(fmt, arg...) __UTIL_LOG(LOG_INFO, fmt, ##arg)
#define UTIL_ERR(fmt, arg...) __UTIL_LOG(LOG_ERR, fmt, ##arg)
#define _UTIL_DBG(fmt, arg...) __UTIL_LOGD(LOG_DEBUG, fmt, ##arg)
#elif defined(UTIL_DLOG_OUT)
#define LOG_TAG "HEYNOTI"
#include <dlog.h>
#define UTIL_DLOG(prio, fmt, arg...) \
		do { } while (0)
#define UTIL_INFO(fmt, arg...) LOGD(fmt,##arg)
#define UTIL_ERR(fmt, arg...) LOGE(fmt,##arg)
#define _UTIL_DBG(fmt, arg...) LOGD(fmt,##arg)
#else
#include <syslog.h>

#define __UTIL_PRT(prio, fmt, arg...) \
	   do { fprintf((LOG_PRI(prio) == LOG_ERR ? stderr : stdout), \
			fmt"\n", ##arg); } while (0)
#define __UTIL_PRTD(prio, fmt, arg...) \
	do { fprintf((LOG_PRI(prio) == LOG_ERR ? stderr : stdout), \
			"[%s:%d] "fmt"\n", __FILE__, __LINE__, ##arg); \
	   } while (0)
#define UTIL_INFO(fmt, arg...) __UTIL_PRT(LOG_INFO, fmt, ##arg)
#define UTIL_ERR(fmt, arg...) __UTIL_PRT(LOG_ERR, fmt, ##arg)
#define _UTIL_DBG(fmt, arg...) __UTIL_PRTD(LOG_DEBUG, fmt, ##arg)
#endif

#ifdef UTILCORE_DEBUG
#define UTIL_DBG _UTIL_DBG

#define util_warn_if(expr, fmt, arg...) do { \
		if (expr) { \
			UTIL_DBG("(%s) -> "fmt, #expr, ##arg); \
		} \
	} while (0)
#define util_ret_if(expr) do { \
		if (expr) { \
			UTIL_DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
			return; \
		} \
	} while (0)
#define util_retv_if(expr, val) do { \
		if (expr) { \
			UTIL_DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
			return (val); \
		} \
	} while (0)
#define util_retm_if(expr, fmt, arg...) do { \
		if (expr) { \
			UTIL_ERR(fmt, ##arg); \
			UTIL_DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
			return; \
		} \
	} while (0)
#define util_retvm_if(expr, val, fmt, arg...) do { \
		if (expr) { \
			UTIL_ERR(fmt, ##arg); \
			UTIL_DBG("(%s) -> %s() return", #expr, __FUNCTION__); \
			return (val); \
		} \
	} while (0)

#else
#define UTIL_DBG(...)

#define util_warn_if(expr, fmt, arg...) do { \
		if (expr) { \
			UTIL_ERR(fmt, ##arg); \
		} \
	} while (0)
#define util_ret_if(expr) do { \
		if (expr) { \
			return; \
		} \
	} while (0)
#define util_retv_if(expr, val) do { \
		if (expr) { \
			return (val); \
		} \
	} while (0)
#define util_retm_if(expr, fmt, arg...) do { \
		if (expr) { \
			UTIL_ERR(fmt, ##arg); \
			return; \
		} \
	} while (0)
#define util_retvm_if(expr, val, fmt, arg...) do { \
		if (expr) { \
			UTIL_ERR(fmt, ##arg); \
			return (val); \
		} \
	} while (0)

#endif

#endif /* __SAMSUNG_LINUX_UTIL_PRIVATE_LOG_H__ */


/*
 * event.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_NETWORK_EVENT_H_
#define SOAR_COMPONENTS_NETWORK_EVENT_H_

#include "../../communal.h"

#ifdef __linux__

#include <sys/epoll.h>
typedef struct epoll_event event_t;

#else

#include <sys/event.h>
typedef struct kevent event_t;

#endif

namespace soar_components_network {

enum EventErr {
	OTHER_ERR = 1,
	INVALID,
	FDLIMIT,
	NOMEM,
	ALREAD_IN_WATCH,
	UNWRITABLE_MEM,
	INTERRUPT,
};
enum EventNotificationType {
	READABLE = 1,
	WRITABLE = 2,
	ONCE 	 = 4,
	PRIORITY = 8,
	ERROCCUR = 16,
};

class EventItem {
private:
	int fd_;
	void* data_;
	int attr_;
public:
	EventItem() :
			fd_(0), data_(NULL), attr_(0) {
	}
	EventItem(int file_desc, void* data_buf = NULL) :
			fd_(file_desc), data_(data_buf) {
	}
	EventItem(const EventItem& src) :
			fd_(src.fd_), data_(src.data_), attr_(src.attr_) {
	}
	inline void set_fd(int file_desc) {
		fd_ = file_desc;
	}
	inline void set_data(void* data) {
		data_ = data;
	}
	inline void set_attr(int attrs) {
		attr_ = attrs;
	}
	inline void monitor_read() {
		attr_ |= READABLE;
	}
	inline void monitor_write() {
		attr_ |= WRITABLE;
	}
	inline void monitor_once() {
		attr_ |= ONCE;
	}
	inline void monitor_priority() {
		attr_ |= PRIORITY;
	}
	inline void set_err() {
		attr_ |= ERROCCUR;
	}
	inline int is_erroccured() const {
		return (attr_ & ERROCCUR) ? 1: 0;
	}
	inline bool is_readable() const {
		return (attr_ & READABLE) ? 1 : 0;
	}
	inline bool is_writable() const {
		return (attr_ & WRITABLE) ? 1 : 0;
	}
	inline bool is_once() const {
		return (attr_ & ONCE) ? 1 : 0;
	}
	inline bool is_priority() const {
		return (attr_ & PRIORITY) ? 1 : 0;
	}
	inline int get_fd() const {
		return fd_;
	}
	inline void* get_data() const {
		return data_;
	}
};

class SoarEvent {
private:
	int core_fd_;
	int error_id_;
	static int err_map_[256];
	static int err_map_init_;
private:
	DISALLOW_COPY_AND_ASSIGN(SoarEvent);
	int SetEvent(const EventItem& item , int method);
	void init_err_map();
public:
	inline int get_last_error() {return error_id_;}
public:
	SoarEvent();
	int NewEvent(const EventItem& item);
	int ModifyEvent(const EventItem& item);
	int DeleteEvent(const EventItem& item);
	int EventLoop(EventItem* events_out , const int max_events , int timeout = -1);
};

}/*namespace soar_components_network*/


#endif /* SOAR_COMPONENTS_NETWORK_EVENT_H_ */

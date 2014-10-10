/*
 * epoll.cc
 *
 *  Created on: Apr 2, 2013
 *      Author: House_Lee
 */
#include <errno.h>
#include <cstring>

#include "../event.h"
using namespace soar_components_network;

int SoarEvent::err_map_init_ = 0;
int SoarEvent::err_map_[256];

void SoarEvent::init_err_map() {
	memset(err_map_ , 0 ,sizeof(err_map_));
	err_map_[EINVAL] = INVALID;
	err_map_[ENFILE] = FDLIMIT;
	err_map_[ENOMEM] = NOMEM;
	err_map_[EBADF] = INVALID;
	err_map_[EEXIST] = ALREAD_IN_WATCH;
	err_map_[ENOENT] = INVALID;
	err_map_[EFAULT] = UNWRITABLE_MEM;
	err_map_[EINTR] = INTERRUPT;
	err_map_init_ = 1;
}

SoarEvent::SoarEvent():error_id_(0) {
	if (!err_map_init_)
		init_err_map();
	core_fd_ = epoll_create(1024);
	if (core_fd_ < 0) {
		error_id_ = err_map_[errno];
		(!error_id_) && (error_id_ = OTHER_ERR);
	}
}

int SoarEvent::SetEvent(const EventItem& item , int method) {
	event_t ev;
	ev.data.fd = item.get_fd();
	ev.data.ptr = item.get_data();
	ev.events = EPOLLET;
	item.is_readable() && (ev.events |= EPOLLIN);
	item.is_writable() && (ev.events |= EPOLLOUT);
	item.is_once() && (ev.events |= EPOLLONESHOT);
	item.is_priority() && (ev.events |= EPOLLPRI);

	int rtn = epoll_ctl(core_fd_, method, item.get_fd(), &ev);
	if (rtn) {
		rtn = 1;
		error_id_ = err_map_[errno];
		(!error_id_) && (error_id_ = OTHER_ERR);
	}
	return rtn;
}

int SoarEvent::NewEvent(const EventItem& item) {
	return SetEvent(item , EPOLL_CTL_ADD);
}
int SoarEvent::ModifyEvent(const EventItem& item) {
	return SetEvent(item , EPOLL_CTL_MOD);
}
int SoarEvent::DeleteEvent(const EventItem& item) {
	return SetEvent(item , EPOLL_CTL_DEL);
}

int SoarEvent::EventLoop(EventItem* events_out , const int max_events , int timeout) {
	if (!events_out || max_events <= 0) {
		error_id_ = INVALID;
		(!error_id_) && (error_id_ = OTHER_ERR);
		return 0;
	}
//	event_t evs[max_events];
	event_t* evs = new event_t[max_events];
	int nevents = epoll_wait(core_fd_, evs, max_events, timeout);
	if (nevents < 0) {
		error_id_ = err_map_[errno];
		(!error_id_) && (error_id_ = OTHER_ERR);
		return 0;
	}
	for (int i = 0; i < nevents; ++i) {
		events_out[i].set_fd(evs[i].data.fd);
		events_out[i].set_data(evs[i].data.ptr);
		events_out[i].set_attr(0);
		if (evs[i].events & EPOLLIN)
			events_out[i].monitor_read();
		if (evs[i].events & EPOLLOUT)
			events_out[i].monitor_write();
		if (evs[i].events & EPOLLONESHOT)
			events_out[i].monitor_once();
		if (evs[i].events & EPOLLPRI)
			events_out[i].monitor_priority();
		if((evs[i].events & EPOLLERR) || (evs[i].events & EPOLLHUP))
			events_out[i].set_err();
	}
	delete[] evs;
	return nevents;
}

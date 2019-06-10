/**
 * schedule/signal.cpp
 * signals to wake up sleeping process
 */

#include "signal.h"
#include "../process/process_t.h"
#include "../logging/logging.h"
#include "schedule.h"
#include <unordered_map>

using std::mutex;
using std::lock_guard;
using std::unordered_map;

namespace signal_id {
	
	const int WAIT_EXIT = -1;
	const int KEYBOARD = -2;

	int signal_top = -128;
}

static unordered_map<int,signal_t *> signal_table;

signal_t::signal_t()
{ }

signal_t::~signal_t()
{ }

void signal_t::notify ( size_t data )
{
	lock_guard<mutex> lk ( mut );
	if ( !this->proc.empty () ) {
		process_t *proc = this->proc.front ();
		this->proc.pop ();
		proc->set_signal_data ( data );
		sched_set_runable ( proc );
	} else {
		que.push ( data );
	}
}

void signal_t::wait ( process_t *proc )
{
	lock_guard<mutex> lk ( mut );
	if ( !que.empty () ) {
		size_t current_data = que.front ();
		que.pop ();
		proc->set_signal_data ( current_data );
		sched_set_runable ( proc );
	} else {
		this->proc.push ( proc );
	}
}

void init_signal ()
{
	logging::debug << "Initializing process wait signals" << logging::log_endl;
	signal_table.insert ( std::make_pair ( signal_id::WAIT_EXIT, new signal_t () ) );
	signal_table.insert ( std::make_pair ( signal_id::KEYBOARD, new signal_t () ) );
}

void destroy_signal ()
{
	logging::debug << "Destroying process wait signals" << logging::log_endl;
	for ( auto &x : signal_table ) {
		delete x.second;
	}
}

int send_signal(int signal_id, size_t data)
{
	if (!signal_table.count(signal_id)) {
		logging::info << "signal " << signal_id << " not found" << logging::log_endl;
		return -1;
	}
	signal_table[signal_id]->notify(data);
	return 0;
}

int wait_signal(int signal_id, process_t *proc)
{
	if (!signal_table.count(signal_id)) {
		logging::info << "signal " << signal_id << " not found" << logging::log_endl;
		return -1;
	}
	signal_table[signal_id]->wait(proc);
	return 0;
}

int register_signal ()
{
	int signal = --signal_id::signal_top;
	signal_table.insert ( std::make_pair ( signal, new signal_t () ) );
	return signal;
}

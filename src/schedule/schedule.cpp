/**
 * schedule/schedule.cpp
 * schedule processes and cpus
 */

#include "schedule.h"
#include "../process/process_t.h"
#include "../core/cpus.h"
#include <list>
#include <mutex>
#include <cassert>

using std::mutex;
using std::lock_guard;
using std::list;
typedef process_t::state state;

struct state_list_t {
	list<process_t*> running;
	list<process_t*> sleeping;
	list<process_t*> uninit;
};

list<process_t*> uninit;
list<process_t*> zombie;

state_list_t *state_list;

/**
 * schedule init
 */
void init_schedule()
{
	state_list = new state_list_t[get_core_num()];
	
}

/**
 * schedule destroy
 */
void destroy_schedule()
{
	delete[] state_list;
}

static mutex sched_mutex;
// for smp

/**
 * init process @proc
 */
void sched_init_proc(process_t *proc)
{
	lock_guard<mutex> lk(sched_mutex);
	uninit.push_front(proc);
	proc->linker = uninit.begin();
}

/**
 * set core for @proc
 * TODO : only CPU0
 */
void sched_set_core(process_t *proc)
{
	lock_guard<mutex> lk(sched_mutex);
	int id = 0;
	uninit.erase(proc->linker);
	proc->set_core(cores + id);
	state_list[id].uninit.push_front(proc);
	proc->linker = state_list[id].uninit.begin();
}

/**
 * set @proc to runable
 */
void sched_set_runable(process_t *proc)
{
	lock_guard<mutex> lk(sched_mutex);
	assert(proc->get_state() != state::RUNABLE);
	int id = proc->get_core()->get_core_id();
	switch(proc->get_state()) {
	case state::UNINIT:
		state_list[id].uninit.erase(proc->linker);
		state_list[id].running.push_front(proc);
		proc->linker = state_list[id].running.begin();
		break;
	case state::SLEEPING:
		state_list[id].sleeping.erase(proc->linker);
		state_list[id].running.push_front(proc);
		proc->linker = state_list[id].running.begin();
		break;
	case state::ZOMBIE:
	case state::RUNABLE:
		break;
	}
}

void schedule(int core_id)
{
	
}

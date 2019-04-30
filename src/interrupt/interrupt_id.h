/**
 * interrupt/interrupt_id.h
 * Interrupt IDs
 */

#pragma once

#include <string>

enum class interrupt_id_t
{
	DIVIDE = 0,
	DEBUG,
	NMI,
	BREAKPOINT,
	OFLOW,
	BOUND,
	ILLEGAL_OPCODE,
	DEVICE,
	DOUBLE_FALUT,
	COPROC,
	TSS,
	SEGNP,
	STACK,
	GPFLT,
	PGFLT,
	RESERVED_15,
	FPERR,
	ALIGN,
	MCHK,
	SIMDERR,

	INTERNAL_TEST = 31,

	END_OF_INTERRUPT = -1,
	DISABLE_LAPIC = -2
};



bool interrupt_id_is_signal ( interrupt_id_t id );   // check whether the interrupt is a signal to LAPIC
bool interrupt_id_is_internal_exception ( interrupt_id_t id );   // check whether the interrupt is an internal CPU exception

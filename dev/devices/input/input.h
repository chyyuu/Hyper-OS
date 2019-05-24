/**
 * dev/devices/input/input.h
 * Device input
 */

#pragma once

#include "../device_t.h"
#include "../../../src/status/status.h"
#include <string>

class dev_input : public device_t
{
public:
	dev_input ( int __motherboard_id );

	virtual void init ();
	virtual void destroy ();

	virtual std::string to_string () const;

	virtual char read ();

private:
};
/**
 * program/alloc
 * alloc on data/bss/stack/heap
 */

#include "program.h"
#include <cstdlib>
#include <cassert>
#include <vector>
#include "../utils/panic.h"
#include "../utils/check.h"

using handle_type::type;
using std::vector;

template<typename T> 
type handle<T>::get_type() const
{
	return this_type;
}

template<typename T>
void handle<T>::set_type(type t)
{
	this_type = t;
}

/**
 * modify the data of the handle in compile
 * @val : source
 */
template<typename T>
void handle<T>::modify_in_compile(const T &val)
{
	if (prog == nullptr) {
		panic("modify null pointer");
	}
	if (this_type != type::STATIC) {
		panic("compile fail. only data in .data can be modified in compile");
	}
	prog->modify_data(addr, val);
}

/**
 * let this handle be an alias of val
 * @val : source
 * -------------
 * can be used in either compile or running time. 
 */
template<typename T>
void handle<T>::alias(const handle<T> &val)
{
	prog = val.get_prog();
	addr = val.get_addr();
	this_type = val.get_type();
	if (prog->is_compiling() && this_type == type::BSS) {
		prog->add_redirect_bss(&(addr));
	}
	// need to be redirect
}

/**
 * alloc continuous memory on static area
 * @len : length of memory to alloc ( bytes ) 
 * @return : the start position of the continuous memory
 */
size_t program::alloc_static_area(size_t len)
{
	if (!add_check(data_size, len)) {
		panic("compile fail. static size is too large");
	}
	data_size += len;
	if (data == nullptr) data = (char*)malloc(data_size);
	else data = (char*)realloc(data, data_size);
	return data_size - len;
}

/**
 * alloc an object of type T in .data
 * @return : the handle of the object
 * -------------------------------
 * you can use handle<T>::modify_in_compile(const T&);
 *  to modify the data in .data
 */
template<typename T>
handle<T> program::alloc_static()
{
	return handle<T>(alloc_static_area(sizeof(T)), this, type::STATIC);
}

/**
 * alloc n objects of type T in .data
 * @n : number of objects to alloc
 * @return : the handle of the first object
 * --------------------------------
 * you can use handle<T>::operator[](size_t id)
 *  to access the handle of id-th object
 * you can use handle<T>::modify_in_compile(const T&);
 *  to modify the data in .data
 */
template<typename T>
handle<T> program::alloc_static(size_t n)
{
	if (!mul_check(sizeof(T), n)) {
		panic("compile fail. static array is too large");
	}
	size_t len = n * sizeof(T);
	return handle<T>(alloc_static_area(len), this, type::STATIC);
}

/**
 * alloc continuous memory in .bss
 * @len : length to alloc
 * @return : the start position of the continuous memory
 */
size_t program::alloc_bss_area(size_t len)
{
	size_t ret = bss_size;
	if (!add_check(bss_size, len)) {
		panic("compile fail. bss array is too large");
	}
	bss_size += len;
	return bss_size - len;
}

/**
 * add into redirect table of .bss
 * @addr : position waiting for redirect
 */
void program::add_redirect_bss(size_t *addr)
{
	redr_table.push_back(addr);
}

/**
 * modify .data
 * @addr : modify data in data[addr]
 * @val  : data to storage
 */
template<typename T>
void program::modify_data(size_t addr, const T &val)
{
	if (!add_check(addr, sizeof(T)) || addr + sizeof(T) > data_size) {
		panic("compile fail. bad modify in compile");
	}
	T *pt = (T*)(data + addr);
	*pt = val;
}

/**
 * alloc an object in .bss
 * @return : the handle of the object
 */
template<typename T>
handle<T> program::alloc_bss()
{
	handle<T> ret(alloc_bss_area(sizeof(T)), this, type::BSS); 
    add_redirect_bss(ret.get_addr_addr());
	// insert into redirect table
	return ret;
}

/**
 * alloc n objects of type T in .bss
 * @n : the number of objects to alloc
 * @return : the handle of the first object
 */
template<typename T>
handle<T> program::alloc_bss(size_t n)
{
	if (!mul_check(sizeof(T), n)) {
		panic("compile fail. static array is too large");
	}
	size_t len = n * sizeof(T);
	handle<T> ret(alloc_bss_area(len), this, type::BSS);
	add_redirect_bss(ret.get_addr_addr());
	// insert into redirect table
	return ret;
}


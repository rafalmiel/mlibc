#pragma once

#include <stdint.h>
#include <mlibc/tcb.hpp>

namespace mlibc {

#if 0
static bool s_is_tcb_ready = false;

void set_tcb_ready() {
	s_is_tcb_ready = true;

}
#endif

inline Tcb *get_current_tcb() {
	//if (!s_is_tcb_ready) {
	//	return nullptr;
	//}

	uintptr_t ptr;
	asm ("movq %%fs:0, %0" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr);
}

} // namespace mlibc

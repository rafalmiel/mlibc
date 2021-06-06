#include <mlibc/thread.hpp>

#include <stdint.h>

namespace mlibc {

static bool s_is_tcb_ready = false;

void set_tcb_ready() {
	s_is_tcb_ready = true;

}

Tcb *get_current_tcb() {
	if (!s_is_tcb_ready) {
		return nullptr;
	}

	uintptr_t ptr;
	asm ("movq %%fs:0, %0" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr);
}

} // namespace mlibc

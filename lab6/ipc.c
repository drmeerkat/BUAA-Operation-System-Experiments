#include "lib.h"
#include "mmu.h"
#include "env.h"

extern struct Env *env;

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void
ipc_send(u_int whom, u_int val, u_int srcva, u_int perm)
{
	int r;
	writef("ipc send whom:%x val:%d srcva:%d perm:%d\n", whom, val, srcva, perm);
	while ((r = syscall_ipc_can_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		writef("ipc send whom 1:%x\n", whom);
		syscall_yield();
		//writef("QQ");
	}
	if (r == 0) {
		return;
	}

	user_panic("error in ipc_send: %d", r);
}

static u_int a, b, i = 0, asend_val, bsend_val;
void
new_ipc_send(u_int whom, u_int val, u_int srcva, u_int perm)
{

	int r;
	writef("ipc send whom:%x val:%d srcva:%d perm:%d\n", whom, val, srcva, perm);
	if (i == 0) {
		b = whom;
		i++;
		asend_val = val;
	}
	if (i == 1) {
		i++;
		a = whom;
		bsend_val = val;
		if (bsend_val != asend_val + 1) {
			user_panic("handshake failed!");
		}
	}
	while ((r = syscall_ipc_can_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		writef("ipc send whom 1:%x\n", whom);
		syscall_yield();
		//writef("QQ");
	}
	if (r == 0) {
		return;
	}

	user_panic("error in ipc_send: %d", r);
}


u_int
new_ipc_recv(u_int *whom, u_int dstva, u_int *perm)
{
	syscall_ipc_recv(dstva);

	if (*whom == a) {
		if (curenv->env_ipc_value != asend_val+1) {
			user_panic("handshake failed!");
		}
	}
	if (*whom == b) {
		if (curenv->env_ipc_value != asend_val) {
			user_panic("handshake failed!");
		}
	}
	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int
ipc_recv(u_int *whom, u_int dstva, u_int *perm)
{
	//printf("ipc_recv:come 0\n");
	syscall_ipc_recv(dstva);

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

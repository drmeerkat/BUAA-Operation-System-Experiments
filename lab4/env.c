/*  If you find any bug, please contact with me.*/

#include "mmu.h"
#include "error.h"
#include "env.h"
#include "kerelf.h"
#include "sched.h"
#include "pmap.h"
#include "printf.h"

struct Env *envs = NULL;        // All environments
struct Env *curenv = NULL;          // the current env

static struct Env_list env_free_list;   // Free list

extern Pde *boot_pgdir;
extern char *KERNEL_SP;


/* Overview:
*  This function is for making an unique ID for every env.
*
* Pre-Condition:
*  Env e is exist.
*
* Post-Condition:
*  return e's envid on success.
*/
u_int mkenvid(struct Env *e)
{
	static u_long next_env_id = 0;

	/*Hint: lower bits of envid hold e's position in the envs array. */
	u_int idx = e - envs;

	/*Hint:  high bits of envid hold an increasing number. */
	return (++next_env_id << (1 + LOG2NENV)) | idx;
}

/* Overview:
*  Converts an envid to an env pointer.
*  If envid is 0 , set *penv = curenv;otherwise set *penv = envs[ENVX(envid)];
*
* Pre-Condition:
*  Env penv is exist,checkperm is 0 or 1.
*
* Post-Condition:
*  return 0 on success,and sets *penv to the environment.
*  return -E_BAD_ENV on error,and sets *penv to NULL.
*/
int envid2env(u_int envid, struct Env **penv, int checkperm)
{
	struct Env *e;

	/* Hint:
	*  If envid is zero, return the current environment.*/
	if (envid == 0) {
		*penv = curenv;
		return 0;
	}
	e = &envs[ENVX(envid)];

	if (e->env_status == ENV_FREE || e->env_id != envid) {
		if (e->env_status == ENV_FREE) {
			printf("FREE\n");
		}
		if (e->env_id != envid) {
			printf("id fault:e->env_id = %d envid = %d\n", e->env_id, envid);
		}
		*penv = 0;
		return -E_BAD_ENV;
	}
	/* Hint:
	*  Check that the calling environment has legitimate permissions
	*  to manipulate the specified environment.
	*  If checkperm is set, the specified environment
	*  must be either the current environment.
	*  or an immediate child of the current environment. */

	if (checkperm && e != curenv && e->env_parent_id != curenv->env_id) {
		*penv = 0;
		return -E_BAD_ENV;
	}
	*penv = e;
	return 0;
}

void
env_init(void)
{
	int i;
	/*Step 1: Initial env_free_list. */
	LIST_INIT(&env_free_list);

	/*Step 2: Travel the elements in 'envs', init every element(mainly initial its status, mark it as free)
	* and inserts them into the env_free_list as reverse order. */
	for (i = NENV - 1; i >= 0; i--) {
		LIST_INSERT_HEAD(&env_free_list, &envs[i], env_link);
		envs[i].env_status = ENV_FREE;
	}

}
static int
env_setup_vm(struct Env *e)
{

	int i, r;
	struct Page *p = NULL;
	Pde *pgdir;

	/*Step 1: Allocate a page for the page directory and add its reference.
	*pgdir is the page directory of Env e. */
	if ((r = page_alloc(&p)) < 0) {
		panic("env_setup_vm - page_alloc error\n");
		return r;
	}
	p->pp_ref++;
	pgdir = (Pde *)page2kva(p);

	/*Step 2: Zero pgdir's field before UTOP. */
	for (i = 0; i < PDX(UTOP); i++) {
		pgdir[i] = 0;
	}
	for (i = PDX(UTOP); i <= PDX(~0); i++) {
		pgdir[i] = boot_pgdir[i];
	}
	e->env_pgdir = pgdir;
	e->env_cr3 = PADDR(pgdir);

	/*Step 4: VPT and UVPT map the env's own page table, with
	*different permissions. */
	e->env_pgdir[PDX(VPT)] = e->env_cr3;
	e->env_pgdir[PDX(UVPT)] = e->env_cr3 | PTE_V | PTE_R;
	return 0;

}

int
env_alloc(struct Env **new, u_int parent_id)
{
	int r;
	struct Env *e;

	/*Step 1: Get a new Env from env_free_list*/
	if (LIST_EMPTY(&env_free_list)) {
		return -E_NO_FREE_ENV;
	}
	e = LIST_FIRST(&env_free_list);
	env_setup_vm(e);


	/*Step 3: Initialize every field of new Env with appropriate values*/
	e->env_status = ENV_NOT_RUNNABLE;
	e->env_parent_id = parent_id;
	e->env_id = mkenvid(e);
	//    printf("env creat success %d\n",e->env_id);
	/*Step 4: focus on initializing env_tf structure, located at this new Env.
	* especially the sp registeir,CPU status. */
	e->env_tf.cp0_status = 0x10001004;
	e->env_tf.regs[29] = USTACKTOP;

	/*Step 5: Remove the new Env from Env free list*/
	LIST_REMOVE(e, env_link);
	*new = e;
	return 0;
}


static int load_icode_mapper(u_long va, u_int32_t sgsize,
	u_char *bin, u_int32_t bin_size, void *user_data)
{
	struct Env *env = (struct Env *)user_data;
	struct Page *p = NULL;
	u_long i;
	int r;
	u_long offset = va - ROUNDDOWN(va, BY2PG);
	for (i = 0; i < bin_size; i += BY2PG) {
		if (page_alloc(&p) < 0 || page_insert(env->env_pgdir, p, va, PTE_V | PTE_R) < 0)
			return -1;
		bzero((void *)page2kva(p), BY2PG);
		bcopy((void *)bin, (void *)(page2kva(p) + offset), BY2PG);
		bin += BY2PG;
		va += BY2PG;
		/* Hint: You should alloc a page and increase the reference count of it. */
	}
	while (i < sgsize) {
		if (page_alloc(&p) < 0 || page_insert(env->env_pgdir, p, va, PTE_V | PTE_R) < 0)
			return -2;
		bzero((void *)page2kva(p), BY2PG);
		i += BY2PG;
		va += BY2PG;
	}

	return 0;
}


static void
load_icode(struct Env *e, u_char *binary, u_int size)
{
	/* Hint:
	*  You must figure out which permissions you'll need
	*  for the different mappings you create.
	*  Remember that the binary image is an a.out format image,
	*  which contains both text and data.
	*/
	struct Page *p = NULL;
	u_long entry_point;
	u_long r;
	u_long perm;
	page_alloc(&p);


	/*Step 2: Use appropriate perm to set initial stack for new Env. */
	/*Hint: The user-stack should be writable? */
	page_insert(e->env_pgdir, p, (USTACKTOP - BY2PG), PTE_V | PTE_R);

	/*Step 3:load the binary by using elf loader. */
	load_elf(binary, size, &entry_point, e, load_icode_mapper);

	/***Your Question Here***/
	/*Step 4:Set CPU's PC register as appropriate value. */
	e->env_tf.pc = entry_point;
	e->env_status = ENV_RUNNABLE;
}

void
env_create(u_char *binary, int size)
{
	struct Env *e;
	/*Step 1: Use env_alloc to alloc a new env. */
	env_alloc(&e, 0);

	/*Step 2: Use load_icode() to load the named elf binary. */
	load_icode(e, binary, size);

}
void
env_run(struct Env *e)
{
	/*Step 1: save register state of curenv. */
	/* Hint: if there is a environment running,you should do
	*  context switch.You can imitate env_destroy() 's behaviors.*/
	struct Trapframe *old;
	old = (struct Trapframe *)(TIMESTACK - sizeof(struct Trapframe));
	if (curenv) {
		bcopy(old, &(curenv->env_tf), sizeof(struct Trapframe));
		curenv->env_tf.pc = curenv->env_tf.cp0_epc;
		//printf("curenv != null: envid:%x envpc:%x",curenv->env_id, curenv->env_tf.pc);
	}
	curenv = e;

	/*Step 3: Use lcontext() to switch to its address space. */
	lcontext(KADDR(curenv->env_cr3));

	/*Step 4: Use env_pop_tf() to restore the environment's
	* environment   registers and drop into user mode in the
	* the   environment.
	*/
	/* Hint: You should use GET_ENV_ASID there.Think why? */
	//printf("now envid:%x is about to run from %x\n",curenv->env_id, curenv->env_tf.pc);
	env_pop_tf(&(curenv->env_tf), GET_ENV_ASID(curenv->env_id));

}

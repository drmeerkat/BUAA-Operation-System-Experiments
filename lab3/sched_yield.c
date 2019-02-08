#include "env.h"
#include "pmap.h"
#include "printf.h"

/* Overview:
*  Implement simple round-robin scheduling.
*  Search through 'envs' for a runnable environment ,
*  in circular fashion statrting after the previously running env,
*  and switch to the first such environment found.
*
* Hints:
*  The variable which is for counting should be defined as 'static'.
*/
static int i = 0;
void sched_yield_old(void)
{
	u_int a = 0;
	u_int b = 0;
	while (1) {
		i = (i + 1) % NENV;
		if (envs[i].env_status == ENV_RUNNABLE) {
			if (envs[i].env_id == a) {
				env_run(&envs[i]);
				break;
			}
			else {
				env_run(&envs[i]);
				break;
			}
		}
		
	}

}

static int i = 0;
static int j = 0;
void sched_yield(void)
{
	u_int a = 0;
	u_int b = 0;
	while (1) {
		i++;
		i = i % 5;
		if (i == 0) {
			j = 0;
		}
		if (i == 1) {
			j = 1;
		}
		if (i == 2) {
			j = 1;
		}
		if (i == 3) {
			j = 1;
		}
		if (i == 4) {
			j = 1;
		}
		if (envs[j].env_status == ENV_RUNNABLE)
		{
			env_run(&envs[j]);

			break;
		}
	}
}

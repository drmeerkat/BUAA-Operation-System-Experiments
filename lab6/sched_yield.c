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
void sched_yield(void)
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

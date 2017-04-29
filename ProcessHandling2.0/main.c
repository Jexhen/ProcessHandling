#include <stdlib.h>
#include "queue.h"
#include "process_handling.h"

int main(int argc, char *argv[])
{
	Queue *wq = new_queue();
	Queue *fq = new_queue();
	
	process_scheduling(wq, fq);
	destroy(wq, fq);

	return 0;
}

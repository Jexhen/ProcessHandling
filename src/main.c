#include <stdlib.h>
#include "queue.h"
#include "process_handling.h"

int main(int argc, char *argv[])
{
	Queue *wq = new_queue();
	Queue *fq = new_queue();
	
	process_scheduling(wq, fq);
	destroy(fq);
	destroy(wq);

	return 0;
}

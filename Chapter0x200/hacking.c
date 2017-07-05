#include"hacking.h"

void fatal(char* message)
{
	char error_message[100];

	strcpy(error_message,"[!!] Fatal Error ");
	strncat(error_message,message,83);
	perror(error_message);

	exit(-1);
}

void *ec_malloc(unsigned int size)
{
	void *ptr;
	ptr=malloc(size);
	if(ptr == NULL)
	{
//		fprintf(stderr,"Error: could not allocate heap memory!");
//		exit(-1);
		fatal("in ec_malloc() on memory allocation");
	}

	return ptr;
}

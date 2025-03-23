//read memory command
int command_readmem(int number_of_arguments, char **arguments)
{
	unsigned int address = strtol(arguments[1], NULL, 16);
	unsigned char *ptr = (unsigned char *)(address ) ;

	cprintf("value at address %x = %c\n", ptr, *ptr);

	return 0;
}

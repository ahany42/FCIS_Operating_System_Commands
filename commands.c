//read from memory command
/*
Name: read_mem <virtual address in hex>
Description: read 1 byte from the memory at the given < virtual address> and print the corresponding
physical address
*/
int command_readmem(int number_of_arguments, char **arguments)
{
	unsigned int address = strtol(arguments[1], NULL, 16);
	unsigned char *ptr = (unsigned char *)(address ) ;

	cprintf("value at address %x = %c\n", ptr, *ptr);

	return 0;
}
/*
Name: write_mem <character> <physical address in hex>
Description: writes the given <character> in the memory at the given <physical address>
*/
//write to memory command 
int command_writemem(int number_of_arguments, char **arguments)
{
	unsigned int address = strtol(arguments[1], NULL, 16);
	unsigned char *ptr = (unsigned char *)(address) ;

	*ptr = arguments[2][0];

	return 0;
}
//writeable bit (read / read - write)
/*
Name: sp <virtual address> <r/w>
Arguments: 
<virtual address>: virtual memory address to set its page permissions.
<r/w>: 'r' for read-only permission, 'w' for read/write permission.
Description: This command should set the desired permission to a given virtual address page
*/
int command_set_permission(int number_of_arguments, char **arguments)
{   int va = strtol(arguments[1],NULL,16);
    uint32 *ptr_page_table = NULL;
    get_page_table(ptr_page_directory,(void*)va,1,&ptr_page_table);
    char op = arguments[2][0];
    if(op == 'w'){
    ptr_page_table[PTX(va)] = ptr_page_table[PTX(va)] | PERM_WRITEABLE;
    }
    else if(op == 'r'){
    ptr_page_table[PTX(va)] = ptr_page_table[PTX(va)] & ~PERM_WRITEABLE;
    }

	return 0 ;
}
//share range command 
/*Name: sr <va1> <va2> <size in KB>
Arguments: 
< va1>: the start virtual address of the range to be shared
<va2>: the start virtual address that will see <virtual address 1> physical frame
<size in KB>: size of the sharing range (in KB)
Description: This command shares the physical frames of the range [<va1>, <va1> + size) with the range [<va2>, <va2> + size). You will need to use "get_page_table" with "create" flag equal 1 in order to create new page table(s) for the range [<va2>, <va2> + size) if there're no tables exist.
	*/
int command_share_range(int number_of_arguments, char **arguments)
{
	int val1 = strtol(arguments[1],NULL,16);
	int val2 = strtol(arguments[2],NULL,16);
	int limit = strtol(arguments[3],NULL,16);
	uint32* page_table_1 = NULL;
	uint32* page_table_2 = NULL;
	for(int i =0 ; i<limit;i+=PAGE_SIZE){
    get_page_table(ptr_page_directory,(void*)val1+i,0,&page_table_1);
    get_page_table(ptr_page_directory,(void*)val2+i,1,&page_table_2);
    page_table_2[PTX(val2+i)] = page_table_1[PTX(val1+i)];
	}
	return 0;
}
//Write Block Command
/* Name: write_block <virtual address> <N>
Arguments:
<virtual address>: start virtual memory address to read from (hexadecimal)
<N>: number of bytes to read from the given address (decimal)
Description: writes the <N> bytes starting from < virtual address> and display them*/
int write_block(int number_of_arguments,char **arguments){
   int n = strtol(arguments[2],NULL,16);
   char arr[n];
   int physical_address = strtol(arguments[1],NULL,16);
   int virtual_address = physical_address + KERNEL_BASE ;
   char *ptr = (char *)virtual_address;
   for (int i = 0 ;i<n;i++){
	 cprintf("Element %d ",i);
	 readline(":",&arr[i]);
	*ptr = arr[i];
	 ptr++;
   }
 return 0;
}
int command_show_mapping(int number_of_arguments, char **arguments)
{
	//TODO: LAB4 Hands-on: fill this function. corresponding command name is "sm"
	//Comment the following line
	//panic("Function is not implemented yet!");
	uint32 va = strtol(arguments[1],NULL,16);
	uint32 page_directory_index = 	PDX(va);
	cprintf("Directory table index : %d\n",page_directory_index) ;
	uint32 page_table_index = 	PTX(va);
	cprintf("Page table index : %d\n",page_table_index);
	uint32 * ptr_page_table = NULL;
	get_page_table(ptr_page_directory,(void*)va,1,&ptr_page_table);
	uint32 page_table_entry = ptr_page_table[PTX(va)];
	int used = page_table_entry & PERM_WRITEABLE;
	if(used){

    cprintf("Not Used");
	}
	else{

    cprintf("Used");
	}

	return 0 ;
}
//[1] Number of references on the given physical address
int command_nr(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "nr"
	//Comment the following line
	//panic("Function is not implemented yet!");
	uint32 va = strtol(arguments[1],NULL,16);
	uint32 * ptr_page_table = NULL;
	get_page_table(ptr_page_directory,(void*)va,1,&ptr_page_table);
	struct Frame_Info* frame = get_frame_info(ptr_page_directory, (void *)va, &ptr_page_table);
	cprintf("Number Of References %d", frame->references);

	return 0;
}
//[2] Count Free Pages in Range
int command_cfp(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Hands-on: fill this function. corresponding command name is "cfp"
	//Comment the following line
	//panic("Function is not implemented yet!");
	int va1 = strtol(arguments[1],NULL,16);
	int va2 = strtol(arguments[2],NULL,16);
	int freeCount = 0;
	uint32 * ptr_page_table ;
	for(int i = ROUNDOWN(i,PAGE_SIZE);i<=va2;i+=PAGE_SIZE){
		struct frame_Info * frame = get_frame_info(ptr_page_directory,(void*)i,&ptr_page_table);
		if(frame == NULL){
			freeCount ++;
		}
	}
	cprintf("Free Pages in Range %d\n",freeCount)

	return 0;
}
//Allocate page
int command_ap(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "ap"
	//Comment the following line
	//panic("Function is not implemented yet!");
	struct Frame_Info *ptr_frame_info;
	int va = strtol(arguments[1],NULL,16);
	int ref = allocate_frame(&ptr_frame_info);
    if(ref == E_NO_MEM){
    	cprintf("No Memory \n");
    }
    else{
    int map = map_frame(ptr_page_directory, ptr_frame_info, (void *) va,PERM_WRITEABLE);
    if(map == E_NO_MEM){
    	cprintf("No Memory \n");
    }
    else{
    	cprintf("Allocated Successfully");
    }
    }
	return 0 ;
}
//[3] Free Page: Un-map a single page at the given virtual address in the user space
int command_fp(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "fp"
	//Comment the following line
	//panic("Function is not implemented yet!");
	int va = strtol(arguments[1],NULL,16);
	unmap_frame(uint32 *ptr_page_directory, (void *)va);

	return 0;
}

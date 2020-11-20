/*
 * =====================================================================================
* THIS CODE PASSES TEST 1, 2 and 4 of A3
 * =====================================================================================
 */

/* Includes */
#include "sma.h" // Please add any libraries you plan to use inside this file

/* Definitions*/
#define MAX_TOP_FREE (128 * 1024) // Max top free block size = 128 Kbytes
//	TODO: Change the Header size if required
#define FREE_BLOCK_HEADER_SIZE 2 * sizeof(char *) + sizeof(int) // Size of the Header in a free memory block
//	TODO: Add constants here


typedef enum //	Policy type definition
{
	WORST,
	NEXT
} Policy;

char *sma_malloc_error;
void *freeListHead = NULL;			  //	The pointer to the HEAD of the doubly linked free memory list
void *freeListTail = NULL;			  //	The pointer to the TAIL of the doubly linked free memory list
unsigned long totalAllocatedSize = 0; //	Total Allocated memory in Bytes
unsigned long totalFreeSize = 0;	  //	Total Free memory in Bytes in the free memory list
Policy currentPolicy = NEXT;		  //	Current Policy
//	TODO: Add any global variables here
void *base = NULL;
void *previousBlock = NULL;

/*
 * =====================================================================================
 *	Public Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: sma_malloc
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates a memory block of input size from the heap, and returns a 
 * 					pointer pointing to it. Returns NULL if failed and sets a global error.
 */
void *sma_malloc(int size)
{
	char str[60];
	void *pMemory = NULL;

	// Checks if the free list is empty

	//CHANGING THIS TO CHECK IF BASE INSRTEAD OF FREELISTHEAD
	if (!base)
	//WAS (freeListHead == NULL)
	{
		// Allocate memory by increasing the Program Break
		pMemory = allocate_pBrk(size);
	}
	// If free list is not empty
	else
	{
		// Allocate memory from the free memory list
		pMemory = allocate_freeList(size);

		// If a valid memory could NOT be allocated from the free memory list

		
		if (pMemory == (void *)-2)
		{
			// Allocate memory by increasing the Program Break

			pMemory = allocate_pBrk(size);
		}
	}

	// Validates memory allocation
	if (pMemory < 0 || pMemory == NULL)
	{
		sma_malloc_error = "Error: Memory allocation failed!";
		return NULL;
	}

	// Updates SMA Info
	totalAllocatedSize += size;

	return pMemory;
}

/*
 *	Funcation Name: sma_free
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Deallocates the memory block pointed by the input pointer
 */
void sma_free(void *ptr)
{
	//	Checks if the ptr is NULL
	if (ptr == NULL)
	{
		puts("Error: Attempting to free NULL!");
	}
	//	Checks if the ptr is beyond Program Break
	else if (ptr > sbrk(0))
	{
		puts("Error: Attempting to free unallocated space!");
	}
	else
	{
		//	Adds the block to the free memory list
		add_block_freeList(ptr);
	}
}

/*
 *	Funcation Name: sma_mallopt
 *	Input type:		int
 * 	Output type:	void
 * 	Description:	Specifies the memory allocation policy
 */
void sma_mallopt(int policy)
{
	// Assigns the appropriate Policy
	if (policy == 1)
	{
		currentPolicy = WORST;
	}
	else if (policy == 2)
	{
		currentPolicy = NEXT;
	}
}

/*
 *	Funcation Name: sma_mallinfo
 *	Input type:		void
 * 	Output type:	void
 * 	Description:	Prints statistics about current memory allocation by SMA.
 */
void sma_mallinfo()
{
	//	Finds the largest Contiguous Free Space (should be the largest free block)
	int largestFreeBlock = get_largest_freeBlock();
	char str[60];

	//	Prints the SMA Stats
	sprintf(str, "Total number of bytes allocated: %lu", totalAllocatedSize);
	puts(str);
	sprintf(str, "Total free space: %lu", totalFreeSize);
	puts(str);
	sprintf(str, "Size of largest contigious free space (in bytes): %d", largestFreeBlock);
	puts(str);
}

/*
 *	Funcation Name: sma_realloc
 *	Input type:		void*, int
 * 	Output type:	void*
 * 	Description:	Reallocates memory pointed to by the input pointer by resizing the
 * 					memory block according to the input size.
 */
void *sma_realloc(void *ptr, int size)
{
	// TODO: 	Should be similar to sma_malloc, except you need to check if the pointer address
	//			had been previously allocated.
	// Hint:	Check if you need to expand or contract the memory. If new size is smaller, then
	//			chop off the current allocated memory and add to the free list. If new size is bigger
	//			then check if there is sufficient adjacent free space to expand, otherwise find a new block
	//			like sma_malloc.
	//			Should not accept a NULL pointer, and the size should be greater than 0.


	//

	char str[60];
	int alloced = 0;

	struct meta_block *meta = get_block_pointer(ptr);
	struct meta_block *current = base;
	void *new = NULL;
	void *temp;
	
	while (current && !alloced){
		if(current == meta){
			sprintf(str, "Size of block: %ld", meta->size);
			puts(str);
			if(meta->size < size){
				sma_free(ptr);
				new = sma_malloc(size);
				//memcpy(ptr, new, size);
				
			}
			else{
				meta->size = size;
				new = ptr;
			}
			alloced=1;
		}
		current = current->next;
	}



	return new;

	//



}

/*
 * =====================================================================================
 *	Private Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: allocate_pBrk
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory by increasing the Program Break
 */
void *allocate_pBrk(int size)
{
	void *newBlock = NULL;
	int excessSize = 0;

	//	TODO: 	Allocate memory by incrementing the Program Break by calling sbrk() or brk()
	//	Hint:	Getting an exact "size" of memory might not be the best idea. Why?
	//			Also, if you are getting a larger memory, you need to put the excess in the free list



	char str[60];

	//MINE

	struct meta_block *meta;

	if(!base){
		meta = sbrk(META_SIZE + size);
		meta->size = size;
		meta->free = 0;
		base = meta;
	}
	else{
		struct meta_block *current = base;
		while(current && current->next){
			current=current->next;
		}
		meta = sbrk(META_SIZE + size);
		meta->size = size;
		meta->free = 0;
		current->next = meta;
	}
	newBlock = meta+1;
    
	//MINE

	
	//COMMENTING OUT THIS FUNCTION BECAUSE IM CONFUSED!!!!!! NEED TO IMPLEMENT AFTER
	//	Allocates the Memory Block
	//allocate_block(newBlock, size, excessSize, 0);

	return newBlock;
}

/*
 *	Funcation Name: allocate_freeList
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory from the free memory list
 */
void *allocate_freeList(int size)
{
	void *pMemory = NULL;


	if (currentPolicy == WORST)
	{
		// Allocates memory using Worst Fit Policy
		pMemory = allocate_worst_fit(size);
	}
	else if (currentPolicy == NEXT)
	{
		// Allocates memory using Next Fit Policy
		pMemory = allocate_next_fit(size);
	}
	else
	{
		pMemory = NULL;
	}

	return pMemory;
}

/*
 *	Funcation Name: allocate_worst_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Worst Fit from the free memory list
 */
void *allocate_worst_fit(int size)
{
	char str[50];

	void *worstBlock = NULL;
	int excessSize;
	int blockFound = 0;

	//	TODO: 	Allocate memory by using Worst Fit Policy
	//	Hint:	Start off with the freeListHead and iterate through the entire list to get the largest block


	//
	struct meta_block *current = base;
	struct meta_block *badBlock;

	while(current){
		if(current->free && current->size >=size){
			if(!badBlock){
				badBlock = current;
			}
			else if(badBlock->size<current->size){
				badBlock = current;
			}
		}
		current = current->next;

	}

	if(!badBlock){
		return (void *)-2;
	}
	// sprintf(str, "Hello\n\n");
	// puts(str);
	excessSize = badBlock->size - size;
	badBlock->size=size;
	badBlock->free=0;
	worstBlock = badBlock+1;



	blockFound = 1;

	//

	//	Checks if appropriate block is found.
	if (blockFound)
	{
		//	Allocates the Memory Block
		allocate_block(worstBlock, size, excessSize, 1);
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		//worstBlock = (void *)-2;
		//CHANGE THIS AFTER
	}

	return worstBlock;
}

/*
 *	Funcation Name: allocate_next_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Next Fit from the free memory list
 */
void *allocate_next_fit(int size)
{
	char str[50];
	void *nextBlock = NULL;
	int excessSize;
	int blockFound = 0;

	//	TODO: 	Allocate memory by using Next Fit Policy
	//	Hint:	Start off with the freeListHead, and keep track of the current position in the free memory list.
	//			The next time you allocate, it should start from the current position.

	//
	struct meta_block *current;
    struct meta_block *start;
	struct meta_block *last;
	
	if(!previousBlock){
		current = base;
	}
	else{		
		current = previousBlock;
	}
	
	start=current;
	int complete = 0;

   	while((!complete) && (current && !(current->free && current->size >=size))){

		last=current;
        current=current->next;
		if(current == NULL){
			current=base;
		}  
		if(current == start) {
			current = NULL;
			complete = 1;
		}   
    }


	previousBlock = last;
	if(!current){
		return (void *)-2;
	}
	current->size=size;
	current->free=0;
	nextBlock = current+1;

	//

	//	Checks if appropriate found is found.

	
	if (blockFound)
	{
		//	Allocates the Memory Block
		
		allocate_block(nextBlock, size, excessSize, 1);
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		//nextBlock = (void *)-2;
		//^^^^^^^COMMENTED THIS OUT CAUSE IT WAS BREAKING MY PROGRAM!!!
	}

	return nextBlock;
}

/*
 *	Funcation Name: allocate_block
 *	Input type:		void*, int, int, int
 * 	Output type:	void
 * 	Description:	Performs routine operations for allocating a memory block
 */
void allocate_block(void *newBlock, int size, int excessSize, int fromFreeList)
{
	void *excessFreeBlock; //	pointer for any excess free block
	int addFreeBlock = 1;

	// 	Checks if excess free size is big enough to be added to the free memory list
	//	Helps to reduce external fragmentation

	//	TODO: Adjust the condition based on your Head and Tail size (depends on your TAG system)
	//	Hint: Might want to have a minimum size greater than the Head/Tail sizes
	addFreeBlock = excessSize > FREE_BLOCK_HEADER_SIZE;

	//	If excess free size is big enough
	if (addFreeBlock)
	{
		//	TODO: Create a free block using the excess memory size, then assign it to the Excess Free Block

		//
		
		struct meta_block *new;
		struct meta_block *old;
		struct meta_block *temp;


		excessFreeBlock = (char *)newBlock + size;

		new = (struct meta_block*)excessFreeBlock;

		old = get_block_pointer(newBlock);

		temp = old->next;
		old->next=new;
		new->next=temp;
		new->size=excessSize;
		new->free=1;
		





		//


		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			//	Removes new block and adds the excess free block to the free list
			//replace_block_freeList(newBlock, excessFreeBlock);
			//CHANGE THIS
		}
		else
		{
			//	Adds excess free block to the free list
			add_block_freeList(excessFreeBlock);
		}
	}
	//	Otherwise add the excess memory to the new block
	else
	{
		//	TODO: Add excessSize to size and assign it to the new Block

		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			//	Removes the new block from the free list
			remove_block_freeList(newBlock);
		}
	}
}

/*
 *	Funcation Name: replace_block_freeList
 *	Input type:		void*, void*
 * 	Output type:	void
 * 	Description:	Replaces old block with the new block in the free list
 */
void replace_block_freeList(void *oldBlock, void *newBlock)
{
	//	TODO: Replace the old block with the new block

	//	Updates SMA info
	totalAllocatedSize += (get_blockSize(oldBlock) - get_blockSize(newBlock));
	totalFreeSize += (get_blockSize(newBlock) - get_blockSize(oldBlock));
}

/*
 *	Funcation Name: add_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Adds a memory block to the the free memory list
 */
void add_block_freeList(void *block)
{
	//	TODO: 	Add the block to the free list
	//	Hint: 	You could add the free block at the end of the list, but need to check if there
	//			exits a list. You need to add the TAG to the list.
	//			Also, you would need to check if merging with the "adjacent" blocks is possible or not.
	//			Merging would be tideous. Check adjacent blocks, then also check if the merged
	//			block is at the top and is bigger than the largest free block allowed (128kB).


	//

	//First, set the free value of the block to 1 to indicate that its freee
	struct meta_block *meta = get_block_pointer(block);
	meta->free=1;

	// Second, check the list and do some merging

	struct meta_block *last;
	struct meta_block *current;

	current=base;

	while(current){
		if(current->free){
			if((current->next) && current->next->free){
				current->size = (current->size) + (current->next->size) + META_SIZE;
				current->next = current->next->next;
			}
			else{
				last=current;
				current=current->next;
			}

		}
		else{
			last=current;
			current = current->next;
		}
	}


	//

	//	Updates SMA info
	totalAllocatedSize -= get_blockSize(block);
	totalFreeSize += get_blockSize(block);
}

/*
 *	Funcation Name: remove_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Removes a memory block from the the free memory list
 */
void remove_block_freeList(void *block)
{
	//	TODO: 	Remove the block from the free list
	//	Hint: 	You need to update the pointers in the free blocks before and after this block.
	//			You also need to remove any TAG in the free block.

	//	Updates SMA info
	totalAllocatedSize += get_blockSize(block);
	totalFreeSize -= get_blockSize(block);
}

/*
 *	Funcation Name: get_blockSize
 *	Input type:		void*
 * 	Output type:	int
 * 	Description:	Extracts the Block Size
 */
int get_blockSize(void *ptr)
{
	int *pSize;

	//	Points to the address where the Length of the block is stored
	pSize = (int *)ptr;
	pSize--;

	//	Returns the deferenced size
	return *(int *)pSize;
}

/*
 *	Funcation Name: get_largest_freeBlock
 *	Input type:		void
 * 	Output type:	int
 * 	Description:	Extracts the largest Block Size
 */
int get_largest_freeBlock()
{
	int largestBlockSize = 0;

	//	TODO: Iterate through the Free Block List to find the largest free block and return its size

	//

	struct meta_block *current = base;


	while(current){
		if(current->free && current->size >=largestBlockSize){
			largestBlockSize = current->free;
		}
		current = current->next;

	}




	//

	return largestBlockSize;
}

struct meta_block *get_block_pointer(void *ptr){
    return (struct meta_block*)ptr - 1;
}

void checkData(){
	struct meta_block *meta=base;
	char str[60];
	while(meta){
			
		sprintf(str, "Block: %p\n Size of Block: %ld\n Block Free: %d\n", meta, meta->size, meta->free);
		puts(str);
		meta=meta->next;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "sfmm.h"

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;

static info information = {
	.internal = 0,
	.external = 0,
	.allocations = 0,
	.frees = 0,
	.coalesce = 0
};



/*
Function to increase the heap and intialize the first sbrk call
*/
void* incrHeap(int a){
	void* pagestart = sf_sbrk(0);
	void* newPos = sf_sbrk(1);
	//newPos = sf_sbrk(0);

	//printf("%s\n", "SBRK");
	//printf("%p %p %ld\n",pagestart, newPos, newPos-pagestart);

	if(newPos != (void*)-1){

		//printf("%s\n", "Creating Header");
	//Create Header
	sf_header * dummyHead;
	dummyHead = pagestart;
	dummyHead->alloc = 0; 
	dummyHead->block_size = 0x100;
	dummyHead->unused_bits = 0;
	dummyHead->padding_size = 0;

	//printf("%s\n", "Creating Footer");

	//Create footer
	sf_footer * dummyFoot;
	void* dummyTemp = dummyHead;
	int shifted = dummyHead->block_size<<4;
	dummyFoot = dummyTemp + shifted -8;
	dummyFoot->alloc = 0;
	dummyFoot->block_size = 0x100;

	//sf_blockprint(dummyHead);



	//printf("%s\n", "we're here");

	freelist_head = pagestart;
	freelist_head->next = NULL;
	freelist_head->prev = NULL;
	//freelist_head->header = *dummyHead;


	information.external=information.external+4080;
	return pagestart;

	}



	// Test the addresses make sure they correct
	/* 
	void* test1 = dummyHead;
	void* test2 = dummyFoot;
	printf("This is the head's address %p\n", test1);
	printf("This is the foot's address %p\n", test2);
	*/
	return NULL;
}


/*
//Decrease the freelist head size 
void modHead(int a, sf_free_header* pointer){

	//Passed through the block size, not shifted
	
	int findFooter = pointer->header.block_size - a;

	void* newAddress = pointer + a;

	sf_footer * newFoot = newAddress + findFooter - 8;
	sf_header * newHead = newAddress;

	newFoot->alloc = 0;
	newFoot->block_size = findFooter>>4;
	newHead->alloc = 0;
	newHead->block_size = findFooter>>4;
	newHead->unused_bits= 0;
	newHead->padding_size=0;


//
	pointer = (void*)newHead;
	
	void* newAddress = pointer;

	int shiftAdd = pointer->header.block_size<<4;
	int noShiftAdd = pointer->header.block_size;
	
	int shiftedIncre = a >>4;


	printf("This is with the shift %i\n", shiftAdd);
	printf("This is without the shift %i\n", noShiftAdd);

	//Change Header
	newAddress = newAddress + shiftAdd;
	pointer = newAddress;
	pointer->header.block_size= noShiftAdd-a;
	//freelist_head->header.alloc = 0;
	//freelist_head->header.unused_bits = 0;
	//freelist_head->header.padding_size = 0;

	//Change Footer
	int newBS = pointer->header.block_size<<4;
	sf_footer *newFoot =(void*)pointer + newBS - 8;
	newFoot->alloc = 0x0000;
	newFoot->block_size = noShiftAdd-a;


	printf("%s\n", "this is the new freelist head");
	//sf_blockprint(freelist_head);

	//
}
*/
//Creates head and returns new Head
/*
void* createHead(size_t a, void* addressOf){

	printf("freeList block size1 %i\n", freelist_head->header.block_size);
		
	int padding = 0;
	if((a%16) == 0){
		padding = 0;
	}
	else{
		padding = 16 - ( a % 16);
	}
	//create head
	sf_header* newHead = addressOf;

	int change = (padding + a + 16)>>4;


	//Check if there is enough room in the head, if not iterate
	//if not either, add page, coalesce
	
	while((newHead->block_size<<4)>freelist_head->header.block_size){
		//printf("%s\n","Let's go4" );
		if(freelist_head->next!=NULL){
			freelist_head=freelist_head->next;
			//printf("%s\n","Let's goe" );
		}
		else{
			void* checkIfFour = sf_sbrk(1);
			if(checkIfFour == (void*)-1){
				return NULL;
				//printf("%s\n","Let's go1" );
			}
			else{
				//printf("%s\n","Let's go" );
					//Because new head is created, we should modify the pointer now
		modHead(newHead->block_size);
		
		//return the Addres of block head
		return (void*)newHead+8;	
			}
		}
	}
	
	//Create a dummy pointer to iterate with
	sf_free_header * dumPoint = NULL;
	dumPoint = freelist_head;

	//Iterate through freelist
	while(change>dumPoint->header.block_size){
		printf("%s\n", "Text appears once");
		if(dumPoint->next!=NULL){
			printf("%s\n", "Really shouldnt be here");
			dumPoint = dumPoint->next;
			continue;
		}

		void* locOfNewPage = sf_sbrk(0);
		void* tester = sf_sbrk(1);
		if(tester == (void*)-1){
			return NULL;
		}

		//coalesce please
		else{
			sf_header * dummyHead;
			dummyHead = locOfNewPage;

			sf_footer * prevFoot = (void*)dummyHead-8;
			//Situation 1, dont coalesce
			if(prevFoot->alloc==0){
				dummyHead->alloc = 0; 
				dummyHead->block_size = 0x100;
				dummyHead->unused_bits = 0;
				dummyHead->padding_size = 0;

	

	
				sf_footer * dummyFoot;
				void* dummyTemp = dummyHead;
				dummyFoot = dummyTemp + dummyHead->block_size -8;
				dummyFoot->alloc = 0;
				dummyFoot->block_size = 0x100;

				//SET THE NEW FREE LIST INTO THE LIST
				sf_free_header * newestFree;
				newestFree =(void*) dummyHead;
				newestFree->prev = dumPoint;
				newestFree->next = NULL;
				printf("This is the address of the newly linked freelist%p\n", newestFree);
				break;

			}
			//situation 2, coalesce
			
			else{

				printf("%s\n","we in here, coalescing" );
				int blockSize = prevFoot->block_size;
				int blockSizeShift = blockSize<<4;

				//FIND THE HEADER
				sf_header * prevHead = (void*) prevFoot
				 - blockSizeShift + 8;

				//set new blocksize of head
				//Coalesce
				prevHead->block_size = blockSize + 0x100;
				prevFoot = (void*)prevHead + blockSizeShift+0x1000 - 8;
				prevFoot->block_size = prevHead->block_size;



			}


	


		}

		

	}
	printf("%s\n","before mod head" );
	modHead(change,dumPoint);
	printf("%s\n","After mod head" );
	newHead->alloc = 0x0001;
	newHead->block_size = (padding + a + 16)>>4;
	newHead->unused_bits=0;
	newHead->padding_size=padding;

	//printf("This is the block_size%i\n", newHead->block_size);
	//printf("%lu\n",newHead->padding_size );
	//printf("%lu\n", newHead->block_size);
	//create footer
	int newBS = newHead->block_size<<4;
	sf_footer *newFoot = (void*)newHead +newBS - 8;
	newFoot->alloc = 0x0001;
	newFoot->block_size = newHead->block_size;

	printf("Dummy block size1 %i\n", newHead->block_size);
	printf("freeList block size1 %i\n", freelist_head->header.block_size);
		
	//sf_blockprint(newHead);
	return (void*)newHead;
}

*/
/**
* This is your implementation of malloc. It creates dynamic memory which
* is aligned and padded properly for the underlying system. This memory
* is uninitialized.
* @param size The number of bytes requested to be allocated.
* @return If successful, the pointer to a valid region of memory to use is
* returned, else the value NULL is returned and the ERRNO is set  accordingly.
* If size is set to zero, then the value NULL is returned.
*/
void *sf_malloc(size_t size){
	if(size== 0){
		return NULL;
	}

	if(size >16384){
		errno = ENOMEM;
		return NULL;
	}
	
	if(freelist_head == NULL){
		
		sf_header* dummy = incrHeap(1);
		if (dummy == NULL){
			errno = ENOMEM;
			return NULL;
		}
		
	}	

	
	//printf("This is size_t %lu\n", size);

	//printf("Block size %i\n",freelist_head->header.block_size );

	//Calculate size needed
	int padding = 0;
	if(size%16 == 0){
		padding = 0;
	}else{
		padding = 16 - ( size % 16);
	}
	
	int newSizeShifted = (padding + 16 + size)>>4;
	
	// Find a place to put the new malloc
	sf_free_header* freeHead = freelist_head;
	//printf("FreeHead block_size : %i\n",freeHead->header.block_size);
	//sf_blockprint(freeHead);

	int triggered = freeHead->header.block_size<<4;

	sf_footer * footAddress = (void*) freeHead  + triggered - 8;
	//printf("New block block_size : %i\n",newSizeShifted);
	//sf_free_header* mightUse = sf_sbrk(0);
	
	//EVERYTHING IS GOOD UP TO HERE THNK GOD

	while(freeHead->header.block_size < newSizeShifted){
		//printf("%i\n",freeHead->header.block_size );
		// CHECK IF THERE IS ANOTHER POINTER
		if(freeHead->next!=NULL){
			//printf("%s\n","looper?" );
			freeHead = freeHead->next;
			continue;
			
		}

		// MAKE A NEW POITNER
		//mightUse = sf_sbrk(0);
		void* testCase = footAddress;
		void* testc = sf_sbrk(1);
		//printf("%s\n","looper?1" );
		if(testc==(void*)-1){
			//printf("%s\n","too much" );
			errno = ENOMEM;
			return NULL;
		}

		else{

			sf_footer * newFoot = (void*)testCase;
			
			//WE dont need to colaese 
			if(newFoot->alloc == 1){
			//printf("%s\n","looper?3" );
				freeHead->header.alloc = 0;
				freeHead->header.block_size = 0x100;
				freeHead->header.unused_bits = 0;
				freeHead->header.padding_size = 0;

				// get to the foot
				int blocksizeShift  = freeHead->header.block_size<<4;
				sf_footer * freeFoot = (void*)freeHead + blocksizeShift - 8;
				freeFoot->alloc = 0;
				freeFoot->block_size=freeHead->header.block_size;

				//Set this as the new free list head



				freelist_head->prev = freeHead;
				freeHead->next = freeHead;
				freeHead->prev = NULL;

				freelist_head = freeHead;

				continue;


			}

			//WE NEED TO COALESCE 
			else{
				//printf("%s\n","looper?4" );
				//printf("%s\n", "Increasing page yes Coalesce");
				int firstBlockSize = newFoot->block_size<<4;

				// Get the previous head add
				sf_header * newHead = (void*)newFoot - firstBlockSize + 8;
				//printf("%s\n", "New Head");
				//sf_blockprint(newHead);
				newHead->block_size = newHead->block_size + 0x100;

				int shifter = newHead->block_size <<4;

				sf_footer * freeFoot = (void*) newHead + shifter - 8;
				freeFoot->alloc = 0;
				freeFoot->block_size = newHead->block_size;



				sf_free_header * newCond = (void*) newHead;

				

				if(freelist_head!=(void*)newHead){
					newCond->next = freelist_head;
					newCond->prev = NULL;
					freelist_head->prev = newCond;
				}

				freelist_head = (void*)newHead;
				freeHead =(void*)freelist_head;

				//sf_blockprint(freelist_head);
				continue;

				//printf("%s\n","do we reach here" );
			}

		}

	}
	

	//decrement the free list head.
	//padding
	//newSizeShifted
	
	int plusAdd = newSizeShifted<<4;
	//printf("How much Address Add : %i\n",plusAdd );
	
	//freeHead = freeHead + plusAdd;

	sf_header * newHead = (void*)freeHead+ plusAdd;
	
	newHead->alloc = 0;
	//int trigger = 
	
	newHead->block_size = freelist_head->header.block_size-newSizeShifted;
	newHead->unused_bits = 0;
	newHead->padding_size = 0;


	int shifted = newHead->block_size<<4;
	
	sf_footer * newFoot = (void*) newHead + shifted - 8;
	
	newFoot->alloc = 0;
	newFoot->block_size = newHead->block_size;


	freelist_head = (void*)newHead;

	//printf("%s\n", "DOES IT MESS UP HERE OR NAH1");
	//sf_blockprint(freelist_head);

	//Do the malloc
	sf_header * malHead = (void*) freeHead;
	malHead->alloc = 1;
	malHead->block_size = newSizeShifted;
	malHead->unused_bits = 0;
	malHead->padding_size = padding;

	int shifted1 = malHead->block_size<<4;
	sf_footer * malFoot = (void*) malHead +shifted1 -8;
	malFoot->alloc = 1;
	malFoot->block_size = malHead->block_size;

	//printf("%s\n", "THIS IS THE FREE LIST HEAD");
	//sf_blockprint(freelist_head);

	information.allocations = information.allocations+1;
	information.internal = information.internal + 16 + padding;
	information.external = information.external - (padding+16+size);

	if(size == 4080){
		freelist_head = NULL;
	}

	return (void*) malHead + 8;
	
 
}


void sf_free(void *ptr){
	information.frees = information.frees + 1;
	if(ptr==(void*)NULL){
		errno=EIO;
		
		return;

	}
	//sf_header* apple1 = (void*)ptr;
	//printf("This is the block size of head %i\n", apple1->block_size);
	void* apple = (void*)ptr - 8;

	//Check if prev is free
	sf_footer * prevFoot =(void*) apple - 8;
	//Check if next is free
	sf_header * thisHead = (void*)apple;
	//sf_blockprint(apple1);
	
	if((thisHead->alloc!= 0) && (thisHead->alloc !=1)){
		
		errno = EIO;
		return;
	}
	//printf("%s\n", "Printing head Before the thing");
	//	sf_blockprint(freelist_head);
	
	//sf_blockprint(apple);


	//printf("This is the block size of head %i\n", thisHead->block_size);


	int blockSizeShift = thisHead->block_size<<4;
	//printf("This is the block size %i\n", blockSizeShift);
	sf_footer * thisFoot = (void*)thisHead + blockSizeShift - 8;

	sf_header * nextHead = (void*)thisHead + blockSizeShift;

	//printf("Prev Foot %i\n",prevFoot->alloc );
	//printf("next head %i\n",nextHead->alloc );

	//BANDAID THE CODE :D

	if(((prevFoot->alloc==0)&&(prevFoot->block_size==0)) &&((nextHead->alloc==0)&&(nextHead->block_size==0))){
		nextHead->alloc = 1;
	}
	else if(((prevFoot->alloc==0)&&(prevFoot->block_size==0)) &&(nextHead->alloc==0)){
		prevFoot->alloc = 1;
	}
	else if((prevFoot->alloc==0)&&((nextHead->alloc==0)&&(nextHead->block_size==0))){
		nextHead->alloc = 1;
	}

	information.coalesce = information.coalesce+1;
	//printf("Prev Foot %i\n",prevFoot->alloc );
	//printf("next head %i\n",nextHead->alloc );
	//CASE 1
	//sf_blockprint(nextHead);
	if(((prevFoot->alloc == 1)||(prevFoot->block_size==0))&& (nextHead->alloc==1)){
		//printf("%s\n", "CASE 1 BEFORE");
		//printf("%s\n", "Printing head");
		//sf_blockprint(freelist_head);
		//printf("%s\n", "Printing next");
		//sf_blockprint(freelist_head->next);
		thisHead->alloc = 0;
		thisFoot->alloc = 0;
		sf_free_header * dumFree = (void*) thisHead;
		thisHead->padding_size = 0;
		dumFree->next = freelist_head;
		dumFree->header.alloc = 0;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
		//printf("%s\n", "block that is freeed");
		//printf("%s\n", "this is the new head");
		//sf_blockprint(freelist_head);
		//printf("%s\n", "this is the new head next");
		//sf_blockprint(freelist_head->next);
		//information.frees = information.frees + 1;
		information.external=information.external+blockSizeShift;
		return;
	}

	//CASE 2
	else if((prevFoot->alloc == 1)&& ((nextHead->alloc==0)||nextHead->block_size==0))
	{

		//printf("%s\n", "CASE 3");
		sf_free_header * nextFreeHead =  (void*) nextHead;
		if(nextFreeHead->next == NULL){

		}else{
			nextFreeHead->next->prev = nextFreeHead->prev;
		}

		if(nextFreeHead->prev==NULL){

		}else{
			nextFreeHead->prev->next = nextFreeHead->next;
		}
		
		thisHead->alloc = 0;
		//get the size
		int triggered = nextHead->block_size;
		int triggeredShift = nextHead->block_size<<4;
		int thisTriggeredShift = thisHead->block_size;
		int thisTriggeredShift1 = thisHead->block_size<<4;

		// find footer
		sf_footer* newestFoot = (void*)thisHead + thisTriggeredShift1+triggeredShift-8;
		thisHead->block_size = thisTriggeredShift+triggered;
		newestFoot->alloc = 0;
		newestFoot->block_size=thisHead->block_size;


		
		sf_free_header * dumFree = (void*) thisHead;
			thisHead->padding_size = 0;

		if(dumFree == freelist_head){
			dumFree->next = freelist_head->next;
			dumFree->prev=NULL;
			if(freelist_head->next!=NULL){
				freelist_head->next->prev = dumFree;
			}

			//freelist_head->prev = dumFree;
			freelist_head  = dumFree;
			//printf("%s\n", "testing");
			//sf_blockprint(freelist_head);
		}
		else{
		dumFree->next = freelist_head;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
	}
		
		
	//information.frees = information.frees + 1;
		//sf_blockprint(freelist_head);
		//sf_blockprint(freelist_head->next);
	information.external=information.external+thisTriggeredShift1;
		return;


	}
	else if (((prevFoot->alloc == 0)||(prevFoot->block_size==0))&& (nextHead->alloc==1)){
		//printf("%s\n", "CASE 2");

		thisFoot->alloc = 0;
		//get the prev head
		int prevHeadSize = prevFoot->block_size;
	//	printf("%i\n", prevHeadSize);
		int prevHeadSizeShift = prevFoot->block_size<<4;
	//	printf("%i\n",prevHeadSizeShift );
		int thisHeadSize = thisHead->block_size;
	//	printf("%i\n",thisHeadSize );
		//int thisHeadSizeShift = thisHead->block_size<<4;

		sf_header * prevHead = (void*)thisHead - prevHeadSizeShift;

		//sf_blockprint(prevHead);
		sf_free_header * prevFreeHead =  (void*) prevHead;

		if(prevFreeHead->next == NULL){

		}else{
			prevFreeHead->next->prev = prevFreeHead->prev;
		}

		if(prevFreeHead->prev==NULL){

		}else{
			prevFreeHead->prev->next = prevFreeHead->next;
		}
		
		//sf_blockprint(prevHead);
		//prevHead->alloc = 0;
		prevHead->block_size = prevHeadSize + thisHeadSize;

		//sf_blockprint(prevHead);
		
		int totalSizeShift = prevHead->block_size<<4;
		sf_footer * thisFoot1 = (void*)prevHead+ totalSizeShift-8;
		thisFoot1->block_size = prevHead->block_size;
		thisFoot1->alloc = 0;

		
		


		sf_free_header * dumFree = (void*) prevHead;
			prevHead->padding_size = 0;
		if(dumFree == freelist_head){
			dumFree->next = freelist_head->next;
			dumFree->prev=NULL;
			if(freelist_head->next!=NULL){
				freelist_head->next->prev = dumFree;
			}

			//freelist_head->prev = dumFree;
			freelist_head  = dumFree;
			//printf("%s\n", "testing");
		//	sf_blockprint(freelist_head);
		}
		else{

		dumFree->next = freelist_head;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
	}
	//printf("%s\n", "these two should be right");
	//sf_blockprint(prevHead);


//information.frees = information.frees + 1;
	//	sf_blockprint(freelist_head);
	information.external=information.external+totalSizeShift;
		return;

		//sf_blockprint(freelist_head);
		//sf_blockprint(freelist_head->next);
	}
	else if(((prevFoot->alloc == 0)||(prevFoot->block_size==0))&& ((nextHead->alloc==0)||(nextHead->block_size==0))){
		//Size of previous
		//printf("%s\n", "CASE 4");

		int prevSizeShift = prevFoot->block_size<<4;
		int prevSize = prevFoot->block_size;
		//Size of this
		int thisSizeSift = thisHead->block_size<<4;
		int thisSize = thisHead->block_size;
		//Size of next
		int nextSizeShift = nextHead->block_size<<4;
		int nextSize = nextHead->block_size;

		//get prevHead
		sf_header * prevHead = (void*)thisHead - prevSizeShift;
		sf_free_header * prevFreeHead =  (void*) prevHead;


		sf_free_header * nextFreeHead1 = (void*)thisHead + thisSizeSift;

		//sf_blockprint(nextFreeHead1);
		if(prevFreeHead->next !=nextFreeHead1){



		if(prevFreeHead->next == NULL){

		}else{
			prevFreeHead->next->prev = prevFreeHead->prev;
		}

		if(prevFreeHead->prev==NULL){

		}else{
			prevFreeHead->prev->next = prevFreeHead->next;
		}

	}
	else{
		if(nextFreeHead1->next==NULL){

		}
		else{
			nextFreeHead1->next->prev = nextFreeHead1->prev;
		}
		if(nextFreeHead1->prev==NULL){

		}
		else{
			nextFreeHead1->prev->next = nextFreeHead1->next;
		}
	}
		
		//prevFreeHead->next->prev = prevFreeHead->prev;
		//prevFreeHead->prev->next = prevFreeHead->next;


		//get nextFoot
		sf_footer * nextFoot = (void*)thisHead + thisSizeSift + nextSizeShift-8;
		sf_free_header * nextFreeHead =  (void*) nextHead;
	if(prevFreeHead !=nextFreeHead1->prev){

		if(nextFreeHead->next == NULL){

		}else{
			nextFreeHead->next->prev = nextFreeHead->prev;
		}

		if(nextFreeHead->prev==NULL){

		}else{
			nextFreeHead->prev->next = nextFreeHead->next;
		}
		//nextFreeHead->next->prev = nextFreeHead->prev;
		//nextFreeHead->prev->next = nextFreeHead->next;

}
else{
		if(prevFreeHead->next==NULL){

		}
		else{
			prevFreeHead->next->prev = prevFreeHead->prev;
		}
		if(prevFreeHead->prev==NULL){

		}
		else{
			prevFreeHead->prev->next = prevFreeHead->next;
		}
}



		prevHead->alloc = 0;
		nextFoot->alloc = 0;

		prevHead->block_size = prevSize+thisSize+nextSize;
		nextFoot->block_size = prevHead->block_size;



/*

		sf_free_header * dumFree = (void*) thisHead;

		if(dumFree == freelist_head){
			dumFree->next = freelist_head->next;
			dumFree->prev=NULL;
			if(freelist_head->next!=NULL){
				freelist_head->next->prev = dumFree;
			}

			//freelist_head->prev = dumFree;
			freelist_head  = dumFree;
			printf("%s\n", "testing");
			sf_blockprint(freelist_head);
		}
		else{
		dumFree->next = freelist_head;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
	}
		
*/	
		if(prevHead == (void*)freelist_head){

			sf_free_header * dumFree = (void*) prevHead;
				prevHead->padding_size = 0;

			if((dumFree->next == freelist_head->next) && (freelist_head->next!=NULL)){
				dumFree->next = freelist_head->next->next;
			}
			else{
				dumFree->next = freelist_head->next;
			}
			dumFree->prev = NULL;
			freelist_head = dumFree;
/*	
			dumFree->next = freelist_head->next;
			dumFree->prev=NULL;
			if(freelist_head->next!=NULL){
				freelist_head->next->prev = dumFree;
			}

			//freelist_head->prev = dumFree;
			freelist_head  = dumFree;

		dumFree->next = freelist_head->next;
		dumFree->prev = NULL;
		//freelist_head->prev = dumFree;


		if(freelist_head->next!=NULL){
			if(freelist_head->next == nextFreeHead1){

			}
			else{
				freelist_head->next->prev = dumFree;
			}
			}
			*/
			
		
		}
		/*
		else if(nextHead == (void*)freelist_head){
			sf_free_header * dumFree = (void*) thisHead;
			if()
				dumFree->next = freelist_head->next->next;
			

		}
		*/
		else{
		sf_free_header * dumFree = (void*) prevHead;
		//sf_header * test = (void*)prevHead;
	prevHead->padding_size = 0;
		dumFree->next = freelist_head;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
	}
	//information.frees = information.frees + 1;

	//sf_blockprint(freelist_head);
	int ddd = prevHead->block_size<<4;
	information.external=information.external+ddd;
		return;

		//sf_blockprint(freelist_head);
		//sf_blockprint(freelist_head->next);

	}

/*
printf("%s\n", "this situation");	
	// Case 1
	if((prevFoot->alloc = 1)&& (nextHead->alloc=1)){
		printf("%s\n", "this situation");
		thisHead->alloc = 0;
		thisFoot->alloc = 0;
		sf_free_header * dumFree = (void*) thisHead;
		dumFree->next = freelist_head;
		dumFree->prev = NULL;
		freelist_head->prev = dumFree;
		freelist_head = dumFree;
	}
	// Case 2
	else if((prevFoot->alloc = 1)&& (nextHead->alloc=0)){
		int triggered = nextHead->block_size;
		int triggeredShift = nextHead->block_size<<4;

		//Get footer of next free block
		sf_footer * newFoot1 = (void*)nextHead + triggeredShift - 8;

		thisHead->alloc = 0;
		thisHead->block_size= thisHead->block_size+triggered;

		newFoot1->alloc = 0;
		newFoot1->block_size = thisHead->block_size;

		sf_free_header * newish = (void*)thisHead;
		sf_free_header * nextNew = (void*) nextHead;

		if(newFoot1 == (void*)freelist_head){
			newish->next = freelist_head->next;
			newish->prev = freelist_head->prev;

			freelist_head->next->prev = newish;
			freelist_head->prev->next = newish;

			freelist_head = newish;

		}
		else{
		nextNew->next->prev = newish;
		newish->next = nextNew->next;
		newish->prev = NULL;
		newish->next = freelist_head;
		newish->prev = NULL;

		freelist_head->prev = newish;

		freelist_head = newish;
	}

	}
	// Case 3
	// Case 4
	*/

}

void *sf_realloc(void *ptr, size_t size){
	if(size == 0){
		errno = ENOENT;
		return NULL;
	}
	//free(ptr);

	int test  = 0;
	int boolean = 0;
	sf_header * head = (void*)ptr -8;
//	sf_blockprint(head);
	printf("%s\n","here" );
	int headSize = head->block_size<<4;
//printf("%s\n","heresadf" );
	if(headSize>(int)size){
		test = headSize - (int)size;
		boolean = 0; // Ggoing down
	}
	else{
		test = (int)size - headSize;
		boolean = 1; //Going up
	}



	if(boolean ==0 ){
		if(test < 32){
			//printf("%s\n","heresadsadf" );
			head->padding_size = head->padding_size+test;
			//printf("%s\n","her3e" );
		}
		return ptr;
	}
	else{
		//printf("%s\n","Im over here" );
		sf_free(ptr);
		//printf("%s\n","here1" );
		return sf_malloc((int)size);
	}

	if(boolean ==1){
		sf_free(ptr);
		//printf("%s\n","here" );
		return sf_malloc((int)size);
		//sf_blockprint(malloc((int)size));
	}



	sf_free(ptr);
	//malloc(size);


  return sf_malloc((int)size);
}

int sf_info(info* meminfo){
	if(meminfo!= NULL){
		meminfo->internal = information.internal;
		meminfo->external = information.external;
		meminfo->allocations = information.allocations;
		meminfo->frees = information.frees;
		meminfo->coalesce = information.coalesce;
	}
	else{
		errno = EIO;
		return -1;
	}
  return -1;
}


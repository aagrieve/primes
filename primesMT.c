// primesMT.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <math.h>
#include <pthread.h>

static int is_verbose = false;
static int num_threads = 1;
static int upper_bound = 10240;
static int num_blocks = 320;

typedef struct BitBlock_s
{
	uint32_t bits;
	pthread_mutex_t mutex;
} BitBlock_t;

static BitBlock_t * b_arr = NULL;

BitBlock_t * alloc_array(void);
void init_array(BitBlock_t *);
void free_array(BitBlock_t *);
int get_next_num(int);
void * thread_func(void *);
void display(void);

BitBlock_t *
alloc_array(void)
{
	BitBlock_t * block = malloc((num_blocks) * sizeof(BitBlock_t));
	/*
	BitBlock_t * block;
	int i = -1;
	for(i = 0; i < num_blocks; i++)
	{
		block[i] = malloc((num_blocks) * sizeof(BitBlock_t));
	} 
	*/
	if(is_verbose)
	{
		printf("Finished allocating Block Array with size: %ld\n", ((num_blocks)*sizeof(BitBlock_t)));
	}
	return block;
}

void
init_array(BitBlock_t * block_array)
{
	int i = -1;
	for(i = 0; i < num_blocks; i++)
	{
		block_array[i].bits = 0;
		pthread_mutex_init(&block_array[i].mutex, NULL);
	}

	if(is_verbose)
	{
		printf("Finished initializing Block Array of size %d with 0s\n", num_blocks);
	}
}

void
free_array(BitBlock_t * block_array)
{
	int i = -1;
	for(i = 0; i < num_blocks; i++)
	{
		pthread_mutex_destroy(&block_array[i].mutex);
		//free(block_array[i]);
	}
	free(block_array);
	
	if(is_verbose)
	{
		printf("Finished freeing Block Array\n");
	}
}

int
get_next_num(int num)
{
	int new_num = -1;
	new_num = ((num_threads * 2) + num);
	return new_num;
}

void * 
thread_func(void * arg)
{
	long start = ((long) arg);
	int block_index = -1;
	int block_bit = -1;
	uint32_t mask = 0;

	for(long curr_slot = start; curr_slot < (sqrt(upper_bound) + 1); curr_slot = get_next_num(curr_slot))
	{
		// if curr_slot is not prime, its multiples are not prime
		block_index = curr_slot / 32;
		block_bit = curr_slot % 32;
		mask = (0x1 << block_bit);
		if((b_arr[block_index].bits & mask) != 0)
		{
			continue;
		}


		//swapped curr_slot * 2 to curr_slot * curr_slot
		for(long curr_num = (curr_slot * curr_slot); curr_num <= upper_bound; curr_num += (2 * curr_slot))
		{
			block_index = curr_num / 32;
			block_bit = curr_num % 32;
			mask = (0x1 << block_bit);

			// set not prime

			// if bit is 1, ignore an continue
			if((b_arr[block_index].bits & mask) != 0)
			{
				continue;
			}
			// if bit is 0, change to 1
			else
			{
				// lock
				pthread_mutex_lock(&b_arr[block_index].mutex);
				
				b_arr[block_index].bits = (b_arr[block_index].bits | mask);

				//unlock
				pthread_mutex_unlock(&b_arr[block_index].mutex);
			}
		}
	}
	return arg;
}

void
display(void)
{
	int block_index = -1;
	uint32_t mask;

	// display 2
	printf("2\n");
	
	//traverse odd entries in bit array and display primes

	for(long curr_num = 3; curr_num <= upper_bound; curr_num += 2)
	{
		block_index = curr_num / 32;
		mask = (0x1 << curr_num);
		
		if((b_arr[block_index].bits & mask) != 0)
		{
			continue;
		}
		printf("%ld\n", curr_num);
	}
}

int main(int argc, char *argv[])
{
	pthread_t * threads = NULL;
	long tid = -1;
	long start = -1;

	// input block
	{
		int opt = -1;
		
		while((opt = getopt(argc, argv, "t:u:hv")) != -1)
		{
			switch(opt)
			{
				case 't':
					num_threads = atoi(optarg);
					break;
				case 'u':
					upper_bound = atoi(optarg);	
					if(upper_bound < 32 && upper_bound > 0)
					{
						num_blocks = 1;
					}
					else
					{
						num_blocks = ((upper_bound / 32) + 1);
					}
					break;
				case 'h':
					printf("%s: -t # -u # -v \n", argv[0]);
					printf("\t-t #: number of threads\n");
					printf("\t-u #: upper bound of calculator\n");
					printf("\t-v  : verbose processing\n");
					exit(0);
					break;
				case 'v':
					is_verbose = true;
					break;
				default: /* '?' */
					exit(EXIT_FAILURE);
					break;
			}
		}
	}

	// business

	//allocate data structure, ie array of BitBlock_t
		
	b_arr = alloc_array();
	// initialize all values in data structure to initial value and initialize mutexes

	init_array(b_arr);

	// find primes
	threads = malloc(num_threads * sizeof(pthread_t));

	for(tid = 0, start = 3; tid < num_threads; tid++, start += 2)
	{
		pthread_create(&threads[tid], NULL, thread_func, (void *) start);
	}

	// join threads
	for(tid = 0; tid < num_threads; tid++)
	{
		pthread_join(threads[tid], NULL);
	}

	// display primes
	display();

	// free
	free(threads);
	free_array(b_arr);
	return EXIT_SUCCESS;
}

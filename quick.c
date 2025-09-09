
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/* define variables for the problem */
#define SEED  100
#define LENGTH 1000000
#define UPPER_LIM 1000000
#define LOWER_LIM  1
#define NUM_THREADS 2

/* define derived values from the variables */
const int NUMBERS_PER_THREAD = LENGTH / NUM_THREADS;
const int OFFSET = LENGTH % NUM_THREADS;
int arr[LENGTH];

/* function definitions */
int generate_random_number(unsigned int lower_limit, unsigned int upper_limit);
void quick_sort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void merge(int arr[], int left, int middle, int right);
void* thread_quick_sort(void* pi[]);
void merge_sections_of_array(int arr[], int number, int aggregation);
void test_array_is_in_order(int arr[]);
struct data{
	int low;
	int high;
};
int main(int argc, const char * argv[]) {
    srand(SEED);
    struct timeval  start, end;
    double time_spent;
    
    /* initialize array with random numbers */
	int i1;    
	for (i1 = 0; i1 < LENGTH; i1 ++) {
        arr[i1] = generate_random_number(LOWER_LIM, UPPER_LIM);
    }
	/*for (i1 = 0; i1 < LENGTH; i1 ++) {
	printf("%d ",arr[i1]);}
    	printf("\n ");*/
    /* begin timing */
    pthread_t threads[NUM_THREADS];
    gettimeofday(&start, NULL);
    int pivot=partition(arr,0,LENGTH);
    struct data pi[NUM_THREADS];
	pi[0].low=0;
	pi[0].high=pivot-1;
	pi[1].low=pivot+1;
	pi[1].high=LENGTH;
	
    /* create threads */
	int i;
     for (i = 0; i < NUM_THREADS; i ++) {
        int rc = pthread_create(&threads[i], NULL, thread_quick_sort, &pi[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for( i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    merge_sections_of_array(arr, NUM_THREADS, 1);
	/*for (i1 = 0; i1 < LENGTH; i1 ++) {
	printf("%d ",arr[i1]);}*/
    
    /* end timing */
    gettimeofday(&end, NULL);
    time_spent = ((double) ((double) (end.tv_usec - start.tv_usec) / 1000000 +
                            (double) (end.tv_sec - start.tv_sec)));
    printf("Time taken for execution: %f seconds\n", time_spent);
    /* test to ensure that the array is in sorted order */
    /* test_array_is_in_order(arr); */
    return 0;
}

/* generate random numbers within the specified limit */
int generate_random_number(unsigned int lower_limit, unsigned int upper_limit) {
    return lower_limit + (upper_limit - lower_limit) * ((double)rand() / RAND_MAX);
}

/* merge locally sorted sections */

void merge_sections_of_array(int arr[], int number, int aggregation) {
	int i;
    for(i = 0; i < number; i = i + 2) {
        int left = i * (NUMBERS_PER_THREAD * aggregation);
        int right = ((i + 2) * NUMBERS_PER_THREAD * aggregation) - 1;
        int middle = left + (NUMBERS_PER_THREAD * aggregation) - 1;
        if (right >= LENGTH) {
            right = LENGTH - 1;
        }
        merge(arr, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections_of_array(arr, number / 2, aggregation * 2);
    }
}

/** assigns work to each thread to perform merge sort */
void *thread_quick_sort(void* pi[])
{
    struct data *temp= (struct data*) pi;
    int low = temp->low;
    int high=temp->high;
	
    if (low < high)
    {
        /* pi là chỉ số nơi phần tử này đã đứng đúng vị trí
         và là phần tử chia mảng làm 2 mảng con trái & phải */
        int pi = partition(arr, low, high);
 
        // Gọi đệ quy sắp xếp 2 mảng con trái và phải
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
    return NULL;
}

/* test to ensure that the array is in sorted order */
void test_array_is_in_order(int arr[]) {
    int max = 0,i;
    for (i = 1; i < LENGTH; i ++) {
        if (arr[i] >= arr[i - 1]) {
            max = arr[i];
        } else {
            printf("Error. Out of order sequence: %d found\n", arr[i]);
            return;
        }
    }
    printf("Array is in sorted order\n");
}

/* perform quick sort */
void quick_sort(int arr[], int low, int high) {
    if (low < high)
    {
        /* pi là chỉ số nơi phần tử này đã đứng đúng vị trí
         và là phần tử chia mảng làm 2 mảng con trái & phải */
        int pi = partition(arr, low, high);
 
        // Gọi đệ quy sắp xếp 2 mảng con trái và phải
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}



/* partition function */
int partition (int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int left = low;
    int right = high - 1;
    while(1){
        while(left <= right && arr[left] < pivot) left++;
        while(right >= left && arr[right] > pivot) right--;
        if (left >= right) break;
	int t=arr[left];
	arr[left]=arr[right];
	arr[right]=t;
        
        left++;
        right--;
    }
	int t=arr[left];
	arr[left]=arr[high];
	arr[high]=t;
    
    return left;
}
/* merge function */
void merge(int arr[], int left, int middle, int right) {
    int i = 0;
    int j = 0;
    int k = 0;
    int left_length = middle - left + 1;
    int right_length = right - middle;
    int left_array[left_length];
    int right_array[right_length];
    
    /* copy values to left array */
	    
	for (i = 0; i < left_length; i ++) {
        left_array[i] = arr[left + i];
    }
    
    /* copy values to right array */
	for (j = 0; j < right_length; j ++) {
        right_array[j] = arr[middle + 1 + j];
    }
    
    i = 0;
    j = 0;
    /** chose from right and left arrays and copy */
    while (i < left_length && j < right_length) {
        if (left_array[i] <= right_array[j]) {
            arr[left + k] = left_array[i];
            i ++;
        } else {
            arr[left + k] = right_array[j];
            j ++;
        }
        k ++;
    }
    
    /* copy the remaining values to the array */
    while (i < left_length) {
        arr[left + k] = left_array[i];
        k ++;
        i ++;
    }
    while (j < right_length) {
        arr[left + k] = right_array[j];
        k ++;
        j ++;
    }
}

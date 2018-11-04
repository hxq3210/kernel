#include <stdio.h>
#include <pthread.h>

#define N 1900000

//pthread_mutex_t mutex;
pthread_spinlock_t spin;

void *do_tid0 (void *data)
{
	int i;

	for (i = 0; i < N; i++) {
	//	pthread_mutex_lock(&mutex);		
		pthread_spin_lock(&spin);
		//pthread_mutex_trylock();
		//pthread_spin_trylock();
		*(int *)data += 1;	
		pthread_spin_unlock(&spin);
	//	pthread_mutex_unlock(&mutex);		
	}
}

void *do_tid1 (void *data)
{
	int i;

	for (i = 0; i < N; i++) {
		//pthread_mutex_lock(&mutex);		
		pthread_spin_lock(&spin);
		*(int *)data -= 1;	
		pthread_spin_unlock(&spin);
		//pthread_mutex_unlock(&mutex);		
	}
}



int main(void)
{
	int cnt = 9527;

	pthread_t tid[2];

//	pthread_mutex_init(&mutex, NULL);

	pthread_spin_init(&spin, 0);

	pthread_create(tid, NULL, do_tid0, &cnt);
	pthread_create(tid+1, NULL, do_tid1, &cnt);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	printf("cnt = %d\n", cnt);	

	return 0;
}

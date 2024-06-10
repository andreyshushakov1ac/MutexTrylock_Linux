/*

	Напишите функцию, удовлетворяющую следующим условиям:
a. Прототип функции:
int lab_pthread_mutex_trylock(pthread_mutex_t *mutex);
b. Входной параметр: адрес мьютекса
c. Функция возвращает следующие значения:
  0 в случае, если мьютекс может быть захвачен без блокировки потока (мьютекс не блокируется, 
просто сообщается о возможности),
  1 если попытка захвата мьютекса с помощью функции pthread_mutex_lock(…) приведет к блокировке 
вызывающего ее потока,
  -1 в случае любой ошибки.
	При реализации функции разрешается использовать стандартные структуры и функции библиотек ОС Linux, 
за исключением функции pthread_mutex_trylock(…).

	Для реализации lab_pthread_mutex_trylock(pthread_mutex_t *mutex) 
используется функция pthread_mutex_destroy(pthread_mutex_t *mutex), которая уничтожает
ранее созданный раннее мьютекс, если он свободен, и возвращает 0. Если занят - возвращает
код ошибки EBUSY

*/


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t my_sync;
pthread_t kidA,kidB;

int lab_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	//
	int mutDestr;
	mutDestr=pthread_mutex_destroy(mutex);
	
	///printf("\n\npthread_mutex_destroy(mutex) = %d\n\n",mutDestr);
	
	if (mutDestr==0)
	{
		printf("Mutex is free!");
		pthread_mutex_init (mutex, NULL);
		return 0;
	}
	
	else if (mutDestr==EBUSY)
	{
		printf("Mutex is busy.");
		return 1;
	}
	
	else	
	{
	perror("mutex:");
	return -1;
	}
} // lab_pthread_mutex_trylock
	
	
	
	
//======================//	
/* int lab_pthread_cond_timedwait(pthread_cond_t *cond,pthread_mutex_t *mutex,unsigned int timetowait) // это должно стать еще одной функцией
{
	
} */
	
	
//======================//
	//Поток А
// Захватывает мьютекс, ждёт 1.5 сек и отпускает в беск. цикле
void * streamA () 
{
	for (;;)
	{
		sleep(1);
		printf("----------\nThread A tries to lock MUTEX...\n");
		pthread_mutex_lock (&my_sync);
		printf("Thread A locks MUTEX\n");
		sleep (1.5);
		printf("Thread A keeps locking MUTEX\n");
		pthread_mutex_unlock (&my_sync);
		printf("Thread A unlocks MUTEX and ends\n----------\n");
	}
}


//======================//
	// Поток B
// В беск. цикле через каждые 3 сек пытается захватить мьютек с помощью lab_pthread_mutex_trylock
void * streamB ()
{
	for (;;)
	{
		sleep(3);
		printf("***********\nThread B tries to lock MUTEX\n");
		int labTryl =lab_pthread_mutex_trylock (&my_sync);
		if (labTryl==0)
			printf("Thread B can lock MUTEX. lab_pthread_mutex_trylock (&my_sync) = %d\n",labTryl);
		else if (labTryl==1)
			printf("Thread B need to be locked! lab_pthread_mutex_trylock (&my_sync) = %d\n",labTryl);
		else if (labTryl==-1)
			printf("Error! Returned: %d\n",labTryl);
		printf("Thread B ends.\n***********\n");
	}
}


//==========================================================================================
int main()
{
	pthread_attr_t attr;
	pthread_mutex_init (&my_sync, NULL);
	pthread_attr_init(&attr);
	
	pthread_create (&kidA, &attr, streamA, NULL);
	pthread_create (&kidB, &attr, streamB, NULL);
	
	pthread_join(kidA,NULL);
	pthread_join(kidB,NULL);
}
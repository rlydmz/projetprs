#define _GNU_SOURCE // permet d'eviter probleme struct sigaction isn't known et implicit declaration, XOPEN, POSIX

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"


void *display_message();
void sigalrm(int);
void test_signaux_multi(long signaux[][2] ,int *gauche, int *droite);


void sigalrm(int signo) {
	(void)signo;
}

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;
  
  return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

// timer_init returns 1 if timers are fully implemented, 0 other
 int timer_init (void)
{
	long signaux[100][2]; //definit le tableau des echances, 0 heure de debut, 1 delay
	int gauche= 0; //Borne gauche de l'intervalle de recherche dans le tableau
	int droite= 0; //Borne droite de l'intervalle de recherche dans le tableau
	signaux[0][0] = 0;

 	sigset_t sigset, oldset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &sigset, &oldset);  // bloque tout les signaux du thread "principla" qui fait tourner le jeu
  
	pthread_t thread1;
	pthread_create(&thread1, NULL, &display_message, NULL); // Creer le thread pour receptionner les signaux

	struct sigaction s;  // Permet de setup le handler pour SIGALRM
	s.sa_handler = sigalrm;
	sigemptyset(&s.sa_mask);
	s.sa_flags = 0;
	sigaction(SIGALRM, &s, NULL);
	/* Permet de tester que le thread recoit 2 SIGALRM avec une pause entre les 2
	alarm(2);
	sleep(3);
	alarm(1);
	*/

	//L'appel commente plus bas permet de tester la reception de plusieurs signaux
	//test_signaux_multi(signaux, &gauche, &droite);
	
	//Permet de tester un signal individuel
	//timer_set(900,NULL);

	return 0; // Implementation not ready
}

void *display_message(void *param){
	sigset_t myset;
	sigemptyset(&myset);
	/*Permet d'afficher l'ID du thread
	pthread_t thread = pthread_self();
	printf("thread %d \n", thread);
	*/
//Permet d'accepter tout les signaux, et surtout le SIGALRM qu'on a suprime du thread plus
	
	while(1){
		if(sigsuspend(&myset)){
			printf("sdl_push_event(%p) appelée au temps %ld \n", param, get_time());
		}
	}	
}

	

void timer_set (Uint32 delay, void *param){
	
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	//timer.it_value.tv_usec = 250000;
	timer.it_value.tv_usec = delay * 1000;
	//timer.it_interval.tv_usec = 250000;
	
	setitimer (ITIMER_REAL, &timer, NULL);
}

void timer_set_multi(Uint32 delay, long signaux[][2], int *gauche, int *droite){
	
	struct itimerval timer;
	long int delay_min = 100000000; // evite de jamais le reduire (en theorie)
	long int time = get_time(); //temps au moment de l'appel de la fonction
	if(signaux[0][0] == 0){
		signaux[0][0] = get_time();
		signaux[0][1] = delay*1000;
		delay_min = signaux[0][1];
	}
	if(*droite<100){ //Cas general: on ajoute notre temps et delay au tableau
		signaux[*droite][0] = get_time();
		signaux[*droite][1] = delay*1000;
	}
	int res=signaux[*gauche][0] + signaux[*gauche][1] - get_time();
	if(res<0){//permet de reduire l'intervalle du tableau dans lequel on recherche le prochain delai d'alarme
		(*gauche)++;
	}
	for( int i=*gauche; i<=*droite; i++){ // ON recherche le prochain delay a armer
		long int res_temp = signaux[*gauche][0] + signaux[*gauche][1] - time;
		printf(" res temp: %li , time : %li\n ", res_temp, time);
		if(res_temp < delay_min){
			delay_min = res_temp;
		}
		printf("delay min: %li\n", delay_min);
	}
	if (*droite>=100){  //Permet de replacer le tableau a partir du debut si on atteind la fin
		int index_temp = 0; 
		int ecart = *droite- *gauche; 
		//printf("ecart : %d\n", ecart);
		for (int i=*gauche ; i<=*droite; i++){
			for(index_temp; index_temp<ecart; index_temp++){
				signaux[index_temp][0] = signaux[i][0];
				signaux[index_temp][1] = signaux[i][1];
			}
		}
	}
		
	timer.it_value.tv_usec =  delay_min; // On arme le delay le plus faible
	(*droite)++;//On incremente la borne de droite de recherche dans notre tableau
	timer.it_value.tv_sec = 0;
	
	
	
	setitimer (ITIMER_REAL, &timer, NULL);

}

void test_signaux_multi(long signaux[][2], int *gauche, int *droite){
	
	timer_set_multi(500, signaux, gauche, droite);
	printf("1ere case: %li, 2eme case: %li, gauche : %d, droite: %d \n", signaux[0][0], signaux[0][1], *gauche, *droite); 
	usleep(1000000);
	timer_set_multi(100, signaux, gauche, droite);
	printf("1ere case: %li, 2eme case: %li, gauche : %d, droite: %d \n", signaux[1][0], signaux[1][1], *gauche, *droite); 
	usleep(1000000);
	timer_set_multi(300, signaux, gauche, droite);
	printf("1ere case: %li, 2eme case: %li, gauche : %d, droite: %d \n", signaux[2][0], signaux[2][1], *gauche, *droite);
}
#endif

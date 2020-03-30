#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<math.h>
#include<pthread.h>
struct station 
{
int passengers_at_station;//waiting passengers
int passengers_in_train;
int passengers_on_board;//boarded passengers
pthread_mutex_t lock;
pthread_cond_t passengers_seated_cond;
};
void station_init (struct station *s);
void station_load_train(struct station *s,int count);
void *station_wait_for_train(struct station *s);
void station_on_board(struct station *s);
void *thread_passenger(void *args);//passenger function
void *train_thread(void *args);// train loading function
//initializing the waiting passengers,in train passengers,boarded passengers,lock,condition statements;
void station_init(struct station *s)
{
s->passengers_at_station = 0;
s->passengers_on_board =0;
s->passengers_in_train =0;
pthread_mutex_init(&(s->lock),NULL);
pthread_cond_init(&(s->passengers_seated_cond),NULL);
}
struct load_train {
	struct station *s;
	int empty_seats;//empty seats in the coming train
};
volatile int load_train_returned = 0;
void *train_thread(void *args)
{
	struct load_train *obj = (struct load_train*)args;
	station_load_train(obj->s, obj->empty_seats);
   load_train_returned = 1;
	return NULL;
}
volatile int thread_completed =0;

int c;
int outvalue;
int main()
{
struct station s;
station_init(&s);
int i;
printf("\t\t\t\t-------------------------------------------------\n");
printf("\t\t\t\t----INDIAN RAILWAY WELCOMES U TO MUMBAI-CSMT-----\n");
printf("\t\t\t\t-------------------------------------------------\n");
printf("ENTER NO.OF PASSENGERS WAITING AT THE STATION: ");
scanf("%d",&c);
int total_passengers =c;

for(i=0;i<c;i++)
{
pthread_t td1;
int res =pthread_create(&td1,NULL,thread_passenger,&s);//passenger threads
res=pthread_join(td1,NULL);
if(res!=0)
{
perror("pthread_create");
exit(1);
}
}
printf("\nNO.OF PASSENGERS WAITING AT THE STATION ARE:%d\n\n",s.passengers_at_station);
//printing the no of waiting passenger
if(s.passengers_at_station==0)
{
	printf("NO PASSENGER IS WAITING AT STATION\n\n");
}
int j;
LOOP :do
{
int f,value;
//value=rand()%3;
f=10;
		//printf("Train entering station with %d empty seats\n", f);
struct load_train args = { &s,  f};
pthread_t td2;
int e = pthread_create(&td2, NULL,train_thread, &args);
if (e != 0) {
perror("pthread_create");
exit(1);
        }
        
        
   
	
//station_load_train(&s,f);

    printf("No of passengers try to enter the train are: %d\n",s.passengers_in_train);
station_on_board(&s);
printf("passengers_on_board are: %d\n",s.passengers_on_board);
printf(" total passengers_in_train updated: %d,%d\n",total_passengers,s.passengers_on_board);//it shows the updated values of the   boarded passengers
}
while(total_passengers > c);
if(s.passengers_on_board == total_passengers)//checking the whether the enter input passengers are equal to boarded or not
{
printf("All passengers have board the train.\n");
}
else
{
if(s.passengers_on_board < c )
{
goto LOOP;
}
}
}

void *thread_passenger(void *args)
{
struct station *s = (struct station*)args;
station_wait_for_train(s);
thread_completed++;

}
void *station_wait_for_train(struct station *s)
{
pthread_mutex_lock(&(s->lock));
s->passengers_at_station++;
/*here in this function it is adding the waiting passengers at the station 
which the value is taken from the user
*/
pthread_mutex_unlock(&(s->lock));
}
void station_load_train(struct station *s,int count)
{
pthread_mutex_lock(&s->lock);
printf("%d\n",count);
while(count >0 && s->passengers_at_station >0)
{
s->passengers_in_train++;//in train passengers increases
count--;//empty seats decreasing
}
if(count == 0)
{	
   printf("train is full\n\n");
}
pthread_cond_wait(&(s->passengers_seated_cond),&(s->lock));
//all the passengers in the train are in seated condition
pthread_mutex_unlock(&(s->lock));
}
void station_on_board(struct station *s)
{
pthread_mutex_lock((&s->lock));
while(s->passengers_in_train >0 && s->passengers_on_board < c)
{
pthread_cond_signal((&s->passengers_seated_cond));
//
s->passengers_in_train--;
s->passengers_on_board++;
}
pthread_cond_broadcast(&s->passengers_seated_cond);
//unlocking all the passengers from seated condition to boarded 
pthread_mutex_unlock(&s->lock);

}

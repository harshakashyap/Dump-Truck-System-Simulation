#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define weighmax 1
#define loadmax 2

int LT[] = {10,5,5,10,15,10,10};
int WT[] = {12,12,12,16,12,16};
int TT[] = {60,100,40,40,80};

int wtp, ttp, ltp;
int sysclock = 0;
struct future_event_list
{
	char event[4];
	int truck_no, n_clock;
	struct future_event_list * next;
};

typedef struct future_event_list FEL;
FEL* head = NULL;

struct truckqueue
{
	int truck_no;
	struct truckqueue *next;
};
typedef struct truckqueue TQ;

TQ *loader = NULL;
TQ *weigher = NULL;	
int LQ, L, WQ, W, Bl, Bs;

void enqueue(TQ **head, int tno)
{
	TQ* temp = *head;
	TQ* node = calloc(1,sizeof(struct truckqueue));
	node->truck_no = tno;
	node->next = NULL;
	if(*head==NULL)
	{
		*head = node;
		return;
	}
	while(temp->next!=NULL)
		temp = temp->next;

	temp->next = node;
}
	
int dequeue(TQ **head)
{
	int x = (*head)->truck_no;
	*head = (*head)->next;
	return x;
}
	
FEL* genevent(char event[], int tno, int nclock)
{
	FEL* temp = calloc(1,sizeof(FEL));
	temp->next = NULL;
	strcpy(temp->event,event);
	temp->truck_no = tno;
	temp->n_clock = nclock;
	return temp;
}

void insert(FEL* node)
{
	FEL* temp = head;
	FEL* prev = head;
	if(head==NULL)
	{	head = node;
		return;
	}
	if(head->n_clock > node->n_clock)
	{
		node->next = head;
		head = node;
		return;
	}
	while(temp!=NULL)
	{
		if(temp->n_clock > node->n_clock)
			break;
		else{
			prev = temp;
			temp = temp->next;
		}
	}
	
	node->next = prev->next;
	prev->next = node;
}

void delete()
{
	FEL* temp = head;
	head = head->next;
	Bl += (temp->n_clock-sysclock)*L;
	Bs += (temp->n_clock-sysclock)*W;
	sysclock = temp->n_clock;
	if(strcmp(temp->event,"EL")==0)
	{	enqueue(&weigher,temp->truck_no);
		WQ++;
		L--;
		if(LQ>0)
		{
			insert(genevent("EL",dequeue(&loader),sysclock+LT[(ltp++)%7]));
			L++;
			LQ--;
		}
		if(WQ>0 && W<weighmax)
		{
			insert(genevent("EW",dequeue(&weigher),sysclock+WT[(wtp++)%6]));
			WQ--;
			W++;
		}
		
	}
	else if(strcmp(temp->event,"EW")==0)
	{
		insert(genevent("ALQ",temp->truck_no,temp->n_clock+TT[(ttp++)%5]));
		W--;
		if(WQ>0)
		{
			insert(genevent("EW",dequeue(&weigher),sysclock+WT[(wtp++)%6]));
			WQ--;
			W++;
		}
	}
	else if(strcmp(temp->event,"ALQ")==0)
	{
		enqueue(&loader,temp->truck_no);
		LQ++;
		if(L<loadmax)
		{
			LQ--;
			L++;
			insert(genevent("EL",dequeue(&loader),sysclock+LT[(ltp++)%7]));
		}
	}
}

void init()
{	
	LQ = 3;
	L = 2;
	WQ = 0;
	W = 1;
	FEL* temp;
	int i=0;
	enqueue(&loader,4);
	enqueue(&loader,5);
	enqueue(&loader,6);
	
	insert(genevent("EW",1,WT[(wtp++)%6]));
	insert(genevent("EL",2,LT[(ltp++)%7]));
	insert(genevent("EL",3,LT[(ltp++)%7]));
}

void display()
{
	int i=0;
	FEL *temp = head;
	TQ *top;
	printf("\n\n\n\t\t Clock = %d\n",sysclock);
	printf("\n LQ:\t%d\t L:\t%d\t WQ:\t%d\t W:\t%d\t BL:\t%d\t BS:\t%d\n",LQ,L,WQ,W,Bl,Bs);
	printf("\nLoader Queue:\t\t");
	top = loader;
	for(i=0;i<LQ;i++)
	{	printf("DT%d\t",top->truck_no);
		top = top->next;
	}
	
	printf("\nWeighing Queue:\t\t");
	top = weigher;
	for(i=0;i<WQ;i++)
	{
		printf("DT%d\t",top->truck_no);
		top = top->next;
	}

	printf("\n\n--------Event List--------\n");
	while(temp!=NULL)
	{
		printf("\n Event: %s\t Time: %d\t Truck: DT%d\t",temp->event, temp->n_clock, temp->truck_no);
		temp = temp->next;
	}
} 
void main()
{
	init();
	while(sysclock<=5000)
	{
		display();
		delete();
	}
	
	printf("\n\n Average loader utilization: %f",(float) (Bl/loadmax)/sysclock);
	printf("\n Average scale utilization: %f\n\n",(float) (Bs/weighmax)/sysclock);
}

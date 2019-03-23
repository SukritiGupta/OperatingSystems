// #include "types.h"
// #include "stat.h"
// #include "user.h"
#include <stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 


int P;
int P1, P2, P3;
int N;

struct message
{
	int msg;
	int sender;
};

int findtotvote(int vdist[])
{
	int ret=0;
	for (int i = 0; i < (2*N-1); ++i)
	{
		if (vdist[i]>=P1)
		{
			ret++;
		}
	}
	return ret;
}
void insert(int waitq[], int newreq, int *b)
{
	int i=0;
	for (; i < *b; ++i)
	{
		if (waitq[i]>newreq)
		{
			break;
		}
	}
	*b=(*b)+1;
	for (int j = *b; j >i ; --j)
	{
		waitq[j]=waitq[j-1];
	}
	waitq[i]=newreq;
}

int removeqq(int waitq[], int *b)
{
	if (b==0)
	{
		printf("dekho+++++++++++++++++++++++++++++++++\n");
	}
	int ret=waitq[0];
	*b=(*b)-1;
	for (int i = 0; i < *b; ++i)
	{
		waitq[i]=waitq[i+1];
	}
	return ret;
}
// 0 request
// 1 locked/success
// 2 failed
// 3 release
// 4 relinquish
// 5 inquire
void calcvdist(int myi, int vdist[])
{
	int row=myi/N;
	int col=myi%N;
	for (int i = 0; i < N; ++i)
	{
		vdist[i]=row*N+i;
		// printf("%d   %d    %d\n", myi,vdist[i],i);
	}
	int t=N;
	for (int i = 0; i < N; ++i)
	{
		int temp=col+i*N;
		if (temp!=myi)
		{
			vdist[t]=temp;
			// printf("%d    %d    %d\n",myi, vdist[t], i+N);
			t++;
		}		
	}

}
int main(int argc, char *argv[])
{

	P=25;
	P1=0;
	P2=12;
	P3=13;
	N=5;




	// for (int i = 0; i < 2*N-1; ++i)
	// {
	// 	inqq[i]=1;
	// 	waitq[i]=-1;
	// }
	int p[2*P][2];

	


	for (int i = 0; i < 2*P; ++i)
	{
		pipe(p[i]);
		// printf("%x\n", p[i]);
	}
	// 0 received 1  from 1 0 received 1  from 1 





	for (int myi = 0; myi < P; ++myi)
	{
		int cid=fork();
		if(cid==0)
		{

		// for (int i = 0; i < 2*N-1; ++i)
		// {
		// 	inqq[i]=1000;
		// 	waitq[i]=1000;
		// }
			int inqq[2*N-1];
			int waitq[2*N-1];
			int vdist[2*N-1];
			int vsize=2*N-1;
			int icomplete=1;
	// 			printf("%x\n",inqq );
	// printf("%x\n",waitq );

			// int inqq[vsize];

			int acquired=0;
			int rec;
			calcvdist(myi, vdist);

			if (myi>=P1)
			{
				// sleep(4);
				icomplete=0;
				for (int i = 0; i < vsize; ++i)
				{
					struct message M, ft;
					M.msg=0;
					M.sender=myi;
					rec=vdist[i];
					// printf("%d  %d\n", myi, rec);
					write(p[rec][1],&M,sizeof(M));	
					// read(p[rec][0],&ft,sizeof(M));
					// printf("ft readddd   %d    %d    %d end\n",ft.sender, ft.msg , rec);
				}
			}

			int votedcscomplete=0;
			int totalvoted=0;
			totalvoted=findtotvote(vdist);
			int inqrec=0;
			int b=0;
			
			int given=0;
			int towho=1000;
			int inqsent=0;
			int failedsig=0;

			while(1)
			{

				// printf("%d############################################%d \n", myi,b);
				if (icomplete==1 && totalvoted==votedcscomplete)
				{
					break;
				}

				struct message received;
				// printf("trying to read  %d  \n",myi);
				read(p[myi][0], &received, sizeof(received));
				// printf("%d received %d  from %d \n",myi, received.msg,received.sender);

				// if(myi==1)
				// for (int i = 0; i < b; ++i)
				// {
				// 	printf("&&&&&&&&&&&&&&&&&&%d\n", waitq[i]);
				// }
				if (received.msg==1)
				{
					acquired++;
					if (icomplete==1)
					{
						printf("yaha pareshaani hai!!!!!!!!!!!!!!!!!!!!!!!!!!!%d   %d\n", myi, received.sender);
					}
				}
				if (received.msg==0)
				{
					if (given==0)
					{
						// int sendto;
						// sendto=removeqq(waitq[],&b);
						// printf("%d    %d\n", myi, received.sender);
						struct message take;
						take.sender=myi;
						take.msg=1;
						given=1;
						towho=received.sender;
						write(p[towho][1],&take,sizeof(struct message));
						// printf("%d received %d  from %d \n",myi, received.msg,received.sender);
						// int inqq1[vsize];
						// sleep(1);
					}
					else
					{
						insert(waitq,received.sender,&b);
						// if(myi==1 && received.sender==0)
						// 	printf("yaha putting into waitq\n");
						if (towho<received.sender||waitq[0]<received.sender)
						{
							struct message fail;
							fail.sender=myi;
							fail.msg=2;
							// given=1;
							// towho=received.sender;
							write(p[received.sender][1],&fail,sizeof(struct message));
						}
						else
						{
							if (inqsent==0)
							{
								inqsent=1;
								struct message inquire;
								inquire.sender=myi;
								inquire.msg=5;
								write(p[towho][1], &inquire, sizeof(struct message));
							}
						}
					}
				}

				// printf("**************************************%d\n", failedsig);

				if (received.msg==5)
				{
					if (failedsig!=0)
					{
						struct message relin;
						relin.sender=myi;
						relin.msg=4;
						write(p[received.sender][1],&relin, sizeof(struct message));
						acquired--;
					}
					else
					{
						inqq[inqrec]=received.sender;
						inqrec++;
					}
				}

				if (received.msg==2)
				{
					failedsig++;
				}

				if (failedsig!=0 && inqrec!=0)
				{
					int inqsend;
					
					struct message relin;
					relin.sender=myi;
					relin.msg=4;
					while(inqrec!=0)
					{
						inqsend=removeqq(inqq, &inqrec);
						write(p[inqsend][1],&relin, sizeof(struct message));
						acquired--;
					}
				}

				if (received.msg==4)
				{
					struct message take;
					take.sender=myi;
					take.msg=1;
					if(given==1 && towho!=1000)
					{
						// if(myi==1 && towho==0)
						// 	printf("yaha putting into waitq\n");
						insert(waitq, towho, &b);
					}
					given=1;

					towho=removeqq(waitq, &b);//**
					write(p[towho][1],&take,sizeof(struct message));
					inqsent--;
				}

				if (received.msg==3)
				{
					votedcscomplete++;
					given=0;
					towho=1000;
					inqsent=0;
					if (b!=0)
					{
						struct message take;
						take.sender=myi;
						take.msg=1;
						given=1;
						// if(myi==1)
						// printf("%d this is b now\n", b);

						towho=removeqq(waitq, &b);//**
						// if(myi==1)
						// printf("%d this is b after remove\n", b);
						// if (myi==1 && towho==0)
						// {
						// 	// printf("%d this is b now\n", );
						// 	printf("release taken from %d\n", received.sender);
						// 	printf("LLLLLLLLLLLLLLLLLLLLLLLLL\n");
						// }
						write(p[towho][1],&take,sizeof(struct message));						
					}
				}

				if (acquired==vsize)
				{
					// printf("hhhhhhhhhhhhhhhhhhhhhhhh\n");
					icomplete=1;
					if (myi<(P1+P2))
					{
						printf("%d acquired the lock at time\n", myi);
						sleep(1);
						printf("%d released the lock at time\n", myi);
					}
					else
					{
						printf("%d P3 acquired the lock at time\n", myi);
						printf("%d P3 released the lock at time\n", myi);
					}
					//send release

					struct message M;
					M.msg=3;
					M.sender=myi;
					for (int i = 0; i < vsize; ++i)
					{
						rec=vdist[i];
						write(p[rec][1],&M,sizeof(struct message));						
					}
					acquired=0;
					inqrec=0;
				}

			}

//failedsig, inqsent;
			// printf("%d exited \n", myi);			
			exit(0);
		}
	}

	// for (int i = 0; i < P; ++i)
	// {
	// 	wait();
	// }
	int status;
	int pid;
	while((pid = wait(&status)) >0){}
	exit(0);

}



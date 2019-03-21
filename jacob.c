// #include "types.h"
// #include "stat.h"
// #include "user.h"


#define N 20
#define E 0.00001
// #define E 100
#define T 1000.0
#define P 8
// #define L 3
#define L 200000

float fabsm(float a){
	if(a<0)
	return -1*a;
return a;
}
int main(int argc, char *argv[])
{
	float diff;
	int i,j;
	float mean;
	float u[N][N];
	float w[N][N];

	int count=0;
	mean = 0.0;
	for (i = 0; i < N; i++){
		u[i][0] = u[i][N-1] = u[0][i] = T;
		u[N-1][i] = 0.0;
		mean += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
	}
	mean /= (4.0 * N);
	for (i = 1; i < N-1; i++ )
		for ( j= 1; j < N-1; j++) u[i][j] = mean;



	// int p[2];char *argv[2];argv[0] ="wc";argv[1] = 0;pipe(p);if(fork() == 0) {close(0);dup(p[0]);close(p[0]);close(p[1]);exec("/bin/wc", argv);} 
	// else {close(p[0]);write(p[1],"hello world\n", 12);close(p[1]);}

	int p[2*P][2];
	int barr[P][P][2];
	int fin[P][2];
	// pipe(finn);

	for (int i = 0; i < 2*P; ++i)
	{
		pipe(p[i]);
	}
	for (int i = 0; i < P; ++i)
	{
		pipe(fin[i]);
	}
	for (int i = 0; i < P; ++i)
	{
		for (int j = 0; j < P; ++j)
		{
			pipe(barr[i][j]);
		}
	}

	int nr=(N-2)/P;
	int left=N-2-nr*P;

	for (int myi = 0; myi < P; ++myi)
	{
		int cid=fork();
		int awr=nr;
		if (myi==P-1)
		{
			awr=nr+left;
		}
		if(cid==0)
		{
			int start=nr*myi+1;
			int end=nr*myi+awr+1;
			for(;;){

				diff = 0.0;
				for(i =start; i < end; i++){
					for(j =1 ; j < N-1; j++){
						w[i][j] = ( u[i-1][j] + u[i+1][j]+
							    u[i][j-1] + u[i][j+1])/4.0;
						if( fabsm(w[i][j] - u[i][j]) > diff )
							diff = fabsm(w[i][j]- u[i][j]);	
					}
				}
			    count++;
			       
				if(count > L){ 
					break;
				}
				int written;
				if (diff<= E)
				{
					//have P pipes, send a 0 to P-1 then check if you have P-1 zeros, assumes pipes are like queue multiple nqer one dqer
					
					written=0;
					for (int i = 0; i < P; ++i)
					{
						if (i!=myi)
						{
							write(barr[i][myi][1],&written,sizeof(int));
							// if(i==0)
							// 	printf("h");
						}
					}
					// printf("%d\n", myi+100);
					int temp=0;
					int buf=0;
					for (int i = 0; i < P; ++i)
					{
						if (i!=myi)
						{
							read(barr[myi][i][0],&buf,sizeof(int));
							// printf("%d  %d   %d\n", myi+100, i, buf);
							temp+=buf;
						}	
					}
					if (temp==0)
					{
						break;
					}
					// printf("%d\n", myi);
				}
				else
				{
					//send 1 and dq P-1
					
					written=1;
					for (int i = 0; i < P; ++i)
					{
						if (i!=myi)
						{
							write(barr[i][myi][1],&written,sizeof(int));
						}
					}
					// printf("%d\n", myi+100);
					int temp=0;
					int buf=0;
					for (int i = 0; i < P; ++i)
					{
						if (i!=myi)
						{
							read(barr[myi][i][0],&buf,sizeof(int));
							temp+=buf;
						}	
					}
					// printf("%d\n", myi+100);
				}
						
				for (i =start; i< end; i++)	
					for (j =1; j< N-1; j++) 
					{
						u[i][j] = w[i][j];
						// printf("%d\n", ((int)u[i][j])+1000);	
					}
				
				for (int i = 1; i < N-1; ++i)
				{
					write(p[myi][1],&(u[end-1][i]),sizeof(float));
					write(p[myi+P][1],&(u[start][i]),sizeof(float));
					// printf("%f %f \n",u[end-1][i], u[start][i] );
				}
				if (myi!=0)
				{
					for (int i = 1; i < N-1; ++i)
					{
						// printf("%f\n", (u[start-1][i]));					

						read(p[myi-1][0],&(u[start-1][i]),sizeof(float));	
						// printf("%f\n", (u[start-1][i]));					
					}
				}

				if (myi!=P-1)
				{
					for (int i = 1; i < N-1; ++i)
					{
						read(p[myi+1+P][0],&(u[end][i]),sizeof(float));
					}
				}
			}

			// send message to parent that I have exited or have exit() and P waits

			//send final output to parent so that it can print in ui

			// printf("*********************************%d\n", myi+100);



			for (i =start; i< end; i++)	
				for (j =1; j< N-1; j++) 
				{
					// write(fin[myi][1],"hello",1);
					write(fin[myi][1],&(u[i][j]),sizeof(float));
					// printf("%f   %d   %d   %d\n",u[i][j], myi, i, j );

					// u[i][j] = w[i][j];

				}
			// close(fin[myi][1]);
			exit(0);
		}
	}

	// for (int i = 0; i < P; ++i)
	// {
	// 	wait();
	// }
	// printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	int pipeno;
	// for(i =0; i <N; i++){
	// 	for(j = 0; j<N; j++)
	// 		printf("%d ",((int)u[i][j]));
	// 	printf("\n");
	// }
	float temp;
	for (int i = 1; i < N-1; ++i)
	{
		pipeno=((i-1))/nr;
		
		if (pipeno>P-1)
		{
			pipeno=P-1;
		}
		// printf("%d\n", pipeno );
		for (int j = 1; j < N-1; ++j)
		{
			// printf("#####################################%d  %d \n", i,j);

			read(fin[pipeno][0],&temp,sizeof(float));
			u[i][j]=temp;
			// printf("*************%f    %d    %d    %d\n",temp,pipeno, i, j);
		}
	}

	for(i =0; i <N; i++){
		for(j = 0; j<N; j++)
			printf("%d ",((int)u[i][j]));
		printf("\n");
	}
	exit(0);

}



#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
int n;
double maxi,mini,avg;
double * arr;
void * max(void * arg)
{   maxi=-1e100;
    int i;
    for(i=0;i<n;i++)
    {   if(maxi<arr[i])
            maxi=arr[i];
    }
    pthread_exit(0);
}
void * min(void * arg)
{   mini=1e100;
    int i;
    for(i=0;i<n;i++)
    {   if(mini>arr[i])
            mini=arr[i];
    }
    pthread_exit(0);
}
void * aver(void * arg)
{   avg=0;
    int i;
    for(i=0;i<n;i++)
        avg+=arr[i];
    avg/=n;
    pthread_exit(0);
}   
void * (*fp[3])(void* );
int main(int argc,char ** argv)
{   int i;
    if(argc>1)
    {   n=argc-1;
        arr=(double *)malloc(n<<3);
        for(i=0;i<n;i++)
            arr[i]=atof(argv[i+1]);
    }
    else
    {   printf("No argumant!\n");
        return 0;
    }
    fp[0]=max;
    fp[1]=min;
    fp[2]=aver;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t tid[3]; 
    for(i=0;i<3;i++)
        pthread_create(&tid[i],&attr,fp[i],NULL);
    for(i=0;i<3;i++)
        pthread_join(tid[i],NULL);
    printf("Maxium :%.6lf\n",maxi);
    printf("Minium :%.6lf\n",mini);
    printf("Average:%.6lf\n",avg);
    free(arr);
    return 0;
}
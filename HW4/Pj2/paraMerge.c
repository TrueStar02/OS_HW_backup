#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
int n;
int * arr;
typedef struct m_arg
{   int l;
    int m;
    int r;
}M_arg;
typedef struct s_arg
{   int l;
    int r;
}S_arg;
int comp(const void * a,const void * b){return *(int *)a>*(int *)b;}
void * Merge(void * arg)
{   M_arg * ptr=(M_arg *)arg;
    int i=ptr->l;
    int j=ptr->m;
    int x;
    int * aux=(int *)malloc(n<<2);
    for(x=0;x<n;x++)
        aux[x]=arr[x];
    for(x=0;x<n;x++)
    {   if(i==ptr->m)
        {   arr[x]=aux[j];
            j++;
        }
        else if(j==ptr->r)
        {   arr[x]=aux[i];
            i++;
        }
        else if(aux[i]<aux[j])
        {   arr[x]=aux[i];
            i++;
        }
        else
        {   arr[x]=aux[j];
            j++;
        }
    }
    free(aux);
    pthread_exit(0);
}
void * Sort(void * arg)
{   S_arg * ptr=(S_arg *)arg;
    qsort(arr+ptr->l,ptr->r-ptr->l,4,comp);
    pthread_exit(0);
}
int main(int argc,char ** argv)
{   int i;
    if(argc>1)
    {   n=argc-1;
        arr=(int *)malloc(n<<2);
        for(i=0;i<n;i++)
            arr[i]=atoi(argv[i+1]);
    }
    else
    {   printf("No argumant!\n");
        return 0;
    }
    int m=n>>1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t tid[3]; 
    S_arg sargs[2]={{0,m},{m,n}};
    M_arg marg={0,m,n};
    pthread_create(&tid[0],&attr,Sort,&sargs[0]);
    pthread_create(&tid[1],&attr,Sort,&sargs[1]);
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_create(&tid[2],&attr,Merge,&marg);
    pthread_join(tid[2],NULL);
    for(i=0;i<n;i++)
        printf("%d ",arr[i]);
    puts("");
    free(arr);
    return 0;
}
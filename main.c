#include<stdio.h>
#include<stdlib.h>
#include"utils.c"
#include<mpi.h>

#include<titan_node_info.c>
#include<struct_cart_domain.c>

/* Most if statements are structured as 
 * if (cond) {command;} in one line for
 * compactness*/


int main(int argc, char *argv[]) 

{

  MPI_Init(&argc,&argv); 
  int n,m,p;
  n=4;m=24;p=1;
  int PopSize=1500;
  int MaxGen=500;
  int N=n*m*p;
  FILE *node_file;
  FILE *subset_file;  
  int nid[19200];
//int xcoors[19200];
//int ycoors[19200];
//int zcoors[19200];
  const int* xcoors;
  const int* ycoors;
  const int* zcoors;
  double r1,r2;
  int rn;
  int topology[6*N];
//  int topology2[6*n*m*p]; 
  int i,j,k,q;
  int number_of_runs=10;
  int NumOfElites=10,elites[PopSize]; 
  int node_count; 
  domain space;
  int id,np; 
  xcoors=&titan_node_coords[0][0];
  ycoors=&titan_node_coords[0][1];
  zcoors=&titan_node_coords[0][2];
   
/*  
  for(i=0;i<15;i++){
      printf("x[%d]: %d y[%d]: %d z[%d]: %d\n",i,titan_node_coords[i][0],i,titan_node_coords[i][1],i,titan_node_coords[i][2]);
    }
  printf("Pointers\n");
  for(i=0;i<15;i++){
    printf("x[%d]: %d y[%d]: %d z[%d]: %d\n",i,xcoors[3*i],i,ycoors[3*i],i,zcoors[3*i]);
  }
*/ 
/* 
  topomat3d(topology,n,m,p);
  for(i=0;i<N;i++){
    printf("%d %d %d %d %d %d\n",topology[i],topology[i+N],topology[i+2*N],topology[i+3*N],topology[i+4*N],topology[i+5*N]);
  }
*/

/* Some code used to test functions in utils.c 
 * ------------------------------------------*/

/*  topomat3d(topology,n,m,p);
  topomat3d_new(topology2,n,m,p);
  printf("old\n");
  for(i=0;i<n;i++){
    printf(" %d %d %d %d %d %d\n",topology[i+0*N],topology[i+1*N],topology[i+2*N],topology[i+3*N],topology[i+4*N],topology[i+5*N]); 
  } 
  printf("new\n");
  for(i=0;i<n;i++){
    printf(" %d %d %d %d %d %d\n",topology2[i+0*N],topology2[i+1*N],topology2[i+2*N],topology2[i+3*N],topology2[i+4*N],topology2[i+5*N]); 
  } 
*/

/*  int test[10]={0,1,2,3,4,5,6,7,8,9};
  int mut=malloc(10*sizeof(int));
  int* p_mut;
  p_mut=mutate_inject(test,10,mut);
  for(i=0;i<10;i++){
    printf("mut[%d]: %d\n",i,p_mut[i]);
  }
*/
//#if 0

// Start program
// -------------
  MPI_Comm_rank(MPI_COMM_WORLD,&id);
  MPI_Comm_size(MPI_COMM_WORLD,&np);

  long idum=(long)id;

  if (id==0){
//  node_file = fopen("/home/Spring13/ORNL/data/coords.txt","r");
//  if (node_file==NULL){
//    printf("Unable to open file");
//    exit(1);
//  }  
//  get_coords(node_file,xcoors,ycoors,zcoors,nid); 
//  fclose(node_file);
    subset_file = fopen("/home/Spring13/ORNL/data/nodes_1_2_24_1_2_1.txt","r");
    if (subset_file == NULL){ 
      printf("Unable to open file");
      exit(1);
    } 
    node_count=count_lines(subset_file); 
  }
  MPI_Bcast((void *)&node_count,1,MPI_INT,0,MPI_COMM_WORLD);  
  int subset_nodes[node_count];
  if (id==0){ 
    rewind(subset_file);
    get_subset(subset_file,subset_nodes,node_count); 
    fclose(subset_file); 
  }
  MPI_Barrier(MPI_COMM_WORLD);
//MPI_Bcast((void *)xcoors,19200,MPI_INT,0,MPI_COMM_WORLD); 
//MPI_Bcast((void *)ycoors,19200,MPI_INT,0,MPI_COMM_WORLD);
//MPI_Bcast((void *)zcoors,19200,MPI_INT,0,MPI_COMM_WORLD); 
  MPI_Bcast((void *)subset_nodes,node_count,MPI_INT,0,MPI_COMM_WORLD);
  space=init_domain(m,n,p);
  
  topomat3d(topology,space);
  
  int *Pop[PopSize]; 
  

  int* bAssign;
  double bFit=INT_MAX,nFit;
  int* mutants[PopSize-NumOfElites];  
  int* NextGen[PopSize]; 

  for(q=0;q<number_of_runs;q++){
    create_pop(node_count,subset_nodes,PopSize,Pop,id+q); 
    for(j=0;j<MaxGen;j++){
      nFit=tournament(N,Pop,PopSize,xcoors,ycoors,zcoors,topology,elites,NumOfElites);
      if (nFit<bFit){bFit=nFit;bAssign=&Pop[elites[0]][0];} /*New best solution*/
  
      for(i=0;i<PopSize;i++){
        if(i<NumOfElites){ NextGen[i]=&Pop[elites[i]][0];} /* Elite*/
        else {free(&Pop[elites[i]][0]);}                  /* Unfit*/
      }
      for(i=NumOfElites;i<PopSize;i++){   
        mutants[i-NumOfElites]=malloc(node_count*sizeof(int)); /*Populate */
        r1=ran3(&idum);
        r2=ran3(&idum);
        rn=(int)(r2*NumOfElites);
        if (r1>.500){ /* coin flip */
          NextGen[i]=mutate_swap(NextGen[rn],node_count,mutants[(i-NumOfElites)]); 
        }else{ 
          NextGen[i]=mutate_inject(NextGen[rn],node_count,mutants[(i-NumOfElites)]); 
        }  
      } 
      for(i=0;i<PopSize;i++){
        Pop[i]=&NextGen[i][0]; 
      }  
    } 
    int* all_solutions=(int*)malloc(node_count*np*sizeof(int));
    double* all_fitness=calloc(np,sizeof(double)); 
    MPI_Gather(bAssign,node_count,MPI_INT,all_solutions,node_count,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Gather(&bFit,1,MPI_DOUBLE,all_fitness,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    if(id==0){ 
      double global_best=INT_MAX; 
      int best_rank;
      for(k=0;k<np;k++){
        if(all_fitness[k]<global_best){global_best=all_fitness[k]; best_rank=k;}
      } 
      printf("%d found best: %f\n",best_rank,global_best); 
      for(i=0;i<m;i++){
        fprintf(stdout," %d  %d  %d  %d\n",zcoors[3*all_solutions[m*0+i+best_rank*node_count]],zcoors[3*all_solutions[m*1+i+best_rank*node_count]],zcoors[3*all_solutions[m*2+i+best_rank*node_count]],zcoors[3*all_solutions[m*3+i+best_rank*node_count]]);
      }
    } 
   free(all_solutions);
   free(all_fitness);
 }
 MPI_Barrier(MPI_COMM_WORLD);
 

  
 MPI_Finalize();  
//#endif  
  return 0; 
}



#include <stdio.h>
#include <stdlib.h>             //malloc library
#include <stdbool.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char* argv[])      //command line arguments
{

  //defining virtual topology
  int size;                    //size of the default communicator (MPI_COMM_WORLD)  // the number of processor
  int old_rank;
  int my_rank;                // my rank in new communicator

  int dim[2],period[2];
  int my_coords[2];
  int dims[2]= {0,0};    // Ask MPI to decompose our processes in a 3D cartesian grid for us
  int periods[2] = {true, true};    // Make all dimensions periodic
  int reorder = true;                   // Let MPI assign arbitrary ranks if it deems it necessary

  int block,row,column;               //variables for block, rows and columns e.g 2 3 3 means 2 blocks, each a 3*3 2d matrix

  
  block = 2400;
  row = 100;
  column = 100;
  int mat_size = block * row * column;   //matrix size as an array x*y*z

  double start_time, end_time;    //to record the execution time on each processor

  MPI_Init(&argc, &argv);

  MPI_Comm new_comm;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD,&old_rank);
  MPI_Dims_create(size, 2, dims);                  // Ask MPI to decompose our processes in a 3D cartesian grid for us
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &new_comm);         // Create a communicator given the 3D torus topology.
  MPI_Comm_rank(new_comm, &my_rank);                         // Get my rank in new communicator
  MPI_Cart_coords(new_comm, my_rank, 2, my_coords);          // Get my coordinates in the new communicator

  printf("[MPI process, old %d, new %d] I am located at (%d, %d).\n", old_rank, my_rank, my_coords[0],my_coords[1]);
  

  int i,j,k;               //nested for loop

  int sndcount = mat_size/size;         // matrix size devided by the number of processors
  int rcvcount = mat_size/size;
  double *A = (double *)malloc(mat_size * sizeof(double));
  double *B = (double *)malloc(mat_size * sizeof(double));
  double *C = (double *)malloc(mat_size * sizeof(double));

  double *A_1 = (double *)malloc((mat_size/size) * sizeof(double));   //for chunks
  double *B_1 = (double *)malloc((mat_size/size) * sizeof(double));
  double *C_1 = (double *)malloc((mat_size/size) * sizeof(double));

  start_time = MPI_Wtime();

  if (my_rank==0){
   //allocating random elements to matrices A and B
   //srand(time(NULL));
    for (i = 0; i < mat_size; i++) {
      A[i] = rand() % 10;
      B[i] = rand() % 10;
    }
  }
  
   // scattering A abd B on all processes
   MPI_Scatter (A, sndcount, MPI_DOUBLE, A_1  , rcvcount,  MPI_DOUBLE, 0, new_comm);    
   MPI_Scatter (B, sndcount, MPI_DOUBLE, B_1  , rcvcount, MPI_DOUBLE, 0, new_comm);

   // matrix summation - store on the chunk C1 then gather to C
   for (i=0; i<(mat_size/size); i++){
     C_1[i] = A_1[i]+B_1[i];
    }

   // gathering the result matrix C on process 0
   MPI_Gather (C_1, sndcount  , MPI_DOUBLE, C, rcvcount, MPI_DOUBLE, 0, new_comm);      
 
   end_time=MPI_Wtime();
   printf ( "\n # walltime on processor %i : %10.8f \n",my_rank, end_time - start_time ) ;

   // printing the matrixes A B and C on rank 0
  //  if (my_rank==0){
  //    printf("\n Matrix A:\n");
  //    for (i = 0; i < mat_size; i++) {
  //      printf("%.2lf ", A[i]);
  //    }
  //    printf("\n\n Matrix B:\n");
  //   for (i = 0; i < mat_size; i++) {
  //     printf("%.2lf ", B[i]);
  //   }
  //    printf("\n\n Matrix C:\n");
  //   for (i = 0; i < mat_size; i++) {
  //     printf("%.2lf ", C[i]);
  //   }
  //   printf("\n\n");

  // }
  
 	if (my_rank == 0) {
		free(A);
		free(A_1);
		free(B);
		free(B_1);
		free(C);
		free(C_1);
	}
  
  MPI_Finalize();
  
  return 0;

}

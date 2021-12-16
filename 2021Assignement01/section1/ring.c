#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]){

    int size;     // the number of processors in the communicator
    int rank; 

    int dim[1]={0};          // 1-dimensional topology. MPI decorates the processors
    int reorder= 1;             // if 1, reordering is true
    int period[1]={1};     // 1-dim array , if 1 periodic dimension is true 

    int left_p= rank-1;          //left processor
    int right_p= rank+1;         // right processor

    int counter=0;
    int sum=0; 
   

    double start_time, end_time;    //to record the execution time on each processor
    
    MPI_Status status1, status2, status3,status4;      //integer array with information on message in case of error   
    MPI_Request req1,req2,req3,req4;

    MPI_Comm new_comm;       //new communicator
   
    // start MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size );   // default communicator
    MPI_Dims_create(size, 1, dim);
    MPI_Cart_create(MPI_COMM_WORLD, 1, dim, period, reorder,&new_comm);   //create new 1D cartesian communicator
    MPI_Cart_shift(new_comm, 0, 1, &left_p, &right_p);   //returns the rank of right and left processor that we need for communication
    MPI_Comm_rank(new_comm, &rank);      // get my rank in the new communicator
    MPI_Comm_size(new_comm, &size);                                           
    
    //int myrank= MPI_Comm_rank;
    int itag=rank*10; 
    int msgleft= rank;               //sending message to left
    int msgright= -rank;            //sending message to right 


    start_time = MPI_Wtime();
   
  int i;
  for (i=0; i<100; i++){
    do {
        counter=counter+2;

       
        int recleft= msgright - rank;               //receiving message from left
        int recright= msgleft + rank;               //receiving message from right

        MPI_Isend( &msgright, 1, MPI_INT, right_p, itag, new_comm, &req1 );        //send msgright=-rank to right= rank+1  with itag            
        MPI_Isend( &msgleft, 1, MPI_INT, left_p, itag, new_comm, &req2);            //send msgleft=rank to left=rank-1   eith itag
        
        MPI_Irecv( &recleft, 1, MPI_INT, left_p, MPI_ANY_TAG, new_comm, &req3);      //receiving from left     
        MPI_Irecv( &recright , 1, MPI_INT, right_p, MPI_ANY_TAG, new_comm, &req4);     //receiving from right

        MPI_Wait(&req1, &status1);
        MPI_Wait(&req2, &status2);
        MPI_Wait(&req3, &status3);
        MPI_Wait(&req4, &status4);

        sum = recleft+recright;

    }while (sum == 0);
  }

    printf( "I am process %d and i have received %d messages. My final messages have tag %d and value %d, %d  \n", rank,counter, itag, msgleft, msgright);
    end_time=MPI_Wtime();
    printf ( "\n # walltime on processor %i : %10.8f \n",rank, end_time - start_time ) ;
    MPI_Finalize( );
    return 0;

}


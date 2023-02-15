#include "my_mpi.h"

using namespace std;

// Naive implementation of MPI_Scatter
int My_MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int np, me;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    if (me == root) {
        int size;
        MPI_Type_size(recvtype, &size);
        for (char i = 0; i < np; i++) if (i != me) MPI_Send(((char*) sendbuf)+i*sendcount*size, sendcount, sendtype, i, 0, comm);
        memcpy(recvbuf, ((char*) sendbuf)+me*sendcount*size, recvcount*size);
    } else MPI_Recv(recvbuf, recvcount, recvtype, root, 0, comm, MPI_STATUS_IGNORE);
    return 0;
}

// Naive implementation of MPI_Gather
int My_MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int np, me;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    if (me == root) {
        int size;
        MPI_Type_size(sendtype, &size);
        for (char i = 0; i < np; i++) if (i != me) MPI_Recv(((char*) recvbuf)+i*recvcount*size, recvcount, recvtype, i, 0, comm, MPI_STATUS_IGNORE);
        memcpy(((char*) recvbuf)+me*recvcount*size, sendbuf, sendcount*size);
    } else MPI_Send(sendbuf, sendcount, sendtype, root, 0, comm);
    return 0;
}

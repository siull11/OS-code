#include "my_mpi.h"

using namespace std;

// int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {

}

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    
}
// MPI tree gather and merge
void treeGatherMerge(vector<double> &vec, int me, int np, int pSizes[], MPI_Comm com, int tag, bool asc) {
    int d = 2, c = 1;
    while (c < np) {
        if (me % d) {
            MPI_Send(&vec[0], vec.size(), MPI_DOUBLE, me-c, tag, com); // Send to process me-c
            break;
        } else {
            if (me+c < np) {
                vec.resize(vec.size()+pSizes[me+c]); // Resize array to accomidate for new elements
                MPI_Recv(&vec[pSizes[me]], pSizes[me+c], MPI_DOUBLE, me+c, tag, com, MPI_STATUS_IGNORE); // Recv from process me+c
                merge(&vec[0], 0, pSizes[me]-1, vec.size()-1, asc); // Merge updated array
            }
            for (int i = me; i < np; i += d) if (i+c < np) pSizes[i] += pSizes[i+c]; // Update sizes
            d *= 2;
            c *= 2;
        }
    }
}

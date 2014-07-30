#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <boost/mpi.hpp>

#include "node_info.h"

/* ---------------------------------------------------------------------- */
// GLOBAL VARIABLES
std::vector<nodeid> gampi_nodelist;
/* ---------------------------------------------------------------------- */

namespace mpi = boost::mpi;

int main(int argc, char *argv[]) { //MAIN MAIN MAIN
mpi::environment env(argc, argv);
mpi::communicator world;
std::srand(135+world.rank());

bool err=true, werr=true; 
int max_i, max_j, max_k;
int opt;

nodeid myid=query_nodeid(); 
std::vector<nodeid> allid; 
all_gather(world, myid, allid);

if(world.rank()==0){
  FILE* nodefile = fopen("TPMfakefile.txt", "w"); 
  for(int i=0; i<allid.size(); i++) {
    fprintf(nodefile, "%d n%5.5d\n",i, allid[i]);
  }
  fclose(nodefile);
}
world.barrier(); 
} //end MAIN MAIN MAIN

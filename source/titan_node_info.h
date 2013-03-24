
// These are the definitions that should be available across all node architectures
typedef int nodeid; 
typedef enum nodeid_validity{
  VALID, INVALID 
} nodeid_validity; 

int init_node_info(void);
nodeid query_nodeid();
int valid_nodeid(nodeid); 
int valid_nodeid_list(int, nodeid*); 

float distance_between_nodes(nodeid, nodeid); 


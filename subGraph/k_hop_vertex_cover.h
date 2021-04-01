#include "graph.h"
//in all the k hop vertex cover problem, we only use adjacency_list and adjacency_list_reverse, we do not store adjacency_list_double
bool delete_adjacency_list_with_specific_edge(vector<NODE_TYPE>* adjacency_list, NODE_TYPE node1, NODE_TYPE node2);
set<NODE_TYPE> k_hop_vertex_cover(vector<NODE_TYPE>* adjacency_list, vector<NODE_TYPE>* adjacency_list_reverse, int k, NODE_TYPE node_num);
bool find_k_path_from_node1(vector<NODE_TYPE>* adjacency_list, int k, NODE_TYPE node1, NODE_TYPE node_num, vector<NODE_TYPE>& c_path, int cur_distance, vector<NODE_TYPE>& result);
set<NODE_TYPE> k_hop_vertex_cover_heuristic(vector<NODE_TYPE>* adjacency_list, vector<NODE_TYPE>* adjacency_list_reverse, int k, NODE_TYPE node_num);
NODE_TYPE find_result_node_in_single_k_path(vector<NODE_TYPE> temp_k_path, int* find_times);

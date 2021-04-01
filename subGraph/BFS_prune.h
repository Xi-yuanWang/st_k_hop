#pragma once
#include "graph.h"
#define INT_MAX 0xffffffff
//typedef pair<NODE_TYPE, DISTANCE_TYPE> N_D_PAIR;
//
class CB_DFS
{
public:
	vector<NODE_TYPE>* adjacency_list;
	map<NODE_TYPE, set<NODE_TYPE>> B;
	int* blocked;
	int node_num;
	//int k;

	CB_DFS(vector<NODE_TYPE>* adjacency_list_init, int input_node_num)
	{
		adjacency_list = adjacency_list_init;
		node_num = input_node_num;
		blocked = new int[node_num + 1];
		//k = k_hop;
		for (int i = 0; i < node_num; i++)
		{
			blocked[i] = 0;
		}
	}
	~CB_DFS()
	{
		delete[] blocked;
	}
	void unblock_all()
	{
		for (int i = 0; i < node_num; i++)
		{
			blocked[i] = 0;
		}

	}

	void unblock(NODE_TYPE u, DISTANCE_TYPE unreach_dis)
	{
		if (blocked[u] > unreach_dis)
		{
			blocked[u] = unreach_dis;
		}
		//blocked[u] = 0;
		auto iter_B = B.find(u);
		if (iter_B == B.end())// no nodes in B list
		{
			return;
		}
		for (auto iter = iter_B->second.begin(); iter != iter_B->second.end(); )
		{
			if (blocked[*iter] > unreach_dis+1)
			{
				unblock(*iter, unreach_dis +1);
				iter_B->second.erase(iter++);
			}
			else 
			{
				iter++;
			}
		}
	}

	int dfs_find_k_paths_with_block(NODE_TYPE cur_node, NODE_TYPE query_node1, 
	NODE_TYPE query_node2, int k, paths &result, set<NODE_TYPE> c_path_set, 
	int cur_distance, vector<NODE_TYPE> c_path)//, unordered_map<NODE_TYPE, DISTANCE_TYPE>& block_list)
	{
		//if (cur_node == 161699)
		//{
			//cout << cur_node;
		//}
		if (cur_distance >= k)
		{
			return -1;
		}
		blocked[cur_node] = k - cur_distance;
		c_path.push_back(cur_node);
		set<NODE_TYPE> new_c_path_set(c_path_set);
		new_c_path_set.insert(cur_node);
		int f = 0;
		bool unb = false;
		for (auto iter = adjacency_list[cur_node].begin(); iter != adjacency_list[cur_node].end(); iter++)
		{
			if (*iter == query_node2)
			{
				//if find a k path
				vector<NODE_TYPE> temp_result_path(c_path);
				temp_result_path.push_back(query_node2);
				result.push_back(temp_result_path);
				f = 0;
				unb = true;
			}
			else if (c_path_set.find(*iter) != c_path_set.end())//repeat node
			{
				continue;
			}
			else if (blocked[*iter] + cur_distance +1< k)
			{
				f = dfs_find_k_paths_with_block(*iter, query_node1, query_node2, k, result, new_c_path_set, cur_distance + 1, c_path);
				if (f != -1)
				{
					unb = true;
				}
			}
		}
		if (unb)
		{
			//unblock_all();
			unblock(cur_node,f);
		}
		else {
			for (auto w = adjacency_list[cur_node].begin(); w != adjacency_list[cur_node].end(); w++)
			{
				auto iter_B = B.find(*w);
				if (iter_B == B.end())//first time to insert
				{
					set<NODE_TYPE> temp_B;
					temp_B.insert(cur_node);
					B.insert(std::make_pair(*w, temp_B));
				}
				else {
					if (iter_B->second.find(cur_node) == iter_B->second.end())
					{
						iter_B->second.insert(cur_node);
					}
				}
			}
		}
		return f; 
	}

};

struct subspace
{
public:
	vector<NODE_TYPE> c_path;
	pair<NODE_TYPE, unordered_set<NODE_TYPE>> ban_node;// = make_pair(-1, unordered_set<NODE_TYPE> {-1});//-1 means no ban node
	DISTANCE_TYPE lower_bound=0;
	unordered_set<NODE_TYPE> result_path;
	subspace() {
		unordered_set<NODE_TYPE> temp;
		ban_node = make_pair(-1, temp);
	}
	subspace(vector<NODE_TYPE> cur_path, pair<NODE_TYPE, unordered_set<NODE_TYPE>>& b_node, DISTANCE_TYPE l_bound, unordered_set<NODE_TYPE> &r_path)
	{
		c_path = cur_path;
		ban_node = b_node;
		lower_bound = l_bound;
		result_path = r_path;
	}
	bool operator < (const subspace& a) const
	{
		return lower_bound > a.lower_bound;// the smaller the first
	}
	void insert_c_path_node(NODE_TYPE n)
	{
		c_path.push_back(n);
	}
	void set_c_path(vector<NODE_TYPE> cur_path)
	{
		c_path = cur_path;
	}
	void set_ban_node(pair<NODE_TYPE, unordered_set<NODE_TYPE>>& n)
	{
		ban_node = n;
	}
};

struct reverse_tree
{
	NODE_TYPE node=-1;
	reverse_tree* parent=NULL;
};



class TopK_dfs//modify the top k algorithm to solve the hop constrained path enumeration problem
{
public:
	NODE_TYPE source;
	NODE_TYPE target;
	NODE_TYPE len;
	NODE_TYPE node_num;
	vector<NODE_TYPE >* indu_graph_reverse;
	DISTANCE_TYPE* reverse_spd;
	vector<NODE_TYPE >* indu_graph; //unordered_map<NODE_TYPE, set<NODE_TYPE>> induced_graph;

	TopK_dfs(vector<NODE_TYPE >* induced_init, vector<NODE_TYPE >* induced_init_reverse, NODE_TYPE s, NODE_TYPE t, DISTANCE_TYPE k, NODE_TYPE node_n)
	{
		source = s;
		target = t;
		len = k;
		node_num = node_n;
		indu_graph = induced_init;
		indu_graph_reverse = induced_init_reverse;
		reverse_spd = new DISTANCE_TYPE[node_n+1];
	
		
	}
	~TopK_dfs(){
		delete[] reverse_spd;
	}
	void construct_reverse_spd()
	{
		//if (source == 368041 && target == 296942)
		//{
		//	cout << "pause" << endl;
		//}
		unordered_set<NODE_TYPE> cur_pro;
		unordered_set<NODE_TYPE> temp_pro;
		cur_pro.insert(target);
		for (int i = 0; i <= node_num; i++)
		{
			reverse_spd[i] = -1;
		}
		reverse_spd[target] = 0;
		DISTANCE_TYPE cur_dis = 0;
		while (!cur_pro.empty())
		{
			cur_dis++;
			temp_pro.clear();
			for (auto iter = cur_pro.begin(); iter != cur_pro.end(); iter++)
			{
				for (auto iter_cur = indu_graph_reverse[*iter].begin(); iter_cur != indu_graph_reverse[*iter].end(); iter_cur++)
				{
					if (reverse_spd[*iter_cur] == -1)
					{
						reverse_spd[*iter_cur] = cur_dis;
						temp_pro.insert(*iter_cur);
					}
					if (*iter_cur == source)
					{
						return;
					}
				}
			}
			cur_pro = temp_pro;
		}
	}

	paths find_path_within_length_constrain_only_reversed_shortestpathtree()
	{
		paths result;
		//vector<NODE_TYPE> temp_path;
		subspace temp;
		temp.insert_c_path_node(source);
		int k = 0;
		priority_queue<subspace> q;
		q.push(temp);
		vector<NODE_TYPE> temp_result;
		while ((temp_result.size() <= len + 1) && (!q.empty()))
		{
			//cout << temp_result.size() << "cur size" << endl;
			subspace temp_space = q.top();
			if (temp_space.lower_bound > len)
			{
				break;
			}
			q.pop();
			temp_result = find_shortest_path_in_subspace(temp_space);
			if (temp_result.size() > len + 1)
			{
				temp_result.clear();
				continue;
			}
			if (temp_result.empty())
			{
				continue;
			}
			NODE_TYPE split_previous;
			vector<NODE_TYPE> temp_split;
			for (auto iter_split = temp_result.begin(); iter_split != temp_result.end(); iter_split++)
			{
				unordered_set<NODE_TYPE> temp_judge(temp_space.c_path.begin(), temp_space.c_path.end());
				if (temp_judge.find(*iter_split) != temp_judge.end())
				{
					split_previous = *iter_split;
					temp_split.push_back(*iter_split);
					continue;
				}
				subspace temp_add;
				temp_add.lower_bound = 0;
				temp_add.set_c_path(temp_split);
				pair < NODE_TYPE, unordered_set<NODE_TYPE>> temp_pair;
				if (temp_space.ban_node.first == split_previous)
				{
					unordered_set<NODE_TYPE> temp_ban_set(temp_space.ban_node.second);
					temp_ban_set.insert(*iter_split);
					temp_pair = make_pair(split_previous, temp_ban_set);
					temp_add.set_ban_node(temp_pair);
				}
				else {
					unordered_set<NODE_TYPE> temp_ban_set;
					temp_ban_set.insert(*iter_split);
					temp_pair = make_pair(split_previous, temp_ban_set);
					temp_add.set_ban_node(temp_pair);
				}
				temp_add.lower_bound = temp_result.size() - 1;
				q.push(temp_add);
				split_previous = *iter_split;
				temp_split.push_back(split_previous);
			}
			//spilt more subspace
			paths temp_out;
			//temp_out.push_back(temp_result);
			//temp_out.write_to_file_append("test_to_check");
			result.push_back(temp_result);
			//result.write_to_file("test_to_check");
			//result.output();
			//cout << "--------------------------" << endl;
		}
		//result.output();
		return result;
	}

	paths find_path_within_length_constrain()
	{
		paths result;
		//vector<NODE_TYPE> temp_path;
		subspace temp;
		temp.insert_c_path_node(source);
		int k = 0;
		priority_queue<subspace> q;
		q.push(temp);
		vector<NODE_TYPE> temp_result;
		while ((temp_result.size() <= len+1) && (!q.empty()))
		{
			//cout << temp_result.size() << "cur size" << endl;
			subspace temp_space = q.top();
			if (temp_space.lower_bound > len)
			{
				break;
			}
			q.pop();
			temp_result = find_shortest_path_in_subspace(temp_space);
			if (temp_result.size() > len + 1)
			{
				temp_result.clear();
				continue;
			}
			if (temp_result.empty())
			{
				continue;
			}
			NODE_TYPE split_previous;
			vector<NODE_TYPE> temp_split;
			for (auto iter_split = temp_result.begin(); iter_split != temp_result.end(); iter_split++)
			{
				unordered_set<NODE_TYPE> temp_judge(temp_space.c_path.begin(), temp_space.c_path.end());
				if (temp_judge.find(*iter_split) != temp_judge.end())
				{
					split_previous = *iter_split;
					temp_split.push_back(*iter_split);
					continue;
				}
				subspace temp_add;
				temp_add.lower_bound = 0;
				temp_add.set_c_path(temp_split);
				pair < NODE_TYPE, unordered_set<NODE_TYPE>> temp_pair;
				if (temp_space.ban_node.first == split_previous)
				{
					unordered_set<NODE_TYPE> temp_ban_set(temp_space.ban_node.second);
					temp_ban_set.insert(*iter_split);
					temp_pair = make_pair(split_previous, temp_ban_set);
					temp_add.set_ban_node(temp_pair);
				}
				else {
					unordered_set<NODE_TYPE> temp_ban_set;
					temp_ban_set.insert(*iter_split);
					temp_pair = make_pair(split_previous, temp_ban_set);
					temp_add.set_ban_node(temp_pair);
				}
				if (reverse_spd[*iter_split] + temp_split.size() == (temp_result.size() - 1))
				{
					temp_add.lower_bound = temp_result.size() - 1;
				}
				else {
					// touch all the valid out neighbors of vertex split_previous
					DISTANCE_TYPE min_out_dis = INT_MAX;
					for (auto iter_lw = indu_graph[split_previous].begin(); iter_lw != indu_graph[split_previous].end(); iter_lw++)
					{
						if (temp_add.ban_node.second.find(*iter_lw) != temp_add.ban_node.second.end() && temp_add.ban_node.first == split_previous)
						{// when meet the ban nodes
							continue;
						}
						else if (*iter_lw == *iter_split)
						{
							continue;
						}
						else {
							DISTANCE_TYPE temp_lw = reverse_spd[*iter_lw];
							if (min_out_dis > temp_lw)
							{
								min_out_dis = temp_lw;
								//temp_add.lower_bound = temp_lw;
							}
						}
					}
					temp_add.lower_bound = min_out_dis + temp_split.size();
				}
				q.push(temp_add);
				split_previous = *iter_split;
				temp_split.push_back(split_previous);
			}
			//spilt more subspace
			paths temp_out;
			//temp_out.push_back(temp_result);
			//temp_out.write_to_file_append("test_to_check");
			result.push_back(temp_result);
			//result.write_to_file("test_to_check");
			//result.output();
			//cout << "--------------------------" << endl;
		}
		//result.output();
		return result;
	}

	vector<NODE_TYPE> find_shortest_path_in_subspace(subspace& s)
	{
		vector<NODE_TYPE> result_path;
		reverse_tree* result_tree = new reverse_tree[node_num + 1];
		//BFS from the source node
		bool* visited_nodes = new bool[node_num+1];
		for (int i = 0; i <= node_num; i++)
		{
			visited_nodes[i] = false;
		}
		for (auto iter = s.c_path.begin(); iter != s.c_path.end(); iter++)
		{
			visited_nodes[*iter] = true;
		}

		NODE_TYPE cur_node = s.c_path[s.c_path.size() - 1];
		NODE_TYPE touched_num = 1;
		unordered_set<NODE_TYPE> cur_proprogate;
		cur_proprogate.insert(cur_node);
		unordered_set<NODE_TYPE> temp_proprogate;
		result_tree[cur_node].node = cur_node;
		result_tree[cur_node].parent = NULL;
		bool find_t = false;
		visited_nodes[cur_node] = false;
		while (touched_num != 0)
		{
			if (find_t)
			{
				break;
			}
			touched_num = 0;
			temp_proprogate.clear();
			for (auto iter_pro = cur_proprogate.begin(); iter_pro != cur_proprogate.end(); iter_pro++)
			{
				if (find_t)
				{
					break;
				}
				//auto iter_next = induced_graph.find(*iter_pro);
				//if (iter_next == induced_graph.end())//no edges here
				//{
				//	continue;
				//}
				for (auto iter = indu_graph[*iter_pro].begin(); iter != indu_graph[*iter_pro].end(); iter++)
				{//travesal
					if (visited_nodes[*iter])
					{
						continue;
					}
					else if ((*iter_pro) == s.ban_node.first && s.ban_node.second.find((*iter)) != s.ban_node.second.end())
					{
						continue;
					}
					else
					{
						visited_nodes[*iter] = true;
						temp_proprogate.insert(*iter);
						touched_num++;
						result_tree[*iter].node = *iter;
						result_tree[*iter].parent = &(result_tree[*iter_pro]);
						if (*iter == target)
						{
							find_t = true;
							break;
						}
					}
				}
			}
			cur_proprogate = temp_proprogate;
		}
		vector<NODE_TYPE> temp_path;
		//temp_path.insert(target);
		if (find_t)
		{
			NODE_TYPE temp_node = target;
			while (temp_node != cur_node)
			{
				temp_path.push_back(temp_node);
				temp_node = result_tree[temp_node].parent->node;
			}
			//construct result path
			for (auto iter = s.c_path.begin(); iter != s.c_path.end(); iter++)
			{
				result_path.push_back(*iter);
			}
			for (auto iter = temp_path.rbegin(); iter != temp_path.rend(); iter++)
			{
				result_path.push_back(*iter);
			}
		}
		else
		{

		}

		delete[] visited_nodes;
		delete[] result_tree;
		return result_path;
	}
};

class CB_DFS_induced
{
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> induced_graph;
	map<NODE_TYPE, set<NODE_TYPE>> B;
	int* blocked;
	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_dis;
	int node_num;
	//int k;
	//another init blocked array

	CB_DFS_induced(unordered_map<NODE_TYPE, 
	set<NODE_TYPE>>& induced_init, 
	int input_node_num, 
	unordered_map<NODE_TYPE, DISTANCE_TYPE>& dst_distance)
	{
		induced_graph = induced_init;
		node_num = input_node_num;
		blocked = new int[node_num + 1];
		//k = k_hop;
		for (int i = 0; i < node_num; i++)
		{
			auto dst_ds = dst_distance.find(i);
			if (dst_ds != dst_distance.end())//there exists distance information for this node
			{
				blocked[i] = dst_ds->second;// we should not minus one
			}
			blocked[i] = 0;
		}
		dst_dis = dst_distance;
	}

	CB_DFS_induced(unordered_map<NODE_TYPE, set<NODE_TYPE>>& induced_init, int input_node_num)
	{
		induced_graph = induced_init;
		node_num = input_node_num;
		blocked = new int[node_num + 1];
		//k = k_hop;
		for (int i = 0; i < node_num; i++)
		{
			blocked[i] = 0;
		}
	}
	~CB_DFS_induced()
	{
		delete[] blocked;
	}
	void unblock_all()
	{
		for (int i = 0; i < node_num; i++)
		{
			blocked[i] = 0;
		}

	}

	void unblock(NODE_TYPE u, DISTANCE_TYPE unreach_dis)
	{
		auto iter_dis = dst_dis.find(u);
		DISTANCE_TYPE dis = 0;
		if (iter_dis != dst_dis.end())
		{
			dis = iter_dis->second;
		}
		if (blocked[u] > unreach_dis)
		{
			blocked[u] = unreach_dis;
		}
		if (blocked[u] > dis)
		{
			blocked[u] = dis;
			unreach_dis = dis;
			//return;
		}
		//blocked[u] = 0;
		auto iter_B = B.find(u);
		if (iter_B == B.end())// no nodes in B list
		{
			return;
		}
		for (auto iter = iter_B->second.begin(); iter != iter_B->second.end(); )
		{
			if (blocked[*iter] > unreach_dis + 1)
			{
				unblock(*iter, unreach_dis + 1);
				iter_B->second.erase(iter++);
			}
			else
			{
				iter++; 
			}
		}
	}

	int dfs_find_k_paths_with_block(NODE_TYPE cur_node, NODE_TYPE query_node1, NODE_TYPE query_node2, int k, paths &result, set<NODE_TYPE> c_path_set, int cur_distance, vector<NODE_TYPE> c_path)//, unordered_map<NODE_TYPE, DISTANCE_TYPE>& block_list)
	{
		//if (cur_node == 161699)
		//{
		//cout << cur_node;
		//}
		if (cur_distance >= k)
		{
			return -1;
		}
		blocked[cur_node] = k - cur_distance;
		c_path.push_back(cur_node);
		set<NODE_TYPE> new_c_path_set(c_path_set);
		new_c_path_set.insert(cur_node);
		int f = 0;
		bool unb = false;
		auto iter_next = induced_graph.find(cur_node);
		if (iter_next == induced_graph.end())//no edges here
		{
			return -1;
		}
		for (auto iter = iter_next->second.begin(); iter != iter_next->second.end(); iter++)
		{
			if (*iter == query_node2)
			{
				//if find a k path
				vector<NODE_TYPE> temp_result_path(c_path);
				temp_result_path.push_back(query_node2);
				result.push_back(temp_result_path);
				f = 0;
				unb = true;
			}
			else if (c_path_set.find(*iter) != c_path_set.end())//repeat node
			{
				continue;
			}
			else if (blocked[*iter] + cur_distance + 1< k)
			{
				f = dfs_find_k_paths_with_block(*iter, query_node1, query_node2, k, result, new_c_path_set, cur_distance + 1, c_path);
				if (f != -1)
				{
					unb = true;
				}
			}
		}
		if (unb)
		{
			//unblock_all();
			unblock(cur_node, f);
		}
		else {
			for (auto w = iter_next->second.begin(); w != iter_next->second.end(); w++)
			{
				auto iter_B = B.find(*w);
				if (iter_B == B.end())//first time to insert
				{
					set<NODE_TYPE> temp_B;
					temp_B.insert(cur_node);
					B.insert(std::make_pair(*w, temp_B));
				}
				else {
					if (iter_B->second.find(cur_node) == iter_B->second.end())
					{
						iter_B->second.insert(cur_node);
					}
				}
			}
		}
		return f;
	}

};



class pruned_subgraph
{
public:
	unordered_set<NODE_TYPE >*  reverse_adjacency_in_subgraph;
	~pruned_subgraph()
	{
		delete[] reverse_adjacency_in_subgraph;
	}
	void construct_pruned_subgraph(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		reverse_adjacency_in_subgraph = new unordered_set<NODE_TYPE >[node_num + 1];
		DISTANCE_TYPE cur_distance = 0;
		unordered_set<NODE_TYPE> cur_proprogate;
		//cur_proprogate.insert(query_node1);
		//unordered_set<NODE_TYPE> node1_set;
		//reverse_adjacency_in_subgraph[query_node1].resize(-1);// query node1's parent is empty
		//node1_set.insert(query_node1);
		for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		{
			cur_proprogate.insert(*iter);
			reverse_adjacency_in_subgraph[*iter].insert(query_node1);// store the first node
		}
		while (cur_distance <= k && !cur_proprogate.empty())
		{
			cur_distance++;
			unordered_set<NODE_TYPE> temp_proprogate;
			NODE_TYPE cur_node;
			for (auto iter = cur_proprogate.begin(); iter != cur_proprogate.end(); iter++)
			{
				cur_node = *iter;
				
				//unordered_set<NODE_TYPE> temp_set;
				for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
				{
					if (*iter2 == query_node1)
					{
						continue;
					}
					if (!reverse_adjacency_in_subgraph[*iter2].empty())//already visited
					{
						reverse_adjacency_in_subgraph[*iter2].insert(cur_node);
						continue;
					}
					reverse_adjacency_in_subgraph[*iter2].insert(cur_node);
					temp_proprogate.insert(*iter2);
				}
			}
			cur_proprogate = temp_proprogate;
		}
	}

	paths find_k_path_using_index(NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< unordered_set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		if (reverse_adjacency_in_subgraph[dst].empty())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(reverse_adjacency_in_subgraph[dst].begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == reverse_adjacency_in_subgraph[dst].end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else if(cur_iters[cur_distance] == reverse_adjacency_in_subgraph[*cur_iters[cur_distance-1]].end() || cur_distance > k)
			{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
  			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == reverse_adjacency_in_subgraph[dst].end())
					{
						force_continue = true;
						break;
					}
				}
				else if (cur_iters[cur_distance] == reverse_adjacency_in_subgraph[*cur_iters[cur_distance-1]].end())
				{
					force_continue = true;
					break;
				}
				
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;
			cur_iters[cur_distance] = reverse_adjacency_in_subgraph[*cur_iters[cur_distance-1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
};


class pruned_subgraph_unordered_map
{
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph;
	void reverse()
	{
		unordered_map<NODE_TYPE, set<NODE_TYPE>> temp;
		for (auto iter = reverse_adjacency_in_subgraph.begin(); iter != reverse_adjacency_in_subgraph.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (temp.find(*iter2) == temp.end())//first time to insert
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					temp.insert(std::make_pair(*iter2, temp_set));
				}
				else
				{
					temp.find(*iter2)->second.insert(iter->first);
				}
			}
		}
		reverse_adjacency_in_subgraph = temp;
	}
	void construct_pruned_subgraph_by_pruned_subgraph(pruned_subgraph_unordered_map &temp_sub, NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> cur_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		reverse_adjacency_in_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		node1_set.insert(query_node1);
		auto iter_temp = temp_sub.reverse_adjacency_in_subgraph.find(query_node1);
		if (iter_temp == temp_sub.reverse_adjacency_in_subgraph.end())//query_node1 is not in index
		{
			return;
		}
		for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
		{
			cur_proprogate.insert(*iter);
			reverse_adjacency_in_subgraph.insert(std::make_pair(*iter, node1_set));// store the first node
		}
		while (cur_distance <= k && !cur_proprogate.empty())
		{
			cur_distance++;
			set<NODE_TYPE> temp_proprogate;
			NODE_TYPE cur_node;
			for (auto iter = cur_proprogate.begin(); iter != cur_proprogate.end(); iter++)
			{
				cur_node = *iter;

				set<NODE_TYPE> temp_set;
				temp_set.insert(cur_node);
				auto iter_temp2 = temp_sub.reverse_adjacency_in_subgraph.find(cur_node);
				if (iter_temp2 == temp_sub.reverse_adjacency_in_subgraph.end())
				{
					continue;
				}
				for (auto iter2 = iter_temp2->second.begin(); iter2 != iter_temp2->second.end(); iter2++)
				{
					if (reverse_adjacency_in_subgraph.find(*iter2) != reverse_adjacency_in_subgraph.end())//already visited
					{
						reverse_adjacency_in_subgraph.find(*iter2)->second.insert(cur_node);
						continue;
					}
					reverse_adjacency_in_subgraph.insert(std::make_pair(*iter2, temp_set));
					temp_proprogate.insert(*iter2);
				}
			}
			cur_proprogate = temp_proprogate;
		}
	}
	void construct_pruned_subgraph(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> cur_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		reverse_adjacency_in_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		node1_set.insert(query_node1);
		for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		{
			cur_proprogate.insert(*iter);
			reverse_adjacency_in_subgraph.insert(std::make_pair(*iter, node1_set));// store the first node
		}
		while (cur_distance <= k && !cur_proprogate.empty())
		{
			cur_distance++;
			set<NODE_TYPE> temp_proprogate;
			NODE_TYPE cur_node;
			for (auto iter = cur_proprogate.begin(); iter != cur_proprogate.end(); iter++)
			{
				cur_node = *iter;

				set<NODE_TYPE> temp_set;
				temp_set.insert(cur_node);
				for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
				{
					if (reverse_adjacency_in_subgraph.find(*iter2) != reverse_adjacency_in_subgraph.end())//already visited
					{
						reverse_adjacency_in_subgraph.find(*iter2)->second.insert(cur_node);
						continue;
					}
					reverse_adjacency_in_subgraph.insert(std::make_pair(*iter2, temp_set));
					temp_proprogate.insert(*iter2);
				}
			}
			cur_proprogate = temp_proprogate;
		}
	}

	paths find_k_path_using_index_reverse(NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;
			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	paths find_k_path_using_index(NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;
			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


};



class pruned_subgraph_unordered_map_double_direction//included dag min index contruct algorithm and middle points algorithm
{
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph;

	unordered_map<NODE_TYPE, set<NODE_TYPE>> induced_subgraph_middle_node;


	set<NODE_TYPE> meet_nodes;//exactly the middle nodes for all paths
	unordered_map<NODE_TYPE, DISTANCE_TYPE> meet_spd_pairs;


	//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse;

	unordered_map<NODE_TYPE, DISTANCE_TYPE> src_distance;
	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_distance;

	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_total;
	void reverse_induced_subgraph(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph)
	{
		unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_subgraph;
		for (auto iter = reverse_adjacency_in_subgraph.begin(); iter != reverse_adjacency_in_subgraph.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (temp_subgraph.find(*iter2) == temp_subgraph.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					temp_subgraph.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					temp_subgraph.find(*iter2)->second.insert(iter->first);
				}
			}
		}
		reverse_adjacency_in_subgraph = temp_subgraph;
	}

	unordered_map<NODE_TYPE, set<NODE_TYPE>> get_new_reverse_induced_subgraph(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph)
	{
		unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_subgraph;
		for (auto iter = reverse_adjacency_in_subgraph.begin(); iter != reverse_adjacency_in_subgraph.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (temp_subgraph.find(*iter2) == temp_subgraph.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					temp_subgraph.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					temp_subgraph.find(*iter2)->second.insert(iter->first);
				}
			}
		}
		return temp_subgraph;
	}



	void join_left_right_index_into_left()
	{
		for (auto iter = reverse_adjacency_in_subgraph_right.begin(); iter != reverse_adjacency_in_subgraph_right.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void join_right_left_into_right()
	{
		for (auto iter = reverse_adjacency_in_subgraph_left.begin(); iter != reverse_adjacency_in_subgraph_left.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void construct_pruned_dag_min_subgraph(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		src_distance.insert(std::make_pair(query_node1, 0));
		dst_distance.insert(std::make_pair(query_node2, 0));
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		//dag_min_induced_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k;
		int right_max_distance = k;
		cur_distance = 0;
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{
						auto iter_src_dis = src_distance.find(*iter2);
						if (iter_src_dis == src_distance.end())//no distance information for node *iter2
						{
							src_distance.insert(std::make_pair(*iter2,cur_distance));
						}
						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						set<NODE_TYPE> temp_set2;
						temp_set2.insert(*iter2);
						//if (dag_min_induced_subgraph_reverse.find(*iter2) == dag_min_induced_subgraph_reverse.end())// not in index subgraph
						//{
						//	dag_min_induced_subgraph_reverse.insert(std::make_pair(cur_node, temp_set2));
						//}
						//else {
						//	dag_min_induced_subgraph_reverse.find(cur_node)->second.insert(*iter2);
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (dag_min_induced_subgraph.find(*iter2) == dag_min_induced_subgraph.end())// not in index subgraph
							{
								dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								dag_min_induced_subgraph.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				break;//left_skip = true;
			}
		}
		//after left bfs, then we do a right bfs to get dag minimum induced subgraph
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_temp;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse_temp;
		cur_distance = 0;

		//left_visited_nodes.clear();
		//right_visited_nodes.clear();
		//left_proprogate.clear();
		//right_proprogate.clear();
		//left_visited_nodes.insert(query_node1);
		//right_visited_nodes.insert(query_node2);
		//left_proprogate.insert(query_node1);
		//right_proprogate.insert(query_node2);

		while(true)
		{
			cur_distance++;
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					//for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					if (dag_min_induced_subgraph.find(cur_node) == dag_min_induced_subgraph.end())
					{
						continue;
					}//no edge for this curnode
					auto iter_temp = dag_min_induced_subgraph.find(cur_node);
					for(auto iter2 = iter_temp->second.begin(); iter2 != iter_temp->second.end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (src_distance.find(*iter2)->second + cur_distance <= k) 
							{//ready to insert this edge
								auto iter_dst_dis = dst_distance.find(*iter2);
								if (iter_dst_dis == dst_distance.end())//no distance information for node *iter2
								{
									dst_distance.insert(std::make_pair(*iter2, cur_distance));
								}
								if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
								{
									reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
								}
								else {
									reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
								}
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if ( src_distance.find(*iter2)->second + cur_distance <= k) 
						{//ready to insert this edge
							auto iter_dst_dis = dst_distance.find(*iter2);
							if (iter_dst_dis == dst_distance.end())//no distance information for node *iter2
							{
								dst_distance.insert(std::make_pair(*iter2, cur_distance));
							}
							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						}
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				break;//right_skip = true;
			}
			//if (left_skip && right_skip)
			//{
			//	break;
			//}
		}
		dag_min_induced_subgraph = reverse_adjacency_in_subgraph_right;
	}
	bool cmp_by_value_decrease(const  pair<NODE_TYPE, DISTANCE_TYPE>& lhs, const  pair<NODE_TYPE, DISTANCE_TYPE>& rhs)
	{
		return lhs.second >= rhs.second;
	}
	paths find_2_paths(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2)//,set<NODE_TYPE> &stop_nodes)
	{
		paths results;
		if(query_node1 == query_node2)
		{
			return results;
		}
		auto left = induced_subgraph.find(query_node1);
		auto right = induced_subgraph_reverse.find(query_node2);
		if(left == induced_subgraph.end() || right == induced_subgraph_reverse.end())
		{
			return results;
		}
		if(left->second.find(query_node2) != left->second.end())//one hop paths
		{
			vector<NODE_TYPE> temp;
			temp.push_back(query_node1);
			temp.push_back(query_node2);
			results.push_back(temp);
		}
		set<NODE_TYPE> intersect;
		set_intersection(left->second.begin(), left->second.end(), right->second.begin(), right->second.end(), std::inserter(intersect, intersect.begin()));
		for(auto iter = intersect.begin(); iter != intersect.end(); iter++)
		{
			vector<NODE_TYPE> temp1;
			temp1.push_back(query_node1);
			temp1.push_back(*iter);
			temp1.push_back(query_node2);
			results.push_back(temp1);
		}
		return results;
	}

	paths find_k_paths_recursive_bfs_middle_nodes_cut_node_order_using_costmodel(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index)
	{//no stopnodes now
	 //// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS + 1)//distance no larger than 3
		{
			//// cout << "k<=5";
			if (k > 3)
			{

				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				if (temp.path.empty())
				{
					if (iter2->second.find(query_node2) != iter2->second.end())
					{
						vector<NODE_TYPE> temp_path;
						temp_path.push_back(query_node1);
						temp_path.push_back(query_node2);
						temp.push_back(temp_path);
					}
				}
				results.add_paths(temp);
				//results.drop_repeat_paths_with_sort();
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
																																						   //temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		else if (k <= 3)//SHORT_CUT_DIS)
		{
			meet_nodes.clear();
			for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
			{
				if (*iter_one_hop == query_node2)
				{// has the one hop path
					vector<NODE_TYPE> temp_path;
					temp_path.push_back(query_node1);
					temp_path.push_back(query_node2);
					results.push_back(temp_path);
				}
			}
			find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
			if (meet_nodes.empty())
			{
				return results;
			}
			for (auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
			{
				NODE_TYPE query = *iter_mn;
				
				//left and right with k/2 is wrong, should be with k-l[i] and r[i]
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - k / 2, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
																																																	//// cout << "after left";

				if (left_paths.path.empty())
				{
					continue;
				}
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - k / 2, stop_nodes, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																	 //// cout << "after right";

				if (right_paths.path.empty())
				{
					continue;
				}
				paths temp_total_paths;
				left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
				results.add_paths(temp_total_paths);
			}
			return results;
		}
		else
		{//recursive function
		 //if(iter2->second.find(query_node2) != iter2->second.end())
		 //{
		 //	vector<NODE_TYPE> temp_path;
		 //	temp_path.push_back(query_node1);
		 //	temp_path.push_back(query_node2);
		 //	results.push_back(temp_path);
		 //}

			paths middle_paths;
			DISTANCE_TYPE short_path_dis = SHORT_CUT_DIS + 1;
			paths short_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, short_path_dis, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2




			short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);
			middle_paths.add_paths(short_paths);
			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs, SHORT_CUT_DIS);//5
			

			
			multimap< DISTANCE_TYPE, pair<DISTANCE_TYPE, NODE_TYPE>> temp_middle_v;
			
			for (auto iter_map = temp_middle_nodes.begin(); iter_map != temp_middle_nodes.end(); iter_map++)
			{
				DISTANCE_TYPE cost = iter_map->second;
				/*auto left_cost = induced_subgraph.find(iter_map->first);
				if(left_cost == induced_subgraph.end())
				{
					cost = 0;
				}
				else
				{
					cost = left_cost->second.size();
				}
				auto right_cost = induced_subgraph_reverse.find(iter_map->first);
				if(right_cost == induced_subgraph_reverse.end())
				{
					cost = 0;
				}
				else
				{
					cost = cost + right_cost->second.size()/ iter_map->second;
				}*/
				temp_middle_v.insert(pair< DISTANCE_TYPE ,pair<DISTANCE_TYPE, NODE_TYPE>>(cost,std::make_pair(iter_map->second, iter_map->first)) );
			}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			for (auto iter_rec = temp_middle_v.begin(); iter_rec != temp_middle_v.end(); iter_rec++)
				//for (auto iter_rec = temp_middle_nodes.rbegin(); iter_rec != temp_middle_nodes.rend(); iter_rec++)
				//for(int i = 0; i <= temp_middle_v.size(); i++)
			{

				NODE_TYPE query = iter_rec->second.second;
				DISTANCE_TYPE dis = iter_rec->second.first;
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " <<query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - dis / 2, stop_nodes_new, level + 1, short_index);//right spd is iter_rec->second/2
				left_paths.drop_path_length_less_than_k((SHORT_CUT_DIS + 1) / 2 + 1);
				//// cout << "after left";
				//	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - (dis + 1) / 2, stop_nodes_new, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																				 //// cout << "after right";
																																																				 //right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				right_paths.drop_path_length_less_than_k(SHORT_CUT_DIS / 2 + 1);
				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}
				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if (level == 1)
				{
					//// cout << " ";
				}
				if (true)//level == 1)
				{
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(right_paths, k, temp_total_paths, SHORT_CUT_DIS+1, stop_nodes_new, (SHORT_CUT_DIS+1)/2+1, (SHORT_CUT_DIS)/2+1,query);// 3, 3);
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, SHORT_CUT_DIS + 1, stop_nodes_new, (SHORT_CUT_DIS + 1) / 2 + 1, (SHORT_CUT_DIS) / 2 + 1);// 3, 3);

																																																								 //left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}
				//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				//temp_total_paths.add_paths(short_paths);
				temp_total_paths.drop_repeat_paths_with_sort();
				middle_paths.add_paths(temp_total_paths);

				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
			if (level == 1)
			{
				//// cout << " ";
			}
			if (level > 1) {
				//middle_paths.drop_repeat_paths_with_sort();
			}
			results.add_paths(middle_paths);
		}
		return results;
	}



	paths find_k_paths_recursive_bfs_middle_nodes(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num,NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index)
	{//no stopnodes now
		//// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS+1)//distance no larger than 3
		{
			//// cout << "k<=5";
			if(k > 3)
			{

				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph,induced_subgraph_reverse,node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse,node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				if(temp.path.empty())
				{
					if (iter2->second.find(query_node2) != iter2->second.end())
					{
						vector<NODE_TYPE> temp_path;
						temp_path.push_back(query_node1);
						temp_path.push_back(query_node2);
						temp.push_back(temp_path);
					}
				}
				results.add_paths(temp);
				//results.drop_repeat_paths_with_sort();
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes,3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		//else if(k <= 3)//SHORT_CUT_DIS)
		//{
		//	meet_nodes.clear();
		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;	
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//		}
		//	}
		//	find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
		//	if(meet_nodes.empty())
		//	{
		//		return results;
		//	}
		//	for(auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
		//	{
		//		NODE_TYPE query = *iter_mn;

		//		//left and right with k/2 is wrong, should be with k-l[i] and r[i]
		//		paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k-k / 2, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
		//																																																	  //// cout << "after left";

		//		if (left_paths.path.empty())
		//		{
		//			continue;
		//		}
		//		paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k-k / 2, stop_nodes, level + 1, short_index);//left spd is (iter_rec->second+1)/2
		//																																																			 //// cout << "after right";

		//		if (right_paths.path.empty())
		//		{
		//			continue;
		//		}
		//		paths temp_total_paths;
		//		left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
		//		results.add_paths(temp_total_paths);
		//	}
		//	return results;
		//}
		else
		{//recursive function
			//if(iter2->second.find(query_node2) != iter2->second.end())
			//{
			//	vector<NODE_TYPE> temp_path;
			//	temp_path.push_back(query_node1);
			//	temp_path.push_back(query_node2);
			//	results.push_back(temp_path);
			//}
			
			paths middle_paths;
			DISTANCE_TYPE short_path_dis = SHORT_CUT_DIS + 1;
			paths short_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, short_path_dis, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
			
			short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);
			middle_paths.add_paths(short_paths);
			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs,SHORT_CUT_DIS);//5
			multimap<DISTANCE_TYPE, NODE_TYPE> temp_middle_v;
			for (auto iter_map = temp_middle_nodes.begin(); iter_map != temp_middle_nodes.end(); iter_map++)
			{
				temp_middle_v.insert(pair<DISTANCE_TYPE, NODE_TYPE>(iter_map->second,iter_map->first));
			}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			for (auto iter_rec = temp_middle_v.begin(); iter_rec != temp_middle_v.end(); iter_rec++)
			//for (auto iter_rec = temp_middle_nodes.rbegin(); iter_rec != temp_middle_nodes.rend(); iter_rec++)
			//for(int i = 0; i <= temp_middle_v.size(); i++)
			{
			
				NODE_TYPE query = iter_rec->second;
				DISTANCE_TYPE dis = iter_rec->first;
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " <<query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - dis /2, stop_nodes_new, level+1,short_index);//right spd is iter_rec->second/2
				left_paths.drop_path_length_less_than_k((SHORT_CUT_DIS + 1)/2+1);
				//// cout << "after left";
			//	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - (dis + 1) / 2, stop_nodes_new, level+1, short_index);//left spd is (iter_rec->second+1)/2
				//// cout << "after right";
				//right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				right_paths.drop_path_length_less_than_k(SHORT_CUT_DIS/2+1);
				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}
				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if(level == 1)
				{
					//// cout << " ";
				}
				if(true)//level == 1)
				{
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(right_paths, k, temp_total_paths, SHORT_CUT_DIS+1, stop_nodes_new, (SHORT_CUT_DIS+1)/2+1, (SHORT_CUT_DIS)/2+1,query);// 3, 3);
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, SHORT_CUT_DIS + 1, stop_nodes_new, (SHORT_CUT_DIS + 1) / 2 + 1, (SHORT_CUT_DIS) / 2 + 1);// 3, 3);

					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}                                                                                                                                     
					//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				//temp_total_paths.add_paths(short_paths);
				temp_total_paths.drop_repeat_paths_with_sort();
				middle_paths.add_paths(temp_total_paths);

				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
			if(level == 1)
			{
				//// cout << " ";
			}
			if (level > 1) {
				//middle_paths.drop_repeat_paths_with_sort();
			}
			results.add_paths(middle_paths);
		}
		return results;
	}

	paths find_k_paths_recursive_bfs_middle_nodes_write_pathnum(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index, string algorithm, string dataset, NODE_TYPE edgeIndex)
	{//no stopnodes now
	 //// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS + 1)//distance no larger than 3
		{
			//// cout << "k<=5";
			if (k > 3)
			{

				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				if (temp.path.empty())
				{
					if (iter2->second.find(query_node2) != iter2->second.end())
					{
						vector<NODE_TYPE> temp_path;
						temp_path.push_back(query_node1);
						temp_path.push_back(query_node2);
						temp.push_back(temp_path);
					}
				}
				results.add_paths(temp);
				//results.drop_repeat_paths_with_sort();
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
																																						   //temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		else if (k <= 3)//SHORT_CUT_DIS)
		{
			meet_nodes.clear();
			for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
			{
				if (*iter_one_hop == query_node2)
				{// has the one hop path
					vector<NODE_TYPE> temp_path;
					temp_path.push_back(query_node1);
					temp_path.push_back(query_node2);
					results.push_back(temp_path);
				}
			}
			find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
			if (meet_nodes.empty())
			{
				return results;
			}
			for (auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
			{
				NODE_TYPE query = *iter_mn;

				//left and right with k/2 is wrong, should be with k-l[i] and r[i]
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - k / 2, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
																																																	//// cout << "after left";

				if (left_paths.path.empty())
				{
					continue;
				}
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - k / 2, stop_nodes, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																	 //// cout << "after right";

				if (right_paths.path.empty())
				{
					continue;
				}
				paths temp_total_paths;
				left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
				results.add_paths(temp_total_paths);
			}
			return results;
		}
		else
		{//recursive function
		 //if(iter2->second.find(query_node2) != iter2->second.end())
		 //{
		 //	vector<NODE_TYPE> temp_path;
		 //	temp_path.push_back(query_node1);
		 //	temp_path.push_back(query_node2);
		 //	results.push_back(temp_path);
		 //}
			NODE_TYPE short_path_nums = 0;
			paths middle_paths;
			DISTANCE_TYPE short_path_dis = SHORT_CUT_DIS + 1;
			paths short_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, short_path_dis, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
			short_path_nums = short_paths.path.size();

			short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);
			middle_paths.add_paths(short_paths);
			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs, SHORT_CUT_DIS);//5
			multimap<DISTANCE_TYPE, NODE_TYPE> temp_middle_v;
			for (auto iter_map = temp_middle_nodes.begin(); iter_map != temp_middle_nodes.end(); iter_map++)
			{
				temp_middle_v.insert(pair<DISTANCE_TYPE, NODE_TYPE>(iter_map->second, iter_map->first));
			}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			for (auto iter_rec = temp_middle_v.begin(); iter_rec != temp_middle_v.end(); iter_rec++)
				//for (auto iter_rec = temp_middle_nodes.rbegin(); iter_rec != temp_middle_nodes.rend(); iter_rec++)
				//for(int i = 0; i <= temp_middle_v.size(); i++)
			{
				NODE_TYPE path_num = 0;
				NODE_TYPE query = iter_rec->second;
				DISTANCE_TYPE dis = iter_rec->first;
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " <<query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - dis / 2, stop_nodes_new, level + 1, short_index);//right spd is iter_rec->second/2
				left_paths.drop_path_length_less_than_k((SHORT_CUT_DIS + 1) / 2 + 1);
				//// cout << "after left";
				//	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - (dis + 1) / 2, stop_nodes_new, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																				 //// cout << "after right";
																																																				 //right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				right_paths.drop_path_length_less_than_k(SHORT_CUT_DIS / 2 + 1);


				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}


				path_num = left_paths.path.size() + right_paths.path.size();

				if (short_path_nums< path_num)
				{
					short_path_nums = path_num;
				}

				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if (level == 1)
				{
					//// cout << " ";
				}
				if (true)//level == 1)
				{
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(right_paths, k, temp_total_paths, SHORT_CUT_DIS+1, stop_nodes_new, (SHORT_CUT_DIS+1)/2+1, (SHORT_CUT_DIS)/2+1,query);// 3, 3);
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, SHORT_CUT_DIS + 1, stop_nodes_new, (SHORT_CUT_DIS + 1) / 2 + 1, (SHORT_CUT_DIS) / 2 + 1);// 3, 3);
					
																																																								 //left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}
				//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				//temp_total_paths.add_paths(short_paths);
				temp_total_paths.drop_repeat_paths_with_sort();
				middle_paths.add_paths(temp_total_paths);

				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
			if (level == 1)
			{
				//// cout << " ";
			}
			if (level > 1) {
				//middle_paths.drop_repeat_paths_with_sort();
			}
			results.add_paths(middle_paths);
			if (true)//level == 1)
			{
				ofstream temp2;
				temp2.open(algorithm + "_" + dataset + "_" + to_string(k) + "_pathnums", ios::app);
				if (!temp2)
				{
					// cout << "pathnums can't open" << endl;
					abort();
				}
				temp2 << edgeIndex << "\t" << query_node1 << "\t" << query_node2 << "\t" << short_path_nums << "\t" << results.path.size() << endl;
				temp2.close();
			}
		}
		return results;
	}


	paths find_k_paths_recursive_middle_cut_write_pathnum(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index, string algorithm, string dataset, NODE_TYPE edgeIndex)
	{//no stopnodes now
	 //// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS + 1)//distance no larger than 3
		{
			//// cout << "k<=5";
			if (k > 3)
			{

				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				if (temp.path.empty())
				{
					if (iter2->second.find(query_node2) != iter2->second.end())
					{
						vector<NODE_TYPE> temp_path;
						temp_path.push_back(query_node1);
						temp_path.push_back(query_node2);
						temp.push_back(temp_path);
					}
				}
				results.add_paths(temp);
				//results.drop_repeat_paths_with_sort();
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
																																						   //temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		else if (k <= 3)//SHORT_CUT_DIS)
		{
			meet_nodes.clear();
			for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
			{
				if (*iter_one_hop == query_node2)
				{// has the one hop path
					vector<NODE_TYPE> temp_path;
					temp_path.push_back(query_node1);
					temp_path.push_back(query_node2);
					results.push_back(temp_path);
				}
			}
			find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
			if (meet_nodes.empty())
			{
				return results;
			}
			set<NODE_TYPE> stop_empty;
			for (auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
			{
				NODE_TYPE query = *iter_mn;

				//left and right with k/2 is wrong, should be with k-l[i] and r[i]

				paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - k / 2, stop_empty, level + 1, short_index);//right spd is iter_rec->second/2
																																																	//// cout << "after left";

				if (left_paths.path.empty())
				{
					continue;
				}
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - k / 2, stop_empty, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																	 //// cout << "after right";

				if (right_paths.path.empty())
				{
					continue;
				}
				paths temp_total_paths;
				left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
				results.add_paths(temp_total_paths);
			}
			return results;
		}
		else
		{//recursive function
		 //if(iter2->second.find(query_node2) != iter2->second.end())
		 //{
		 //	vector<NODE_TYPE> temp_path;
		 //	temp_path.push_back(query_node1);
		 //	temp_path.push_back(query_node2);
		 //	results.push_back(temp_path);
		 //}
			NODE_TYPE short_path_nums = 0;
			paths middle_paths;
			DISTANCE_TYPE short_path_dis = SHORT_CUT_DIS + 1;
			paths short_paths = find_k_paths_recursive_middle_cut_write_pathnum(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, short_path_dis, stop_nodes, level + 1, short_index,algorithm,dataset,edgeIndex);//right spd is iter_rec->second/2
			short_path_nums = short_paths.path.size();

			short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);
			middle_paths.add_paths(short_paths);
			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			//map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs, SHORT_CUT_DIS);//5
			//change this part to achieve new cut and spd pairs
			//construct_middle

			map<NODE_TYPE, DISTANCE_TYPE> middle_nodes_cut;

			middle_nodes_cut = find_all_middle_nodes_in_induced_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
			//find the correct spd
			set<NODE_TYPE> middle_nodes;
			for (auto iter_cut = middle_nodes_cut.begin(); iter_cut != middle_nodes_cut.end(); iter_cut++)
			{
				middle_nodes.insert(iter_cut->first);
			}
			map<NODE_TYPE, DISTANCE_TYPE> final_middle_nodes_cut = bfs_return_spd_pairs(induced_subgraph,node_num,k,query_node1,middle_nodes);

			map<NODE_TYPE, DISTANCE_TYPE> final_middle_nodes_cut_right = bfs_return_spd_pairs(induced_subgraph_reverse, node_num, k, query_node2, middle_nodes);




			multimap<DISTANCE_TYPE, NODE_TYPE> temp_middle_v;
			for (auto iter_map = final_middle_nodes_cut.begin(); iter_map != final_middle_nodes_cut.end(); iter_map++)
			{
				temp_middle_v.insert(pair<DISTANCE_TYPE, NODE_TYPE>(iter_map->second, iter_map->first));
			}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			for (auto iter_rec = temp_middle_v.rbegin(); iter_rec != temp_middle_v.rend(); iter_rec++)
				//for (auto iter_rec = temp_middle_nodes.rbegin(); iter_rec != temp_middle_nodes.rend(); iter_rec++)
				//for(int i = 0; i <= temp_middle_v.size(); i++)
			{
				NODE_TYPE path_num = 0;
				NODE_TYPE query = iter_rec->second;
				DISTANCE_TYPE left_dis = iter_rec->first;
				DISTANCE_TYPE right_dis = 0;
				auto iter_right_dis = final_middle_nodes_cut_right.find(query);
				if (iter_right_dis != final_middle_nodes_cut_right.end())
				{
					right_dis = iter_right_dis->second;
				}
				else {
					continue;
				}
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " <<query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_middle_cut_write_pathnum(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - right_dis, stop_nodes_new, level + 1, short_index,algorithm,dataset,edgeIndex);//right spd is iter_rec->second/2
				//left_paths.drop_path_length_less_than_k((SHORT_CUT_DIS + 1) / 2 + 1);
				//// cout << "after left";
				//	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_middle_cut_write_pathnum(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - left_dis, stop_nodes_new, level + 1, short_index,algorithm,dataset,edgeIndex);//left spd is (iter_rec->second+1)/2
																																																				 //// cout << "after right";
																																																				 //right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				//right_paths.drop_path_length_less_than_k(SHORT_CUT_DIS / 2 + 1);


				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}


				path_num = left_paths.path.size() + right_paths.path.size();

				if (short_path_nums< path_num)
				{
					short_path_nums = path_num;
				}

				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if (level == 1)
				{
					//// cout << " ";
				}
				if (true)//level == 1)
				{
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(right_paths, k, temp_total_paths, SHORT_CUT_DIS+1, stop_nodes_new, (SHORT_CUT_DIS+1)/2+1, (SHORT_CUT_DIS)/2+1,query);// 3, 3);
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, SHORT_CUT_DIS + 1, stop_nodes_new, (SHORT_CUT_DIS + 1) / 2 + 1, (SHORT_CUT_DIS) / 2 + 1);// 3, 3);
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 0, 0);// 3, 3);

																																																								 //left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}
				//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				//temp_total_paths.add_paths(short_paths);
				temp_total_paths.drop_repeat_paths_with_sort();
				middle_paths.add_paths(temp_total_paths);

				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << left_dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
			if (level == 1)
			{
				//// cout << " ";
			}
			if (level > 1) {
				//middle_paths.drop_repeat_paths_with_sort();
			}
			results.add_paths(middle_paths);
			if (level == 1)
			{
				ofstream temp2;
				temp2.open(algorithm + "_" + dataset + "_" + to_string(k) + "_pathnums", ios::app);
				if (!temp2)
				{
					// cout << "pathnums can't open" << endl;
					abort();
				}
				temp2 << edgeIndex << "\t" << query_node1 << "\t" << query_node2 << "\t" << short_path_nums << "\t" << results.path.size() << endl;
				temp2.close();
			}
		}
		results.drop_repeat_paths_with_sort();
		return results;
	}




	paths find_k_paths_recursive_bfs_middle_nodes_node_order_random(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index)
	{//no stopnodes now
	 //// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS + 1)//distance no larger than 3
		{
			//// cout << "k<=5";
			if (k > 3)
			{

				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				if (temp.path.empty())
				{
					if (iter2->second.find(query_node2) != iter2->second.end())
					{
						vector<NODE_TYPE> temp_path;
						temp_path.push_back(query_node1);
						temp_path.push_back(query_node2);
						temp.push_back(temp_path);
					}
				}
				results.add_paths(temp);
				//results.drop_repeat_paths_with_sort();
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
																																						   //temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		else if (k <= 3)//SHORT_CUT_DIS)
		{
			meet_nodes.clear();
			for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
			{
				if (*iter_one_hop == query_node2)
				{// has the one hop path
					vector<NODE_TYPE> temp_path;
					temp_path.push_back(query_node1);
					temp_path.push_back(query_node2);
					results.push_back(temp_path);
				}
			}
			find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
			if (meet_nodes.empty())
			{
				return results;
			}
			for (auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
			{
				NODE_TYPE query = *iter_mn;

				//left and right with k/2 is wrong, should be with k-l[i] and r[i]
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - k / 2, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
																																																	//// cout << "after left";

				if (left_paths.path.empty())
				{
					continue;
				}
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - k / 2, stop_nodes, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																	 //// cout << "after right";

				if (right_paths.path.empty())
				{
					continue;
				}
				paths temp_total_paths;
				left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
				results.add_paths(temp_total_paths);
			}
			return results;
		}
		else
		{//recursive function
		 //if(iter2->second.find(query_node2) != iter2->second.end())
		 //{
		 //	vector<NODE_TYPE> temp_path;
		 //	temp_path.push_back(query_node1);
		 //	temp_path.push_back(query_node2);
		 //	results.push_back(temp_path);
		 //}

			paths middle_paths;
			DISTANCE_TYPE short_path_dis = SHORT_CUT_DIS + 1;
			paths short_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, short_path_dis, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2

			short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);
			middle_paths.add_paths(short_paths);
			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs, SHORT_CUT_DIS);//5
			//multimap<DISTANCE_TYPE, NODE_TYPE> temp_middle_v;
			//for (auto iter_map = temp_middle_nodes.begin(); iter_map != temp_middle_nodes.end(); iter_map++)
			//{
			//	temp_middle_v.insert(pair<DISTANCE_TYPE, NODE_TYPE>(iter_map->second, iter_map->first));
			//}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			
			vector<pair<NODE_TYPE, DISTANCE_TYPE>> mapList;// = new vector<pair<NODE_TYPE, DISTANCE_TYPE>>(temp_middle_nodes);
			for (auto iter_rec = temp_middle_nodes.begin(); iter_rec != temp_middle_nodes.end(); iter_rec++)
			{
				mapList.push_back(std::make_pair(iter_rec->first, iter_rec->second));
			}

			int count = mapList.size();
			default_random_engine e;
			uniform_real_distribution<double> u(0.0, 1.0);
			for(int i = 0; i <count; i++)
			//for (auto iter_rec = temp_middle_nodes.begin(); iter_rec != temp_middle_nodes.end(); iter_rec++)
			{
				int _index = (int)(u(e)*mapList.size());
				NODE_TYPE query = mapList[_index].first;// iter_rec->first;
				DISTANCE_TYPE dis = mapList[_index].second;// iter_rec->second;
				mapList.erase(mapList.begin() + _index);
				
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " <<query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - dis / 2, stop_nodes_new, level + 1, short_index);//right spd is iter_rec->second/2
				left_paths.drop_path_length_less_than_k((SHORT_CUT_DIS + 1) / 2 + 1);
				//// cout << "after left";
				//	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes_node_order_random(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - (dis + 1) / 2, stop_nodes_new, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																				 //// cout << "after right";
																																																				 //right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				right_paths.drop_path_length_less_than_k(SHORT_CUT_DIS / 2 + 1);
				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}
				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if (level == 1)
				{
					//// cout << " ";
				}
				if (true)//level == 1)
				{
					//left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(right_paths, k, temp_total_paths, SHORT_CUT_DIS+1, stop_nodes_new, (SHORT_CUT_DIS+1)/2+1, (SHORT_CUT_DIS)/2+1,query);// 3, 3);
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, SHORT_CUT_DIS + 1, stop_nodes_new, (SHORT_CUT_DIS + 1) / 2 + 1, (SHORT_CUT_DIS) / 2 + 1);// 3, 3);

																																																								 //left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}
				//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				//temp_total_paths.add_paths(short_paths);
				temp_total_paths.drop_repeat_paths_with_sort();
				middle_paths.add_paths(temp_total_paths);

				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
			if (level == 1)
			{
				//// cout << " ";
			}
			if (level > 1) {
				//middle_paths.drop_repeat_paths_with_sort();
			}
			results.add_paths(middle_paths);
		}
		return results;
	}



	paths find_k_paths_recursive_bfs_middle_nodes_only_bfs(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE k, set<NODE_TYPE> stop_nodes, int level, short_cut_index& short_index)
	{//no stopnodes now
	 //// cout << query_node1 << "\t" << query_node2 << " \t " << k << "query "<< endl;

		if (DEBUG_ALL_QUERYS)
		{
			ofstream temp;
			temp.open("allQuerys", ios::app);
			if (!temp)
			{
				// cout << "allQuerys can't open" << endl;
				abort();
			}
			temp << query_node1 << "\t" << query_node2 << " \t " << k << endl;
			temp.close();
			// cout << query_node1 << "\t" << query_node2 << " \t " << k << endl;
		}
		paths results;
		if (query_node1 == query_node2)
		{
			//// cout << "node1 equal node2";
			return results;
		}

		auto iter2 = induced_subgraph.find(query_node1);
		if (iter2 == induced_subgraph.end())//no index in induced_subgraph for query_node1
		{
			return results;
		}


		//if (k <= 1)
		//{

		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//			return results;
		//		}
		//	}
		//
		//}
		//else 
		if (k <= SHORT_CUT_DIS)//distance no larger than 3
		{
			//// cout << "k<=5";
			if (k > 3)
			{
				if (iter2->second.find(query_node2) != iter2->second.end())
				{
					vector<NODE_TYPE> temp_path;
					temp_path.push_back(query_node1);
					temp_path.push_back(query_node2);
					results.push_back(temp_path);
				}
				meet_nodes.clear();
				find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				paths temp = find_all_k_pahts_dfs_with_provided_subgraph(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
				//paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
				//temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}
			bool node_in_index = false;
			paths temp = short_index.find_short_cuts_using_index(query_node1, query_node2, k, node_in_index);
			if (node_in_index)
			{
				//temp.drop_path_with_stop_nodes(stop_nodes);// need to add a end distance

				//temp.drop_path_with_stop_nodes_with_distance_range(stop_nodes,LEFT_SHORT_DIS,RIGHT_SHORT_DIS);// need to add a end distance
				results.add_paths(temp);
				return results;
			}
			else
			{
				paths temp = find_k_path_using_index_with_stop_nodes_and_start_dis(induced_subgraph, node_num, k, query_node2, query_node1, stop_nodes, 3);//3 should be with stop nodes
																																						   //temp.drop_repeat_paths_with_sort();
				results.add_paths(temp);
				//short_index.push_back(query_node1, query_node2, k, results);
				return results;
			}

		}
		//else if (k <= 3)//SHORT_CUT_DIS)
		//{
		//	meet_nodes.clear();
		//	for (auto iter_one_hop = iter2->second.begin(); iter_one_hop != iter2->second.end(); iter_one_hop++)
		//	{
		//		if (*iter_one_hop == query_node2)
		//		{// has the one hop path
		//			vector<NODE_TYPE> temp_path;
		//			temp_path.push_back(query_node1);
		//			temp_path.push_back(query_node2);
		//			results.push_back(temp_path);
		//		}
		//	}
		//	find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2);
		//	if (meet_nodes.empty())
		//	{
		//		return results;
		//	}
		//	for (auto iter_mn = meet_nodes.begin(); iter_mn != meet_nodes.end(); iter_mn++)
		//	{
		//		NODE_TYPE query = *iter_mn;

		//		//left and right with k/2 is wrong, should be with k-l[i] and r[i]
		//		paths left_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - k / 2, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2
		//																																															//// cout << "after left";

		//		if (left_paths.path.empty())
		//		{
		//			continue;
		//		}
		//		paths right_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - k / 2, stop_nodes, level + 1, short_index);//left spd is (iter_rec->second+1)/2
		//																																															 //// cout << "after right";

		//		if (right_paths.path.empty())
		//		{
		//			continue;
		//		}
		//		paths temp_total_paths;
		//		left_paths.join_drop_longpaths_and_repeat_nodes(right_paths, k, temp_total_paths);
		//		results.add_paths(temp_total_paths);
		//	}
		//	return results;
		//}
		else
		{//recursive function
			if (iter2->second.find(query_node2) != iter2->second.end())
			{
				vector<NODE_TYPE> temp_path;
				temp_path.push_back(query_node1);
				temp_path.push_back(query_node2);
				results.push_back(temp_path);
			}



			//paths short_paths = find_k_paths_recursive_bfs_middle_nodes(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query_node2, SHORT_CUT_DIS, stop_nodes, level + 1, short_index);//right spd is iter_rec->second/2

			//short_paths.drop_repeat_paths_with_sort();
			//results.add_paths(short_paths);

			set<NODE_TYPE> stop_nodes_new(stop_nodes);
			map<NODE_TYPE, DISTANCE_TYPE> small_sp_pairs;
			map<NODE_TYPE, DISTANCE_TYPE> temp_middle_nodes = find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes_0_0(induced_subgraph, induced_subgraph_reverse, node_num, k, query_node1, query_node2, stop_nodes_new, small_sp_pairs, 0);// SHORT_CUT_DIS / 2 * 2);//5
			multimap<DISTANCE_TYPE, NODE_TYPE> temp_middle_v;
			for (auto iter_map = temp_middle_nodes.begin(); iter_map != temp_middle_nodes.end(); iter_map++)
			{
				temp_middle_v.insert(pair<DISTANCE_TYPE, NODE_TYPE>(iter_map->second, iter_map->first));
			}
			//vector< pair<NODE_TYPE, DISTANCE_TYPE>> temp_middle_v(temp_middle_nodes.begin(), temp_middle_nodes.end());
			//std::sort(temp_middle_v.begin(), temp_middle_v.end(), cmp_by_value_decrease);
			for (auto iter_rec = temp_middle_v.rbegin(); iter_rec != temp_middle_v.rend(); iter_rec++)
				//for(int i = 0; i <= temp_middle_v.size(); i++)
			{

				NODE_TYPE query = iter_rec->second;
				DISTANCE_TYPE dis = iter_rec->first;
				if (level == 1 && query == 259805)
				{
					//// cout << "query and dis " << query << " : " << dis << endl;
				}
				//// cout << "before left";
				set<NODE_TYPE> stop_nodes_empty;
				paths left_paths = find_k_paths_recursive_bfs_middle_nodes_only_bfs(induced_subgraph, induced_subgraph_reverse, node_num, query_node1, query, k - dis / 2, stop_nodes_new, level + 1, short_index);//right spd is iter_rec->second/2

																																																		  //// cout << "after left";
																																																		  //	left_paths.drop_path_length_less_than_k(3); //(SHORT_CUT_DIS + 1) / 2 - 1);
				if (left_paths.path.empty())
				{
					//// cout << "left empty";
					stop_nodes_new.insert(query);
					continue;
				}
				//// cout << "before right";
				//// cout << query << " : " << query_node2 << " : " << k - (dis + 1) / 2 << endl;
				paths right_paths = find_k_paths_recursive_bfs_middle_nodes_only_bfs(induced_subgraph, induced_subgraph_reverse, node_num, query, query_node2, k - (dis + 1) / 2, stop_nodes_new, level + 1, short_index);//left spd is (iter_rec->second+1)/2
																																																				 //// cout << "after right";
																																																				 //right_paths.drop_path_length_less_than_k(3);// SHORT_CUT_DIS / 2 - 1);
				if (right_paths.path.empty())
				{
					//// cout << "right empty";
					stop_nodes_new.insert(query);
					continue;
				}
				paths temp_total_paths;
				clock_t start, end;
				start = clock();
				//// cout << "before join";
				//left_paths.drop_repeat_paths_with_sort();
				//right_paths.drop_repeat_paths_with_sort();
				if (true)//level == 1)
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);

																																														  //left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(right_paths, k, temp_total_paths, 0, stop_nodes_new, 1, 1);// 3, 3);
				}
				else
				{
					left_paths.join_drop_longpaths_and_repeat_nodes_and_short_paths(right_paths, k, temp_total_paths, 0);
				}
				//// cout << "after join";

				//temp_total_paths.drop_path_with_repeat_node();
				end = clock();

				//temp_total_paths.drop_path_length_more_than_k_drop_repeated_node(k);
				//temp_total_paths.sort_by_string_order();
				//temp_total_paths.drop_repeat_path();
				temp_total_paths.drop_repeat_paths_with_sort();
				results.add_paths(temp_total_paths);
				if (level > 1) {
					//results.drop_repeat_paths_with_sort();
				}
				if (DEBUG_ALL_QUERYS)
				{
					// cout << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << k << endl;
					// cout << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;
					ofstream temp;
					temp.open("allQuerys", ios::app);
					if (!temp)
					{
						// cout << "allQuerys can't open" << endl;
						abort();
					}
					temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
					temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

					temp.close();
				}
				stop_nodes_new.insert(query);
			}
		}
		return results;
	}



	//only one bfs
	void construct_pruned_dag_min_subgraph_single_direction(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		dag_min_induced_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
																				//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k;// k - (k / 2);;
		int right_max_distance = k;// (k / 2);
		cur_distance = 0;
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						set<NODE_TYPE> temp_set2;
						temp_set2.insert(*iter2);
						//if (dag_min_induced_subgraph_reverse.find(*iter2) == dag_min_induced_subgraph_reverse.end())// not in index subgraph
						//{
						//	dag_min_induced_subgraph_reverse.insert(std::make_pair(cur_node, temp_set2));
						//}
						//else {
						//	dag_min_induced_subgraph_reverse.find(cur_node)->second.insert(*iter2);
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				break;//left_skip = true;
			}
		}
		//after left bfs, then we do a right bfs to get dag minimum induced subgraph
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_temp;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse_temp;
	}


	paths find_all_k_pahts_dfs_using_dfs_k_nolargerthan_5(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		auto iter_temp = induced_subgraph.find(query_node1);
		if (iter_temp != induced_subgraph.end())
		{
			for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			{
				if (*iter == query_node2)//there is one hop path
				{
					vector<NODE_TYPE> one_hop_path;
					one_hop_path.push_back(query_node1);
					one_hop_path.push_back(query_node2);
					result.push_back(one_hop_path);
					break;
				}
			}
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(induced_subgraph, query_node1, query_node2, k - (k / 2), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(induced_subgraph_reverse, query_node2, query_node1, (k / 2), node_num);
		long long temp_num = 0;
		long long temp_num2 = 0;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			temp_num = temp_num + left_iter->second.path.size() + right_iter->second.path.size();
			//// cout << "left : " << left_iter->second.path.size();
			//// cout << " : right : " << right_iter->second.path.size() << endl;
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		//// cout << temp_num << " paths in total" << endl;
		//// cout << result.path.size() << " results in total" << endl;
		return result;
	}

	void find_all_meet_nodes_in_induced_subgraph_using_dfs_k_nolargerthan_5(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse,NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		//join_left_right_index_into_left();// we store the left induced subgraph into right one with reverse order
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);

		//set<NODE_TYPE> visited_nodes;
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							if(left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
								continue;
							}
							left_visited_nodes.insert(*iter2);
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if(*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
								continue;
							}
							left_visited_nodes.insert(*iter2);
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
								continue;
							}
							right_visited_nodes.insert(*iter2);
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if(*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
								continue;
							}
							right_visited_nodes.insert(*iter2);
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}
	void find_all_meet_nodes_in_induced_subgraph_with_provided_induced_sbugraph(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse,NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}









	void find_all_meet_nodes_in_induced_subgraph( NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		join_left_right_index_into_left();// we store the left induced subgraph into right one with reverse order
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;
																		   //right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		auto dst_iter = reverse_adjacency_in_subgraph.find(src);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else 
			{
//				if (adjacency_list[*cur_iters[cur_distance - 1]].empty() || cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else 
				{
//					if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				if (iter_result == result.end())//first time to insert
				{
					paths temp_paths;
					temp_paths.push_back(temp_result_path);
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.push_back(temp_result_path);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
			//cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		auto dst_iter = reverse_adjacency_in_subgraph.find(src);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				//				if (adjacency_list[*cur_iters[cur_distance - 1]].empty() || cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					//					if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
			//cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}

	//use with find_all_k_pahts_dfs_from_middle
	void find_all_meet_nodes_longspd_in_induced_subgraph(NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		join_left_right_index_into_left();// we store the left induced subgraph into right one with reverse order
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								auto iter_spd_pairs = meet_spd_pairs.find(*iter2);
								if (iter_spd_pairs == meet_spd_pairs.end())//first time to insert
								{
									meet_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2 - 1));
								}
								else
								{
									iter_spd_pairs->second = cur_distance * 2 - 1;
								}
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								if (right_proprogate.find(*iter2) != right_proprogate.end())
								{
									auto iter_spd_pairs = meet_spd_pairs.find(*iter2);
									if (iter_spd_pairs == meet_spd_pairs.end())//first time to insert
									{
										meet_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2-1));
									}
									else
									{
										iter_spd_pairs->second = cur_distance * 2-1;
									}
								}
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = meet_spd_pairs.find(*iter2);
								if (iter_spd_pairs == meet_spd_pairs.end())//first time to insert
								{
									meet_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2 ));
								}
								else
								{
									iter_spd_pairs->second = cur_distance * 2;
								}
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = meet_spd_pairs.find(*iter2);
								if (iter_spd_pairs == meet_spd_pairs.end())//first time to insert
								{
									meet_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2));
								}
								else
								{
									iter_spd_pairs->second = cur_distance * 2;
								}
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	map<NODE_TYPE, DISTANCE_TYPE> find_all_middle_nodes_in_induced_subgraph(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		map<NODE_TYPE, DISTANCE_TYPE> results;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								auto iter_spd_pairs = results.find(*iter2);
								if (iter_spd_pairs == results.end())//first time to insert
								{
									results.insert(std::make_pair(*iter2, cur_distance * 2 - 1));
								}
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								if (right_proprogate.find(*iter2) != right_proprogate.end())
								{
									auto iter_spd_pairs = results.find(*iter2);
									if (iter_spd_pairs == results.end())//first time to insert
									{
										results.insert(std::make_pair(*iter2, cur_distance * 2 - 1));
									}
								}
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = results.find(*iter2);
								if (iter_spd_pairs == results.end())//first time to insert
								{
									results.insert(std::make_pair(*iter2, cur_distance * 2));
								}
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = results.find(*iter2);
								if (iter_spd_pairs == results.end())//first time to insert
								{
									results.insert(std::make_pair(*iter2, cur_distance * 2));
								}
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}
		return results;
	}





	map<NODE_TYPE, DISTANCE_TYPE> find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes_0_0(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, set<NODE_TYPE> &stop_nodes, map<NODE_TYPE, DISTANCE_TYPE>& middle_spd_pairs, DISTANCE_TYPE stop_start_dis)
	{
		//map<NODE_TYPE, DISTANCE_TYPE> src_spd;
		//map<NODE_TYPE, DISTANCE_TYPE> dst_spd;

		//map<NODE_TYPE, DISTANCE_TYPE> middle_spd_pairs;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		//set<NODE_TYPE> visited_nodes;
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance ==1) {//for k==1, we do not store the middle nodes, and process them using dfs to find short paths
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{
						
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							//if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							//{
							//	continue;
							//}
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
								continue;
							}

							//left_visited_nodes.insert(*iter2);


							left_visited_nodes.insert(*iter2);
	
							/*if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2-1 ));
								}

							}*/
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{

						cur_node = *iter;
						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)continue;;
							if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							{
								continue;
							}
							if(left_visited_nodes.find(*iter2) != left_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 - 1 > stop_start_dis &&left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
								if(right_proprogate.find(*iter2) != right_proprogate.end())
								{
									DISTANCE_TYPE temp_dis = cur_distance*2-1;
									auto iter_src_spd = src_spd.find(*iter2);
									if(iter_src_spd != src_spd.end())
									{
										temp_dis = iter_src_spd->second*2-1;
									}
									auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
									if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
									{
										middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
									}
								}
								continue;
							}*/
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								//if (cur_distance*2-1 > SHORT_CUT_DIS && right_proprogate.find(*iter2) != right_proprogate.end() )
								
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2-1));
								}
									//else
									//{
									//	iter_spd_pairs->second = cur_distance * 2 - 1;
									//}
							}
							left_visited_nodes.insert(*iter2);
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {// we do not store middle nodes for k <= 2
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							//if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							//{
							//	continue;
							//}
							if (*iter2 == query_node1)
							{
								continue;
							}
							if(right_visited_nodes.find(*iter2) != right_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 > stop_start_dis  && right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
								if (left_proprogate.find(*iter2) != left_proprogate.end())
								{
									DISTANCE_TYPE temp_dis = cur_distance * 2;
									auto iter_dst_spd = dst_spd.find(*iter2);
									if (iter_dst_spd != dst_spd.end())
									{
										temp_dis = iter_dst_spd->second*2;
									}
									auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
									if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
									{
										middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
									}
								}
								continue;
							}*/
							//right_visited_nodes.insert(*iter2);
							if (left_proprogate.find(*iter2) != left_proprogate.end())
								//if(left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2));
								}
							}

							right_visited_nodes.insert(*iter2);
	
							//cur_distance*2 > SHORT_CUT_DIS && 
							//if (left_proprogate.find(*iter2) != left_proprogate.end())
							//{
							//	auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							//	if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							//	{
							//		middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2));
							//	}
							//}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)continue;
							if(right_visited_nodes.find(*iter2) != right_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 > stop_start_dis  && right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
								if (left_proprogate.find(*iter2) != left_proprogate.end())
								{
									DISTANCE_TYPE temp_dis = cur_distance * 2;
									auto iter_dst_spd = dst_spd.find(*iter2);
									if (iter_dst_spd != dst_spd.end())
									{
										temp_dis = iter_dst_spd->second*2;
									}
									auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
									if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
									{
										middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
									}
								}
								continue;
							}*/
							if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							{
								continue;
							}
							
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							//if(left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2));
								}
							}

							right_visited_nodes.insert(*iter2);
								//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}
		//for(auto iter_pair = middle_spd_pairs.begin(); iter_pair != middle_spd_pairs.end(); iter_pair++)
		//{
		//	// cout << iter_pair->first << " : " << iter_pair->second << endl;
		//}
		return middle_spd_pairs;
	}


	map<NODE_TYPE, DISTANCE_TYPE> find_all_meet_nodes_longspd_in_induced_subgraph_with_stop_nodes(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, set<NODE_TYPE> &stop_nodes, map<NODE_TYPE, DISTANCE_TYPE>& middle_spd_pairs, DISTANCE_TYPE stop_start_dis)
	{// to solve the spd problem
		//map<NODE_TYPE, DISTANCE_TYPE> src_spd;
		//map<NODE_TYPE, DISTANCE_TYPE> dst_spd;

		//map<NODE_TYPE, DISTANCE_TYPE> middle_spd_pairs;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		//set<NODE_TYPE> visited_nodes;
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {//for k==1, we do not store the middle nodes, and process them using dfs to find short paths
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							//if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							//{
							//	continue;
							//}
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
								continue;
							}

							//left_visited_nodes.insert(*iter2);

							if(cur_distance * 2 -1 > stop_start_dis)
							{
								left_visited_nodes.insert(*iter2);
							}

							/*if (cur_distance * 2 - 1> stop_start_dis)
							{
								if (src_spd.find(*iter2) != src_spd.end())
								{
									src_spd.insert(std::make_pair(*iter2, cur_distance));
									left_visited_nodes.insert(*iter2);
								}
							}*/



							/*if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
							auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							{
							middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2-1 ));
							}

							}*/
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{

						cur_node = *iter;
						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph.find(cur_node);
						if (iter_map == induced_subgraph.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)continue;;
							if (cur_distance * 2 - 1 > stop_start_dis && stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							{
								continue;
							}
							if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 - 1 > stop_start_dis &&left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
							{
							if(right_proprogate.find(*iter2) != right_proprogate.end())
							{
							DISTANCE_TYPE temp_dis = cur_distance*2-1;
							auto iter_src_spd = src_spd.find(*iter2);
							if(iter_src_spd != src_spd.end())
							{
							temp_dis = iter_src_spd->second*2-1;
							}
							auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							{
							middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
							}
							}
							continue;
							}*/
							if (cur_distance * 2 - 1 > stop_start_dis+1 &&right_proprogate.find(*iter2) != right_proprogate.end())
							{
								//if (cur_distance*2-1 > SHORT_CUT_DIS && right_proprogate.find(*iter2) != right_proprogate.end() )

								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2 - 1));
								}
								//else
								//{
								//	iter_spd_pairs->second = cur_distance * 2 - 1;
								//}
							}
							if(cur_distance*2 -1 >stop_start_dis)
							{
								left_visited_nodes.insert(*iter2);
							}
							/*if (cur_distance * 2 - 1 > stop_start_dis) {
								if (src_spd.find(*iter2) != src_spd.end())
								{
									src_spd.insert(std::make_pair(*iter2, cur_distance));
									left_visited_nodes.insert(*iter2);
								}
							}*/
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {// we do not store middle nodes for k <= 2
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							//if (stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							//{
							//	continue;
							//}
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 > stop_start_dis  && right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
							DISTANCE_TYPE temp_dis = cur_distance * 2;
							auto iter_dst_spd = dst_spd.find(*iter2);
							if (iter_dst_spd != dst_spd.end())
							{
							temp_dis = iter_dst_spd->second*2;
							}
							auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							{
							middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
							}
							}
							continue;
							}*/
							//right_visited_nodes.insert(*iter2);
							if (cur_distance * 2 > stop_start_dis+1  && left_proprogate.find(*iter2) != left_proprogate.end())
								//if(left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{
									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2));
								}
							}
							if(cur_distance * 2 > stop_start_dis)
							{
								right_visited_nodes.insert(*iter2);
							}
							/*if (cur_distance * 2 > stop_start_dis)
							{
								if (dst_spd.find(*iter2) != dst_spd.end())
								{
									dst_spd.insert(std::make_pair(*iter2, cur_distance));
									right_visited_nodes.insert(*iter2);
								}
							}*/
							//cur_distance*2 > SHORT_CUT_DIS && 
							//if (left_proprogate.find(*iter2) != left_proprogate.end())
							//{
							//	auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							//	if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							//	{
							//		middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance*2));
							//	}
							//}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = induced_subgraph_reverse.find(cur_node);
						if (iter_map == induced_subgraph_reverse.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)continue;
							if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())
							{
								continue;
							}
							/*if (cur_distance * 2 > stop_start_dis  && right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
							{
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
							DISTANCE_TYPE temp_dis = cur_distance * 2;
							auto iter_dst_spd = dst_spd.find(*iter2);
							if (iter_dst_spd != dst_spd.end())
							{
							temp_dis = iter_dst_spd->second*2;
							}
							auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
							if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
							{
							middle_spd_pairs.insert(std::make_pair(*iter2, temp_dis));
							}
							}
							continue;
							}*/
							if (cur_distance * 2 > stop_start_dis  &&stop_nodes.find(*iter2) != stop_nodes.end())//avoid stop nodes
							{
								continue;
							}

							if (cur_distance * 2 > stop_start_dis+1  && left_proprogate.find(*iter2) != left_proprogate.end())
								//if(left_proprogate.find(*iter2) != left_proprogate.end())
							{
								auto iter_spd_pairs = middle_spd_pairs.find(*iter2);
								if (iter_spd_pairs == middle_spd_pairs.end())//first time to insert
								{

									middle_spd_pairs.insert(std::make_pair(*iter2, cur_distance * 2));
								}
							}
							if(cur_distance*2 > stop_start_dis)
							{
								right_visited_nodes.insert(*iter2);
							}
							/*if (cur_distance * 2 > stop_start_dis)
							{
								if (dst_spd.find(*iter2) != dst_spd.end())
								{
									dst_spd.insert(std::make_pair(*iter2, cur_distance));
									right_visited_nodes.insert(*iter2);
								}
							}*/
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}
		//for(auto iter_pair = middle_spd_pairs.begin(); iter_pair != middle_spd_pairs.end(); iter_pair++)
		//{
		//	// cout << iter_pair->first << " : " << iter_pair->second << endl;
		//}
		return middle_spd_pairs;
	}




	paths find_all_k_pahts_dfs_from_middle(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//memory efficient method
		reverse_adjacency_in_subgraph_left.clear();
		join_left_right_index_into_left();//both left and right have index(index and index reverse)
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		auto iter_temp = reverse_adjacency_in_subgraph_right.find(query_node1);
		if (iter_temp != reverse_adjacency_in_subgraph_right.end())
		{
			for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			{
				if (*iter == query_node2)//there is one hop path
				{
					vector<NODE_TYPE> one_hop_path;
					one_hop_path.push_back(query_node1);
					one_hop_path.push_back(query_node2);
					result.push_back(one_hop_path);
					break;
				}
			}
		}
		//map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(reverse_adjacency_in_subgraph_right, query_node1, query_node2, k - (k / 2), node_num);
		//map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(reverse_adjacency_in_subgraph_left, query_node2, query_node1, (k / 2), node_num);

		for (auto iter = meet_spd_pairs.begin(); iter != meet_spd_pairs.end(); iter++)
		//for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			//// cout << iter->first << " \t " << iter->second << endl;
			
			construct_pruned_dag_min_subgraph_for_middle_node(reverse_adjacency_in_subgraph_left, node_num, iter->second - (iter->second / 2), query_node1, iter->first, src_distance);
			//reverse_induced_subgraph(induced_subgraph_middle_node);
			paths left_temp = find_k_path_using_index(induced_subgraph_middle_node, node_num, iter->second - (iter->second / 2), iter->first, query_node1);
			construct_pruned_dag_min_subgraph_for_middle_node(reverse_adjacency_in_subgraph_right, node_num, iter->second/2, query_node2, iter->first, dst_distance);
			//reverse_induced_subgraph(induced_subgraph_middle_node);
			paths right_temp = find_k_path_using_index_reverse(induced_subgraph_middle_node, node_num, iter->second / 2,iter->first,query_node2);
					
			/*construct_pruned_dag_min_subgraph_for_middle_node(reverse_adjacency_in_subgraph_left, node_num, k - (k / 2), query_node1, *iter, src_distance);
			paths left_temp = find_k_path_using_index(induced_subgraph_middle_node, node_num, k - (k / 2), *iter, query_node1);
			construct_pruned_dag_min_subgraph_for_middle_node(reverse_adjacency_in_subgraph_right, node_num, k / 2, query_node2, *iter, dst_distance);
			paths right_temp = find_k_path_using_index_reverse(induced_subgraph_middle_node, node_num, k / 2, *iter, query_node2);
			*/
			
			
			//auto left_iter = left_map_paths.find(*iter);
			//auto right_iter = right_map_paths.find(*iter);
			if (left_temp.path.empty())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_temp.path.empty())
			{
				continue;
			}
			left_temp.sort_by_string_order();
			right_temp.sort_by_string_order();
			paths total_paths;
			left_temp.drop_repeat_path();
			right_temp.drop_repeat_path();
			total_paths = left_temp.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_temp, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}


	void construct_pruned_dag_min_subgraph_for_middle_node(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE middle_node, unordered_map<NODE_TYPE, DISTANCE_TYPE>& node1_distance)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		induced_subgraph_middle_node.clear();
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> right_visited_nodes;

																				//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		right_visited_nodes.insert(middle_node);
		right_proprogate.insert(middle_node);
		cur_distance = 0;
		int right_max_distance = k;

		//left_visited_nodes.clear();
		//right_visited_nodes.clear();
		//left_proprogate.clear();
		//right_proprogate.clear();
		//left_visited_nodes.insert(query_node1);
		//right_visited_nodes.insert(query_node2);
		//left_proprogate.insert(query_node1);
		//right_proprogate.insert(query_node2);

		while (true)
		{
			cur_distance++;
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					//for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					if (reverse_adjacency_in_subgraph.find(cur_node) == reverse_adjacency_in_subgraph.end())
					{
						continue;
					}//no edge for this curnode
					auto iter_temp = reverse_adjacency_in_subgraph.find(cur_node);
					for (auto iter2 = iter_temp->second.begin(); iter2 != iter_temp->second.end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (node1_distance.find(*iter2)->second + cur_distance <= k)
							{
								if (induced_subgraph_middle_node.find(*iter2) == induced_subgraph_middle_node.end())// not in index subgraph
								{
									induced_subgraph_middle_node.insert(std::make_pair(*iter2, temp_set));
								}
								else {
									induced_subgraph_middle_node.find(*iter2)->second.insert(cur_node);
								}
							}
							//induced_subgraph_middle_node.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if (node1_distance.find(*iter2)->second + cur_distance <= k)
						{
							induced_subgraph_middle_node.insert(std::make_pair(*iter2, temp_set));
						}
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				break;//right_skip = true;
			}
			//if (left_skip && right_skip)
			//{
			//	break;
			//}
		}
		//dag_min_induced_subgraph = reverse_adjacency_in_subgraph_right;
	}





	paths find_all_k_pahts_dfs_with_provided_subgraph(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse, NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		if(meet_nodes.empty())
		{
			return result;
		}
		auto iter_temp = induced_subgraph.find(query_node1);
		if (iter_temp != induced_subgraph.end())
		{
			if(iter_temp->second.find(query_node2) != iter_temp->second.end())
			{
				vector<NODE_TYPE> one_hop_path;
				one_hop_path.push_back(query_node1);
				one_hop_path.push_back(query_node2);
				result.push_back(one_hop_path);
			}
			//for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			//{
			//	if (*iter == query_node2)//there is one hop path
			//	{
			//		vector<NODE_TYPE> one_hop_path;
			//		one_hop_path.push_back(query_node1);
			//		one_hop_path.push_back(query_node2);
			//		result.push_back(one_hop_path);
			//		break;
			//	}
			//}
		}
		else
		{
			return result;
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(induced_subgraph, query_node1, query_node2, k - (k / 2), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(induced_subgraph_reverse, query_node2, query_node1, (k / 2), node_num);
		long long temp_num = 0;
		long long temp_num2 = 0;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			temp_num = temp_num + left_iter->second.path.size() + right_iter->second.path.size();
			//// cout << "left : " << left_iter->second.path.size();
			//// cout << " : right : " << right_iter->second.path.size() << endl;
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}



	paths find_all_k_pahts_dfs(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		auto iter_temp = reverse_adjacency_in_subgraph_right.find(query_node1);
		if (iter_temp != reverse_adjacency_in_subgraph_right.end())
		{
			for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			{
				if (*iter == query_node2)//there is one hop path
				{
					vector<NODE_TYPE> one_hop_path;
					one_hop_path.push_back(query_node1);
					one_hop_path.push_back(query_node2);
					result.push_back(one_hop_path);
					break;
				}
			}
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(reverse_adjacency_in_subgraph_right, query_node1, query_node2, k - (k / 2), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(reverse_adjacency_in_subgraph_left, query_node2, query_node1, (k / 2), node_num);
		long long temp_num = 0;
		long long temp_num2 = 0;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			temp_num = temp_num + left_iter->second.path.size() + right_iter->second.path.size();
			//// cout << "left : " << left_iter->second.path.size();
			//// cout << " : right : " << right_iter->second.path.size() << endl;
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		// cout << temp_num << " paths in total" << endl;
		// cout << result.path.size() << " results in total" << endl;
		return result;
	}



	paths find_all_k_pahts_dfs_write_number_left_right_path(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2, string algorithm, string dataset, NODE_TYPE edgeId)
	{
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		auto iter_temp = reverse_adjacency_in_subgraph_right.find(query_node1);
		if (iter_temp != reverse_adjacency_in_subgraph_right.end())
		{
			for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			{
				if (*iter == query_node2)//there is one hop path
				{
					vector<NODE_TYPE> one_hop_path;
					one_hop_path.push_back(query_node1);
					one_hop_path.push_back(query_node2);
					result.push_back(one_hop_path);
					break;
				}
			}
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(reverse_adjacency_in_subgraph_right, query_node1, query_node2, k - (k / 2), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(reverse_adjacency_in_subgraph_left, query_node2, query_node1, (k / 2), node_num);
		long long temp_num = 0;
		long long temp_num2 = 0;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			temp_num = temp_num + left_iter->second.path.size() + right_iter->second.path.size();
			//// cout << "left : " << left_iter->second.path.size();
			//// cout << " : right : " << right_iter->second.path.size() << endl;
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		if (result.path.size() != 0)
		{
			ofstream temp;
			temp.open(algorithm + "_" + dataset + "_" + to_string(k) + "_pathnums", ios::app);
			if (!temp)
			{
				// cout << "path nums file can't open" << endl;
				abort();
			}
			temp << edgeId << "\t" << query_node1 << "\t" << query_node2 << "\t" << temp_num << "\t" << result.path.size() << endl;
			//temp << double(end - start) / CLOCKS_PER_SEC << " for join " << query_node1 << " : " << query << " : " << query_node2 << " : " << dis << " : " << k << endl;
			//temp << left_paths.path.size() << " : " << right_paths.path.size() << " : " << temp_total_paths.path.size() << endl;

			temp.close();
		}
		//// cout << temp_num << " paths in total" << endl;
		//// cout << result.path.size() << " results in total" << endl;
		return result;
	}



	void construct_pruned_subgraph(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k -(k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{
					
						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}
						
						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	void construct_pruned_subgraph_k_nolargerthan3(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		set<NODE_TYPE> visited_nodes;

		//set<NODE_TYPE> left_visited_nodes;
		//set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		visited_nodes.insert(query_node1);
		visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = ceil(k*1.0 / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (visited_nodes.find(*iter2) != visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= k / 2 && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (visited_nodes.find(*iter2) != visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	void construct_pruned_subgraph_equal_double_degrees(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, vector<int> in_degrees, vector<int> out_degrees, bool& left)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}
		long long left_nodes = 1;// out_degrees[query_node1];
		long long right_nodes = 1;// in_degrees[query_node2];
		
		double total_left_nodes = 1;// out_degrees[query_node1];
		double total_right_nodes = 1;// in_degrees[query_node2];

		while (cur_distance < k)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (left_nodes <= right_nodes)
			{
				left_nodes = 0;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
						left_nodes += out_degrees[*iter2];
					}
				}
				left_proprogate = temp_proprogate;
				total_left_nodes += left_nodes;
			}
			else//right propogate
			{
				right_nodes = 0;
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
						right_nodes = right_nodes + in_degrees[*iter2];
						//right_nodes++;
					}
				}
				right_proprogate = temp_proprogate;
				total_right_nodes += right_nodes;
			}
		}
		if (total_right_nodes < total_left_nodes)
		{
			left = false;
		}
		else
		{
			left = true;
		}
	}

	void construct_pruned_subgraph_with_meetnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		//left_visited_nodes.insert(query_node1);
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					if (left_visited_nodes.find(*iter) != left_visited_nodes.end())//already visited
					{
						continue;
					}
					left_visited_nodes.insert(*iter);

					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}

						if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						}
						if (right_proprogate.find(*iter2) != right_proprogate.end())// || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					if (right_visited_nodes.find(*iter) != right_visited_nodes.end())//already visited
					{
						continue;
					}
					right_visited_nodes.insert(*iter);
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						}
						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2)!= left_visited_nodes.end() )
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}



	paths find_all_k_pahts(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			paths left_paths, right_paths, total_paths;
			left_paths = find_k_path_using_index_reverse(reverse_adjacency_in_subgraph_left, node_num, k - (k / 2), query_node1, *iter);
			right_paths = find_k_path_using_index(reverse_adjacency_in_subgraph_right, node_num, k / 2, query_node2, *iter);
			total_paths = left_paths.join(right_paths);
			total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}




	paths find_k_path_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph,NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty()||cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if ( cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if(reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{ 
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	paths find_k_path_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph,NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//find all the path from query_node2 to query_node1, the reverser_adjacency is from query_node2 to node1
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


	paths find_k_path_using_index_with_stop_nodes(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, set<NODE_TYPE>& stop_nodes)
	{//find all the path from query_node2 to query_node1, the reverser_adjacency is from query_node2 to node1
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end() || stop_nodes.find(*cur_iters[cur_distance]) != stop_nodes.end())//*cur_iters[cur_distance] < src || 
			{//repeat nodes or node in stop_nodes
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}

	paths find_k_path_using_index_reverse_with_stop_nodes(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, set<NODE_TYPE>& stop_nodes)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end() || stop_nodes.find(*cur_iters[cur_distance]) != stop_nodes.end())//*cur_iters[cur_distance] < src || 
			{//node already in current path or in stop_nodes
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}



	paths find_k_path_using_index_with_stop_nodes_and_start_dis(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, set<NODE_TYPE>& stop_nodes, DISTANCE_TYPE startDis)
	{//find all the path from query_node2 to query_node1, the reverser_adjacency is from query_node2 to node1
		paths result;
		if(query_node1 == query_node2)
		{
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end() || (cur_distance > startDis && stop_nodes.find(*cur_iters[cur_distance]) != stop_nodes.end()) )//*cur_iters[cur_distance] < src || 
			{//repeat nodes or node in stop_nodes
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}





};



class pruned_subgraph_unordered_map_double_direction_only_meet_nodes_half_k
{
public:
	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_total;
	//bool one_hop_path = false;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph;
	set<NODE_TYPE> meet_nodes;

	//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse;

	unordered_map<NODE_TYPE, DISTANCE_TYPE> src_distance;
	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_distance;

	void construct_pruned_subgraph_with_meetnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//meet_nodes.insert(query_node2);
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

																						   //left_visited_nodes.insert(query_node1);
																						   //right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					if (left_visited_nodes.find(*iter) != left_visited_nodes.end())//already visited
					{
						continue;
					}
					left_visited_nodes.insert(*iter);

					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}


						if (right_proprogate.find(*iter2) != right_proprogate.end())// || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					if (right_visited_nodes.find(*iter) != right_visited_nodes.end())//already visited
					{
						continue;
					}
					right_visited_nodes.insert(*iter);
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2) != left_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}

	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints(vector<NODE_TYPE >* adjacency_list, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			//// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< vector<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		if (adjacency_list[src].size() == 0)//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(adjacency_list[src].begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == adjacency_list[src].end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else if (adjacency_list[*cur_iters[cur_distance - 1]].empty() ||cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
			{
				// go back
				cur_distance--;
				cur_iters[cur_distance]++;
				cur_node = c_path.top();
				c_path.pop();
				c_path_v.pop_back();
				auto iter2 = c_path_set.find(cur_node);
				if (iter2 != c_path_set.end())
				{
					c_path_set.erase(iter2);
				}
				continue;
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == adjacency_list[src].end())
					{
						force_continue = true;
						break;
					}
				}
				else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
				{
					force_continue = true;
					break;
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end() )//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				if (iter_result == result.end())//first time to insert
				{
					paths temp_paths;
					temp_paths.push_back(temp_result_path);
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.push_back(temp_result_path);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;
			cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints_reverse(vector<NODE_TYPE >* adjacency_list, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			//// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< vector<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		if (adjacency_list[src].size() == 0)//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(adjacency_list[src].begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == adjacency_list[src].end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
			{
				// go back
				cur_distance--;
				cur_iters[cur_distance]++;
				cur_node = c_path.top();
				c_path.pop();
				c_path_v.pop_back();
				auto iter2 = c_path_set.find(cur_node);
				if (iter2 != c_path_set.end())
				{
					c_path_set.erase(iter2);
				}
				continue;
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == adjacency_list[src].end())
					{
						force_continue = true;
						break;
					}
				}
				else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
				{
					force_continue = true;
					break;
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;
			cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}



	paths find_all_k_pahts_dfs(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		{
			if (*iter == query_node2)//there is one hop path
			{
				vector<NODE_TYPE> one_hop_path;
				one_hop_path.push_back(query_node1);
				one_hop_path.push_back(query_node2);
				result.push_back(one_hop_path);
				break;
			}
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(adjacency_list, query_node1, query_node2, k - (k / 2 ), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(adjacency_list_reverse, query_node2, query_node1, (k / 2), node_num);

		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second,k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}


	paths find_k_path_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	paths find_k_path_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


};

class pruned_subgraph_simpath
{// not finish yet
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph;
	set<NODE_TYPE> meet_nodes;//exactly the middle nodes for all paths

							  //unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse;

	unordered_map<NODE_TYPE, DISTANCE_TYPE> src_distance;
	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_distance;

	void join_left_right_index_into_left()
	{
		for (auto iter = reverse_adjacency_in_subgraph_right.begin(); iter != reverse_adjacency_in_subgraph_right.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void join_right_left_into_right()
	{
		for (auto iter = reverse_adjacency_in_subgraph_left.begin(); iter != reverse_adjacency_in_subgraph_left.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void construct_pruned_dag_min_subgraph(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		dag_min_induced_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
																				//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k;// k - (k / 2);;
		int right_max_distance = k;// (k / 2);
		cur_distance = 0;
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{
						auto iter_src_dis = src_distance.find(*iter2);
						if (iter_src_dis != src_distance.end())//no distance information for node *iter2
						{
							src_distance.insert(std::make_pair(*iter2, cur_distance));
						}
						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						set<NODE_TYPE> temp_set2;
						temp_set2.insert(*iter2);
						//if (dag_min_induced_subgraph_reverse.find(*iter2) == dag_min_induced_subgraph_reverse.end())// not in index subgraph
						//{
						//	dag_min_induced_subgraph_reverse.insert(std::make_pair(cur_node, temp_set2));
						//}
						//else {
						//	dag_min_induced_subgraph_reverse.find(cur_node)->second.insert(*iter2);
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (dag_min_induced_subgraph.find(*iter2) == dag_min_induced_subgraph.end())// not in index subgraph
							{
								dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								dag_min_induced_subgraph.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				break;//left_skip = true;
			}
		}
		//after left bfs, then we do a right bfs to get dag minimum induced subgraph
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_temp;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse_temp;
		cur_distance = 0;
		while (true)
		{
			cur_distance++;
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					//for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					if (dag_min_induced_subgraph.find(cur_node) == dag_min_induced_subgraph.end())
					{
						continue;
					}//no edge for this curnode
					auto iter_temp = dag_min_induced_subgraph.find(cur_node);
					for (auto iter2 = iter_temp->second.begin(); iter2 != iter_temp->second.end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (src_distance.find(*iter2)->second + cur_distance + 1 <= k) {
								if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
								{
									reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
								}
								else {
									reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
								}
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if (src_distance.find(*iter2)->second + cur_distance + 1 <= k) {
							reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						}
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				break;//right_skip = true;
			}
			//if (left_skip && right_skip)
			//{
			//	break;
			//}
		}

	}



};


class pruned_subgraph_unordered_map_double_direction_triple_join
{
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_total;
	set<NODE_TYPE> left_nodes, right_nodes, common_nodes; 
	map<NODE_TYPE,DISTANCE_TYPE> left_cut_nodes, right_cut_nodes;
	set<NODE_TYPE> meet_nodes;
	void join_left_right_index_into_left()
	{
		for (auto iter = reverse_adjacency_in_subgraph_right.begin(); iter != reverse_adjacency_in_subgraph_right.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void join_right_left_into_right()
	{
		for (auto iter = reverse_adjacency_in_subgraph_left.begin(); iter != reverse_adjacency_in_subgraph_left.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}
	void construct_pruned_subgraph_with_find_leftnodes_rightnodes_commonnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end() || *iter2 == query_node2)//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if (right_proprogate.find(*iter2) != right_proprogate.end() || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
						{

							common_nodes.insert(*iter2);
						}
						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end() || *iter2 == query_node1)//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2) != left_visited_nodes.end())
						{

							common_nodes.insert(*iter2);
						}
						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}
		set_difference(left_visited_nodes.begin(), left_visited_nodes.end(), common_nodes.begin(), common_nodes.end(), inserter(left_nodes, left_nodes.begin()));
		set_difference(right_visited_nodes.begin(), right_visited_nodes.end(), common_nodes.begin(), common_nodes.end(), inserter(right_nodes, right_nodes.begin()));
		//set_difference(common_nodes.begin(), common_nodes.end(), left_nodes.begin(), left_nodes.end(), inserter(common_nodes, common_nodes.begin()));
		//set_difference(common_nodes.begin(), common_nodes.end(), right_nodes.begin(), right_nodes.end(), inserter(common_nodes, common_nodes.begin()));

	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_commonnodes(vector<NODE_TYPE >* adjacency_list, NODE_TYPE src, NODE_TYPE dst, int k, int node_num, DISTANCE_TYPE& left_min_distance)// distance means length of path
	{
		left_min_distance = k;
		map<NODE_TYPE, paths> result;
		if (common_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< vector<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		if (adjacency_list[src].size() == 0)//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(adjacency_list[src].begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == adjacency_list[src].end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else if (adjacency_list[*cur_iters[cur_distance - 1]].empty() || cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
			{
				// go back
				cur_distance--;
				cur_iters[cur_distance]++;
				cur_node = c_path.top();
				c_path.pop();
				c_path_v.pop_back();
				auto iter2 = c_path_set.find(cur_node);
				if (iter2 != c_path_set.end())
				{
					c_path_set.erase(iter2);
				}
				continue;
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == adjacency_list[src].end())
					{
						force_continue = true;
						break;
					}
				}
				else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
				{
					force_continue = true;
					break;
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (common_nodes.find(*iter) != common_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;
				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				DISTANCE_TYPE temp_dis = temp_result_path.size() - 1;
				auto iter_cut = left_cut_nodes.find(*iter);
				if (iter_cut == left_cut_nodes.end())// *iter is not in left_cut_node
				{
					left_cut_nodes.insert(std::make_pair(*iter, temp_dis));
				}
				else
				{
					if (temp_dis < iter_cut->second)
					{
						iter_cut->second = temp_dis;
					}
				}
				if (temp_dis < left_min_distance)
				{
					left_min_distance = temp_dis;
				}
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				if (iter_result == result.end())//first time to insert
				{
					paths temp_paths;
					temp_paths.push_back(temp_result_path);
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.push_back(temp_result_path);
				}
				cur_iters[cur_distance]++;
				continue;// when touch the cut nodes, we will not go furhter
			}

			cur_distance++;
			cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_commonnodes_reverse(vector<NODE_TYPE >* adjacency_list, NODE_TYPE src, NODE_TYPE dst, int k, int node_num, DISTANCE_TYPE& right_min_distance)// distance means length of path
	{
		right_min_distance = k;

		map<NODE_TYPE, paths> result;
		if (common_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< vector<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		if (adjacency_list[src].size() == 0)//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(adjacency_list[src].begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == adjacency_list[src].end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
			{
				// go back
				cur_distance--;
				cur_iters[cur_distance]++;
				cur_node = c_path.top();
				c_path.pop();
				c_path_v.pop_back();
				auto iter2 = c_path_set.find(cur_node);
				if (iter2 != c_path_set.end())
				{
					c_path_set.erase(iter2);
				}
				continue;
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == adjacency_list[src].end())
					{
						force_continue = true;
						break;
					}
				}
				else if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
				{
					force_continue = true;
					break;
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (common_nodes.find(*iter) != common_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				DISTANCE_TYPE temp_dis = temp_result_path.size() - 1;
				auto iter_cut = right_cut_nodes.find(*iter);
				if (iter_cut == right_cut_nodes.end())// *iter is not in right_cut_node
				{
					right_cut_nodes.insert(std::make_pair(*iter, temp_dis));
				}
				else
				{
					if (temp_dis < iter_cut->second)
					{
						iter_cut->second = temp_dis;
					}
				}

				if (temp_dis < right_min_distance)
				{
					right_min_distance = temp_dis;
				}
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;
			cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}

	





	void construct_pruned_subgraph_equal_double_degrees(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2, vector<int> in_degrees, vector<int> out_degrees, bool& left)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}
		long long left_nodes = 1;// out_degrees[query_node1];
		long long right_nodes = 1;// in_degrees[query_node2];

		double total_left_nodes = 1;// out_degrees[query_node1];
		double total_right_nodes = 1;// in_degrees[query_node2];

		while (cur_distance < k)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (left_nodes <= right_nodes)
			{
				left_nodes = 0;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
						left_nodes += out_degrees[*iter2];
					}
				}
				left_proprogate = temp_proprogate;
				total_left_nodes += left_nodes;
			}
			else//right propogate
			{
				right_nodes = 0;
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
						right_nodes = right_nodes + in_degrees[*iter2];
						//right_nodes++;
					}
				}
				right_proprogate = temp_proprogate;
				total_right_nodes += right_nodes;
			}
		}
		if (total_right_nodes < total_left_nodes)
		{
			left = false;
		}
		else
		{
			left = true;
		}
	}

	void construct_pruned_subgraph_with_meetnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
																						   //left_visited_nodes.insert(query_node1);
																						   //right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					if (left_visited_nodes.find(*iter) != left_visited_nodes.end())//already visited
					{
						continue;
					}
					left_visited_nodes.insert(*iter);

					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}

						if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						}
						if (right_proprogate.find(*iter2) != right_proprogate.end())// || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					if (right_visited_nodes.find(*iter) != right_visited_nodes.end())//already visited
					{
						continue;
					}
					right_visited_nodes.insert(*iter);
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						}
						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2) != left_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}



	//paths find_all_k_pahts(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	//{
	//	paths result;
	//	for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
	//	{
	//		paths left_paths, right_paths, total_paths;
	//		left_paths = find_k_path_using_index_reverse(reverse_adjacency_in_subgraph_left, node_num, k - (k / 2), query_node1, *iter);
	//		right_paths = find_k_path_using_index(reverse_adjacency_in_subgraph_right, node_num, k / 2, query_node2, *iter);
	//		total_paths = left_paths.join(right_paths);
	//		total_paths.drop_path_length_more_than_k(k);
	//		result.add_paths(total_paths);
	//	}
	//	return result;
	//}


	//paths find_all_k_pahts_dfs(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	//{
	//	paths result;

	//	//for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
	//	//{
	//	//	paths left_paths, right_paths, total_paths;
	//	//	left_paths = find_k_path_using_index_reverse(reverse_adjacency_in_subgraph_left, node_num, k - (k / 2), query_node1, *iter);
	//	//	right_paths = find_k_path_using_index(reverse_adjacency_in_subgraph_right, node_num, k / 2, query_node2, *iter);
	//	//	total_paths = left_paths.join(right_paths);
	//	//	total_paths.drop_path_length_more_than_k(k);
	//	//	result.add_paths(total_paths);
	//	//}
	//	return result;
	//}

	map<NODE_TYPE, paths> find_paths_bt_cut_nodes_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, map<NODE_TYPE, DISTANCE_TYPE>& cut_nodes, NODE_TYPE query_node2, NODE_TYPE q1, NODE_TYPE q2)
	{
		map<NODE_TYPE, paths> result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == q1 || *iter == q2)
			{
				cur_iters[cur_distance]++;
				continue;
			}
			auto iter_cut = cut_nodes.find(*iter);
			if (iter_cut != cut_nodes.end() && c_path_v.size() <= (k - iter_cut->second))//*iter is already in cut_nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				//cur_iters[cur_distance]++;
				//continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	map<NODE_TYPE, paths> find_paths_bt_cut_nodes_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, map<NODE_TYPE, DISTANCE_TYPE>& cut_nodes, NODE_TYPE query_node2, NODE_TYPE q1, NODE_TYPE q2)
	{
		map<NODE_TYPE, paths> result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}

		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == q1 || *iter == q2)
			{
				cur_iters[cur_distance]++;
				continue;
			}
			auto iter_cut = cut_nodes.find(*iter);
			if (iter_cut != cut_nodes.end() && c_path_v.size() <= k)//*iter is already in cut_nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				//cur_iters[cur_distance]++;
				//continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}





	paths find_k_path_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	paths find_k_path_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}

	paths find_all_k_pahts_triple_join(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE left_min_distance, right_min_distance;
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_commonnodes(adjacency_list, query_node1, query_node2, k - (k / 2), node_num, left_min_distance);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_commonnodes_reverse(adjacency_list_reverse, query_node2, query_node1, (k / 2), node_num, right_min_distance);

		for (auto iter = left_cut_nodes.begin(); iter != left_cut_nodes.end();)
		{
			if (iter->first == query_node1 || iter->first == query_node2)
			{
				iter = left_cut_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		for (auto iter = right_cut_nodes.begin(); iter != right_cut_nodes.end();)
		{
			if (iter->first == query_node1 || iter->first == query_node2)
			{
				iter = right_cut_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		paths result;
		for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		{
			if (*iter == query_node2)//there is one hop path
			{
				vector<NODE_TYPE> one_hop_path;
				one_hop_path.push_back(query_node1);
				one_hop_path.push_back(query_node2);
				result.push_back(one_hop_path);
				break;
			}
		}
		//to find paths between left_cuts nodes to right_cuts nodes
		join_left_right_index_into_left();
		//we use the left subgraph index to find paths between cut nodes.


		for (auto iter = right_cut_nodes.begin(); iter != right_cut_nodes.end(); iter++)
		{
			map<NODE_TYPE, paths> middle_paths = find_paths_bt_cut_nodes_using_index(reverse_adjacency_in_subgraph_left,node_num,k-left_min_distance- iter->second,left_cut_nodes,iter->first,query_node1,query_node2);

			auto right_iter = right_map_paths.find(iter->first);
			for (auto iter_left = middle_paths.begin(); iter_left != middle_paths.end(); iter_left++)
			{
				auto left_iter = left_map_paths.find(iter_left->first);
				if (left_iter == left_map_paths.end())
				{
					continue;
				}
				else if (right_iter == right_map_paths.end())
				{
					continue;
				}

				auto middle_iter = middle_paths.find(iter_left->first);
				if (middle_iter == middle_paths.end())
				{
					continue;
				}


				left_iter->second.sort_by_string_order();
				right_iter->second.sort_by_string_order();
				paths total_paths;
				left_iter->second.drop_repeat_path();
				right_iter->second.drop_repeat_path();


				total_paths = left_iter->second.join(middle_iter->second);
				total_paths = total_paths.join(right_iter->second);
				//total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
				total_paths.drop_path_length_more_than_k(k);
				result.add_paths(total_paths);
			}
			
			if (left_cut_nodes.find(iter->first) != left_cut_nodes.end())//same node both in left and right cut nodes
			{
				auto left_iter = left_map_paths.find(iter->first);

				left_iter->second.sort_by_string_order();
				right_iter->second.sort_by_string_order();
				paths total_paths;
				left_iter->second.drop_repeat_path();
				right_iter->second.drop_repeat_path();


				total_paths = left_iter->second.join(right_iter->second);
				//total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
				total_paths.drop_path_length_more_than_k(k);
				result.add_paths(total_paths);
			}

		}
		return result;
	}




};




class pruned_subgraph_unordered_map_greedy_costmodel//included dag min index contruct algorithm and middle points algorithm
{// to be coded, not completed yet
public:
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
	unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph;
	set<NODE_TYPE> meet_nodes;//exactly the middle nodes for all paths

							  //unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse;

	unordered_map<NODE_TYPE, DISTANCE_TYPE> src_distance;
	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_distance;

	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_total;

	void join_left_right_index_into_left()
	{
		for (auto iter = reverse_adjacency_in_subgraph_right.begin(); iter != reverse_adjacency_in_subgraph_right.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void join_right_left_into_right()
	{
		for (auto iter = reverse_adjacency_in_subgraph_left.begin(); iter != reverse_adjacency_in_subgraph_left.end(); iter++)
		{
			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
			{
				if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
				{
					set<NODE_TYPE> temp_set;
					temp_set.insert(iter->first);
					reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
				}
				else {
					reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(iter->first);
				}
			}
		}
	}

	void construct_pruned_dag_min_subgraph(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		src_distance.insert(std::make_pair(query_node1, 0));
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		dag_min_induced_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
																				//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k;// k - (k / 2);;
		int right_max_distance = k;// (k / 2);
		cur_distance = 0;
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{
						auto iter_src_dis = src_distance.find(*iter2);
						if (iter_src_dis == src_distance.end())//no distance information for node *iter2
						{
							src_distance.insert(std::make_pair(*iter2, cur_distance));
						}
						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						set<NODE_TYPE> temp_set2;
						temp_set2.insert(*iter2);
						//if (dag_min_induced_subgraph_reverse.find(*iter2) == dag_min_induced_subgraph_reverse.end())// not in index subgraph
						//{
						//	dag_min_induced_subgraph_reverse.insert(std::make_pair(cur_node, temp_set2));
						//}
						//else {
						//	dag_min_induced_subgraph_reverse.find(cur_node)->second.insert(*iter2);
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (dag_min_induced_subgraph.find(*iter2) == dag_min_induced_subgraph.end())// not in index subgraph
							{
								dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								dag_min_induced_subgraph.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						dag_min_induced_subgraph.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				break;//left_skip = true;
			}
		}
		//after left bfs, then we do a right bfs to get dag minimum induced subgraph
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_temp;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse_temp;
		cur_distance = 0;

		//left_visited_nodes.clear();
		//right_visited_nodes.clear();
		//left_proprogate.clear();
		//right_proprogate.clear();
		//left_visited_nodes.insert(query_node1);
		//right_visited_nodes.insert(query_node2);
		//left_proprogate.insert(query_node1);
		//right_proprogate.insert(query_node2);

		while (true)
		{
			cur_distance++;
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					//for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					if (dag_min_induced_subgraph.find(cur_node) == dag_min_induced_subgraph.end())
					{
						continue;
					}//no edge for this curnode
					auto iter_temp = dag_min_induced_subgraph.find(cur_node);
					for (auto iter2 = iter_temp->second.begin(); iter2 != iter_temp->second.end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (src_distance.find(*iter2)->second + cur_distance <= k)
							{
								if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
								{
									reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
								}
								else {
									reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
								}
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}
						if (src_distance.find(*iter2)->second + cur_distance <= k)
						{
							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						}
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				break;//right_skip = true;
			}
			//if (left_skip && right_skip)
			//{
			//	break;
			//}
		}
		dag_min_induced_subgraph = reverse_adjacency_in_subgraph_right;
	}

	//only one bfs
	void construct_pruned_dag_min_subgraph_single_direction(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//store the final induced subgraph into reverse_adjacency_in_subgraph_left
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		dag_min_induced_subgraph.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
																				//dag_min_induced_subgraph_reverse.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k;// k - (k / 2);;
		int right_max_distance = k;// (k / 2);
		cur_distance = 0;
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						set<NODE_TYPE> temp_set2;
						temp_set2.insert(*iter2);
						//if (dag_min_induced_subgraph_reverse.find(*iter2) == dag_min_induced_subgraph_reverse.end())// not in index subgraph
						//{
						//	dag_min_induced_subgraph_reverse.insert(std::make_pair(cur_node, temp_set2));
						//}
						//else {
						//	dag_min_induced_subgraph_reverse.find(cur_node)->second.insert(*iter2);
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				break;//left_skip = true;
			}
		}
		//after left bfs, then we do a right bfs to get dag minimum induced subgraph
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_temp;
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> dag_min_induced_subgraph_reverse_temp;
	}

	void find_all_meet_nodes_in_induced_subgraph(NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		//unordered_map<NODE_TYPE, set<NODE_TYPE>> temp_map;
		//reverse_adjacency_in_subgraph_right = temp_map;
		//reverse_adjacency_in_subgraph_left = dag_min_induced_subgraph;
		join_left_right_index_into_left();// we store the left induced subgraph into right one with reverse order
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;
		//right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				if (cur_distance == 1) {
					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node2)
							{
								continue;
							}
							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
				else {


					set<NODE_TYPE> temp_proprogate;
					NODE_TYPE cur_node;
					for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
					{


						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_right.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_right.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (right_proprogate.find(*iter2) != right_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					left_proprogate = temp_proprogate;
				}
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				if (cur_distance == 1) {
					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{
						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{
							if (*iter2 == query_node1)
							{
								continue;
							}
							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
				else {


					for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
					{

						cur_node = *iter;

						set<NODE_TYPE> temp_set;
						temp_set.insert(cur_node);
						auto iter_map = reverse_adjacency_in_subgraph_left.find(cur_node);
						if (iter_map == reverse_adjacency_in_subgraph_left.end())
						{
							continue;
						}
						for (auto iter2 = iter_map->second.begin(); iter2 != iter_map->second.end(); iter2++)
						{

							if (left_proprogate.find(*iter2) != left_proprogate.end())
							{
								meet_nodes.insert(*iter2);
							}
							//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							temp_proprogate.insert(*iter2);
						}
					}
					right_proprogate = temp_proprogate;
				}
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		auto dst_iter = reverse_adjacency_in_subgraph.find(src);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				//				if (adjacency_list[*cur_iters[cur_distance - 1]].empty() || cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					//					if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				if (iter_result == result.end())//first time to insert
				{
					paths temp_paths;
					temp_paths.push_back(temp_result_path);
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.push_back(temp_result_path);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
			//cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


	map<NODE_TYPE, paths> dfs_without_recursion_all_meetpoints_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, NODE_TYPE dst, int k, int node_num)// distance means length of path
	{
		map<NODE_TYPE, paths> result;
		if (meet_nodes.empty())
		{
			// cout << "construct meet nodes first " << endl;
			return result;
		}
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		c_path.push(src);
		c_path_v.push_back(src);
		auto dst_iter = reverse_adjacency_in_subgraph.find(src);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(src);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				//				if (adjacency_list[*cur_iters[cur_distance - 1]].empty() || cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end() || cur_distance > k)
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					//					if (cur_iters[cur_distance] == adjacency_list[*cur_iters[cur_distance - 1]].end())
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == dst)//dst 
			{
				cur_iters[cur_distance]++;
				continue;
			}
			if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				auto iter_result = result.find(*iter);
				paths temp_paths;
				temp_paths.push_back(temp_result_path);
				temp_paths.reverse();
				if (iter_result == result.end())//first time to insert
				{
					result.insert(std::make_pair(*iter, temp_paths));
				}
				else
				{
					iter_result->second.add_paths(temp_paths);
				}
				//cur_iters[cur_distance]++;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}
			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
			//cur_iters[cur_distance] = adjacency_list[*cur_iters[cur_distance - 1]].begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}



	paths find_all_k_pahts_dfs(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end();)
		{
			if (*iter == query_node1 || *iter == query_node2)
			{
				iter = meet_nodes.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		paths result;
		auto iter_temp = reverse_adjacency_in_subgraph_right.find(query_node1);
		if (iter_temp != reverse_adjacency_in_subgraph_right.end())
		{
			for (auto iter = iter_temp->second.begin(); iter != iter_temp->second.end(); iter++)
			{
				if (*iter == query_node2)//there is one hop path
				{
					vector<NODE_TYPE> one_hop_path;
					one_hop_path.push_back(query_node1);
					one_hop_path.push_back(query_node2);
					result.push_back(one_hop_path);
					break;
				}
			}
		}
		map<NODE_TYPE, paths> left_map_paths = dfs_without_recursion_all_meetpoints(reverse_adjacency_in_subgraph_right, query_node1, query_node2, k - (k / 2), node_num);
		map<NODE_TYPE, paths> right_map_paths = dfs_without_recursion_all_meetpoints_reverse(reverse_adjacency_in_subgraph_left, query_node2, query_node1, (k / 2), node_num);

		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			auto left_iter = left_map_paths.find(*iter);
			auto right_iter = right_map_paths.find(*iter);
			if (left_iter == left_map_paths.end())
			{
				continue;
			}
			//else if (*iter == query_node2)
			//{
			//	result.add_paths(left_iter->second);
			//	continue;
			//}
			else if (right_iter == right_map_paths.end())
			{
				continue;
			}
			left_iter->second.sort_by_string_order();
			right_iter->second.sort_by_string_order();
			paths total_paths;
			left_iter->second.drop_repeat_path();
			right_iter->second.drop_repeat_path();
			total_paths = left_iter->second.join_remove_repeat_nodes_only_join_right_sizeor_minus_one(right_iter->second, k);
			//total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}




	void construct_pruned_subgraph(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
		left_visited_nodes.insert(query_node1);
		right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}
						if (left_visited_nodes.find(*iter2) != left_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						left_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (right_visited_nodes.find(*iter2) != right_visited_nodes.end())//already visited
						{
							if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
							{
								reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
							}
							else {
								reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							}
							//reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
							continue;
						}

						reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						right_visited_nodes.insert(*iter2);
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}


	void construct_pruned_subgraph_with_meetnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		DISTANCE_TYPE cur_distance = 0;
		set<NODE_TYPE> left_proprogate;
		set<NODE_TYPE> right_proprogate;
		//cur_proprogate.insert(query_node1);
		set<NODE_TYPE> node1_set;
		set<NODE_TYPE> node2_set;
		//set<NODE_TYPE> visited_nodes;

		set<NODE_TYPE> left_visited_nodes;
		set<NODE_TYPE> right_visited_nodes;

		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
																						   //left_visited_nodes.insert(query_node1);
																						   //right_visited_nodes.insert(query_node2);
		left_proprogate.insert(query_node1);
		right_proprogate.insert(query_node2);
		//node1_set.insert(query_node1);
		//node2_set.insert(query_node2);

		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
		//{
		//	left_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
		//}


		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
		//{
		//	right_proprogate.insert(*iter);
		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
		//}

		bool left_skip = false;
		bool right_skip = false;
		int left_max_distance = k - (k / 2);;
		int right_max_distance = (k / 2);
		while (true)
		{
			//left_skip = false;
			//right_skip = false;
			cur_distance++;
			if (cur_distance <= left_max_distance && !left_proprogate.empty())
			{
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
				{
					if (left_visited_nodes.find(*iter) != left_visited_nodes.end())//already visited
					{
						continue;
					}
					left_visited_nodes.insert(*iter);

					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
					{

						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//	
						//	//continue;
						//}

						if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
						}
						if (right_proprogate.find(*iter2) != right_proprogate.end())// || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				left_proprogate = temp_proprogate;
			}
			else {
				left_skip = true;
			}
			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
			{
				//cur_distance++;
				set<NODE_TYPE> temp_proprogate;
				NODE_TYPE cur_node;
				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
				{
					if (right_visited_nodes.find(*iter) != right_visited_nodes.end())//already visited
					{
						continue;
					}
					right_visited_nodes.insert(*iter);
					cur_node = *iter;

					set<NODE_TYPE> temp_set;
					temp_set.insert(cur_node);
					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
					{
						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
						//{
						//	meet_nodes.insert(*iter2);
						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						//	//{
						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						//	//}
						//	//else {
						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						//	//}
						//	//continue;
						//}
						if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
						{
							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						}
						else {
							reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
						}
						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2) != left_visited_nodes.end())
						{
							meet_nodes.insert(*iter2);
						}
						//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
						temp_proprogate.insert(*iter2);
					}
				}
				right_proprogate = temp_proprogate;
			}
			else {
				right_skip = true;
			}
			if (left_skip && right_skip)
			{
				break;
			}
		}

	}



	paths find_all_k_pahts(NODE_TYPE node_num, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		for (auto iter = meet_nodes.begin(); iter != meet_nodes.end(); iter++)
		{
			paths left_paths, right_paths, total_paths;
			left_paths = find_k_path_using_index_reverse(reverse_adjacency_in_subgraph_left, node_num, k - (k / 2), query_node1, *iter);
			right_paths = find_k_path_using_index(reverse_adjacency_in_subgraph_right, node_num, k / 2, query_node2, *iter);
			total_paths = left_paths.join(right_paths);
			total_paths.drop_path_length_more_than_k(k);
			result.add_paths(total_paths);
		}
		return result;
	}




	paths find_k_path_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}
	paths find_k_path_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{//find all the path from query_node2 to query_node1, the reverser_adjacency is from query_node2 to node1
		paths result;
		stack<NODE_TYPE> c_path;
		vector<NODE_TYPE> c_path_v;
		set<NODE_TYPE> c_path_set;
		int cur_distance = 1;
		vector< set<NODE_TYPE>::iterator > cur_iters;
		NODE_TYPE dst = query_node2;
		c_path.push(dst);
		c_path_v.push_back(dst);
		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
		{
			return result;
		}
		if (query_node1 == query_node2)
		{
			vector<NODE_TYPE> temp_path;
			temp_path.push_back(query_node1);
			result.push_back(temp_path);
			return result;
		}
		for (int i = 0; i <= k + 1; i++)
		{
			cur_iters.push_back(dst_iter->second.begin());
		}
		//set<NODE_TYPE> new_c_path_set(c_path_set);
		c_path_set.insert(dst);
		bool dfs_finish = false;
		NODE_TYPE cur_node;
		while (!dfs_finish)
		{// we do not use adjacency_list_double
			NODE_TYPE cur_node, next_node;
			if (cur_distance <= 0)
			{
				break;
			}
			auto iter = cur_iters[cur_distance];
			if (cur_distance == 1)
			{
				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
				{
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			else
			{
				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
				{
					// go back
					cur_distance--;
					cur_iters[cur_distance]++;
					cur_node = c_path.top();
					c_path.pop();
					c_path_v.pop_back();
					auto iter2 = c_path_set.find(cur_node);
					if (iter2 != c_path_set.end())
					{
						c_path_set.erase(iter2);
					}
					continue;
				}
			}
			cur_node = *iter;
			//normal logic
			bool force_continue = false;
			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
			{
				cur_iters[cur_distance]++;
				if (cur_distance == 1)
				{
					if (cur_iters[cur_distance] == dst_iter->second.end())
					{
						force_continue = true;
						break;
					}
				}
				else
				{
					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
					{
						force_continue = true;
						break;
					}
				}
			}
			if (force_continue)
			{
				continue;
			}
			iter = cur_iters[cur_distance];
			if (*iter == query_node1)
			{
				// add result path and continue;

				vector<NODE_TYPE> temp_result_path(c_path_v);
				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
				temp_result_path.push_back(*iter);
				vector<NODE_TYPE> new_temp;
				//std::reverse(temp_result_path.begin(), temp_result_path.end());
				result.push_back(temp_result_path);
				cur_iters[cur_distance]++;
				continue;
			}

			cur_distance++;

			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
			{
				set<NODE_TYPE> temp_set;
				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
			}

			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();

			cur_node = *cur_iters[cur_distance - 1];
			c_path_set.insert(cur_node);
			c_path.push(cur_node);
			c_path_v.push_back(cur_node);
			//cur_iters[cur_distance]++;

		}
		return result;
	}


};




// not used, this class need to be fixed
//class double_direction_baseline_paths
//{
//public:
//	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_left;
//	unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_right;
//	unordered_map<NODE_TYPE, DISTANCE_TYPE> src_distance;
//	unordered_map<NODE_TYPE, DISTANCE_TYPE> dst_distance;
//	set<NODE_TYPE> meet_nodes;
//
//
//	void join_left_right_index_into_left()
//	{
//		for (auto iter = reverse_adjacency_in_subgraph_right.begin(); iter != reverse_adjacency_in_subgraph_right.end(); iter++)
//		{
//			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
//			{
//				if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
//				{
//					set<NODE_TYPE> temp_set;
//					temp_set.insert(iter->first);
//					reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
//				}
//				else {
//					reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(iter->first);
//				}
//			}
//		}
//	}
//
//	void join_right_left_into_right()
//	{
//		for (auto iter = reverse_adjacency_in_subgraph_left.begin(); iter != reverse_adjacency_in_subgraph_left.end(); iter++)
//		{
//			for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
//			{
//				if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
//				{
//					set<NODE_TYPE> temp_set;
//					temp_set.insert(iter->first);
//					reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
//				}
//				else {
//					reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(iter->first);
//				}
//			}
//		}
//	}
//
//
//	//unordered_map<NODE_TYPE, set<NODE_TYPE>> reverse_adjacency_in_subgraph_total;
//
//	void construct_pruned_subgraph_with_meetnodes(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
//	{
//		DISTANCE_TYPE cur_distance = 0;
//		set<NODE_TYPE> left_proprogate;
//		set<NODE_TYPE> right_proprogate;
//		//cur_proprogate.insert(query_node1);
//		set<NODE_TYPE> node1_set;
//		set<NODE_TYPE> node2_set;
//		//set<NODE_TYPE> visited_nodes;
//
//
//		reverse_adjacency_in_subgraph_left.insert(std::make_pair(query_node1, node1_set));// query node1's parent is empty
//		reverse_adjacency_in_subgraph_right.insert(std::make_pair(query_node2, node2_set));// query node1's parent is empty
//																						   //left_visited_nodes.insert(query_node1);
//																						   //right_visited_nodes.insert(query_node2);
//		left_proprogate.insert(query_node1);
//		right_proprogate.insert(query_node2);
//		//node1_set.insert(query_node1);
//		//node2_set.insert(query_node2);
//
//		//for (auto iter = adjacency_list[query_node1].begin(); iter != adjacency_list[query_node1].end(); iter++)
//		//{
//		//	left_proprogate.insert(*iter);
//		//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter, node1_set));// store the first node
//		//}
//
//
//		//for (auto iter = adjacency_list_reverse[query_node2].begin(); iter != adjacency_list_reverse[query_node2].end(); iter++)
//		//{
//		//	right_proprogate.insert(*iter);
//		//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter, node2_set));// store the first node
//		//}
//
//		bool left_skip = false;
//		bool right_skip = false;
//		int left_max_distance = k - (k / 2);;
//		int right_max_distance = (k / 2);
//		while (true)
//		{
//			//left_skip = false;
//			//right_skip = false;
//			cur_distance++;
//			if (cur_distance <= left_max_distance && !left_proprogate.empty())
//			{
//				set<NODE_TYPE> temp_proprogate;
//				NODE_TYPE cur_node;
//				for (auto iter = left_proprogate.begin(); iter != left_proprogate.end(); iter++)
//				{
//					cur_node = *iter;
//
//					set<NODE_TYPE> temp_set;
//					temp_set.insert(cur_node);
//					for (auto iter2 = adjacency_list[cur_node].begin(); iter2 != adjacency_list[cur_node].end(); iter2++)
//					{
//
//						//if (right_proprogate.find(*iter2) != right_proprogate.end())// judge whether *iter is a meet node
//						//{
//						//	meet_nodes.insert(*iter2);
//						//	//if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
//						//	//{
//						//	//	reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
//						//	//}
//						//	//else {
//						//	//	reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
//						//	//}
//						//	//	
//						//	//continue;
//						//}
//
//						if (reverse_adjacency_in_subgraph_left.find(*iter2) == reverse_adjacency_in_subgraph_left.end())// not in index subgraph
//						{
//							reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
//						}
//						else {
//							reverse_adjacency_in_subgraph_left.find(*iter2)->second.insert(cur_node);
//						}
//						if (right_proprogate.find(*iter2) != right_proprogate.end())// || right_visited_nodes.find(*iter2) != right_visited_nodes.end())
//						{
//							meet_nodes.insert(*iter2);
//						}
//						//reverse_adjacency_in_subgraph_left.insert(std::make_pair(*iter2, temp_set));
//						temp_proprogate.insert(*iter2);
//					}
//				}
//				left_proprogate = temp_proprogate;
//			}
//			else {
//				left_skip = true;
//			}
//			if (cur_distance <= right_max_distance && !right_proprogate.empty())//right propogate
//			{
//				//cur_distance++;
//				set<NODE_TYPE> temp_proprogate;
//				NODE_TYPE cur_node;
//				for (auto iter = right_proprogate.begin(); iter != right_proprogate.end(); iter++)
//				{
//					cur_node = *iter;
//
//					set<NODE_TYPE> temp_set;
//					temp_set.insert(cur_node);
//					for (auto iter2 = adjacency_list_reverse[cur_node].begin(); iter2 != adjacency_list_reverse[cur_node].end(); iter2++)
//					{
//						//if (left_proprogate.find(*iter2) != left_proprogate.end())// judge whether *iter is a meet node
//						//{
//						//	meet_nodes.insert(*iter2);
//						//	//if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
//						//	//{
//						//	//	reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
//						//	//}
//						//	//else {
//						//	//	reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
//						//	//}
//						//	//continue;
//						//}
//						if (reverse_adjacency_in_subgraph_right.find(*iter2) == reverse_adjacency_in_subgraph_right.end())// not in index subgraph
//						{
//							reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
//						}
//						else {
//							reverse_adjacency_in_subgraph_right.find(*iter2)->second.insert(cur_node);
//						}
//						if (left_proprogate.find(*iter2) != left_proprogate.end() || left_visited_nodes.find(*iter2) != left_visited_nodes.end())
//						{
//							meet_nodes.insert(*iter2);
//						}
//						//reverse_adjacency_in_subgraph_right.insert(std::make_pair(*iter2, temp_set));
//						temp_proprogate.insert(*iter2);
//					}
//				}
//				right_proprogate = temp_proprogate;
//			}
//			else {
//				right_skip = true;
//			}
//			if (left_skip && right_skip)
//			{
//				break;
//			}
//		}
//
//	}
//	paths find_k_path_using_index_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
//	{
//		paths result;
//		stack<NODE_TYPE> c_path;
//		vector<NODE_TYPE> c_path_v;
//		set<NODE_TYPE> c_path_set;
//		int cur_distance = 1;
//		vector< set<NODE_TYPE>::iterator > cur_iters;
//		NODE_TYPE dst = query_node2;
//		c_path.push(dst);
//		c_path_v.push_back(dst);
//		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
//		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
//		{
//			return result;
//		}
//		if (query_node1 == query_node2)
//		{
//			vector<NODE_TYPE> temp_path;
//			temp_path.push_back(query_node1);
//			result.push_back(temp_path);
//			return result;
//		}
//		for (int i = 0; i <= k + 1; i++)
//		{
//			cur_iters.push_back(dst_iter->second.begin());
//		}
//		//set<NODE_TYPE> new_c_path_set(c_path_set);
//		c_path_set.insert(dst);
//		bool dfs_finish = false;
//		NODE_TYPE cur_node;
//		while (!dfs_finish)
//		{// we do not use adjacency_list_double
//			NODE_TYPE cur_node, next_node;
//			if (cur_distance <= 0)
//			{
//				break;
//			}
//			auto iter = cur_iters[cur_distance];
//			if (cur_distance == 1)
//			{
//				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
//				{
//					cur_distance--;
//					cur_iters[cur_distance]++;
//					cur_node = c_path.top();
//					c_path.pop();
//					c_path_v.pop_back();
//					auto iter2 = c_path_set.find(cur_node);
//					if (iter2 != c_path_set.end())
//					{
//						c_path_set.erase(iter2);
//					}
//					continue;
//				}
//			}
//			else
//			{
//				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
//				{
//					// go back
//					cur_distance--;
//					cur_iters[cur_distance]++;
//					cur_node = c_path.top();
//					c_path.pop();
//					c_path_v.pop_back();
//					auto iter2 = c_path_set.find(cur_node);
//					if (iter2 != c_path_set.end())
//					{
//						c_path_set.erase(iter2);
//					}
//					continue;
//				}
//			}
//			cur_node = *iter;
//			//normal logic
//			bool force_continue = false;
//			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
//			{
//				cur_iters[cur_distance]++;
//				if (cur_distance == 1)
//				{
//					if (cur_iters[cur_distance] == dst_iter->second.end())
//					{
//						force_continue = true;
//						break;
//					}
//				}
//				else
//				{
//					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
//					{
//						force_continue = true;
//						break;
//					}
//				}
//			}
//			if (force_continue)
//			{
//				continue;
//			}
//			iter = cur_iters[cur_distance];
//			if (*iter == query_node1)
//			{
//				// add result path and continue;
//
//				vector<NODE_TYPE> temp_result_path(c_path_v);
//				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
//				temp_result_path.push_back(*iter);
//				vector<NODE_TYPE> new_temp;
//				std::reverse(temp_result_path.begin(), temp_result_path.end());
//				result.push_back(temp_result_path);
//				cur_iters[cur_distance]++;
//				continue;
//			}
//
//			cur_distance++;
//
//			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
//			{
//				set<NODE_TYPE> temp_set;
//				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
//			}
//
//			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
//
//			cur_node = *cur_iters[cur_distance - 1];
//			c_path_set.insert(cur_node);
//			c_path.push(cur_node);
//			c_path_v.push_back(cur_node);
//			//cur_iters[cur_distance]++;
//
//		}
//		return result;
//	}
//	paths find_k_path_using_index(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2)
//	{
//		paths result;
//		stack<NODE_TYPE> c_path;
//		vector<NODE_TYPE> c_path_v;
//		set<NODE_TYPE> c_path_set;
//		int cur_distance = 1;
//		vector< set<NODE_TYPE>::iterator > cur_iters;
//		NODE_TYPE dst = query_node2;
//		c_path.push(dst);
//		c_path_v.push_back(dst);
//		auto dst_iter = reverse_adjacency_in_subgraph.find(dst);
//		if (dst_iter == reverse_adjacency_in_subgraph.end())//if node1 can not reach node2 in k distance
//		{
//			return result;
//		}
//		if (query_node1 == query_node2)
//		{
//			vector<NODE_TYPE> temp_path;
//			temp_path.push_back(query_node1);
//			result.push_back(temp_path);
//			return result;
//		}
//		for (int i = 0; i <= k + 1; i++)
//		{
//			cur_iters.push_back(dst_iter->second.begin());
//		}
//		//set<NODE_TYPE> new_c_path_set(c_path_set);
//		c_path_set.insert(dst);
//		bool dfs_finish = false;
//		NODE_TYPE cur_node;
//		while (!dfs_finish)
//		{// we do not use adjacency_list_double
//			NODE_TYPE cur_node, next_node;
//			if (cur_distance <= 0)
//			{
//				break;
//			}
//			auto iter = cur_iters[cur_distance];
//			if (cur_distance == 1)
//			{
//				if (dst_iter->second.empty() || cur_iters[cur_distance] == dst_iter->second.end())
//				{
//					cur_distance--;
//					cur_iters[cur_distance]++;
//					cur_node = c_path.top();
//					c_path.pop();
//					c_path_v.pop_back();
//					auto iter2 = c_path_set.find(cur_node);
//					if (iter2 != c_path_set.end())
//					{
//						c_path_set.erase(iter2);
//					}
//					continue;
//				}
//			}
//			else
//			{
//				auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//				if (temp_iter_minus_one->second.empty() || cur_iters[cur_distance] == temp_iter_minus_one->second.end() || cur_distance > k)
//				{
//					// go back
//					cur_distance--;
//					cur_iters[cur_distance]++;
//					cur_node = c_path.top();
//					c_path.pop();
//					c_path_v.pop_back();
//					auto iter2 = c_path_set.find(cur_node);
//					if (iter2 != c_path_set.end())
//					{
//						c_path_set.erase(iter2);
//					}
//					continue;
//				}
//			}
//			cur_node = *iter;
//			//normal logic
//			bool force_continue = false;
//			while (c_path_set.find(*cur_iters[cur_distance]) != c_path_set.end())//*cur_iters[cur_distance] < src || 
//			{
//				cur_iters[cur_distance]++;
//				if (cur_distance == 1)
//				{
//					if (cur_iters[cur_distance] == dst_iter->second.end())
//					{
//						force_continue = true;
//						break;
//					}
//				}
//				else
//				{
//					auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//					if (cur_iters[cur_distance] == temp_iter_minus_one->second.end())
//					{
//						force_continue = true;
//						break;
//					}
//				}
//			}
//			if (force_continue)
//			{
//				continue;
//			}
//			iter = cur_iters[cur_distance];
//			if (*iter == query_node1)
//			{
//				// add result path and continue;
//
//				vector<NODE_TYPE> temp_result_path(c_path_v);
//				//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
//				temp_result_path.push_back(*iter);
//				vector<NODE_TYPE> new_temp;
//				//std::reverse(temp_result_path.begin(), temp_result_path.end());
//				result.push_back(temp_result_path);
//				cur_iters[cur_distance]++;
//				continue;
//			}
//
//			cur_distance++;
//
//			if (reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]) == reverse_adjacency_in_subgraph.end())
//			{
//				set<NODE_TYPE> temp_set;
//				reverse_adjacency_in_subgraph.insert(std::make_pair(*cur_iters[cur_distance - 1], temp_set));
//			}
//
//			auto temp_iter_minus_one = reverse_adjacency_in_subgraph.find(*cur_iters[cur_distance - 1]);
//			cur_iters[cur_distance] = temp_iter_minus_one->second.begin();
//
//			cur_node = *cur_iters[cur_distance - 1];
//			c_path_set.insert(cur_node);
//			c_path.push(cur_node);
//			c_path_v.push_back(cur_node);
//			//cur_iters[cur_distance]++;
//
//		}
//		return result;
//	}
//
//};
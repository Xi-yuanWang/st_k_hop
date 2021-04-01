// assumption:
// node id start from 0

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <functional>
#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <map>
#include <stack>
#include <queue>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <random>
#define NULL_NODE -1
#define BUFFER_LENTH 255
#define MAX_K 100
#define NODE_TYPE long long
#define DISTANCE_TYPE int
//#define SHORT_CUT_DIS 4
extern int SHORT_CUT_DIS;
#define LEFT_SHORT_DIS (SHORT_CUT_DIS-SHORT_CUT_DIS/2)
#define RIGHT_SHORT_DIS (SHORT_CUT_DIS/2)
#define DEBUG_ALL_QUERYS false
using namespace std;




class node_distance_pair
{
public:
	NODE_TYPE node;
	DISTANCE_TYPE distance;
};

class spd_distance_map
{
public:
	map<NODE_TYPE,DISTANCE_TYPE> distance_map;
	map<NODE_TYPE, DISTANCE_TYPE> get_map()
	{
		return distance_map;
	}
	void push_back(NODE_TYPE node, DISTANCE_TYPE distance)
	{
		auto iter = distance_map.find(node);
		if (iter == distance_map.end())
		{
			distance_map.insert(std::make_pair(node, distance));
		}
		else
		{
			if (distance < iter->second)
			{
				iter->second = distance;
			}
			else
			{
				// cout << node << " : " << distance << " key is already in this map index" << endl;
			} 
		}
	}
	void add_map(spd_distance_map temp)
	{
		for (auto iter = temp.distance_map.begin(); iter != temp.distance_map.end(); iter++)
		{
			push_back(iter->first, iter->second);
		}
	}
};

class spd_index {
public:
	map<NODE_TYPE, spd_distance_map > h_h_index;// hot points to hot points index,int means distance
	map<NODE_TYPE, spd_distance_map > h_nh_index;// hot points to non hot points index
	set<NODE_TYPE> hot_points;// index include left part(only include hot points) and right part(all k-reach nodes without crossing hot points)
							  //hot points to contruct index into two parts. The first one is hot-hot spds index and second is hot-nonhot spds index

	void insert_single_hot_point(NODE_TYPE node)
	{
		hot_points.insert(node);
	}
	void insert_hot_points_set(set<NODE_TYPE> hots)
	{
		for (auto iter = hots.begin(); iter != hots.end(); iter++)
		{
			this->hot_points.insert(*iter);
		}
	}
	void push_back(NODE_TYPE node1, NODE_TYPE node2, DISTANCE_TYPE distance)
	{
		if (hot_points.find(node2) != hot_points.end())//node2 is hot points
		{
			auto iter = h_h_index.find(node1);
			if (iter != h_h_index.end())// node1 already in h_h_index
			{
				iter->second.push_back(node2, distance);
			}
			else
			{
				spd_distance_map temp;
				h_h_index.insert(std::make_pair(node1, temp));
				auto iter2 = h_h_index.find(node1);
				iter2->second.push_back(node2, distance);
			}
		}
		else {
			auto iter = h_nh_index.find(node1);
			if (iter != h_nh_index.end())// node1 already in h_h_index
			{
				iter->second.push_back(node2, distance);
			}
			else
			{
				spd_distance_map temp;
				h_nh_index.insert(std::make_pair(node1, temp));
				auto iter2 = h_nh_index.find(node1);
				iter2->second.push_back(node2, distance);
			}
		}
	}
	void hot_push_back(NODE_TYPE node1, NODE_TYPE node2, DISTANCE_TYPE distance)
	{
		auto iter = h_h_index.find(node1);
		if (iter != h_h_index.end())// node1 already in h_h_index
		{
			iter->second.push_back(node2, distance);
		}
		else
		{
			spd_distance_map temp;
			h_h_index.insert(std::make_pair(node1, temp));
			auto iter2 = h_h_index.find(node1);
			iter2->second.push_back(node2, distance);
		}
	}

	void non_hot_push_back(NODE_TYPE node1, NODE_TYPE node2, DISTANCE_TYPE distance)
	{
		auto iter = h_nh_index.find(node1);
		if (iter != h_nh_index.end())// node1 already in h_h_index
		{
			iter->second.push_back(node2, distance);
		}
		else
		{
			spd_distance_map temp;
			h_nh_index.insert(std::make_pair(node1, temp));
			auto iter2 = h_nh_index.find(node1);
			iter2->second.push_back(node2, distance);
		}
	}
	void construct_spd_index(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k);
	bool judge_reachability(NODE_TYPE node1, NODE_TYPE node2, DISTANCE_TYPE distance);//node1 must be in hot points

	/*bool judge_reachability_in_index_subgraphs(NODE_TYPE cur_node, NODE_TYPE node2, DISTANCE_TYPE cur_distance)
	{
	bool result = false;
	if (cur_node == node2)
	{
	return true;
	}
	else
	{
	auto iter = h_nh_index.find(cur_node);
	if (iter == h_nh_index.end())
	{
	return false;
	}
	else
	{
	for (auto iter2 = iter->second.distance_map.begin(); iter2 != iter->second.distance_map.end(); iter2++)
	{
	if (iter2->first > cur_distance)
	{
	continue;
	}
	if(judge_reachability_in_index_subgraphs())
	}
	}
	}
	}*/

};

//spd means shortest paht distance, this class is used in dynamic_shortestpath_distance.cpp and .h



class map_distance_node_pair
{
public:
	map<int, set<NODE_TYPE>*> v;//map distance to label sets
	map_distance_node_pair() {}
	map_distance_node_pair(NODE_TYPE src)
	{
		set<NODE_TYPE>* temp_pair = new set<NODE_TYPE>;
		temp_pair->insert(src);
		v.insert(std::make_pair(0, temp_pair));
	}
	void destroy_map_distance_node_pair()
	{//destructor for set pointer
		map<int, set<NODE_TYPE>*>::iterator iter = v.begin();
		while (iter != v.end())
		{
			delete iter->second;
			v.erase(iter++);
		}
	}
	void remove(NODE_TYPE node, int distance)
	{
		auto temp = v.find(distance);
		if (temp != v.end())// already has a set
		{
			auto temp2 = temp->second->find(node);
			if (temp2 != temp->second->end())
			{
				temp->second->erase(temp2);
			}
			else {// if there is no node in this distance
				// cout << " there does not exist node distance pair in distance " << distance << " for node " << node << endl;
			}
		}
		else
		{
			// cout << "there does not exist node distance pair for distance " << distance << endl;
		}
	}
	void insert(NODE_TYPE node, int distance)//insert node and distance(key)
	{
		auto temp = v.find(distance);
		if (temp != v.end())// already has a set
		{
			temp->second->insert(node);
		}
		else 
		{
			set<NODE_TYPE>* temp_pair = new set<NODE_TYPE>;
			temp_pair->insert(node);
			v.insert(std::make_pair(distance, temp_pair));
		}

	}
	bool intersection(map_distance_node_pair vb, int k)//k is the distance constraint, all map_distance_node_pair has their default node with distance 0
	{
		for (auto iter = v.begin(); iter != v.end(); iter++)
		{
			if (iter->first > k)
			{
				break;
			}
			for (auto iter2 = vb.v.begin(); iter2 != vb.v.end(); iter2++)
			{
				if (iter->first + iter2->first > k)
				{
					break;
				}
				set<NODE_TYPE> result;
				set_intersection(iter->second->begin(), iter->second->end(),
					iter2->second->begin(), iter2->second->end(),
					inserter(result, result.begin()));
				if (!result.empty())//not empty
				{
					return true;
				}
			}
		}
		return false;
	}
};

class paths;
class start_paths;
class meet_node
{
public:
	NODE_TYPE node;//meet node
	NODE_TYPE left_distance;
	NODE_TYPE right_distance;
	meet_node(NODE_TYPE node, NODE_TYPE left_distance, NODE_TYPE right_distance): node(node),left_distance(left_distance),right_distance(right_distance){}
	bool operator == (const meet_node &m) const {
		return (node == m.node && left_distance == m.left_distance && right_distance== m.right_distance);
	}
	bool operator < (const meet_node &m) const {
		if (node < m.node)
		{
			return true;
		}
		else if (node == m.node && left_distance < m.left_distance)
		{
			return true;
		}
		else if(node == m.node && left_distance == m.left_distance && right_distance< m.right_distance)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};




class meet_nodes
{
public:
	set<meet_node> nodes_pairs;// <left node, left distance>,<right node, right distance>
	meet_nodes(meet_node a) {
		nodes_pairs.insert(a);
	}
	meet_nodes() {}
	void push_back(meet_node a)
	{
		nodes_pairs.insert(a);
	}
	set<meet_node> & get_nodes_pairs()
	{
		return nodes_pairs;
	}
};


class hot_degree {
public:
	NODE_TYPE node;
	NODE_TYPE degree;
	hot_degree(NODE_TYPE a, NODE_TYPE b) :node(a), degree(b) {}
	bool operator < (const hot_degree &m)const {
		return degree > m.degree;
	}
	bool operator == (const hot_degree &m) const {
		return (node == m.node && degree == m.degree);
	}
};

class two_nodes_path_index {
public:
	NODE_TYPE nodeStart;
	NODE_TYPE nodeEnd;
	vector< vector<NODE_TYPE> > paths;
	two_nodes_path_index(NODE_TYPE start, NODE_TYPE end) :nodeStart(start), nodeEnd(end) {}//should define start and end node first
																						   //two_nodes_path_index() {}
	void push_back(vector<NODE_TYPE> a) {
		if (nodeStart != a.front() || nodeEnd != a.back())
		{
			// cout << " error in start node or end node, it should be the same as this class's" << endl;
			return;
		}
		else
		{
			paths.push_back(a);
		}
	}
	NODE_TYPE get_end() { return nodeEnd; }
	NODE_TYPE get_start() { return nodeStart; }
	vector< vector<NODE_TYPE> >& get_paths() { return paths; }
	void output() {
		if (paths.size() == 0)
		{
			//// cout << " no path" << endl;
			return ;
		}
		// cout << paths.size() << " paths in total " << endl;
		for (auto iter = paths.begin(); iter != paths.end(); iter++)
		{
			for (auto iter2 = iter->begin(); iter2 != iter->end(); iter2++)
			{
				// cout << *iter2 << " ";
			}
			// cout << endl;
		}
	}
	vector<vector<NODE_TYPE> > get_paths_with_distance(int distance)
	{
		if (distance < 0)
		{
			vector<vector<NODE_TYPE> > a;
			//// cout << "distance must be no less than 0 " << endl;
			return a;
		}
		else {
			vector< vector<NODE_TYPE> > result;
			for (auto iter = paths.begin(); iter != paths.end(); iter++)
			{
				if (iter->size() <= distance + 1)
				{
					result.push_back(*iter);
				}
			}
			return result;
		}
	}
	void add_path(two_nodes_path_index t)
	{
		if (nodeStart != t.get_start() || nodeEnd != t.get_end())
		{
			// cout << "two path share not the same start node and end node " << endl;
		}
		else
		{
			for (auto iter = t.get_paths().begin(); iter != t.get_paths().end(); iter++)
			{
				paths.push_back(*iter);
			}
		}
	}
};

class start_paths {
public:
	map<NODE_TYPE, two_nodes_path_index > t_path;// map end to paths
	set<NODE_TYPE> reach_nodes;
	NODE_TYPE start_node;// = -1;
	start_paths(NODE_TYPE s) :start_node(s) {}
	start_paths(two_nodes_path_index &path, NODE_TYPE start)
	{
		start_node = start;
		reach_nodes.insert(path.get_end());
		auto iter = t_path.find(path.get_end());
		if (iter != t_path.end())//there is an element
		{
			iter->second.add_path(path);
		}
		else {
			//vector<two_nodes_path_index> temp;
			t_path.insert(map<NODE_TYPE, two_nodes_path_index> ::value_type(path.get_end(),path) );
			//iter->second = path;
			//iter->second.add_path(path);
		}
	}
	paths get_paths();

	void push_back(two_nodes_path_index path)
	{
		if (path.nodeStart != start_node)
		{
			// cout << " not the same start node " << endl;
			return;
		}
		else
		{
			auto iter = t_path.find(path.get_end());
			if (iter != t_path.end())//there is an element
			{
				iter->second.add_path(path);
			}
			else {
				//vector<two_nodes_path_index> temp;
				//iter->second = temp;
				//iter->second.add_path(path);
				t_path.insert(map<NODE_TYPE, two_nodes_path_index> ::value_type(path.get_end(), path));
			}
		}
	}
	vector< vector<NODE_TYPE> > get_paths_from_end_nodes(NODE_TYPE end_node, int distance)
	{
		vector< vector<NODE_TYPE> > result;
		auto iter = t_path.find(end_node);
		if (iter != t_path.end())//there is an element
		{
			result = iter->second.get_paths_with_distance(distance);
			return result;
			//iter->second.add_path(path);
		}
		else {// there is no element
			return result;
		}
	}
	void output()
	{
		for (auto iter = t_path.begin(); iter != t_path.end(); iter++)
		{
			// cout << "end node : " << iter->first << " path :";
			iter->second.output();
		}
	}
};


//bool operator < (const vector<NODE_TYPE> &m, const vector<NODE_TYPE> &n) {
//	auto iter1 = m.begin();
//	auto iter2 = n.begin();
//	while (iter1 != m.end() && iter2 != n.end())
//	{
//		if (*iter1 < *iter2) {
//			return true;
//		}
//		else if (*iter1 > *iter2)
//		{
//			return false;
//		}
//		else {
//			iter1++;
//			iter2++;
//		}
//	}
//	if (m.size() < n.size())
//	{
//		return true;
//	}
//	return false;
//}	
//bool operator == (const vector<NODE_TYPE> &m, const vector<NODE_TYPE> &n)  {
//	auto iter1 = m.begin();
//	auto iter2 = n.begin();
//	if (m.size() != n.size())
//	{
//		return false;
//	}
//	while (iter1 != m.end() && iter2 != n.end())
//	{
//		if (*iter1 != *iter2) {
//			return false;
//		}
//		else {
//			iter1++;
//			iter2++;
//		}
//	}
//	return true;
//}

class paths {
public:
	vector< vector<NODE_TYPE> > path;
	paths() {};
	paths(vector<vector<NODE_TYPE > > a) : path(a) {}

	inline void drop_repeat_paths_with_sort()
	{
		sort(path.begin(), path.end());
		auto it = unique(path.begin(), path.end());
		path.erase(it, path.end());
	}

	void sort_by_string_order()
	{
		stable_sort(path.begin(), path.end(), less< vector<NODE_TYPE>>());
	}
	void drop_path_with_stop_nodes(set<NODE_TYPE>& stop_nodes)//, DISTANCE_TYPE startDis)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (intersect_vector_set(*iter, stop_nodes))
			{
				iter = path.erase(iter);
			}
			else 
			{
				iter++;
			}
		}
	}

	void drop_path_with_stop_nodes_with_distance_range(set<NODE_TYPE>& stop_nodes, DISTANCE_TYPE startDis, DISTANCE_TYPE endDis)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (intersect_vector_set_start_end_dis(*iter, stop_nodes,startDis,endDis))
			{
				iter = path.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}


	void drop_repeat_path()
	{
		if (path.size() <= 1)
		{
			return;
		}
		auto iter1 = path.begin();
		auto iter2 = path.begin();
		iter2++;
		while (iter2 != path.end())
		{
			if (*iter1 == *iter2)
			{
				iter2 = path.erase(iter2);
			}
			else {
				iter1++;
				iter2++;
			}
		}
	}
	void clear()
	{
		path.clear();
	}
	void push_back(vector<NODE_TYPE> a)
	{
		path.push_back(a);
	}
	void output()
	{
		// cout << "paths output : " << endl;
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				 cout << *iter1 << " ";
			}
			 cout <<" end " << endl;
		}
	}
	vector<vector<NODE_TYPE> > & get_path()
	{
		return path;
	}
	void add_paths(paths a)
	{
		for (auto iter = a.get_path().begin(); iter != a.get_path().end(); iter++)
		{
			path.push_back(*iter);
		}
	}
	void write_to_file_append(string filename)
	{
		ofstream out;
		out.open(filename,ios::app);
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}
	void write_to_file(string filename)
	{
		ofstream out;
		out.open(filename);
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}
	void clear_file(string filename)
	{
		ofstream out;
		out.open(filename, fstream::out | ios_base::trunc);
		out.close();
	}
	void write_to_file_append_edgeID_and_result_size(string filename, NODE_TYPE edgeID, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		ofstream out;
		out.open(filename, std::ofstream::out | std::ofstream::app);
		out << edgeID << " : " << query_node1 << " : " << query_node2 << " : " << path.size() << endl;
		out.close();
	}
	void write_to_file_append_edgeID(string filename, NODE_TYPE edgeID, NODE_TYPE query_node1, NODE_TYPE query_node2)
	{
		ofstream out;
		out.open(filename,std::ofstream::out | std::ofstream::app);
		out << edgeID << " : " <<  query_node1 << " : " << query_node2 << " : " << path.size() <<endl;
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			for (auto iter1 = iter->begin(); iter1 != iter->end(); iter1++)
			{
				out << *iter1 << " ";
			}
			out << endl;
		}
		out.close();
	}
	void drop_path_length_less_than_k(NODE_TYPE k)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (iter->size() < k+1)
			{
				iter = path.erase(iter);
			}
			else {
				iter++;
			}
		}
	}
	void drop_path_not_start_from_nodeS(NODE_TYPE s)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (*(iter->begin()) != s )
			{
				iter = path.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	bool intersect_vector_set(vector<NODE_TYPE>& v, set<NODE_TYPE> &s)
	{
		for (auto iter = v.begin(); iter != v.end(); iter++)
		{
			if (s.find(*iter) != s.end())// has common nodes
			{
				return true;
			}
		}
		return false;
	}


	bool intersect_vector_set_start_end_dis(vector<NODE_TYPE>& v, set<NODE_TYPE> &s, DISTANCE_TYPE startDis, DISTANCE_TYPE endDis)
	{

		for(auto i = startDis; i < v.size()-1-endDis; i++)
		{
			if(s.find(v[i]) != s.end())// has common nodes
			{
				return true;
			}
		}
		return false;
	}

	bool containsDuplicate(const vector<NODE_TYPE>& v)
	{
		for(int i = 0; i < v.size()-1;i++)
		{
			for(int j = i+1; j <v.size();j++)
			{
				if (v[i] == v[j])
					return true;
			}
		}
		return false;
	}


	bool containsDuplicate_middle(vector<NODE_TYPE> v)
	{
		sort(v.begin(), v.end());
		auto it = unique(v.begin(), v.end());
		if(it != v.end())
		{
			return true;
		}
		return false;

	}

	bool containsDuplicate_slow(const vector<NODE_TYPE>& v)
	{
		unordered_set<NODE_TYPE> s(v.size() * 2);
		for (auto x : v)
		{
			if (!s.insert(x).second)
				return true;
		}
		return false;
	}

	void drop_path_with_repeat_node()
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			//set<NODE_TYPE> c_path(iter->begin(),iter->end());
			//if (iter->size() > c_path.size())
			if (containsDuplicate(*iter))
			{
				iter = path.erase(iter);
			}
			else {
				iter++;
			}
		}
	}
	void drop_path_length_more_than_k_drop_repeated_node(NODE_TYPE k)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (iter->size() > k + 1 || containsDuplicate(*iter))
			{
				iter = path.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	void drop_path_length_more_than_k(NODE_TYPE k)
	{
		for (auto iter = path.begin(); iter != path.end();)
		{
			if (iter->size() > k + 1)
			{
				iter = path.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	void reverse()
	{
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			std::reverse(iter->begin(), iter->end());
		}
	}
	map<DISTANCE_TYPE, paths> construct_distance_paths(vector<vector<NODE_TYPE>>& p)
	{
		map<DISTANCE_TYPE, paths> result;
		for (auto iter = p.begin(); iter != p.end(); iter++)
		{
			auto iter2 = result.find(iter->size() - 1);
			if (iter2 == result.end())
			{
				paths temp;
				temp.push_back(*iter);
				result.insert(std::make_pair(iter->size() - 1, temp));
			}
			else
			{
				iter2->second.push_back(*iter);
			}
		}
		return result;
	}
	map<DISTANCE_TYPE, paths> construct_distance_paths(paths& p)
	{
		map<DISTANCE_TYPE, paths> result;
		for (auto iter = p.path.begin(); iter != p.path.end(); iter++)
		{
			auto iter2 = result.find(iter->size() - 1);
			if (iter2 == result.end())
			{
				paths temp;
				temp.push_back(*iter);
				result.insert(std::make_pair(iter->size() - 1, temp));
			}
			else
			{
				iter2->second.push_back(*iter);
			}
		}
		return result;
	}
	paths join_remove_repeat_nodes_only_join_right_sizeor_minus_one(paths& a, DISTANCE_TYPE k)// result path's length must be no more than distance
	{
		paths result;
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			//construct map<size, paths>
			map<DISTANCE_TYPE, paths> left_distance_paths = construct_distance_paths(path);
			map<DISTANCE_TYPE, paths> right_distance_paths = construct_distance_paths(a);
			for (int i = 1; i <= k - (k / 2); i++)
			{
				auto iter_left = left_distance_paths.find(i);
				auto iter_right = right_distance_paths.find(i);
				auto iter_right_minus_one = right_distance_paths.find(i - 1);
				if (iter_left == left_distance_paths.end() || (iter_right == right_distance_paths.end() && iter_right_minus_one == right_distance_paths.end()) )
				{// left or right is empty
					continue;
				}
				paths left_paths = iter_left->second;
				paths right_paths;
				if (iter_right == right_distance_paths.end())
				{
					right_paths = iter_right_minus_one->second;
				}
				else if (iter_right_minus_one == right_distance_paths.end())
				{
					right_paths = iter_right->second;
				}
				else
				{
					right_paths = iter_right->second;
					right_paths.add_paths(iter_right_minus_one->second);
				}
				for (auto iter1 = left_paths.path.begin(); iter1 != left_paths.path.end(); iter1++)
				{
					for (auto iter2 = right_paths.path.begin(); iter2 != right_paths.path.end(); iter2++)
					{
						if (iter1->back() != iter2->front())
						{
							//// cout << " end points must equal to start point in two path " << endl;
							continue;
						}
						else
						{
							unordered_set<NODE_TYPE> temp_set(iter1->begin() - 1, iter1->end() - 1);
							bool skip = false;
							for (int i = 1; i < iter2->size() - 1; i++)
							{
								if (!temp_set.insert(iter2->at(i)).second)
								{
									skip = true;
									break;
								}
							}
							if (skip)
							{
								continue;
							}
							vector<NODE_TYPE> temp_path(*iter1);
							temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
							result.push_back(temp_path);
						}
					}
				}
			}
		}
		return result;
	}



	paths join_remove_repeat_nodes(paths& a)// result path's length must be no more than distance
	{
		paths result;
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else
					{
						unordered_set<NODE_TYPE> temp_set(iter1->begin()-1, iter1->end() - 1);
						bool skip = false;
						for (int i = 1; i < iter2->size()-1; i++)
						{
							if (!temp_set.insert(iter2->at(i)).second)
							{
								skip = true;
								break;
							}
						}
						if (skip)
						{
							continue;
						}
						vector<NODE_TYPE> temp_path(*iter1);
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						result.push_back(temp_path);
					}
				}
			}
		}
		return result;
	}

	paths join(paths& a)// result path's length must be no more than distance
	{
		paths result;
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if(path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else
					{

						vector<NODE_TYPE> temp_path(*iter1);
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						result.push_back(temp_path);
					}
				}
			}
		}
		return result;
	}
	NODE_TYPE get_min_distance()
	{
		bool first_updated = true;
		NODE_TYPE min_distance = 0;
		for (auto iter = path.begin(); iter != path.end(); iter++)
		{
			if (iter->size() == 0)
			{
				continue;
			}
			if (first_updated) {
				min_distance = iter->size()-1;
				first_updated = false;
			}
			else {
				if (iter->size()-1 < min_distance)
				{
					min_distance = iter->size()-1;
				}
			}
		}
		return min_distance;
	}

	void join_drop_longpaths_and_repeat_nodes(paths& a, NODE_TYPE distance, paths& result)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				unordered_map<NODE_TYPE, bool> duplicated;
				for(int i =0; i < iter1->size(); i++)
				{
					duplicated.insert(std::make_pair((*iter1)[i], true));
				}
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else if (iter1->size() + iter2->size()  > distance + 2) {
						continue;
					}
					bool force_con = false;
					for(int j = 1; j < iter2->size(); j++)
					{
						if(duplicated.find((*iter2)[j] ) != duplicated.end())
						{
							force_con = true;
							break;
						}
					}
					if (force_con) continue;
					vector<NODE_TYPE> temp_path(*iter1);
					temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
					result.push_back(temp_path);
				}
			}
		}
		//return result;
	}

	inline void join_drop_longpaths_and_repeat_nodes_and_short_paths(paths& a, NODE_TYPE distance, paths& result,DISTANCE_TYPE short_dis)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				unordered_map<NODE_TYPE, bool> duplicated;
				for (int i = 0; i < iter1->size(); i++)
				{
					duplicated.insert(std::make_pair((*iter1)[i], true));
				}
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else if (iter1->size() + iter2->size()  > distance + 2 || iter1->size() + iter2->size() <= short_dis+2) {
						continue;
					}
					bool force_con = false;
					for (int j = 1; j < iter2->size(); j++)
					{
						if (duplicated.find((*iter2)[j]) != duplicated.end())
						{
							force_con = true;
							break;
						}
					}
					if (force_con) continue;
					vector<NODE_TYPE> temp_path(*iter1);
					temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
					result.push_back(temp_path);
				}
			}
		}
		//return result;
	}


	void join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis(paths& a, NODE_TYPE distance, paths& result, DISTANCE_TYPE short_dis, set<NODE_TYPE>& stop_nodes, DISTANCE_TYPE start_dis, DISTANCE_TYPE end_pos_dis)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			
			//result.add_paths(path);//should be remain?
			return;
		}
		else if (path.size() == 0)
		{
			//result.add_paths(a);
			return;
		}
		else
		{
			unordered_map<NODE_TYPE, bool> stop_map;
			for(auto iter_stop_map = stop_nodes.begin();  iter_stop_map != stop_nodes.end(); iter_stop_map++)
			{
				stop_map.insert(std::make_pair(*iter_stop_map,true));
			}
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{

				unordered_map<NODE_TYPE, bool> duplicated;
				for (int i = 0; i < iter1->size(); i++)
				{
					duplicated.insert(std::make_pair((*iter1)[i], true));
				}
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else if (iter1->size() + iter2->size()  > distance + 2 || iter1->size() + iter2->size() <= short_dis + 2) 
					{
						continue;
					}
					bool force_con = false;
					for (int j = 1; j < iter2->size(); j++)
					{
						if (duplicated.find((*iter2)[j]) != duplicated.end())
						{
							force_con = true;
							break;
						}
					}
					if (force_con) continue;
				
					vector<NODE_TYPE> temp_path(*iter1);
					if(stop_nodes.empty())
					{
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						result.push_back(temp_path);
					}
					else {
						bool force_continue = false;
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						for(auto iter_temp_path = temp_path.begin()+start_dis; iter_temp_path != temp_path.end()-end_pos_dis; iter_temp_path++)
						{
							if(stop_map.find(*iter_temp_path) != stop_map.end())//in stop map
							{
								force_continue = true;
								break;
							}
						}
						if(force_continue)
						{
							continue;
						}
						//vector <NODE_TYPE > v;
						//set_intersection(temp_path.begin() + start_dis, temp_path.end() - end_pos_dis, stop_nodes.begin(), stop_nodes.end(), back_inserter(v));
						//if (!v.empty())// here is at least a stop nodes
						//{
						//	continue;;
						//}
						result.push_back(temp_path);
					}
				}
			}
		}
		//result.drop_repeat_paths_with_sort();
		//return result;
	}

	void join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning(paths& a, NODE_TYPE distance, paths& result, DISTANCE_TYPE short_dis, set<NODE_TYPE>& stop_nodes, DISTANCE_TYPE start_dis, DISTANCE_TYPE end_pos_dis)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			//result.add_paths(path);//should be remain?
		}
		else if (path.size() == 0)
		{
			//result.add_paths(a);
		}
		else
		{
			unordered_map<NODE_TYPE, bool> stop_map;
			for (auto iter_stop_map = stop_nodes.begin(); iter_stop_map != stop_nodes.end(); iter_stop_map++)
			{
				stop_map.insert(std::make_pair(*iter_stop_map, true));
			}

			map<DISTANCE_TYPE, paths> left_distance_paths = construct_distance_paths(path);
			map<DISTANCE_TYPE, paths> right_distance_paths = construct_distance_paths(a);
			for(auto left_len = left_distance_paths.begin(); left_len != left_distance_paths.end(); left_len++)
			{
				for(auto right_len = right_distance_paths.begin(); right_len != right_distance_paths.end(); right_len ++)
				{
					DISTANCE_TYPE total_dis = left_len->first + right_len->first;
					if (total_dis  > distance || total_dis <= short_dis)//skip paths whose length do not meet our requirments
					{
						continue;
					}
					else
					{
						for (auto iter1 = left_len->second.path.begin(); iter1 != left_len->second.path.end(); iter1++)
						{

							unordered_map<NODE_TYPE, bool> duplicated;
							for (int i = 0; i < iter1->size(); i++)
							{
								duplicated.insert(std::make_pair((*iter1)[i], true));
							}
							for (auto iter2 = right_len->second.path.begin(); iter2 != right_len->second.path.end(); iter2++)
							{

								if (iter1->back() != iter2->front())
								{
									//// cout << " end points must equal to start point in two path " << endl;
									continue;
								}

								bool force_con = false;
								for (int j = 1; j < iter2->size(); j++)
								{
									if (duplicated.find((*iter2)[j]) != duplicated.end())
									{
										force_con = true;
										break;
									}
								}
								if (force_con) continue;

								vector<NODE_TYPE> temp_path(*iter1);
								if (stop_nodes.empty())
								{
									temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
									result.push_back(temp_path);
								}
								else {
									bool force_continue = false;
									temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
									for (auto iter_temp_path = temp_path.begin() + start_dis; iter_temp_path != temp_path.end() - end_pos_dis; iter_temp_path++)
									{
										if (stop_map.find(*iter_temp_path) != stop_map.end())//in stop map
										{
											force_continue = true;
											break;
										}
									}
									if (force_continue)
									{
										continue;
									}
									//vector <NODE_TYPE > v;
									//set_intersection(temp_path.begin() + start_dis, temp_path.end() - end_pos_dis, stop_nodes.begin(), stop_nodes.end(), back_inserter(v));
									//if (!v.empty())// here is at least a stop nodes
									//{
									//	continue;;
									//}
									result.push_back(temp_path);
								}
							}
						}
					}
				}
			}
		}

			
			
		//result.drop_repeat_paths_with_sort();
		//return result;
	}

	void join_drop_longpaths_and_repeat_nodes_and_short_paths_stop_ndoeswithdis_with_length_fast_pruning_include_meetnode(paths& a, NODE_TYPE distance, paths& result, DISTANCE_TYPE short_dis, set<NODE_TYPE>& stop_nodes, DISTANCE_TYPE start_dis, DISTANCE_TYPE end_pos_dis, NODE_TYPE meetnode)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			//result.add_paths(path);//should be remain?
		}
		else if (path.size() == 0)
		{
			//result.add_paths(a);
		}
		else
		{
			unordered_map<NODE_TYPE, bool> stop_map;
			for (auto iter_stop_map = stop_nodes.begin(); iter_stop_map != stop_nodes.end(); iter_stop_map++)
			{
				stop_map.insert(std::make_pair(*iter_stop_map, true));
			}

			map<DISTANCE_TYPE, paths> left_distance_paths = construct_distance_paths(path);
			map<DISTANCE_TYPE, paths> right_distance_paths = construct_distance_paths(a);
			for (auto left_len = left_distance_paths.begin(); left_len != left_distance_paths.end(); left_len++)
			{
				for (auto right_len = right_distance_paths.begin(); right_len != right_distance_paths.end(); right_len++)
				{
					DISTANCE_TYPE total_dis = left_len->first + right_len->first;
					if (total_dis  > distance || total_dis <= short_dis)//skip paths whose length do not meet our requirments
					{
						continue;
					}
					else
					{
						for (auto iter1 = left_len->second.path.begin(); iter1 != left_len->second.path.end(); iter1++)
						{

							unordered_map<NODE_TYPE, bool> duplicated;
							for (int i = 0; i < iter1->size(); i++)
							{
								duplicated.insert(std::make_pair((*iter1)[i], true));
							}
							for (auto iter2 = right_len->second.path.begin(); iter2 != right_len->second.path.end(); iter2++)
							{

								if (iter1->back() != iter2->front())
								{
									//// cout << " end points must equal to start point in two path " << endl;
									continue;
								}

								bool force_con = false;
								for (int j = 1; j < iter2->size(); j++)
								{
									if (duplicated.find((*iter2)[j]) != duplicated.end())
									{
										force_con = true;
										break;
									}
								}
								if (force_con) continue;

								vector<NODE_TYPE> temp_path(*iter1);
								temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
								bool not_include_meetnode = true;
								for (auto iter_temp_path = temp_path.begin() + start_dis; iter_temp_path != temp_path.end() - end_pos_dis; iter_temp_path++)
								{
									if (*iter_temp_path == meetnode)//in stop map
									{
										not_include_meetnode = false;
										break;
									}
								}
								if(not_include_meetnode)
								{
									continue;
								}

								if (stop_nodes.empty())
								{
									
									result.push_back(temp_path);
								}
								else {
									bool force_continue = false;
									for (auto iter_temp_path = temp_path.begin() + start_dis; iter_temp_path != temp_path.end() - end_pos_dis; iter_temp_path++)
									{
										if (stop_map.find(*iter_temp_path) != stop_map.end())//in stop map
										{
											force_continue = true;
											break;
										}
									}
									if (force_continue)
									{
										continue;
									}
									//vector <NODE_TYPE > v;
									//set_intersection(temp_path.begin() + start_dis, temp_path.end() - end_pos_dis, stop_nodes.begin(), stop_nodes.end(), back_inserter(v));
									//if (!v.empty())// here is at least a stop nodes
									//{
									//	continue;;
									//}
									result.push_back(temp_path);
								}
							}
						}
					}
				}
			}
		}



		//result.drop_repeat_paths_with_sort();
		//return result;
	}




	void join(paths& a, NODE_TYPE distance, paths& result)// result path's length must be no more than distance
	{
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else if (iter1->size() + iter2->size()  > distance + 2) {
						continue;
					}
					else
					{
						vector<NODE_TYPE> temp_path(*iter1);
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						result.push_back(temp_path);
					}
				}
			}
		}
		//return result;
	}



	paths join(paths& a, NODE_TYPE distance)// result path's length must be no more than distance
	{
		paths result;
		if (a.get_path().size() == 0)
		{
			result.add_paths(path);
		}
		else if (path.size() == 0)
		{
			result.add_paths(a);
		}
		else
		{
			for (auto iter1 = path.begin(); iter1 != path.end(); iter1++)
			{
				for (auto iter2 = a.path.begin(); iter2 != a.path.end(); iter2++)
				{
					if (iter1->back() != iter2->front())
					{
						//// cout << " end points must equal to start point in two path " << endl;
						continue;
					}
					else if (iter1->size()+iter2->size()  > distance + 2) {
						continue;
					}
					else
					{
						vector<NODE_TYPE> temp_path(*iter1);
						temp_path.insert(temp_path.end(), iter2->begin() + 1, iter2->end());
						result.push_back(temp_path);
					}
				}
			}
		}
		return result;
	}
};

class d_path {
public:
	vector<NODE_TYPE> path;
	int path_length;// = 0;
	d_path() {}
	d_path(vector<NODE_TYPE>a, int size_b) : path(a), path_length(size_b) {}
	d_path(vector<NODE_TYPE>a) : path(a) { path_length = a.size() - 1; }
	void push_back(NODE_TYPE node) { path.push_back(node); path_length++; }
	NODE_TYPE get_first() { return path[0]; }
	NODE_TYPE get_last() { return path[path.size() - 1]; }
	bool check_path_size() { return path_length == path.size() - 1; }
};


class cur_path
{
public:
	vector< vector<NODE_TYPE > > path;
	cur_path (vector<vector<NODE_TYPE> > a) : path(a){};
	cur_path(){};
	void push_back(vector<NODE_TYPE> temp)
	{
		path.push_back((temp));
	}
	NODE_TYPE get_length(){
		NODE_TYPE length = 0;
		for(auto iter = path.begin(); iter!= path.end(); iter++)
		{
			length += iter->size();
		}
		return length;
	}
	paths get_path()
	{
		paths result;
		for(auto iter = path.begin(); iter != path.end(); iter++)
		{
			paths temp;
			temp.push_back(*iter);
			//// cout << "temp:";
			//temp.output();
			result = result.join(temp);
			//// cout << "result :";
			//result.output();
		}
		return result;
	}
};

class path_index {
public:
	map<NODE_TYPE ,start_paths > t_index;// map start nodes to start_paths, in start_paths, map endnodes to path_two_nodes
	set<NODE_TYPE> hot_points;
	path_index() {}
	path_index(set<NODE_TYPE> hot) : hot_points(hot) {};
	path_index(two_nodes_path_index t) {
		NODE_TYPE start_node = t.get_start();
		auto iter = t_index.find(start_node);
		if (iter != t_index.end())//there is an element
		{
			start_paths temp_s(t, t.get_start());
			iter->second = temp_s;
		}
		else {
			iter->second.push_back(t);
		}
	}
	void push_back(two_nodes_path_index t) {
		NODE_TYPE start_node = t.get_start();
		auto iter = t_index.find(start_node);
		if (iter != t_index.end())//there is an element
		{
			iter->second.push_back(t);
		}
		else {
			start_paths temp(start_node);
			temp.push_back(t);
			t_index.insert(map<NODE_TYPE, start_paths > :: value_type(start_node,temp));
//			start_paths temp_s(t, t.get_start());
//			iter->second = temp_s;
		}
	}


	void push_back(paths t) {
		for (auto path = t.get_path().begin(); path != t.get_path().end(); path++)
		{
			NODE_TYPE start_node = path->front();
			auto iter = t_index.find(start_node);
			if (iter != t_index.end())//there is an element
			{
				two_nodes_path_index temp_t(path->front(),path->back());
				temp_t.push_back(*path);
				iter->second.push_back(temp_t);
			}
			else {
				two_nodes_path_index temp_t(path->front(), path->back());
				temp_t.push_back(*path);
				start_paths temp(start_node);
				temp.push_back(temp_t);
				t_index.insert(map<NODE_TYPE, start_paths > ::value_type(start_node, temp));
				//			start_paths temp_s(t, t.get_start());
				//			iter->second = temp_s;
			}
		}
	}



	paths find_paths_between_two_hot_nodes(NODE_TYPE node1, NODE_TYPE node2, int distance)// distance means length of path
	{
		paths result;
		auto iter1 = t_index.find(node1);
		if (iter1 != t_index.end())//there is an element
		{
			result = iter1->second.get_paths_from_end_nodes(node2,distance);//find all paths' path less or equal than distance bt node1 and node2

			return result;
		}
		else// there is no element
		{
			//// cout << node1 << ":" << node2 << ":" << distance << " int find hot paths " << endl;
			return result;
		}
	}

	void find_paths_between_two_hot_nodes_index_without_cross_other_hotpoints(paths &result, cur_path c_path,NODE_TYPE node1, NODE_TYPE node2, int distance, int cur_distance)// distance means length of path
	{
		//paths result;
		if (node1 == node2)
		{
			return;
		}
		auto iter1 = t_index.find(node1);
		if(iter1 == t_index.end())
		{
			return;
		}
		paths temp_paths = iter1->second.get_paths();
		for(auto iter2 = temp_paths.path.begin(); iter2 != temp_paths.path.end();iter2++)
		{
			if (iter2->size() == 0)
			{
				continue;
			}
			int temp_distance = cur_distance + iter2->size()-1;
			//// cout << temp_distance << " : temp_distance " << endl;
			if(temp_distance > distance)
			{
				continue;
			}
			else{
				cur_path temp_c_path(c_path.path);
				temp_c_path.push_back(*iter2);
			// add temp_result to result 
				if(iter2->back() == node2)
				{
					paths temp_result = temp_c_path.get_path();
					//// cout << " result path" << endl;
					//temp_result.output();
					result.add_paths(temp_result);
				}
				else
				{
					//cur_distance = temp_distance;
					NODE_TYPE start_node = iter2->back();
					//// cout << "start node" << start_node << endl;
					find_paths_between_two_hot_nodes_index_without_cross_other_hotpoints(result,temp_c_path,start_node,node2,distance,temp_distance);
				}
			}
		}
	}


	//vector< vector<NODE_TYPE> >  find_paths_with_index(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse,NODE_TYPE query_node1, NODE_TYPE query_node2, int k)
	//{
	//	vector<vector<NODE_TYPE> > result;

//		return result;
//	}
	void output() {
		for (auto iter = t_index.begin(); iter != t_index.end(); iter++)
		{
			// cout << " start node : " << iter->first << " path";
			iter->second.output();
		}
	}
};


class short_cut_index
{
public:
	unordered_map<NODE_TYPE, unordered_map<NODE_TYPE, map<DISTANCE_TYPE, paths>>> short_cuts;
	void push_back(NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE dis, paths& path)
	{
		auto iter = short_cuts.find(query_node1);
		if (iter == short_cuts.end())//first time to insert query_node1
		{
			unordered_map<NODE_TYPE, map<DISTANCE_TYPE, paths>> insert_query_node1;
			map<DISTANCE_TYPE, paths> temp_distance;
			temp_distance.insert(std::make_pair(dis, path));
			insert_query_node1.insert(std::make_pair(query_node2, temp_distance));
			short_cuts.insert(std::make_pair(query_node1, insert_query_node1));
			return;
			//node_indexed = false;
		}
		else
		{
			auto iter1 = iter->second.find(query_node2);
			if (iter1 == iter->second.end())//first time to insert query_node2
			{
				map<DISTANCE_TYPE, paths> temp_distance;
				temp_distance.insert(std::make_pair(dis, path));
				iter->second.insert(std::make_pair(query_node2, temp_distance));
				return;
			}
			else
			{
				auto iter2 = iter1->second.find(dis);
				if (iter2 == iter1->second.end())//first time to insert dis
				{
					iter1->second.insert(std::make_pair(dis, path));
					return;
				}
				else
				{
					iter2->second.add_paths(path);
					return;
				}
			}
		}
	}

	void construct_src_dst_short_paths(NODE_TYPE src, NODE_TYPE dst, DISTANCE_TYPE k, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph_reverse)
	{
		dfs_without_recursion_construct_short_paths_for_single_node(induced_subgraph, src, k);
		dfs_without_recursion_construct_short_paths_for_single_node_reverse(induced_subgraph_reverse, dst, k,src);//src is to remove duplicated for edge src to dst
	}

	void dfs_without_recursion_construct_short_paths_for_single_node(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, int k)// distance means length of path
	{
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
			return;
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
			//iter = cur_iters[cur_distance];
			//if (*iter == dst)//dst 
			//{
			//	cur_iters[cur_distance]++;
			//	continue;
			//}
			//if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			//{
			//	// add result path and continue;

			//	vector<NODE_TYPE> temp_result_path(c_path_v);
			//	//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
			//	temp_result_path.push_back(*iter);
			//	vector<NODE_TYPE> new_temp;
			//	//std::reverse(temp_result_path.begin(), temp_result_path.end());
			//	auto iter_result = result.find(*iter);
			//	if (iter_result == result.end())//first time to insert
			//	{
			//		paths temp_paths;
			//		temp_paths.push_back(temp_result_path);
			//		result.insert(std::make_pair(*iter, temp_paths));
			//	}
			//	else
			//	{
			//		iter_result->second.push_back(temp_result_path);
			//	}
			//	//cur_iters[cur_distance]++;
			//}

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
			paths temp_paths;
			//NODE_TYPE* c_end = &c_path.top() + sizeof(NODE_TYPE);
		//	NODE_TYPE* c_begin = c_end - c_path.size();
			//vector<NODE_TYPE> c_path_v;//(c_begin,c_end);
			c_path_v.push_back(cur_node);
			temp_paths.push_back(c_path_v);
			push_back(src, cur_node, c_path_v.size()-1, temp_paths);
			//cur_iters[cur_distance]++;

		}
		return;
	}


	void dfs_without_recursion_construct_short_paths_for_single_node_reverse(unordered_map<NODE_TYPE, set<NODE_TYPE>> &reverse_adjacency_in_subgraph, NODE_TYPE src, int k, NODE_TYPE start_node)// distance means length of path
	{
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
			return;
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
			//iter = cur_iters[cur_distance];
			//if (*iter == dst)//dst 
			//{
			//	cur_iters[cur_distance]++;
			//	continue;
			//}
			//if (meet_nodes.find(*iter) != meet_nodes.end())//cur_node is in meet nodes
			//{
			//	// add result path and continue;

			//	vector<NODE_TYPE> temp_result_path(c_path_v);
			//	//vector<NODE_TYPE> temp_result_path(c_path., c_path.end());
			//	temp_result_path.push_back(*iter);
			//	vector<NODE_TYPE> new_temp;
			//	//std::reverse(temp_result_path.begin(), temp_result_path.end());
			//	auto iter_result = result.find(*iter);
			//	if (iter_result == result.end())//first time to insert
			//	{
			//		paths temp_paths;
			//		temp_paths.push_back(temp_result_path);
			//		result.insert(std::make_pair(*iter, temp_paths));
			//	}
			//	else
			//	{
			//		iter_result->second.push_back(temp_result_path);
			//	}
			//	//cur_iters[cur_distance]++;
			//}

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
			//NODE_TYPE* c_end = &c_path.top() + 1;
			//NODE_TYPE* c_begin = c_end - c_path.size();
			//vector<NODE_TYPE> c_path_v2(c_begin, c_end);
			c_path_v.push_back(cur_node);
			if (c_path_v.size() != 2 || cur_node != start_node)
			{
				paths temp_paths;
				temp_paths.push_back(c_path_v);
				temp_paths.reverse();
				push_back(cur_node, src, c_path_v.size() - 1, temp_paths);
			}
				//cur_iters[cur_distance]++;

		}
		return;
	}


	paths find_short_cuts_using_index(NODE_TYPE query_node1, NODE_TYPE query_node2, DISTANCE_TYPE distance, bool &node_indexed)
	{
		node_indexed = false;
		paths results;
		auto iter = short_cuts.find(query_node1);
		if(iter == short_cuts.end())
		{
			//node_indexed = false;
			return results;
		}
		else
		{
			auto iter1 = iter->second.find(query_node2);
			if(iter1 == iter->second.end())
			{
				return results;
			}
			else
			{
				node_indexed = true;
				for(auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++)
				{
					if(iter2->first > distance)//end since here is a ordered map
					{
						if (results.path.size() == 0)
						{
							node_indexed = false;
						}
						return results;
					}
					else
					{
						results.add_paths(iter2->second);
					}
				}
				
				return results;
				/*auto iter2 = iter1->second.find(distance);
				if(iter2 == iter1->second.end())
				{
					return results;
				}
				else
				{
					node_indexed = true;
					results = iter2->second;
					return results;
				}*/
			}
		}
	}
};

string reverse_path(string path);
bool circle_detection_string(string path);//true means there is a circle
void data_clean();
string DatetimeToString(time_t time);
time_t StringToDatetime(string str);
bool circle_detection(vector<NODE_TYPE> path);
NODE_TYPE find_max_node();
int split(char sdt[][BUFFER_LENTH], char* str, const char* spl);
void extractAliEdges(char str[], NODE_TYPE &x, NODE_TYPE &y);
void load_ali_data(vector<NODE_TYPE>* adjacency_list, vector<NODE_TYPE>* adjacency_list_reverse);
void load_graph_data(char* filename, vector<NODE_TYPE >* adjacency_list,vector<NODE_TYPE >* adjacency_list_reverse);
void extractEdges(char str[], NODE_TYPE  &x, NODE_TYPE  &y);
void output_adjacency_list(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num);
paths single_direction_baseline(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
void double_direction_baseline(vector<NODE_TYPE >* adjacency_list,vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
//void hot_point_index_algorithm(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
path_index construct_hot_point_index(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, set<NODE_TYPE> hot_points);
set<NODE_TYPE> find_hot_points(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, double threshold);
void double_direction_unordered(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
void printpath(vector<NODE_TYPE>& path);
int isNotVisited(NODE_TYPE x, vector<NODE_TYPE>& path);
two_nodes_path_index findpaths(vector<NODE_TYPE>* g, NODE_TYPE src, NODE_TYPE dst, int k);
paths find_all_paths_with_hotpoints(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points, path_index index);
paths find_all_paths_use_parents(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  src, NODE_TYPE  dst, NODE_TYPE  distance, map<NODE_TYPE, vector<NODE_TYPE > >* parents);
paths find_all_paths_use_parents_single_distance(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  src, NODE_TYPE  dst, NODE_TYPE  distance, map<NODE_TYPE, vector<NODE_TYPE > >* parents);
paths find_all_paths_with_hotpoints_step_by_step(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points, path_index &index);
paths find_all_meet_paths(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE src, NODE_TYPE dst, meet_nodes nodes_nohot, map<NODE_TYPE, vector<NODE_TYPE > >* parents_left, map<NODE_TYPE, vector<NODE_TYPE > >*parents_right, NODE_TYPE k);
paths single_direction_baseline_stop_at_hotpoints(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k,set<NODE_TYPE> hot_points);
void load_ali_undirected_data(vector<NODE_TYPE>* adjacency_list_double, vector<NODE_TYPE>* adjacency_list, vector<NODE_TYPE>* adjacency_list_reverse);
void dfs(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, map<NODE_TYPE, paths> &result, NODE_TYPE cur_node, vector<NODE_TYPE> c_path, set<NODE_TYPE> stop_nodes, int distance, int cur_distance, int& min_stop_disatance, set<NODE_TYPE> c_path_set);
path_index construct_hot_point_index_dfs(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list,  NODE_TYPE  k, set<NODE_TYPE> hot_points);
paths find_all_paths_with_hotpoints_dfs(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points, path_index &index);
void load_undirected_graph_data(char* filename, vector<NODE_TYPE >* adjacency_list_double);
spd_distance_map single_direction_baseline_stop_at_hotpoints_dfs_spd(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points);
void single_direction_baseline_stop_at_hotpoints_bfs_spd(vector<NODE_TYPE >* adjacency_list, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  k, set<NODE_TYPE> hot_points, spd_index& index);
void dfs_find_k_paths(vector<NODE_TYPE>* adjacency_list, NODE_TYPE cur_node, NODE_TYPE query_node1, NODE_TYPE query_node2, int k, paths &result, set<NODE_TYPE> c_path_set, int cur_distance, vector<NODE_TYPE> c_path);
paths find_k_paths_between_two_nodes_spd(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE k, NODE_TYPE query_node1, NODE_TYPE query_node2, spd_index& index);
void load_graph_data(string filename, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse);
//paths double_direction_baseline_paths(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
paths double_direction_baseline_paths(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k);
paths find_all_paths_with_hotpoints_dfs_get_update_time(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points, path_index &index, double& update_time);
void dfs_ex_node2(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, map<NODE_TYPE, paths> &result, NODE_TYPE cur_node, vector<NODE_TYPE> c_path, set<NODE_TYPE> stop_nodes, int distance, int cur_distance, int& min_stop_disatance, set<NODE_TYPE> c_path_set, NODE_TYPE query_node2);
path_index construct_hot_point_index_dfs_using_new_algorithm(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, NODE_TYPE  k, set<NODE_TYPE> hot_points, NODE_TYPE node_num);
paths find_all_paths_with_hotpoints_dfs_get_update_time_new_algor(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, unordered_map<NODE_TYPE, set<NODE_TYPE>>& induced_subgraph, unordered_map<NODE_TYPE, set<NODE_TYPE>>& induced_subgraph_reverse, NODE_TYPE  node_num, NODE_TYPE  query_node1, NODE_TYPE  query_node2, NODE_TYPE  k, set<NODE_TYPE> hot_points, path_index &index, double& update_time);
void random_query_pair(NODE_TYPE & query_node1, NODE_TYPE &query_node2, NODE_TYPE nodenum);
void write_random_query_edges(string dataset, int k, NODE_TYPE query_node1, NODE_TYPE query_node2, NODE_TYPE edgeIndex);
void write_random_query_edges_withspd(string dataset, int k, NODE_TYPE query_node1, NODE_TYPE query_node2, NODE_TYPE edgeIndex, DISTANCE_TYPE spd);
int bfs_return_spd(vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, NODE_TYPE query_node2);
map<NODE_TYPE, DISTANCE_TYPE> bfs_return_spd_pairs(unordered_map<NODE_TYPE, set<NODE_TYPE>> &induced_subgraph, NODE_TYPE  node_num, NODE_TYPE  k, NODE_TYPE query_node1, set<NODE_TYPE>& cut_nodes);
set<NODE_TYPE> find_hot_points_top_t(vector<NODE_TYPE >* adjacency_list_double, vector<NODE_TYPE >* adjacency_list, vector<NODE_TYPE >* adjacency_list_reverse, NODE_TYPE  node_num, NODE_TYPE  k, int t);

#include <BFS_prune.h>
#include <unordered_map>
#include <Snap.h>
#include <sys/time.h>
#define FORN(i, n) for (NODE_NUM_TYPE i = 0; i < n; ++i)
typedef int DIST_TYPE;
typedef long long NODE_NUM_TYPE;

NODE_NUM_TYPE randint(NODE_NUM_TYPE a, NODE_NUM_TYPE b) // [a,b)
{
    NODE_NUM_TYPE n = b - a;
    return rand() % n + a;
}

int k_hop_rand_walk(vector<NODE_TYPE> *adjacency_list,
                    vector<NODE_TYPE> *reverse_adjacency_list,
                    NODE_TYPE s, DIST_TYPE k, bool rev = 0)
{
    NODE_NUM_TYPE deg;
    NODE_TYPE nextid;
    for (DIST_TYPE i = 0; i < k; ++i)
    {
        if (rev)
        {
            deg = reverse_adjacency_list[s].size();
            if (deg == 0)
                return s;
            nextid = reverse_adjacency_list[s][randint(0, deg)];
        }
        else
        {
            deg = adjacency_list[s].size();
            if (deg == 0)
                return s;
            nextid = adjacency_list[s][randint(0, deg)];
        }
        s = nextid;
    }
    return s;
}

bool test(vector<NODE_TYPE> *adj, vector<NODE_TYPE> *rev_adj, NODE_NUM_TYPE Gsize, int s, int t, int k)
{
    std::cout << k << '\n';
    timeval a, b;
    double total = 0;
    {
        gettimeofday(&a,0);
        paths tmp;
        pruned_subgraph_unordered_map_double_direction_triple_join temp_triple;
        temp_triple.construct_pruned_subgraph_with_find_leftnodes_rightnodes_commonnodes(adj, rev_adj, Gsize, k, s, t);
        tmp = temp_triple.find_all_k_pahts_triple_join(adj, rev_adj, Gsize, k, s, t);
        gettimeofday(&b,0);
        std::cout << double((b.tv_sec-a.tv_sec)*1000*1000+(b.tv_usec-a.tv_usec))/(1000*1000)
        << ' ' << tmp.get_path().size() << '\n';
        total += b.tv_sec-a.tv_sec;
        if(total > 1000 || tmp.get_path().size() > 10000000)return 0;
    }
    return 1;
}

int main()
{
    vector<NODE_TYPE> *adj, *rev_adj;
    NODE_NUM_TYPE Gsize;
    {
        PNGraph G;
        TFIn Fin("../data/web-Google.graph");//("../socJournal.graph");
        std::cout << "get G" << endl;
        G = TNGraph::Load(Fin);
        Gsize = G->GetNodes();
        adj = new vector<NODE_TYPE>[Gsize + 5];
        rev_adj = new vector<NODE_TYPE>[Gsize + 5];
        for (auto iter = G->BegEI(); iter != G->EndEI(); iter++)
        {
            adj[iter.GetSrcNId()].push_back(iter.GetDstNId());
            rev_adj[iter.GetDstNId()].push_back(iter.GetSrcNId());
        }
    }

    ios::sync_with_stdio(false);
    srand(time(NULL));
    for (int i = 0; i < 50; ++i)
    {
        NODE_TYPE t,s;
        t = k_hop_rand_walk(adj, rev_adj, 0, 30);
        s = k_hop_rand_walk(adj, rev_adj, t, 30, 1);
        if (s == t)
            continue;
        std::cout << s << ' ' << t << endl;
        for (int k = 3; test(adj, rev_adj, Gsize, s, t, k) 
            && k < 20; ++k)
        {
        }
        std::cout << "st End" << '\n';
    }
    delete[] adj;
    delete[] rev_adj;
}


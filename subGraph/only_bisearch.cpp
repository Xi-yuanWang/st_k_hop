#include <BFS_prune.h>
#include <unordered_map>
#include <Snap.h>
#include <sys/time.h>
#define FORN(i, n) for (NODE_NUM_TYPE i = 0; i < n; ++i)
typedef int DIST_TYPE;
typedef int NODE_NUM_TYPE;
class SubGraph
{
public:
    vector<NODE_TYPE> node;
    unordered_map<NODE_TYPE, NODE_TYPE> old2new;
    NODE_NUM_TYPE node_num;
    vector<DIST_TYPE> lt;
    vector<vector<NODE_TYPE>> adjacency_list, reverse_adjacency_list;
    SubGraph(){}
    void init(vector<NODE_TYPE> &node_, NODE_NUM_TYPE node_num_,
             vector<vector<NODE_TYPE>> adjacency_list_old,
             const vector<DIST_TYPE> &lt_old)
    {
        node_num=node_num_;
        node=node_;
        node.resize(node_num);
        lt.resize(node_num);

        for (NODE_NUM_TYPE i = 0; i < node_num; ++i)
        {
            old2new.emplace(node[i], i);
        }
        for (NODE_NUM_TYPE i = 0; i < node_num; ++i)
        {
            lt[i] = lt_old[node[i]];
        }
        adjacency_list.resize(node_num);
        reverse_adjacency_list.resize(node_num);
        for (NODE_NUM_TYPE i = 0; i < node_num; ++i)
        {
            NODE_TYPE cur = node[i];
            for (NODE_TYPE next : adjacency_list_old[cur])
            {
                auto iter = old2new.find(next);
                if (iter != old2new.end())
                {
                    adjacency_list[i].push_back(iter->second);
                    reverse_adjacency_list[iter->second].push_back(i);
                }
            }
        }
    }
    NODE_TYPE getoldid(NODE_TYPE newid)
    {
        return node[newid];
    }
    NODE_TYPE getnewid(NODE_TYPE oldid)
    {
        return old2new[oldid];
    }
    ~SubGraph()
    {
    }
};

class Bisearch
{
public:
    vector<vector<NODE_TYPE>>& adjacency_list, &reverse_adjacency_list;
    vector<DIST_TYPE> ls, lt;
    NODE_NUM_TYPE node_num;
    vector<NODE_TYPE> qs, qt, critical_node;
    vector<NODE_TYPE> subGnode;
    SubGraph subG;
    vector<vector<NODE_TYPE>> path;
    NODE_TYPE s, t;
    DIST_TYPE k;
    vector<NODE_TYPE> stk;

    Bisearch(vector<vector<NODE_TYPE>>& adjacency_list_init,
             NODE_NUM_TYPE input_node_num, vector<vector<NODE_TYPE>>& reverse_adjacency_list_init):
             adjacency_list(adjacency_list_init),reverse_adjacency_list(reverse_adjacency_list_init)
    {
        node_num = input_node_num;
        ls.resize(node_num);
        lt.resize(node_num);
    }
    void backProp_s(NODE_TYPE cur)
    {
        DIST_TYPE lt_cur = lt[cur];
        for (NODE_TYPE prev : reverse_adjacency_list[cur])
        {
            DIST_TYPE lt_prev = lt[prev], ls_prev = ls[prev];
            if (lt_prev > lt_cur + 1 && ls_prev + lt_cur + 1 <= k)
            {
                if (lt_prev + ls_prev > k)
                {
                    subGnode.push_back(prev);
                }
                lt[prev] = lt_cur + 1;
                backProp_s(prev);
            }
        }
    }
    void backProp_t(NODE_TYPE cur)
    {
        DIST_TYPE ls_cur = ls[cur];
        for (NODE_TYPE nextid : adjacency_list[cur])
        {
            DIST_TYPE ls_next = ls[nextid], lt_next = lt[nextid];
            if (ls_next > ls_cur + 1 && lt_next + ls_cur + 1 <= k)
            {
                if (ls_next + lt_next > k)
                {
                    subGnode.push_back(nextid);
                }
                ls[nextid] = ls_cur + 1;
                backProp_t(nextid);
            }
        }
    }
    void init(NODE_TYPE s_, NODE_TYPE t_, DIST_TYPE k_)
    {
        s = s_;
        t = t_;
        k = k_;
        FORN(i, node_num)
        {
            lt[i] = k + 2;
        }
        FORN(i, node_num)
        {
            ls[i] = k + 2;
        }
        qs.clear();
        qt.clear();
        critical_node.clear();
        subGnode.clear();
    }
    void func(NODE_TYPE s_, NODE_TYPE t_, DIST_TYPE k_)
    {
        init(s_, t_, k_);
        vector<NODE_TYPE> tqs, tqt;
        NODE_NUM_TYPE lqs = adjacency_list[s].size();
        NODE_NUM_TYPE lqt = reverse_adjacency_list[t].size();
        qs.push_back(s);
        qt.push_back(t);
        ls[s] = 0;
        lt[t] = 0;
        for (DIST_TYPE i = 0; i < k; ++i)
        {
            if (lqs <= 0 || lqt <= 0)
            {
                break;
            }
            else if (lqs < lqt)
            {
                tqs.clear();
                lqs = 0;
                for (NODE_TYPE cur : qs)
                {
                    for (NODE_TYPE nextid : adjacency_list[cur])
                    {
                        if (ls[nextid] > k)
                        {
                            tqs.push_back(nextid);
                            ls[nextid] = ls[cur] + 1;
                            lqs += adjacency_list[nextid].size();
                            if (ls[nextid] + lt[nextid] <= k)
                            {
                                critical_node.push_back(nextid);
                            }
                        }
                    }
                }
                swap(qs, tqs);
            }
            else
            {
                tqt.clear();
                lqt = 0;
                for (NODE_TYPE cur : qt)
                {
                    for (NODE_TYPE previd : reverse_adjacency_list[cur])
                    {
                        if (lt[previd] > k)
                        {
                            tqt.push_back(previd);
                            lt[previd] = lt[cur] + 1;
                            lqt += reverse_adjacency_list[previd].size();
                            if (ls[previd] + lt[previd] <= k)
                            {
                                critical_node.push_back(previd);
                            }
                        }
                    }
                }
                swap(qt, tqt);
            }
        }
        for (NODE_TYPE cn : critical_node)
        {
            backProp_s(cn);
            backProp_t(cn);
        }
        for (NODE_TYPE cn : critical_node)
        {
            subGnode.push_back(cn);
        }
    }
    void dfs(NODE_TYPE cur, NODE_TYPE tar)
    {
        if (cur == tar)
        {
            stk.push_back(tar);
            path.push_back(stk);
            stk.pop_back();
            path.pop_back();// 防止内存溢出
            return;
        }
        stk.push_back(cur);
        DIST_TYPE tlt = subG.lt[cur];
        subG.lt[cur] = k + 2;
        for (NODE_TYPE next : subG.adjacency_list[cur])
        {
            if (subG.lt[next] + stk.size() <= k)
            {
                dfs(next, tar);
            }
        }
        subG.lt[cur] = tlt;
        stk.pop_back();
    }
    void getSubG()
    {
        subG.init(subGnode, subGnode.size(), adjacency_list, lt);
    }
    void getPath()
    {
        stk.clear();
        if (subGnode.empty())
            return;
        getSubG();
        path.clear();
        dfs(subG.getnewid(s), subG.getnewid(t));
    }
    ~Bisearch()
    {
    }
};

NODE_NUM_TYPE randint(NODE_NUM_TYPE a, NODE_NUM_TYPE b) // [a,b)
{
    NODE_NUM_TYPE n = b - a;
    return rand() % n + a;
}

int k_hop_rand_walk(vector<vector<NODE_TYPE>> adjacency_list,
                    vector<vector<NODE_TYPE>> reverse_adjacency_list,
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

bool test(vector<vector<NODE_TYPE> > &adj, vector<vector<NODE_TYPE> > rev_adj, NODE_NUM_TYPE Gsize, int s, int t, int k)
{
    cout << k << '\n';
    double total = 0;
    Bisearch bi(adj, Gsize, rev_adj);
    timeval a, b;

    gettimeofday(&a, 0);
    bi.func(s, t, k);
    gettimeofday(&b, 0);
    std::cout << double((b.tv_sec - a.tv_sec) * 1000 * 1000 + (b.tv_usec - a.tv_usec)) / (1000 * 1000)
              << ' ' << '\n';
    total += b.tv_sec - a.tv_sec;

    gettimeofday(&a, 0);
    bi.getPath();
    gettimeofday(&b, 0);
    cout << double((b.tv_sec - a.tv_sec) * 1000 * 1000 + (b.tv_usec - a.tv_usec)) / (1000 * 1000) << ' ' << bi.path.size() << '\n';
    total += b.tv_sec - a.tv_sec;

    return total < 1000 && bi.path.size() < 10000000;
}

int main()
{
    vector<vector<NODE_TYPE>> adj, rev_adj;
    NODE_NUM_TYPE Gsize;
    {
        PNGraph G;
        TFIn Fin("../data/web-Google.graph"); //("../socJournal.graph"); //
        cout << "get G" << endl;
        G = TNGraph::Load(Fin);
        Gsize = G->GetNodes();
        adj.resize(Gsize);
        rev_adj.resize(Gsize);
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
        NODE_TYPE t = k_hop_rand_walk(adj, rev_adj, 0, 30);
        NODE_TYPE s = k_hop_rand_walk(adj, rev_adj, t, 30, 1);
        if (s == t)
            continue;
        cout << s << ' ' << t << endl;
        for (int k = 3; test(adj, rev_adj, Gsize, s, t, k) && k < 20; ++k)
        {
            cout << "k End" << endl;
        }
        cout << "st End" << '\n';
    }
    cout << "End" << endl;
}
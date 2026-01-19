#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <string>

struct adjEdge //邻接边
{
    int to;
    double w;
};


struct node
{
    // int id;
    // 这里的id事实上就是它在容器V中的位置，所以此处可以省略
    std::string name;
    std::vector<adjEdge> adj;
};

class graph{
    public:
        graph(): V(), name2id(), id2name(){}

        void addEdge(std::string src, std::string dest, double weight){
            // unordered_map会自动插入元素，这意味着我们甚至不需要单独处理添加节点的逻辑！
            // 上面这一句说法是错的。为了维护哈希表中value的值和vector V中的节点下标的对应关系，
            // 我们还是需要显式分配id的
            int src_id = getId(src);
            int dest_id = getId(dest);
            addEdge(src_id, dest_id, weight);
            addEdge(dest_id, src_id, weight);
        }

        int vsize() const {return V.size();}

        // 获取name -> neighbor的函数是不必要的，因为我们在写算法的时候实际上只需要操作id
        const std::vector<adjEdge>& neighbors(int u) const { return V[u].adj; }
        const std::string& name(int id) const { return id2name[id]; }
        int idOf(const std::string& name) const {
            auto it = name2id.find(name);
            if (it == name2id.end()) return -1;
            return it->second;
        }



    private:
        std::unordered_map<std::string, int> name2id; // 用来存储地点名称到id的映射，其底层是一个哈希表
        std::vector<std::string> id2name;
        std::vector<node> V;

        int getId(const std::string& name){ // 这个函数可以维护name到id的映射关系，在节点不存在的时候自动加入
            auto it = name2id.find(name);
            if(it != name2id.end()) return it->second; // 成功找到，返回value
            
            int id = V.size();
            name2id[name] = id;

            node nd;
            nd.name = name;
            V.push_back(nd);

            id2name.push_back(name);
            return id;
        }

        void addEdge(int src, int dest, double w){
            // 假设src和dest都在V中已经存在（内部调用，可以保证这一点）
            // 检查是否已经存在 src -> dest 的边
            auto it = std::find_if(V[src].adj.begin(), V[src].adj.end(),
                                   [dest](const adjEdge& edge) {
                                       return edge.to == dest;
                                   });
            
            if (it == V[src].adj.end()) {
                // 边不存在，添加新边
                V[src].adj.push_back({dest, w});
            } else {
                // 边已存在
                it->w = w;
            }
        }

};
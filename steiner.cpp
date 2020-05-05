#include <iostream>
#include <fstream>
#include <cstdint>
#include <list>
#include <array>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <array>
#include <iterator>
#include <string>
#include <regex>
#include <unordered_map>
#include <set>
#include <queue>
#include <utility>
#include <algorithm>


enum Type
{
    NONE,
    TERM,
    PATH
};

class Point
{
public:
    int x;
    int y;
    int status;

    Point(int x_, int y_):x(x_), y(y_){}

    Point(int x_, int y_, int _status):x(x_), y(y_), status(_status) {}
    bool operator==(const Point& that) 
    {
        if(this->x == that.x && this->y == that.y)
            return true;
        return false;
    }

    bool operator<(const Point& that) const
    {
        if(this->y < that.y)
            return true;
       else if(this->y == that.y)
       {
            if(this->x < that.x)
                return true;
       }    
       return false;
            
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& point);
};

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os <<  "(" << point.x << ", " << point.y << ")";
    return os;
}

class Edge
{
public:
    int u;
    int v;
    int weight;
    Edge(int u_, int v_): u(u_), v(v_){}
    Edge(int u_, int v_, int weight_): u(u_), v(v_), weight(weight_){}
};

bool comparator(const Edge& e1, const Edge& e2)
{
    return (e1.weight < e2.weight);
}


std::vector<Point> parsePoints(std::ifstream& file)
{
    std::vector<Point> points;
    std::string temp;
    std::regex re(".*point.*x=\"([0-9]+).*y=\"([0-9]+)\".*");

    while(std::getline(file, temp))
    {

        std::smatch match;
        if(std::regex_search(temp, match, re))
        {
            int x = std::stoi(match[1]);
            int y = std::stoi(match[2]);
            //Point p = {x, y};
            points.push_back(Point(x, y, TERM));
        }
   
    }
    
    return points;
}

/*----------------------------------------------------*/
int dist(Point& a, Point& b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
class Dsu
{
public:
    Dsu(int n);
    void makeSet(int x);
    void unite(int r, int s);
    int find(int x);
private:
    std::vector<int> parents;
};

Dsu::Dsu(int n)
{
    parents.resize(n);
}

void Dsu::makeSet(int x)
{
    parents[x] = x;
}

int Dsu::find(int x)
{
    if(x == parents[x])
        return x;
    parents[x] = find(parents[x]);
    return parents[x];
}

void Dsu::unite(int a, int b)
{
    a = find(a);
    b = find(b);
    if(a != b)
        parents[b] = a;
}
/*-----------------------------------------------------*/


struct addInfo
{
    std::vector<int> edges;
    int id;
};
class Graph
{
public:
    Graph(std::vector<Point>& points): nodes(points){}
    Graph() {}
    int addNode(Point node);
    int addNodeComplete(Point node);
    int addEdge(Edge edge);
    void deleteNode(int info);
    void dump();
    Graph* getMst();

    int cost = 0;
    std::vector<Point> nodes;
    std::vector<Edge> edges;

};

void Graph::dump()
{
    for(auto n : nodes)
        std::cout << n << "\n";
    std::cout << "\n";

    
    for(auto e : edges)
        std::cout   << nodes[e.u]
                    << " --- " << nodes[e.v]
                    << " : " << e.weight << "\n";
    
    std::cout << "cost = " << cost << "\n";

}


int Graph::addNodeComplete(Point node)
{
    int id = nodes.size();
    for(int i = 0; i < nodes.size(); i++)
    {
        int d = dist(nodes[i], node);
        edges.push_back(Edge(i, id, d));
    }
    nodes.push_back(node);
    return id;
}

int Graph::addNode(Point node)
{
    int id = nodes.size();
    nodes.push_back(node);
    return id;
}

void Graph::deleteNode(int id)
{
    auto iter = nodes.begin() + id;
    
    std::vector<int> tmp;
    
    for(int i = 0; i < edges.size(); i++)
    {
        if(edges[i].u == id || edges[i].v == id)
            tmp.push_back(i);
    }
    
    for(int i = tmp.size() - 1; i >= 0; i--)
        edges.erase(edges.begin() + tmp[i]); 
    
    nodes.erase(iter);
}


int Graph::addEdge(Edge edge)
{
    assert(edge.u < nodes.size() && edge.v < nodes.size());
    int id = edges.size();
    edges.push_back(edge);
    return id;
}

Graph* Graph::getMst()
{
    Dsu dsu(nodes.size());

    Graph* g = new Graph(nodes);
    std::sort(edges.begin(), edges.end(), comparator);
    for(int i = 0; i < nodes.size(); i++)
        dsu.makeSet(i);

    for(auto e : edges)
    {
        if(dsu.find(e.u) != dsu.find(e.v))
        {
            g->cost += e.weight;
            g->addEdge(e);
            dsu.unite(e.u, e.v);
        }
    }

    return g;
}

Graph* createMst(std::vector<Point>& points)
{
    Graph* g = new Graph();
    for(auto& p : points)
        g->addNodeComplete(p);
    auto mst = g->getMst();
    delete g;
    return mst;
}



enum Layer
{
    PINS,
    M2,
    M3, 
    M2_M3,
    PINS_M2,
    PINS_M3
};

std::unordered_map<int, const char*> layers = {
    {PINS, "pins"},
    {M2, "m2"},
    {M3, "m3"},
    {M2_M3, "m2_m3"},
    {PINS_M2, "pins_m2"},
    {PINS_M3, "pins_m3"}};


struct OutPoint
{
    int x;
    int y;
    int layer;

    friend std::ostream& operator<<(std::ostream& os, const OutPoint& point);
};

std::ostream& operator<<(std::ostream& os, const OutPoint& point)
{
    os  << "<point x=\"" << point.x << "\" y=\"" << point.y 
        << "\" layer=\"" << layers[point.layer] << "\" />";
    return os;
}

struct Segment
{
    Point a;
    Point b;
    int layer;
    
    friend std::ostream& operator<<(std::ostream& os, const Segment& segment);
};

std::ostream& operator<<(std::ostream& os, const Segment& segment)
{
    os  << "<segment x1=\"" << segment.a.x << "\" y1=\"" << segment.a.y
        << "\" x2=\"" << segment.b.x << "\" y2=\"" << segment.b.y
        << "\" layer=\"" << layers[segment.layer] << "\" />";
    return os;
}


class OutputGenerator
{
public:
    void feed(Graph* g); 
    void generate(std::ifstream& input, std::ofstream& output);
private:
    std::vector<Segment> segments;
    std::vector<OutPoint> points;
};

void OutputGenerator::feed(Graph* g)
{
    int xsz = 0;
    int ysz = 0;

    for(auto& n : g->nodes)
    {
        if(xsz < n.x)
            xsz = n.x;
        if(ysz < n.y)
            ysz = n.y;
    }
    xsz++;
    ysz++;
    std::vector< std::vector<int> > vmap(ysz, std::vector<int> (xsz));
    std::vector< std::vector<int> > hmap(ysz, std::vector<int> (xsz));
    for(auto& edge : g->edges)
    {
        auto u = g->nodes[edge.u];
        auto v = g->nodes[edge.v];
        Point tmp(u.x, v.y);

        int i = u.x;
        if(u.x < v.x)
            for(int i = u.x; i <= v.x; i++)
                hmap[u.y][i] = 1;
        else if(u.x > v.x)
            for(int i = u.x; i >= v.x; i--)
                hmap[u.y][i] = 1;

        if(u.y < v.y)
            for(int i = u.y; i <= v.y; i++)
                vmap[i][v.x] = 1;
        else if(u.y > v.y)
            for(int i = u.y; i >= v.y; i--)
                vmap[i][v.x] = 1;
    }

    /*
    for(int i = 0; i < ysz; i++)
    {
        for(int j = 0; j < xsz; j++)
            std::cerr << vmap[i][j] << " ";
        std::cerr << "\n";
    }
    */
    for(int i = 0; i < ysz; i++)
    {
        for(int j = 0; j < xsz; j++)
        {
            int start = j;
            int end = start;
            if(hmap[i][j] == 1)
            {
                while(hmap[i][j++] == 1)
                {
                    end = j - 1;
                    if(j == xsz)
                        break;
                }

            }
            if(start != end)
                segments.push_back({ {start, i}, {end, i}, M2 });
        }
    }
    
    for(int j = 0; j < xsz; j++)
    {
        for(int i = 0; i < ysz; i++)
        {
            int start = i;
            int end = start;
            if(vmap[i][j] == 1)
            {
                while(vmap[i++][j] == 1)
                {
                    end = i - 1;
                    if(i == ysz)
                        break;
                }
                
            }
            if(start != end)
                segments.push_back({ {j, start}, {j, end}, M3 });
        }
    }

    for(int i = 0; i < g->nodes.size(); i++)
    {
        auto node = g->nodes[i];
        if(node.status == TERM)
            points.push_back({node.x, node.y, PINS_M2});
        if(hmap[node.y][node.x] == 1 && vmap[node.y][node.x] == 1)
            points.push_back({node.x, node.y, M2_M3});
        if(hmap[node.y][node.x] == 0 && vmap[node.y][node.x] == 1 && node.status == TERM) 
        {
            points.push_back({node.x, node.y, M2});
            points.push_back({node.x, node.y, M2_M3});
        }
    }

}

void OutputGenerator::generate(std::ifstream& input, std::ofstream& output)
{
    std::string tmp;
    while(std::getline(input, tmp))
    {
        if(tmp.find("</net") != std::string::npos)
        {
            output << "\n";
            for(auto& p : points)
                output << "    " << p << "\n";
            for(auto& s: segments)
                output <<  "    " <<s << "\n";
        }
        output << tmp << "\n";
    }

}

/*
void OutputGenerator::feed(Graph* g)
{
    int xsz = 0;
    int ysz = 0;

    for(auto& n : g->nodes)
    {
        if(xsz < n.x)
            xsz = n.x;
        if(ysz < n.y)
            ysz = n.y;
    }
    xsz++;
    ysz++;
    std::vector< std::vector<int> > vmap(ysz, std::vector<int> (xsz));
    std::vector< std::vector<int> > hmap(ysz, std::vector<int> (xsz));
  
    for(int node1 = 0; node1 < g->adj.size(); node1++)
    {
        for(auto& node2 : g->adj[node1])
        {
            if(std::abs(g->nodes[node1].x - g->nodes[node2].x) == 1)
            {
                hmap[g->nodes[node1].y][g->nodes[node1].x] = 1;
                hmap[g->nodes[node2].y][g->nodes[node2].x] = 1;
            }
            if(std::abs(g->nodes[node1].y - g->nodes[node2].y) == 1)
            {
                vmap[g->nodes[node1].y][g->nodes[node1].x] = 1;
                vmap[g->nodes[node2].y][g->nodes[node2].x] = 1;
            }
        }
    }
    
    for(int i = 0; i < ysz; i++)
    {
        for(int j = 0; j < xsz; j++)
        {
            int start = j;
            int end = start;
            if(hmap[i][j] == 1)
            {
                while(hmap[i][j++] == 1)
                {
                    end = j - 1;
                    if(j == xsz)
                        break;
                }

            }
            if(start != end)
                segments.push_back({ {start, i}, {end, i}, M2 });
        }
    }
     
    for(int j = 0; j < xsz; j++)
    {
        for(int i = 0; i < ysz; i++)
        {
            int start = i;
            int end = start;
            if(vmap[i][j] == 1)
            {
                while(vmap[i++][j] == 1)
                {
                    end = i - 1;
                    if(i == ysz)
                        break;
                }
                
            }
            if(start != end)
                segments.push_back({ {j, start}, {j, end}, M3 });
        }
    }
    
    std::vector<bool> external(g->size);

    for(auto& segment : segments)
    {
        int idx1 = g->find(segment.a);
        int idx2 = g->find(segment.b);
        external[idx1] = true;
        external[idx2] = true;
    }

    for(int i = 0; i < g->size; i++)
    {
        auto node = g->nodes[i];
        if(node.status == TERM)
            points.push_back({node.x, node.y, PINS_M2});
        if(hmap[node.y][node.x] == 1 && vmap[node.y][node.x] == 1)
            points.push_back({node.x, node.y, M2_M3});
        if(hmap[node.y][node.x] == 0 && vmap[node.y][node.x] == 1 && node.status == TERM) 
        {
            points.push_back({node.x, node.y, M2});
            points.push_back({node.x, node.y, M2_M3});
        }
    }

}

void OutputGenerator::generate(std::ifstream& input, std::ofstream& output)
{
    std::string tmp;
    while(std::getline(input, tmp))
    {
        if(tmp.find("</net") != std::string::npos)
        {
            output << "\n";
            for(auto& p : points)
                output << "    " << p << "\n";
            for(auto& s: segments)
                output <<  "    " <<s << "\n";
        }
        output << tmp << "\n";
    }

}
*/

std::list<Point> findHananPoints(std::vector<Point>& points)
{
    int xsize = 0;
    int ysize = 0;
    for(auto& p : points)
    {
        if(xsize < p.x)
            xsize = p.x;
        if(ysize < p.y)
            ysize = p.y;
    }
    xsize++;
    ysize++;

    std::list<Point> hanan;
    std::vector< std::vector<int> > map(ysize, std::vector<int> (xsize));

    for(auto& p : points)
    {
        for(int x = 0; x < map[p.y].size(); x++)
        {
            if(map[p.y][x] != 2)
                map[p.y][x] += 1;
        }

        for(int y = 0; y < map.size(); y++)
        {
            if(map[y][p.x] != 2)
                map[y][p.x] += 1;
        }
    }
    for(auto& p : points)
        map[p.y][p.x] = 0; 

    for(int y = 0; y < map.size(); y++)
    {
        for(int x = 0; x < map[0].size(); x++)
        {
            if(map[y][x] == 2)
                hanan.push_back(Point(x, y));
        }
    }
    
    return hanan;
}

Graph* createSteiner(std::vector<Point>& points)
{
    auto hanan = findHananPoints(points);
    Graph temp;
    for(auto& p : points)
        temp.addNodeComplete(p);

    while(true)
    {
        int maxDelta = 0;
        auto optimal = hanan.end();

        auto init = temp.getMst();
        //init->dump();
        for(auto it = hanan.begin(); it != hanan.end(); it++)
        {
            auto info = temp.addNodeComplete(*(it));
            auto mst = temp.getMst();
            auto delta = init->cost - mst->cost;
            //std::cerr << delta << "\n";
            if(delta > maxDelta)
            {
                maxDelta = delta;
                optimal = it;
            }
            delete mst;
            temp.deleteNode(info);
        }
        delete init;
        if(optimal == hanan.end())
            break;
        hanan.erase(optimal);
        temp.addNodeComplete(*(optimal));
    }
    auto mst = temp.getMst();
    return mst;
}

int main(int argc, char* argv[])
{   
    if(argc == 1)
    {
        std::cerr << "No input file\n";
        std::exit(1);
    }

    std::string inName = std::string(argv[1]);
    std::ifstream input(inName);
    if(!input.is_open())
    {
        std::cerr << "Cannot open " << inName << "\n";
        std::exit(1);
    }
    std::string outName(inName);
    outName.replace(outName.begin() + outName.find(".xml"),
                    outName.end(), "_out.xml");

    auto points = parsePoints(input);


        
    auto steiner = createSteiner(points);
    //steiner->dump();
     
    input.clear();
    input.seekg(0);
    std::ofstream output(outName);  

    OutputGenerator generator;
    generator.feed(steiner);
    generator.generate(input, output);

    delete  steiner;
    
    return 0;
}



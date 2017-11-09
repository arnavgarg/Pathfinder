#include <vector>
#include <cstdio>
#include <cmath>

#include <opencv2/core.hpp>

class node
{
public:
    int line;
    int group;
    node* next = nullptr;

    node(int line) : line(line) {}
};

bool closeEnough(cv::Vec4i l1, cv::Vec4i l2);
bool intersects(cv::Vec4i l1, cv::Vec4i l2);
int orientation(int x1, int y1, int x2, int y2, int x3, int y3);
void addEdge(std::vector<node*> &graph, int i, int j);
void floodfill(std::vector<node*> &graph, int line, bool visited[], int group);
void dealocGraph(std::vector<node*> &graph);

/*
 * given a vector of lines, remove all lines that are not part of the biggest "clump"
 */
void eliminateOutliers(std::vector<cv::Vec4i> &lines)
{
    // creates a graph with a vertice for every line
    std::vector<node*> graph = std::vector<node*>(lines.size());
    for (int i = 0; i < graph.size(); i++)
        graph[i] = new node(i);

    // adds an edge between every pair of lines that intersect
    // or has points that are close enough
    for (int i = 0; i < lines.size(); i++)
        for (int j = i+1; j < lines.size(); j++)
            if (intersects(lines[i], lines[j]) || closeEnough(lines[i], lines[j]))
                addEdge(graph, i, j);

    // uses a floodfill to divide the graph into groups of connected vertices
    bool visited[graph.size()];
    for (int i = 0; i < graph.size(); i++)
        visited[i] = false;
    int group = 0;
    for (int i = 0; i < graph.size(); i++)
        if (!visited[i]) {
            floodfill(graph, i, visited, group);
            group++;
        }

    // counts the number of items in each group and finds the group with the most
    int groups[group];
    for (int i = 0; i < group; i++)
        groups[i] = 0;
    for (int i = 0; i < graph.size(); i++)
        groups[graph[i]->group]++;
    int max = 0;
    for (int i = 1; i < group; i++)
        if (groups[i] > groups[max])
            max = i;

//     erases all lines that are not part of biggest group
    for (int i = graph.size()-1; i >= 0; i--)
        if (graph[i]->group != max)
            lines.erase(lines.begin() + i);

    // deallocate graph memory
    dealocGraph(graph);
}

/*
 * adds an edge to the graph
 */
void addEdge(std::vector<node*> &graph, int i, int j)
{
    node* inode = graph[i];
    while (inode->next != nullptr)
        inode = inode->next;
    inode->next = new node(j);

    node* jnode = graph[j];
    while (jnode->next != nullptr)
        jnode = jnode->next;
    jnode->next = new node(i);
}

/*
 * checks if 2 line's points are close enough to have an edge
 */
bool closeEnough(cv::Vec4i l1, cv::Vec4i l2)
{
    double d1 = sqrt(pow(l1[0]-l2[0], 2) + pow(l1[1]-l2[1], 2));
    double d2 = sqrt(pow(l1[0]-l2[2], 2) + pow(l1[1]-l2[4], 2));
    double d3 = sqrt(pow(l1[2]-l2[0], 2) + pow(l1[3]-l2[1], 2));
    double d4 = sqrt(pow(l1[2]-l2[2], 2) + pow(l1[3]-l2[3], 2));

    // ensures we're not dividing by 0
    l1[0] += l1[0] == l1[2] ? 1 : 0;
    l2[0] += l2[0] == l2[2] ? 1 : 0;
    // if the lines are approximetly parallel, allow for further distance
    double s1 = (l1[1]-l1[3])/(l1[0]-l1[2]);
    double s2 = (l2[1]-l2[3])/(l2[0]-l2[2]);
    if (s1 * 0.9 > s2 && s1 * 1.1 < s2)
        return d1 < 12 || d2 < 12 || d3 < 12 || d4 < 12;

    return d1 < 7 || d2 < 7 || d3 < 7 || d4 < 7;
}

/*
 * checks if 2 lines intersect
 */
bool intersects(cv::Vec4i l1, cv::Vec4i l2) {
    int o1 = orientation(l1[0], l1[1], l2[0], l2[1], l1[2], l1[3]); // p1 q1 p2
    int o2 = orientation(l1[0], l1[1], l2[0], l2[1], l2[2], l2[3]); // p1 q1 q2
    int o3 = orientation(l1[2], l1[3], l2[2], l2[3], l1[0], l1[1]); // p1 q1 p1
    int o4 = orientation(l1[2], l1[3], l2[2], l2[3], l2[0], l2[1]); // p2 q2 q1
    return o2 != o1 && o3 != o4;
}

/*
 * returns orientation of 3 points given x and y coordinates
 * 1 - clockwise
 * 2 - counterclockwise
 */
int orientation(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // ensures we're not dividing by 0
    x1 += x1 == x2 ? 1 : 0;
    x3 += x3 == x2 ? 1 : 0;
    double slope1 = (y1 - y2)/(x1 - x2);
    double slope2 = (y2 - y3)/(x2 - x3);
    return slope1 > slope2 ? 1 : 2;
}

/*
 * finds all connected vertices
 */
void floodfill(std::vector<node*> &graph, int line, bool visited[], int group)
{
    if (visited[line])
        return;
    visited[line] = true;

    node* curr = graph[line];
    curr->group = group;
    while (curr->next)
    {
        curr = curr->next;
        floodfill(graph, curr->line, visited, group);
    }
}

/*
 * dealocates all memory in graph
 */
void dealocGraph(std::vector<node*> &graph)
{
    for (int i = 0; i < graph.size(); i++)
    {
        node* head = graph[i];
        while (head->next != nullptr)
        {
            node* next = head->next;
            delete head;
            head = next;
        }
        delete head;
    }
}


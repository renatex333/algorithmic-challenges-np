#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <omp.h>

// Author: Renato Laffranchi Falcão
// 12/2023

// This is a recursive implementation of the exhaustive search algorithm for the maximum clique problem with backtracking optimization.
// The algorithm is based on the following paper:
// "A review on algorithms for maximum clique problems" by Qinghua Wu and Jin-Kao Hao. European Journal of Operational Research. Volume 242, Issue 3, 1 May 2015, Pages 693-709.
// http://dx.doi.org/10.1016/j.ejor.2014.09.064

std::vector<std::vector<int>> readGraph(const std::string& filename, int& numVertices)
// Module to read graph from file, provided as an adjacency matrix.
{
    std::ifstream file(filename);
    int numEdges;
    file >> numVertices >> numEdges;

    std::vector<std::vector<int>> graph(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numEdges; i++) {
        int u, v;
        file >> u >> v;
        // The graph is undirected, so we add the edge in both directions
        graph[u - 1][v - 1] = 1;
        graph[v - 1][u - 1] = 1;
    }

    file.close();

    return graph;
}

void clique(std::vector<int>& currentSolution, std::deque<int>& candidateVertices, std::vector<int>& maximumClique, std::vector<std::vector<int>>& graph)
// Recursive function to find the maximum clique
{
    // Update maximum clique found so far
    if ( currentSolution.size() > maximumClique.size() )
    {
        maximumClique = currentSolution;
    }

    // Backtracking optimization: if the size of the current solution plus the amount of
    // candidate vertices left is smaller than the size of the maximum clique found so far,
    // there is no need to continue
    if ((currentSolution.size() + candidateVertices.size()) <= maximumClique.size())
    {
        return;
    }

    // Creates a recursive call for each candidate vertex and its neighbors
    for (int u : candidateVertices)
    {
        candidateVertices.pop_front();
        std::vector<int> largerSolution(currentSolution);
        largerSolution.push_back(u);
        std::deque<int> neighborVertices;
        for (int v : candidateVertices)
        {
            if (graph[u][v] == 1)
            {
                neighborVertices.push_back(v);
            }
        }
        clique(largerSolution, neighborVertices, maximumClique, graph);
    }
    return;
}

std::vector<int> findMaximumClique()
// Main function to manage the execution of the algorithm
{
    // Read graph from file
    int numVertices;
    const std::string filename = "files/graph.txt";
    std::vector<std::vector<int>> graph = readGraph(filename, numVertices);

    // Create support data structures
    std::vector<int> maximumClique;
    std::deque<int> candidateVertices;

    // Fill candidate vertices
    for (int i = 0; i < numVertices; i++)
    {
        candidateVertices.push_back(i);
    }

    // For each candidate vertex, assign a thread
    // to find the maximum clique based on that vertex neighbors.
    #pragma omp parallel for schedule(dynamic)
    for (size_t u = 0; u < candidateVertices.size(); u++)
    {
        std::vector<int> localMaximumClique;
        std::vector<int> largerSolution;
        largerSolution.push_back(u);
        std::deque<int> neighborVertices;
        for (int v : candidateVertices)
        {
            if (graph[u][v] == 1)
            {
                neighborVertices.push_back(v);
            }
        }
        // Find maximum clique based on the neighbors of the candidate vertex
        clique(largerSolution, neighborVertices, localMaximumClique, graph);

        // Merge each thread results into the global maximum clique
        #pragma omp critical
        {
            if (localMaximumClique.size() > maximumClique.size())
            {
                maximumClique = localMaximumClique;
            }
        }
    }

    return maximumClique;
}

int main(int argc, char *argv[])
{
    // Find maximum clique
    std::vector<int> maximumClique = findMaximumClique();

    // Optional argument to check the correctness of the answer
    size_t correctAnswerLen = 0;

    if (argc > 1)
    {
        correctAnswerLen = atoi(argv[1]);
    }

    if (correctAnswerLen > 0 && maximumClique.size() != correctAnswerLen)
    {
        std::cout << "Wrong answer: clique size is " << maximumClique.size() << ", correct answer is " << correctAnswerLen << std::endl;
        return 1;
    }

    // Optional argument to print the maximum clique found
    bool verbose = false;

    if (argc > 2)
    {
        std::string arg = argv[2];
        if (arg == "-v" || arg == "--verbose")
        {
            verbose = true;
        }
    }

    if (verbose)
    {
        std::cout << "Maximum clique: ";
        for (int v : maximumClique)
        {
            // Print with corrected vertex index for comparison (starts at 1)
            std::cout << v+1 << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}

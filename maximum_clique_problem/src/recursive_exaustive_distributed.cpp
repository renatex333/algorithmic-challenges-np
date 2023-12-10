#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <mpi.h>

// Author: Renato Laffranchi Falcão
// 12/2023

// This is a recursive implementation of the exaustive search algorithm for the maximum clique problem with backtracking optimization.
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

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    // rank: ID of process, from 0 to size-1, being "size" the total number of processes
    // size: The total number of processes
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Create variables to store graph data
    int numVertices;
    std::vector<std::vector<int>> graph;

    // Create vector to store maximum clique
    std::vector<int> maximumClique;

    if (rank == 0)
    {
        // Read graph from file
        const std::string filename = "files/graph.txt";
        graph = readGraph(filename, numVertices);
    }

    // Broadcast the number of vertices to all processes
    MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        // Processes resize the graph vector according to the number of vertices
        graph.resize(numVertices, std::vector<int>(numVertices));
    }

    // Broadcast the graph data to all processes
    for (int i = 0; i < numVertices; ++i) {
        MPI_Bcast(graph[i].data(), numVertices, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Main process (rank 0), assigns tasks to other processes
    if (rank == 0 && size > 1)
    {
        // Create deque of candidate vertices
        std::deque<int> candidateVertices;

        // Fill candidate vertices
        for (int i = 0; i < numVertices; i++)
        {
            candidateVertices.push_back(i);
        }

        // For each candidate vertex, commision a process
        // to find the maximum clique based on that vertex neighbors.
        int process = 1;
        for (size_t u = 0; u < candidateVertices.size(); u++)
        {
            // Create support vectors
            std::vector<int> currentSolution;
            std::vector<int> neighborVertices;

            // Add current vertex to solution
            currentSolution.push_back(u);

            // Add neighbors of current vertex to neighborVertices
            for (int v : candidateVertices)
            {
                if (graph[u][v] == 1)
                {
                    neighborVertices.push_back(v);
                }
            }

            // Serialize the size of vectors along with the data
            int currentSolutionSize = currentSolution.size();
            int neighborVerticesSize = neighborVertices.size();

            // Send size of currentSolution
            MPI_Send(&currentSolutionSize, 1, MPI_INT, process, process*10, MPI_COMM_WORLD);

            // Send currentSolution data
            MPI_Send(currentSolution.data(), currentSolutionSize, MPI_INT, process, process*11, MPI_COMM_WORLD);

            // Send size of neighborVertices
            MPI_Send(&neighborVerticesSize, 1, MPI_INT, process, process*12, MPI_COMM_WORLD);

            // Send neighborVertices data
            MPI_Send(neighborVertices.data(), neighborVerticesSize, MPI_INT, process, process*13, MPI_COMM_WORLD);

            // Move to the next process using Round-robin assignment
            process = (process % (size - 1)) + 1;
        }
    }

    // Other processes receive the data and execute the clique function
    else if (rank != 0)
    {
        // Receive size of currentSolution
        int currentSolutionSize;
        MPI_Recv(&currentSolutionSize, 1, MPI_INT, 0, rank*10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive currentSolution data
        std::vector<int> currentSolution(currentSolutionSize);
        MPI_Recv(currentSolution.data(), currentSolutionSize, MPI_INT, 0, rank*11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive size of neighborVertices
        int neighborVerticesSize;
        MPI_Recv(&neighborVerticesSize, 1, MPI_INT, 0, rank*12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive neighborVertices data
        std::vector<int> neighborVerticesVector(neighborVerticesSize);
        MPI_Recv(neighborVerticesVector.data(), neighborVerticesSize, MPI_INT, 0, rank*13, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Casts neighborVertices to deque
        std::deque<int> neighborVertices(neighborVerticesVector.begin(), neighborVerticesVector.end());

        // Call clique function
        clique(currentSolution, neighborVertices, maximumClique, graph);

        // Send size of maximum clique found by process
        int maximumCliqueSize = maximumClique.size();
        MPI_Send(&maximumCliqueSize, 1, MPI_INT, 0, rank*14, MPI_COMM_WORLD);

        // Send maximumClique data
        MPI_Send(maximumClique.data(), maximumCliqueSize, MPI_INT, 0, rank*15, MPI_COMM_WORLD);
    }

    // Main process (rank 0) receives the data from other processes and finds the maximum clique
    if (rank == 0 && size > 1)
    {
        for (int process = 1; process < size; process++) {
            // Receive the size of the clique
            int receivedSize;
            MPI_Recv(&receivedSize, 1, MPI_INT, process, process*14, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Receive the actual clique data
            std::vector<int> receivedClique(receivedSize);
            MPI_Recv(receivedClique.data(), receivedSize, MPI_INT, process, process*15, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Update the overall maximum clique
            if (receivedSize > (int) maximumClique.size())
            {
                maximumClique = receivedClique;
            }
        }
    }

    if (rank == 0)
    {
        // In the event that there is only one process,
        // the main process will call the clique function.
        if (size == 1)
        {
            // Create support vector
            std::vector<int> currentSolution;

            // Create deque of candidate vertices
            std::deque<int> candidateVertices;

            // Fill candidate vertices
            for (int i = 0; i < numVertices; i++)
            {
                candidateVertices.push_back(i);
            }

            // Find maximum clique
            clique(currentSolution, candidateVertices, maximumClique, graph);
        }

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
    }

    MPI_Finalize();
    return 0;
}

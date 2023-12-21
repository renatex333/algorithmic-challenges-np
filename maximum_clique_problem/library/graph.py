import networkx as nx

def generator(num_vertices = 50, conection_prob = 0.5, filename = "graph.txt", verbose = True):
    """
    Arguments:
    num_vertices: number of vertices in the graph
    conection_prob: probability of connection between two vertices
    filename: name of the output file
    verbose: if True, prints when the graph is generated and saved
    """

    # Create a random graph
    graph = nx.fast_gnp_random_graph(num_vertices, conection_prob)

    # Write the graph to a file
    with open(f"files/{filename}", "w") as file:
        # Header line with number of vertices and edges
        file.write(f"{num_vertices} {graph.number_of_edges()}\n")

        # Write the edges in the file
        for aresta in graph.edges():
            # +1 to convert from 0-indexed to 1-indexed
            file.write(f"{aresta[0]+1} {aresta[1]+1}\n")

    if verbose:
        print(f"Graph generated and saved in {filename}")

    return 0

def verifier(read_filename = "graph.txt", write_filename = "graph_cliques.txt", verbose = True):
    """
    Arguments:
    read_filename: name of the file to read the graph from
    write_filename: name of the file to write the results to
    verbose: if True, prints the maximals and maximum cliques found
    """

    # Open the file to read the graph
    with open(f"files/{read_filename}", "r") as file:
        # Skip header line
        next(file)

        # Read the graph from the file to a networkx graph
        G = nx.parse_adjlist(file)

    # Find all maximal cliques
    maximal_cliques = list(nx.find_cliques(G))
    for clique in maximal_cliques:
        clique.sort(key=int)
    sorted_maximal_cliques = sorted(maximal_cliques, key=len, reverse=True)

    # Find the maximum clique (the longest one)
    # maximum_clique = max(maximal_cliques, key=len)
    maximum_clique = sorted_maximal_cliques[0]

    # Find all maximum cliques (the ones with the same length as the maximum clique)
    maximum_clique_size = len(maximum_clique)
    maximum_cliques = []
    for clique in sorted_maximal_cliques:
        if len(clique) != maximum_clique_size:
            break
        maximum_cliques.append(clique)

    # Open the file to write the results
    with open(f"files/{write_filename}", "w") as file:
        file.write("Maximal Cliques found:\n")
        for clique in sorted_maximal_cliques:
            file.write(f"{clique}\n")
        file.write(f"\Maximum Clique found: {maximum_clique}\n")

    if verbose:
        print("Other maximum cliques found:")
        for clique in maximum_cliques:
            print(clique)

    return maximum_clique_size

def pipeline(num_vertices = 50, conection_prob = 0.5, graph_filename = "graph.txt", cliques_filename = "graph_cliques.txt", verbose = False):
    """
    Arguments:
    num_vertices: number of vertices in the graph
    conection_prob: probability of connection between two vertices
    filename: name of the output file
    verbose: if True, prints when the graph is generated and saved
    """

    generator(num_vertices, conection_prob, filename = graph_filename, verbose = verbose)

    return verifier(read_filename = graph_filename, write_filename = cliques_filename, verbose = verbose)
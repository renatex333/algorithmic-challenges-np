# Maximum Clique Problem

## Social Network Analysis: Finding the Maximum Clique in a Graph

![Example](https://upload.wikimedia.org/wikipedia/commons/thumb/d/d0/VR_complex.svg/1200px-VR_complex.svg.png)

Social Network Analysis (SNA) is an approach that originates from areas such as Sociology, Social Psychology, and Anthropology. This approach studies the relational ties between social actors. Actors in SNA can be individuals like people and companies, or collective social units such as departments within an organization, public service agencies in a city, nation-states of a continent, among others. SNA fundamentally differs from other studies in that its emphasis is not on the attributes (characteristics) of the actors, but on the connections between them.

The idea of a clique in a graph is relatively simple. At the most general level, a clique is a subset of a network in which the actors are closer to each other than to other members of the network. In terms of friendship ties, for example, it's not uncommon to find human groups that form cliques based on age, gender, race, ethnicity, religion, ideology, and many other things. A clique is, therefore, a set of vertices in a graph in which each pair of vertices is directly connected by an edge.

Finding the maximum clique in a graph is a computationally challenging task due to the combinatorial nature of the problem. The computational difficulty arises from the need to explore all possible combinations of vertices to identify the largest clique, which becomes exponential in relation to the number of vertices. This results in high computational complexity, even for moderately large graphs.

The importance of studying cliques is notably present in social network analysis, where cliques represent cohesive groups of individuals who share interests, friendships, or common connections. Identifying cliques helps to understand the structure of a social network, identify influencers and affinity groups, and assist in detecting communities and analyzing social dynamics.

Cliques are important because, in addition to developing homogeneous behaviors among their members, they have, by definition, great proximity, increasing the speed of exchanges. Thus, information directed at a clique is quickly absorbed by its members, who tend to perceive it similarly. This is important, for example, in segmentation strategies.

Therefore, efficiently solving the maximum clique problem has valuable applications in areas ranging from computer science to data analysis in social networks.

## Running the Algorithms

### Setting Up Python Virtual Environment

A Python virtual environment is a self-contained directory that contains a Python installation for a particular version of Python, plus a number of additional packages. Using a virtual environment is recommended to avoid conflicts between the dependencies of different projects.

1. **Create a Virtual Environment**

   Navigate to your project's directory and run the following command. Replace `env_name` with your preferred name for the virtual environment.
   ```bash
   python3 -m venv env_name
   ```

2. **Activate the Virtual Environment**

   - On Windows, run:
     ```bash
     .\env_name\Scripts\activate
     ```
   - On Unix or MacOS, run:
     ```bash
     source env_name/bin/activate
     ```

   Once activated, your command line prompt will change to show the name of the activated environment.

3. **Deactivating the Virtual Environment**

   When you're done, you can deactivate the virtual environment by running:
   ```bash
   deactivate
   ```

### Installing Dependencies

This project require external Python libraries. These dependencies are listed in a file named `requirements.txt`.

1. **Install Required Packages**

   With your virtual environment activated, install all required packages using:
   ```bash
   pip install -r requirements.txt
   ```

   Or, in the event that the command above fails to install the dependencies, install the following packages:
   ```bash
   pip install matplotlib networkx pandas pyarrow
   ```

### Running the live report

To run the algorithms and retrieve their timing data, the `project_report.ipynb` Jupyter Notebook provides a series of modules to help. Additionaly, the full report can be run to, step by step, demonstrate the development process.

Make sure to follow any additional instructions specific to each algorithm, such as providing input data or configuration settings.

## References

Qinghua Wu, Jin-Kao Hao. (2015). European Journal of Operational Research,
Volume 242, Issue 3, Pages 693-709, ISSN 0377-2217. [A review on algorithms for maximum clique problems](https://doi.org/10.1016/j.ejor.2014.09.064).
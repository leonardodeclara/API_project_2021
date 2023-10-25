# Algorithms and Principles of Computer Science project 2020/2021

Project for Algorithms and Principles of Computer Science course 2020/21 at Politecnico di Milano.

**Professors**:  Matteo Pradella, Davide Martinenghi, Alessandro Barenghi <br>

**Final grade**: 30/30 cum laude <br>

### Project Specification

This project aims at creating a ranking of a given set of weighted directed graphs according to a score determined by the sum of the shortest paths from the initial node to all other nodes.

The program takes two command-line parameters, separated by a space:
<ul>
    <li> <em>d </em>⁓ number of nodes of the graphs</li>
    <li> <em>k </em>⁓ length of the ranking</li>
</ul>
Other commands:
<ul>
    <li> <em>Aggiungi grafo </em>: followed by the graph's adjacency matrix of the graph to be added, it appends a new graph to those being considered for ranking. </li>
    <li> <em>TopK </em>: command to print the indices corresponding to the top K graphs according to the metric described above.</li>
</ul>

### Tools
**GCC** - C compiler. <br>
**Valgrind,  CallGrind, Massif-Visualizer & Address Sanitizer** - profiling and memory error detection tools. <br>



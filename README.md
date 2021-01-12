# Travelling-Salesman-Application

## Table of Contents
* [Introduction](introduction)
* [Technologies](technologies)
* [Features](features)

## Introduction
The initial motivation for this project was to attempt to implement the Simulated Annealing algorithm to a
problem of my choice. The problem I chose was the Travelling Salesman Problem: given a collection of points 
in a space and the distance between them, what is the shortest path that crosses all of them exactly once.

A collection of vertices in this context will be further referred to as a 'graph'

As of this update, the algorithm to compute the shortest path between points has not yet been implemented. 
The focus as of now is to implement new features.

## Technologies
The application is written in C++17 with Visual Studio 2019.
The wxWidgets library was used to create the GUI.
OpenGL was used to render the vertices on display.
These technologies were chosen so that the application could be theoretically cross-platform.

## Features
* Loading and Saving graphs
* Can select a number of vertices to move and delete
* Can change the colour pallete used to display the graph.

### Todo:
* Undo/Redo actions
* Copying selecting vertices
* Addition of L-p metrics
* Load images to overlay onto the graph
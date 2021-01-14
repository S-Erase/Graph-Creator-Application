# Graph-Creator-Application

## Table of Contents
* [Introduction](introduction)
* [Technologies](technologies)
* [Features](features)
* [Controls](controls)

## Introduction
The initial motivation for this project was to attempt to implement the Simulated Annealing algorithm to a
problem of my choice. The problem I chose was the Travelling Salesman Problem: given a collection of points 
in a space and the distance between them, what is the shortest path that crosses all of them exactly once.

A collection of vertices in this context will be further referred to as a 'graph'

The main purpose of this algorithm is to allow the user to easily create large collections of vertices in
whichever arrangement they want.

This application is a work in progress.
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
* Combining multiple graphs together
* Copy/Pasting selections
* Customized distance metric
* Load images to overlay onto the graph

## Controls
On the left side of the window is a vertical toolbar whose buttons correspond to different control modes.

### All modes
* Using Ctrl+Left Mouse will drag the display.
* Scrolling the mouse wheel will zoom the display in/out.
### View Mode
* Dragging the mouse on the display will drag the display across.
### Creation Mode
* Left-clicking the mouse will create a vertex.
* Using Shift+Left Mouse will drag the selection vertices across the display.
### Selection Mode
* Ctrl+Left Mouse drags the screen.
* Shift+Left Mouse creates a rectangle which will select the vertices inside when the mouse is released.
### Edit Mode (WIP)
This control mode will be used to customize distances between vertices.

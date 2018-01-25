# OSM Editor 3D

This tool focuses on road data in the OSM file. You can read the road data from a OSM file, edit it, and save it to a OSM file. Other objects such as building are ignored. Also, only a subset of attributes, road type, #lanes, one way, are supported. The main purpose of this tool is to create a clean and planar graph structure of roads so that the resultant roads can be used to easily generate a 3D city model using a procedural modeling even though the procedural modeling engine is not integrated yet.

How to use this?
- Drag a vertex to move the vertex. If the moved vertex is close enough to another one, it will snap to that one.
- Click an edge and press DELETE button to delte the edge
- CTRL + left click to start adding a new edge. Double click to finish it.
- Double click on an edge to add a vertex on it.
- In the property window, the attributes of the selected edge can be updated.
- View -> 3D to make the 3D model of the city using the procedural modeling based on the current roads.

![Screen shot 1](/images/screenshot_1.png)
![Screen shot 2](/images/screenshot_2.png)

Assignment One

Dan Aston 300406690

CGRA 251 Class of 2017 Assignment One 

Simple Wavefront Mesh Viewer

I have implemented the cube, the button that loads the cube

I have implemented the mesh loader. At first the CGRA one wasnt working for me so I wrote another one inside the imGUI button if block. Now the CGRA one works, too.

I have implemented manual coords on sliders

The rotation scheme is a polar coordinate setup. The first two coordinates set the latitude and longtitude of the Z axis (or, the Zorth pole), which is mapped to the third coordinate.

I have implemented mouse interactive translation, scaling and arcball rotation.

I got rid of the else statements so you can move it, scale it, and rotate it concurrently.

The interactive arcball rotation updates the polar coordinates in the GUI, and the area of the arcball scales and translates to match the mesh.

The xy translation scales with z axis translation.

I have implemented the Wireframe toggle

I included a Triangle/Vertex count.

I also modified and extended cgra::Wavefront to include information about the length of the maximum vertex, so the arcball interface may be scaled accordingly.

On to the next one!
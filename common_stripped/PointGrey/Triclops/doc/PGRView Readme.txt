
===============================================================================
PGRView Readme

These are just some quick notes on how to use the PGRView application.
This application is provided "as-is" for Triclops SDK users to allow them
to view 3D point cloud data.  


We welcome your questions and comments:  
http://www.ptgrey.com/support/contact/


Table of Contents

1.    How to view data
2.    PGRView file formats



1. How to view data

PGRView recognizes 3 new file format extensions:

    .pts     - points file
    .edg     - edge file
    .trg     - triangle file

The fundamental data set that can be loaded by PGRView is a points file.  
PGRView can also load edge and triangle files but only if there is already
a points file loaded.  The edge and triangle files are loaded automatically
and must have the same base name as the points file.  (ie: model.pts, 
model.edg, model.trg make a complete set).

Rotations in PGRView happen around the center of mass of the point cloud.
The rotations can be controlled with the left mouse button through the 
following operations:

Right-Left drag	- Rotation about the Y axis
Up-Down drag	- Rotation about the X axis
Right-Left drag with shift key depressed  - Rotation around the Z axis

Translations can be controlled with the right mouse button through the 
following operations:

Right-Left drag	- Translation in the X direction
Up-Down drag	- Translation in the Y direction
Right-Left drag with shift key depressed  - Translation in the Z direction

When using the "second point cloud" option (the red open file icon), holding 
down ctrl with any of the above key and mouse button combinations will apply
transforms to the second point cloud instead of the entire scene.



2. PGRView file formats

The points file format is the following:

<x> <y> <z> <r> <g> <b>
....

where x, y, z are the x, y, z coordinate positions (in meters) and 
r, g, b are the red, green, blue values for the point.  R, g, b values
are based on a 0 -> 255 range.  It is a good idea to try with 255, 255, 255
if you have problems seeing the points.

And example line in the points file would be something like:

-0.1 1.0 0.2 255 0 0

Indicating a red point at location (-.1, 1, .2)


Edge files allow you to display wire-mesh models.  The edge file is just a 
list of connected points.  The format is simply:

<p1> <p2>

Where p1 and p2 indicate the point "number" from the points file.  Points
are numbered based on their order of appearance in the points file.  So, for 
example, to connect a line between the first and second point in the points
file you need a line like the following in your edge file:

0 1

(Note, we are C programmers, so we starting counting from 0, not 1.)
 
The triangle file allows you to create triangle faces in the 3D model.  It
works the same as the edge file but requires 3 points.  Ie:
<p1> <p2> <p3>

 

$Id: PGRView\040Readme.txt,v 1.2 2004/01/22 19:19:21 mwhite Exp $

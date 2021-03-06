-----------------------------------------------
-----------------------------------------------
Using the periodic_general mesh generation tool
-----------------------------------------------
-----------------------------------------------

This tool will generate a planar centroidal Voronoi tesselation (CVT) mesh that is periodic 
and can have a variable resolution driven by a density function.  The output is
in netCDF format that can be used with MPAS mesh generation tools to create an MPAS-compatible
mesh.  The code was written primarily by Michael Duda at NCAR (duda@ucar.edu).

Building: Update the Makefile if needed, and then run 'make' to generate two executables:
periodic_general, mkgrid

* periodic_general is used to create a pointset of CVT cell centers with the desired
characteristics.
* mkgrid could potentially (with some additional modifications) be used on the output
to create a netCDF file.  However, the MpasMeshConverter.x program in the standard 
MPAS mesh generation toolchain can do this instead.  This executable is no longer needed for
building meshes for MPAS.



-----------------------------------------------
Setting up periodic_general
-----------------------------------------------

Runtime options in periodic_general are controlled by a file called "Params.txt".  If this file
does not exist, when the exeutable is run it will create a copy of the file and abort.  You can
then edit the options in the file and re-run it.  Most of the options in the file should be
fairly self-explanatory but documentation is currently limited.

There are two ways to seed the generators (controlled in Params.txt):
1. Random points based on the density function.
2. Input from the file "centroids.txt".
This file is a list of x,y coordinate pairs, one per line.  An example centroids.txt
file is part of the repository in this directory.  Note that your final mesh will
have the same number of points as were input in centroids.txt, but they will be moved
around to satisfy the density function under the constraint of the CVT property.

There are two ways to specify density functions (controlled in Params.txt):
1. An analytic density function.  This is hard-coded in DensityFunction.cxx.  Edit the
fucntion AnalyticDensityFunction as needed for your application and re-compile.
2. A data density function.  periodic_general will look for a netCDF file named "density.nc" 
using a regular grid.  The format of the file should look like this:
netcdf density {
dimensions:
        x = 834 ;
        y = 834 ;
variables:
        double x(x) ;
        double y(y) ;
        double density(y, x) ;
}
periodic_general will perform bilinear interpolation from the density grid to evaluate the
density at any location in the periodic_general domain.  Extrapolation will be performed for 
regions of the periodic_general domain that fall outside of the extent of the density field.

There are two hard-coded parameters that users may want to adjust before compiling.
1. GLEV in Triangle.cxx:  This controls how many times each triangle is subdivided when 
the centroid is calculated.  The value chosen is a tradeoff between speed and accuracy.
This parameter very strongly affects the time the code takes to run.
2. MaxSize in fortune/voronoi_main.c:  This limits the number of cells that can be generated.

-----------------------------------------------
Running periodic_general
-----------------------------------------------

Execute: ./periodic_general

Output for each iteration is displayed like:

Iteration 0
38 obtuse angles
Iteration 1
38 obtuse angles
Iteration 2
38 obtuse angles
Iteration 3
38 obtuse angles

When it completes, you will find two files:
* restart.txt - this is an updated version of centroids.txt that gives the new 
positions of all of the points.  It can be used to restart periodic_generaland iterate further.
* grid.nc - this is the output grid in netCDF format.  It contains the following information:
netcdf grid {
dimensions:
	nCells = 1600 ;
	nVertices = 3200 ;
	vertexDegree = 3 ;
variables:
	double xCell(nCells) ;
	double yCell(nCells) ;
	double zCell(nCells) ;
	double meshDensity(nCells) ;
	double xVertex(nVertices) ;
	double yVertex(nVertices) ;
	double zVertex(nVertices) ;
	int cellsOnVertex(nVertices, vertexDegree) ;

// global attributes:
		:on_a_sphere = "NO              " ;
		:is_periodic = "YES             " ;
		:sphere_radius = 0. ;
		:x_offset = 40. ;
		:y_offset = 34.6410161513776 ;
}



-----------------------------------------------
Next Steps
-----------------------------------------------

You can view the pointset generated in grid.nc with the simple python script
plot_grid.py before continuing.  View the script to see some commented out plot options.

The grid.nc file contains the set of information required by the MpasMeshConverter.x program,
which is the next step in the MPAS mesh generation toolchain.
See https://docs.google.com/drawings/d/1dnCC_xrIhg3qrYJoo4reinV8ffWNsxQ1BdxCSvguekI/edit
for an example of the MPAS mesh generation workflow.





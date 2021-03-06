#include <iostream>
#include <fstream>
#include <assert.h>
#include <math.h>
#include <cstdlib>
#include "PointSet.h"
#include "DensityFunction.h"

#define MIN(A,B) (B)<(A)?(B):(A)
#define MAX(A,B) (B)>(A)?(B):(A)

void voronoi_main(PointSet *);


PointSet::PointSet()
{
	nPoints = 0;
}


PointSet::~PointSet()
{

}


void PointSet::makeMCPoints(int n, double X_PERIOD, double Y_PERIOD, int USE_DATA_DENSITY)
{
	//Create Monte Carlo random point set
	int i;
	srand(2); // set the seed for reproducibility
	double x, y;
	Point * p;
	DensityFunction density(X_PERIOD, Y_PERIOD, USE_DATA_DENSITY);

	for(i = 0; i < n; i++){
		p = density.randomPoint();

		p->setNum(nPoints);
		nPoints++;
		points.push_back(p);

	}
}


int PointSet::initFromTextFile(double X_PERIOD, double Y_PERIOD, const char * filename)
{
	ifstream fin(filename);
	double xloc, yloc;
	Point * p;
	ifstream new_edges("new_edges");

	assert(fin.is_open());

	fin >> xloc >> yloc;
	do {
		p = new Point(xloc, yloc, 0);
		p->setNum(nPoints);
		nPoints++;
		points.push_back(p); 
		fin >> xloc >> yloc;
	} while (!fin.eof());
}


void PointSet::print()
{
	vector<Point*>::iterator it;

	cout << "We have " << nPoints << " points" << endl;

	for (it = points.begin(); it != points.end(); it++) {
		cout << **it << endl;
	}
}


void PointSet::printToTextFile(const char * filename)
{
	ofstream fout(filename);
	vector<Point*>::iterator it;

	assert(fout.is_open());

	for (it = points.begin(); it != points.end(); it++) {
		fout << (*it)->getX() << " " << (*it)->getY() << " " << (*it)->isBoundaryPoint() << endl;
	}
}


void PointSet::addPoint(double x, double y, int boundary_point)
{
	Point * p = new Point(x, y, boundary_point);
	p->setNum(nPoints);
	nPoints++;
	
	points.push_back(p);
}


void PointSet::addPoint(Point& p)
{
	Point * pp = new Point(p);
	nPoints++;
	
	points.push_back(pp);
}


int PointSet::size()
{
	return nPoints;
}


vector<Triangle>* PointSet::getTriangulation()
{
	triangulation = new vector<Triangle>; 

	voronoi_main(this);

	return triangulation;
}


vector<Point> * PointSet::getVoronoiDiagram()
{
	vector<Triangle> * t;
	vector<Triangle>::iterator it;
	vector<Point> * voronoiCorners = new vector<Point>[nPoints];
	Point p;
	int i, n;
	int nobtuse;

	double PI = 2.0 * acos(0.0);

	// 1) Get a triangulation
	t = PointSet::getTriangulation();

	// 2) For each triangle, compute the associated Voronoi point
	//	 Add this point to the list of Voronoi corner for each of the triangle's vertices
	nobtuse = 0;
	for (it = triangulation->begin(); it != triangulation->end(); it++) {
		if (fabs(angle(it->getVertex(0), it->getVertex(1), it->getVertex(2))) > PI/2.0) nobtuse++;
		if (fabs(angle(it->getVertex(1), it->getVertex(2), it->getVertex(0))) > PI/2.0) nobtuse++;
		if (fabs(angle(it->getVertex(2), it->getVertex(0), it->getVertex(1))) > PI/2.0) nobtuse++;
		p = it->circumcenter();
		for (i=0; i<3; i++) {
			n = it->getVertex(i).getNum();
			//assert(n >= 0 && n < nPoints);
			voronoiCorners[n].push_back(p);
		}
	}

cout << nobtuse << " obtuse angles\n";

	delete t;

	// 3) For each point, order its list of Voronoi corners in ccw order
	for (i=0; i<nPoints; i++)
		orderCCW(voronoiCorners[i], *points[i]);	

	// 4) Return list lists of Voronoi corners
	return voronoiCorners;
}


vector<Point> * PointSet::getDelaunayAdjacency()
{
	vector<Triangle> * t;
	vector<Triangle>::iterator it;
	vector<Point> * adjacencyList = new vector<Point>[nPoints];
	Point p0, p1, p2;
	int i, j, found, n0, n1, n2;

	t = PointSet::getTriangulation();

	for (it = triangulation->begin(); it != triangulation->end(); it++) {
		p0 = it->getVertex(0);
		p1 = it->getVertex(1);
		p2 = it->getVertex(2);
		
		n0 = p0.getNum(); 
		n1 = p1.getNum(); 
		n2 = p2.getNum(); 

		found = 0;
		for(j=0; j<adjacencyList[n0].size(); j++) {
			if (adjacencyList[n0][j].getNum() == n1)
				found = 1;
		}
		if (!found) adjacencyList[n0].push_back(p1);

		found = 0;
		for(j=0; j<adjacencyList[n0].size(); j++) {
			if (adjacencyList[n0][j].getNum() == n2)
				found = 1;
		}
		if (!found) adjacencyList[n0].push_back(p2);


		found = 0;
		for(j=0; j<adjacencyList[n1].size(); j++) {
			if (adjacencyList[n1][j].getNum() == n0)
				found = 1;
		}
		if (!found) adjacencyList[n1].push_back(p0);

		found = 0;
		for(j=0; j<adjacencyList[n1].size(); j++) {
			if (adjacencyList[n1][j].getNum() == n2)
				found = 1;
		}
		if (!found) adjacencyList[n1].push_back(p2);
		

		found = 0;
		for(j=0; j<adjacencyList[n2].size(); j++) {
			if (adjacencyList[n2][j].getNum() == n0)
				found = 1;
		}
		if (!found) adjacencyList[n2].push_back(p0);

		found = 0;
		for(j=0; j<adjacencyList[n2].size(); j++) {
			if (adjacencyList[n2][j].getNum() == n1)
				found = 1;
		}
		if (!found) adjacencyList[n2].push_back(p1);
	}

	// MGD Could replace the above later with a blind insertion followed by call to a routine remove_duplicates,
	//	  though this would involve removing items from the list

	delete t;

	for (i=0; i<nPoints; i++)
		orderCCW(adjacencyList[i], *points[i]);	

	return adjacencyList;
}


int PointSet::nearestPoint(Point& p)
{
	double d, minD;
	double x, y;
	int idx, minIdx;
	vector<Point*>::iterator it;

	x = p.getX();
	y = p.getY();

	minD = 1.e20;
	for (it = points.begin(), idx=0; it != points.end(); it++, idx++) {
		d = pow((*it)->getX() - x, 2.0) + pow((*it)->getY() - y, 2.0);
		if (d < minD) {minD = d; minIdx = idx;}
	}

	return minIdx;
}


Point* PointSet::operator[](int i)
{
	assert(i >= 0 && i < nPoints);
	return points[i];
}


double angle(Point o, Point p1, Point p2)
{
	double P1x, P1y, mP1;
	double P2x, P2y, mP2;
	double cos_angle;

	P1x = p1.getX() - o.getX();
	P1y = p1.getY() - o.getY();

	mP1 = sqrt(P1x*P1x + P1y*P1y);

	P2x = p2.getX() - o.getX();
	P2y = p2.getY() - o.getY();

	mP2 = sqrt(P2x*P2x + P2y*P2y);

	cos_angle = (P1x*P2x + P1y*P2y) / (mP1 * mP2);

	if (((P1x * P2y) - (P1y * P2x)) >= 0.0)
		return acos(MAX(MIN(cos_angle,1.0),-1.0));
	else
		return -acos(MAX(MIN(cos_angle,1.0),-1.0));

	return 1.0;
}


void orderCCW(vector<Point>& vc, Point p)
{
	int i, j;
	int vsize;
	double * angles;
	double ftemp;
	Point ptemp;

	double PI = 2.0 * acos(0.0);

	vsize = vc.size();
	angles = new double[vsize];

	angles[0] = 0.0;
	for (i=1; i<vsize; i++) {
		angles[i] = angle(p, vc[0], vc[i]);
		if (angles[i] < 0.0) angles[i] += 2.0 * PI;
	}

	for(i=1; i<vsize; i++) {
		for(j=i+1; j<vsize; j++) {
			if (angles[j] < angles[i]) {
				ftemp = angles[i];
				angles[i] = angles[j];
				angles[j] = ftemp;

				ptemp = vc[i];
				vc[i] = vc[j];
				vc[j] = ptemp;
			}
		}
	}

	delete [] angles;
}


void orderCCW_normalize(vector<Point>& vc, Point p, double x_period, double y_period)
{
	int i, j;
	int vsize;
	double * angles;
	double ftemp;
	Point ptemp;

	double PI = 2.0 * acos(0.0);

	vsize = vc.size();
	angles = new double[vsize];

	for (i=0; i<vsize; i++) {
		if ( (vc[i].getX() - p.getX()) > (x_period / 2.0) ) {
			vc[i].setX( vc[i].getX() - x_period );
		}
		else if ( (vc[i].getX() - p.getX()) < (-x_period / 2.0) ) {
			vc[i].setX( vc[i].getX() + x_period );
		}

		if ( (vc[i].getY() - p.getY()) > (y_period / 2.0) ) {
			vc[i].setY( vc[i].getY() - y_period );
		}
		else if ( (vc[i].getY() - p.getY()) < (-y_period / 2.0) ) {
			vc[i].setY( vc[i].getY() + y_period );
		}
	}

	angles[0] = 0.0;
	for (i=1; i<vsize; i++) {
		angles[i] = angle(p, vc[0], vc[i]);
		if (angles[i] < 0.0) angles[i] += 2.0 * PI;
	}

	for(i=1; i<vsize; i++) {
		for(j=i+1; j<vsize; j++) {
			if (angles[j] < angles[i]) {
				ftemp = angles[i];
				angles[i] = angles[j];
				angles[j] = ftemp;

				ptemp = vc[i];
				vc[i] = vc[j];
				vc[j] = ptemp;
			}
		}
	}

	delete [] angles;
}


void orderCCW_normalize2(vector<Point>& vc1, vector<Point>& vc2, Point p, double x_period, double y_period)
{
	int i, j;
	double * angles;
	double ftemp;
	Point ptemp;

	double PI = 2.0 * acos(0.0);

	if (vc1.size() != vc2.size()) {
		cerr << "Error: In orderCCW_normalize2, input vectors have different size." << endl;
		return;
	}

	angles = new double[vc1.size()];


	/* Normalize points in vc1 */
	for (i=0; i<vc1.size(); i++) {
		if ( (vc1[i].getX() - p.getX()) > (x_period / 2.0) ) {
			vc1[i].setX( vc1[i].getX() - x_period );
		}
		else if ( (vc1[i].getX() - p.getX()) < (-x_period / 2.0) ) {
			vc1[i].setX( vc1[i].getX() + x_period );
		}

		if ( (vc1[i].getY() - p.getY()) > (y_period / 2.0) ) {
			vc1[i].setY( vc1[i].getY() - y_period );
		}
		else if ( (vc1[i].getY() - p.getY()) < (-y_period / 2.0) ) {
			vc1[i].setY( vc1[i].getY() + y_period );
		}
	}


	/* Normalize points in vc2 */
	for (i=0; i<vc1.size(); i++) {
		if ( (vc1[i].getX() - p.getX()) > (x_period / 2.0) ) {
			vc1[i].setX( vc1[i].getX() - x_period );
		}
		else if ( (vc1[i].getX() - p.getX()) < (-x_period / 2.0) ) {
			vc1[i].setX( vc1[i].getX() + x_period );
		}

		if ( (vc1[i].getY() - p.getY()) > (y_period / 2.0) ) {
			vc1[i].setY( vc1[i].getY() - y_period );
		}
		else if ( (vc1[i].getY() - p.getY()) < (-y_period / 2.0) ) {
			vc1[i].setY( vc1[i].getY() + y_period );
		}
	}


	/* Order points in vc1 */
	angles[0] = 0.0;
	for (i=1; i<vc1.size(); i++) {
		angles[i] = angle(p, vc1[0], vc1[i]);
		if (angles[i] < 0.0) angles[i] += 2.0 * PI;
	}

	for(i=1; i<vc1.size(); i++) {
		for(j=i+1; j<vc1.size(); j++) {
			if (angles[j] < angles[i]) {
				ftemp = angles[i];
				angles[i] = angles[j];
				angles[j] = ftemp;

				ptemp = vc1[i];
				vc1[i] = vc1[j];
				vc1[j] = ptemp;
			}
		}
	}


	/* Order points in vc2 */
	for (i=0; i<vc2.size(); i++) {
		angles[i] = angle(p, vc1[0], vc2[i]);
		if (angles[i] < 0.0) angles[i] += 2.0 * PI;
	}

	for(i=0; i<vc2.size(); i++) {
		for(j=i+1; j<vc2.size(); j++) {
			if (angles[j] < angles[i]) {
				ftemp = angles[i];
				angles[i] = angles[j];
				angles[j] = ftemp;

				ptemp = vc2[i];
				vc2[i] = vc2[j];
				vc2[j] = ptemp;
			}
		}
	}

	delete [] angles;
}


void orderCCW_print(vector<Point>& vc, Point p)
{
	int i, j;
	int vsize;
	double * angles;
	double ftemp;
	Point ptemp;

	double PI = 2.0 * acos(0.0);

	vsize = vc.size();
	angles = new double[vsize];

	angles[0] = 0.0;
	for (i=1; i<vsize; i++) {
		angles[i] = angle(p, vc[0], vc[i]);
		if (angles[i] < 0.0) angles[i] += 2.0 * PI;
//cout << p << vc[0] << vc[i] << endl;
//cout << "angle=" << angles[i] << endl;
	}

	for(i=1; i<vsize; i++) {
		for(j=i+1; j<vsize; j++) {
			if (angles[j] < angles[i]) {
				ftemp = angles[i];
				angles[i] = angles[j];
				angles[j] = ftemp;

				ptemp = vc[i];
				vc[i] = vc[j];
				vc[j] = ptemp;
			}
		}
	}

	delete [] angles;
}

double poly_area(vector<Point>& vc)
{
	Point centroid(0.0, 0.0, 0);
	int i;
	double poly_area;
	Triangle t;


	/* Find centroid of the vector of points */
	for (i=0; i<vc.size(); i++)
		centroid = centroid + vc[i];	
	centroid = centroid * (1.0 / (double)vc.size());

	
	/*
	 * Compute the area of the polygon by summing the areas of triangles
	 *    in a triangulation of the polygon
	 */
	poly_area = 0.0;
	t.setVertex(0, centroid);
	t.setVertex(1, vc[vc.size()-1]);
	for (i=0; i<vc.size(); i++) {
		t.setVertex(2, vc[i]);
		poly_area += t.area();
		t.setVertex(1, t.getVertex(2));
	}

	return poly_area;
}


void periodic_normalize(vector<Point>& vc, double x_period, double y_period)
{
	int i;

	for (i=1; i<vc.size(); i++) {
		if ( (vc[i].getX() - vc[0].getX()) > (x_period / 2.0) ) {
			vc[i].setX( vc[i].getX() - x_period );
		}
		else if ( (vc[i].getX() - vc[0].getX()) < (-x_period / 2.0) ) {
			vc[i].setX( vc[i].getX() + x_period );
		}

		if ( (vc[i].getY() - vc[0].getY()) > (y_period / 2.0) ) {
			vc[i].setY( vc[i].getY() - y_period );
		}
		else if ( (vc[i].getY() - vc[0].getY()) < (-y_period / 2.0) ) {
			vc[i].setY( vc[i].getY() + y_period );
		}
	}
}

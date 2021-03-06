#include "Point.h"

Point::Point()
{
	x = 0.0;
	y = 0.0;
	boundary_point = 0;
	num = 0;
}


Point::Point(double x, double y, int boundary_point)
{
	this->x = x;
	this->y = y;
	this->boundary_point = boundary_point;
}


Point::~Point()
{

}


void Point::setX(double x)
{
	this->x = x;
}


void Point::setY(double y)
{
	this->y = y;
}


void Point::setXY(double x, double y)
{
	this->x = x;
	this->y = y;
}


void Point::setBoundaryPoint(int boundary_point)
{
	this->boundary_point = boundary_point;
}


void Point::setNum(int n)
{
	num = n;
}


double Point::getX() const
{
	return x;
}


double Point::getY() const
{
	return y;
}


double Point::distance(Point& p)
{
	double xd, yd;

	xd = p.getX() - x;
	yd = p.getY() - y;
	return sqrt(xd*xd + yd*yd);
}


int Point::isBoundaryPoint() const
{
	return boundary_point;
}


int Point::getNum() const
{
	return num;
}


Point Point::operator+(Point p)
{
	Point retval;

	retval.x = x + p.x;
	retval.y = y + p.y;
	retval.boundary_point = boundary_point;
	retval.num = num;

	return retval;
}


Point Point::operator-(Point p)
{
	Point retval;

	retval.x = x - p.x;
	retval.y = y - p.y;
	retval.boundary_point = boundary_point;
	retval.num = num;

	return retval;
}


Point Point::operator*(double s)
{
	Point retval;

	retval.x = s * x;
	retval.y = s * y;
	retval.boundary_point = boundary_point;
	retval.num = num;

	return retval;
}


ostream& operator<<(ostream& output, const Point& p)
{
	output << p.x << " " << p.y;
	return output;
}


bool operator<(Point const& lhs, Point const& rhs)
{
        double a[2], b[2];

        a[0] = lhs.x;
        a[1] = lhs.y;

        b[0] = rhs.x;
        b[1] = rhs.y;

        if (a[0] < b[0]) {
                return true;
        }
        else if (a[0] == b[0]) {
                if (a[1] < b[1]) {
                        return true;
                }
        }

        return false;
}

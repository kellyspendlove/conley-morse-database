/* EWA Map */

#ifndef CMDP_EWA_H
#define CMDP_EWA_H

#include "database/maps/Map.h"
#include "database/structures/EuclideanParameterSpace.h"
#include "database/structures/RectGeo.h"
#include "database/numerics/simple_interval.h"
#include <memory>
#include <vector>

class ModelMap : public Map {
public:
  typedef simple_interval<double> interval;

// User interface: method to be provided by user
  // Parameter variables
  interval c, d;
  
  // Constructor: sets parameter variables
  void assign ( RectGeo const& rectangle ) {
    // Read parameter intervals from input rectangle
    c = getRectangleComponent ( rectangle, 0 );
    d = getRectangleComponent ( rectangle, 1 );  
  }

  // Map
  UnionGeo operator () ( const RectGeo & rectangle ) const {
    UnionGeo result;	  
    // Convert input to intervals
    interval x0 = getRectangleComponent ( rectangle, 0 );
    interval x1 = getRectangleComponent ( rectangle, 1 );
    interval y0 = getRectangleComponent ( rectangle, 2 );
    interval y1 = getRectangleComponent ( rectangle, 3 );

    
    //Cube lies outside of simplex
    //Handle via Fold
    
    if ( x0.lower() + x1.lower() >= 1.0 ) {
	//interval Tx = x0;
	//x0 = 1.0 - x1;
	//x1 = 1.0 - Tx;
	return result;
    }
    if ( y0 . lower() + y1 . lower() >= 1.0 ) {
	//interval Ty = y0;
	//y0 = 1.0 - y1;
	//y1 = 1.0 - Ty;
	return result;
    }
    
    //Handle via map to origin
    /*
    if (x0.lower() + x1.lower() >= 1.0) {
	x0 = interval(0.0,0.0);
	x1 = interval(0.0,0.0);
    }
    if (y0.lower() + y1.lower() >= 1.0) {
	y0 = interval(0.0,0.0);
	y1 = interval(0.0,0.0);
    }
    */
    
    interval X = x0+x1;
    interval Y = y0+y1;

    interval x2 = 1.0-X;
    interval y2 = 1.0-Y;
    
    //Check if cube hits boundary of simplex
    /*
    if ( x0.lower()+x1.lower()<1.0 and x0.upper()+x1.upper() > 1.0){
	x2 = interval(0.0, x2.upper() );
    }    
    if ( y0.lower()+y1.lower()<1.0 and y0.upper()+y1.upper() > 1.0){
	y2 = interval(0.0, y2.upper() );
    }	
    */

    if ( x2 . lower() < 0.0) {
	x2 = interval ( 0.0, x2.upper());
    }
    if ( y2 . lower() < 0.0) {
	y2 = interval ( 0.0, y2.upper());
    }

    interval a11, a12, a13, a21, a22, a23, a31, a32, a33;
    interval b11, b12, b13, b21, b22, b23, b31, b32, b33;

    double a = 0.18;
    double b = 1.41;

    a11 = c*(0.2-1.0)+1.0;
    a12 = d*0.9;
    a13 = d*-0.1;
    a21 = d*-0.1;
    a22 = a*(1.0-1.0)+1.0;
    a23 = d*-1.1;
    a31 = d*0.1;
    a32 = d*-1.1;
    a33 = c*(.1-1.0)+1.0;

    b11 = d*0.6;
    b12 = c*(0.8-1.0)+1.0;
    b13 = d*-0.5;
    b21 = c*(0.9-1.0)+1.0;
    b22 = d*-0.6;
    b23 = d*0.4;
    b31 = d*1.0;
    b32 = d*-0.4;
    b33 = c*(2.4-1.0)+1.0;

    interval Ay0 = a11*y0+a12*y1+a13*y2;
    interval Ay1 = a21*y0+a22*y1+a23*y2;
    interval Ay2 = a31*y0+a32*y1+a33*y2;

    interval Bx0 = b11*x0+b12*x1+b13*x2;
    interval Bx1 = b21*x0+b22*x1+b23*x2;
    interval Bx2 = b31*x0+b32*x1+b33*x2;

    //Preprocess
    interval X0Num = pow(x0,1.0-a)*exp(b*Ay0);
    interval X1Num = pow(x1,1.0-a)*exp(b*Ay1);
    interval XDenom = X0Num + X1Num + pow(x2,1.0-a)*exp(b*Ay2);

    interval Y0Num = pow(y0,1.0-a)*exp(b*Bx0);
    interval Y1Num = pow(y1,1.0-a)*exp(b*Bx1);
    interval YDenom = Y0Num + Y1Num + pow(y2,1.0-a)*exp(b*Bx2);
    
    // Evaluate map
    interval fx0 = X0Num / XDenom; 
    interval fx1 = X1Num / XDenom;     
    interval fy0 = Y0Num / YDenom;
    interval fy1 = Y1Num / YDenom;

    //Check if image is out of bounds of simplex
    //If so, project back onto simplex
    
    if ( fx0.upper() + fx1.upper() > 1.0) {
	 double Mx = 0.5 * fx0.upper() - 0.5 * fx1.upper();
	 fx0 = interval ( fx0 . lower(), Mx + 0.5 );
	 fx1 = interval ( fx1 . lower(), -1.0*Mx + 0.5 );
    }
    if ( fy0 . upper() + fy1 . upper() > 1.0 ) {
	 double My = 0.5 * fy0 . upper() - 0.5 * fy1 . upper();
	 fy0 = interval ( fy0 . lower(), My + 0.5 );
	 fy1 = interval ( fy1 . lower(), -1.0*My + 0.5 );
    }
    

    //return result
    std::shared_ptr<Geo> rect_result ( new RectGeo ( makeRectangle(fx0,fx1,fy0,fy1)) );
    result . insert ( rect_result );

    //return makeRectangle ( fx0, fx1, fy0, fy1 );
    return result;
  } 

// Program interface (methods used by program)

  ModelMap ( std::shared_ptr<Parameter> parameter ) {
    const RectGeo & rectangle = 
      * std::dynamic_pointer_cast<EuclideanParameter> ( parameter ) -> geo;
    assign ( rectangle );
  }

  std::shared_ptr<Geo> 
  operator () ( std::shared_ptr<Geo> geo ) const {   
    return std::shared_ptr<Geo> ( new UnionGeo ( 
        operator () ( * std::dynamic_pointer_cast<RectGeo> ( geo ) ) ) );
  }
private:
  interval getRectangleComponent ( const RectGeo & rectangle, int d ) const {
    return interval (rectangle . lower_bounds [ d ], rectangle . upper_bounds [ d ]); 
  }
  RectGeo makeRectangle ( interval const& y0, interval const& y1, interval const& y2, interval const& y3 ) const {
    RectGeo return_value ( 4 );
    return_value . lower_bounds [ 0 ] = y0 . lower ();
    return_value . upper_bounds [ 0 ] = y0 . upper ();
    return_value . lower_bounds [ 1 ] = y1 . lower ();
    return_value . upper_bounds [ 1 ] = y1 . upper ();
    return_value . lower_bounds [ 2 ] = y2 . lower ();
    return_value . upper_bounds [ 2 ] = y2 . upper ();
    return_value . lower_bounds [ 3 ] = y3 . lower ();
    return_value . upper_bounds [ 3 ] = y3 . upper ();
    return return_value;
  }
};

#endif

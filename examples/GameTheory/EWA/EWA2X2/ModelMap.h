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
  RectGeo operator () ( const RectGeo & rectangle ) const {    
    // Convert input to intervals
    interval x0 = getRectangleComponent ( rectangle, 0 );
    interval y0 = getRectangleComponent ( rectangle, 1 );
    
    interval x1 = 1.0-x0;
    interval y1 = 1.0-y0;
    interval a11, a12, a21, a22;
    interval b11, b12, b21, b22;

    double a = 0.18;
    double b = 1.41;

    a11 = c*(0.2-1.0)+1.0;
    a12 = d*0.9;
    a21 = d*-0.1;
    a22 = a*(1.0-1.0)+1.0;
    b11 = d*0.6;
    b12 = c*(0.8-1.0)+1.0;
    b21 = c*(0.9-1.0)+1.0;
    b22 = d*-0.6;

    interval Ay0 = a11*y0+a12*y1;
    interval Ay1 = a21*y0+a22*y1;
    interval Bx0 = b11*x0+b12*x1;
    interval Bx1 = b21*x0+b22*x1;

    interval XNum = pow(x0,1.0-a)*exp(b*Ay0);
    interval XDenom = XNum + pow(x1,1.0-a)*exp(b*Ay1);
    interval YNum = pow(y0,1.0-a)*exp(b*Bx0);
    interval YDenom = YNum + pow(y1,1.0-a)*exp(b*Bx1);
    
    // Evaluate map
    interval fx0 = XNum / XDenom; 
    // (p0 * x0 + p1 * x1 ) * exp ( -0.1 * (x0 + x1) );     
    interval fy0 = YNum / YDenom;
    
    // Return result
    return makeRectangle ( fx0, fy0 );
  } 

// Program interface (methods used by program)

  ModelMap ( std::shared_ptr<Parameter> parameter ) {
    const RectGeo & rectangle = 
      * std::dynamic_pointer_cast<EuclideanParameter> ( parameter ) -> geo;
    assign ( rectangle );
  }

  std::shared_ptr<Geo> 
  operator () ( std::shared_ptr<Geo> geo ) const {   
    return std::shared_ptr<Geo> ( new RectGeo ( 
        operator () ( * std::dynamic_pointer_cast<RectGeo> ( geo ) ) ) );
  }
private:
  interval getRectangleComponent ( const RectGeo & rectangle, int d ) const {
    return interval (rectangle . lower_bounds [ d ], rectangle . upper_bounds [ d ]); 
  }
  RectGeo makeRectangle ( interval const& y0, interval const& y1 ) const {
    RectGeo return_value ( 2 );
    return_value . lower_bounds [ 0 ] = y0 . lower ();
    return_value . upper_bounds [ 0 ] = y0 . upper ();
    return_value . lower_bounds [ 1 ] = y1 . lower ();
    return_value . upper_bounds [ 1 ] = y1 . upper ();
    return return_value;
  }
};

#endif

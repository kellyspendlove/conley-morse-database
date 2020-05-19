/* Replicator Mapping 
 * Two Player, Three Strategy
 * Continuation from binary matrix 
 * Four-Dimensional Map
 * */

#ifndef CMDP_Rep3D_H
#define CMDP_Rep3D_H

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
  interval a,b;
  // Constructor: sets parameter variables
  void assign ( RectGeo const& rectangle ) {
    // Read parameter intervals from input rectangle
    a = getRectangleComponent ( rectangle, 0 );
    b = getRectangleComponent ( rectangle, 1 );  
  }

  // Map
  RectGeo operator () ( const RectGeo & rectangle ) const {    
    // Convert input to intervals
    interval x0 = getRectangleComponent ( rectangle, 0 );
    interval x1 = getRectangleComponent ( rectangle, 1 );
    interval y0 = getRectangleComponent ( rectangle, 2 );
    interval y1 = getRectangleComponent ( rectangle, 3 );
    
    //Extra variables
    interval x2 = 1.0-x0-x1;
    interval y2 = 1.0-y0-y1;
    // Extra parameters
    double delta = 0.1;
    interval a11, a12, a13, a21, a22, a23, a31, a32, a33;
    interval b11, b12, b13, b21, b22, b23, b31, b32, b33;
    a11 = a*(0.2-1.0)+1.0;
    a12 = b*0.9;
    a13 = b*-0.1;
    a21 = b*-0.1;
    a22 = a*(1.0-1.0)+1.0;
    a23 = b*-1.1;
    a31 = b*0.1;
    a32 = b*-1.1;
    a33 = a*(0.1-1.0)+1.0;
    b11 = b*0.6;
    b12 = a*(0.8-1.0)+1.0;
    b13 = b*-0.5;
    b21 = b*0.9;
    b22 = b*-0.6;
    b23 = b*0.4;
    b31 = b*1.0;
    b32 = b*-0.4;
    b33 = a*(2.4-1.0)+1.0;
    // Evaluate terms of map
    interval Ay0 = a11*y0 + a12*y1 + a13*y2;
    interval Ay1 = a21*y0 + a22*y1 + a23*y2;
    interval Bx0 = b11*x0 + b12*x1 + b13*x2;
    interval Bx1 = b21*x0 + b22*x1 + b23*x2;
    interval xAy = y0*(a11*x0+a21*x1+a31*x2)+y1*(a12*x0+a22*x1+a32*x2)+y2*(a13*x0+a23*x1+a33*x2);
    interval yBx = x0*(b11*y0+b21*y1+b31*y2)+x1*(b12*y0+b22*y1+b32*y2)+x2*(b13*y0+b23*y1+b33*y2); 
    // Evaluate map
    interval fx0 = x0 * ( (1.0 + delta*Ay0) / (1.0+delta*xAy) );
    interval fx1 = x1 * ( (1.0 + delta*Ay1) / (1.0+delta*xAy) );
    interval fy0 = y0 * ( (1.0 + delta*Bx0) / (1.0+delta*yBx) );
    interval fy1 = y1 * ( (1.0 + delta*Bx1) / (1.0+delta*yBx) );

    interval Fx0 = interval (std::max(std::min(fx0.lower(),1.0),0.0),std::max(std::min(fx0.upper(),1.0),0.0));
    interval Fx1 = interval (std::max(std::min(fx1.lower(),1.0),0.0),std::max(std::min(fx1.upper(),1.0),0.0));
    interval Fy0 = interval (std::max(std::min(fy0.lower(),1.0),0.0),std::max(std::min(fy0.upper(),1.0),0.0));
    interval Fy1 = interval (std::max(std::min(fy1.lower(),1.0),0.0),std::max(std::min(fy1.upper(),1.0),0.0));
    //
    //
    //interval x1 = x0 * ( (1.0+0.1*(2.0*A+2.0*B)*y0)/ (1.0+0.1*( (2.0*A+2.0*B)*x0*y0 + (6.0*A+2.0*B)*(1.0-x0)*(1.0-y0))));
    //interval y1 = y0 * ( (1.0-0.1*(2.0*A+2.0*B)*x0)/ (1.0-0.1*( (2.0*A+2.0*B)*x0*y0 + (6.0*A+2.0*B)*(1.0-x0)*(1.0-y0))));
    
    // Return result
    return makeRectangle ( Fx0, Fx1, Fy0, Fy1 );
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
  RectGeo makeRectangle ( interval const& fx0, interval const& fx1, interval const& fy0, interval const& fy1 ) const {
    RectGeo return_value ( 4 );
    return_value . lower_bounds [ 0 ] = fx0 . lower ();
    return_value . upper_bounds [ 0 ] = fx0 . upper ();
    return_value . lower_bounds [ 1 ] = fx1 . lower ();
    return_value . upper_bounds [ 1 ] = fx1 . upper ();
    return_value . lower_bounds [ 2 ] = fy0 . lower ();
    return_value . upper_bounds [ 2 ] = fy0 . upper ();
    return_value . lower_bounds [ 3 ] = fy1 . lower ();
    return_value . upper_bounds [ 3 ] = fy1 . upper ();
    return return_value;
  }
};

#endif

/* Replicator 2D Map
 * Continuation from binary payoff bimatrix */

#ifndef CMDP_Rep2D_H
#define CMDP_Rep2D_H

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
    interval y0 = getRectangleComponent ( rectangle, 1 );
    //Extra parameters and variables
    double delta = 0.1;
    interval x1, y1;
    interval a11, a12, a21, a22;
    interval b11, b12, b21, b22;
    x1 = 1.0-x0;
    y1 = 1.0-y0;
    a11 = a*(0.2-1.0)+1.0;
    a12 = b*0.9;
    a21 = b*(-0.1);
    a22 = a*(1.0-1.0)+1.0;
    b11 = b*0.6;
    b12 = a*(0.8-1.0)+1.0;
    b21 = a*(0.9-1.0)+1.0;
    b22 = b*(-0.6);
    interval Ay, Bx, xAy, yBx;
    Ay = a11*y0+a12*y1;
    Bx = b11*x0+b12*x1;
    xAy = y0*(a11*x0+a21*x1)+y1*(a12*x0+a22*x1);
    yBx = x0*(b11*y0+b21*y1)+x1*(b12*y0+b22*y1);
    // Evaluate map
    interval fx0 = x0 * ( (1.0+delta*Ay) / (1.0+delta*xAy)); 
    interval fy0 = y0 * ( (1.0+delta*Bx) / (1.0+delta*yBx));
    // Adjust map to stay in bounds
    //
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
  RectGeo makeRectangle ( interval const& x1, interval const& y1 ) const {
    RectGeo return_value ( 2 );
    return_value . lower_bounds [ 0 ] = x1 . lower ();
    return_value . upper_bounds [ 0 ] = x1 . upper ();
    return_value . lower_bounds [ 1 ] = y1 . lower ();
    return_value . upper_bounds [ 1 ] = y1 . upper ();
    return return_value;
  }
};

#endif

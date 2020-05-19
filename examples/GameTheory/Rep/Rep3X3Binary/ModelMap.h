/* Replicator Mapping 
 * Two Player, Three Strategy
 * Binary Matrix 
 * Four-Dimensional Map
 * */

#ifndef CMDP_Rep3DBinary_H
#define CMDP_Rep3DBinary_H

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
    double delta = 0.75;
    // Evaluate terms of map
    interval Ay0 = y0;
    interval Ay1 = y1;
    interval Bx0 = x1;
    interval Bx1 = x0;
    interval xAy = x0*y0+x1*y1+x2*y2;
    interval yBx = x0*y1+x1*y0+x2*y2; 
    // Evaluate map
    interval fx0 = x0 * ( (1.0+delta*Ay0) / (1.0+delta*xAy) );
    interval fx1 = x1 * ( (1.0+delta*Ay1) / (1.0+delta*xAy) );
    interval fy0 = y0 * ( (1.0+delta*Bx0) / (1.0+delta*yBx) );
    interval fy1 = y1 * ( (1.0+delta*Bx1) / (1.0+delta*yBx) );
    //
    //
    //interval x1 = x0 * ( (1.0+0.1*(2.0*A+2.0*B)*y0)/ (1.0+0.1*( (2.0*A+2.0*B)*x0*y0 + (6.0*A+2.0*B)*(1.0-x0)*(1.0-y0))));
    //interval y1 = y0 * ( (1.0-0.1*(2.0*A+2.0*B)*x0)/ (1.0-0.1*( (2.0*A+2.0*B)*x0*y0 + (6.0*A+2.0*B)*(1.0-x0)*(1.0-y0))));
    //interval x1 = x0 * ( (1.0+0.05*(a*y0)) / (1.0+0.05*(a*x0*y0+d*(1.0-x0)*(1.0-y0))) ); 
    //interval y1 = y0 * ( (1.0-0.05*(a*x0)) / (1.0-0.05*(a*x0*y0+d*(1.0-x0)*(1.0-y0))) );
    
    // Return result
    return makeRectangle ( fx0, fx1, fy0, fy1 );
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

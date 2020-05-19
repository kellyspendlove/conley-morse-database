/* Replicator 2D Map */

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
  interval a, b, c, d;
  
  // Constructor: sets parameter variables
  void assign ( RectGeo const& rectangle ) {
    // Read parameter intervals from input rectangle
    b = getRectangleComponent ( rectangle, 0 );
    c = getRectangleComponent ( rectangle, 1 );  
  }

  // Map
  RectGeo operator () ( const RectGeo & rectangle ) const {    
    // Convert input to intervals
    interval x0 = getRectangleComponent ( rectangle, 0 );
    interval y0 = getRectangleComponent ( rectangle, 1 );
    
    interval A = -1.0*b-1.0*c;
    interval E = -1.0*b;
    interval F = b;
    interval G = c;
    // Evaluate map
    interval x1 = x0 * ( (1.0 + 0.1*(E*y0+b)) / ( 1.0 + 0.1*(A*x0*y0+F*x0+G*y0)) );
    interval y1 = y0 * ( (1.0 + 0.1*(-1.0*E*x0+-1.0*b)) / ( 1.0 + 0.1*(-1.0*A*y0*x0+-1.0*F*y0+-1.0*G*x0)) );
    
    // Return result
    return makeRectangle ( x1, y1 );
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

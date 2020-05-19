/* Multiplicative Weights Update Map */

#ifndef CMDP_MWU_H
#define CMDP_MWU_H

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
  interval a, b;
  
  // Constructor: sets parameter variables
  void assign ( RectGeo const& rectangle ) {
    // Read parameter intervals from input rectangle
    a = getRectangleComponent ( rectangle, 0 );
    b = getRectangleComponent ( rectangle, 1 ); 
  }

  // Map
  RectGeo operator () ( const RectGeo & rectangle ) const {    
    // Convert input to intervals
    interval x = getRectangleComponent ( rectangle, 0 );
    interval y = getRectangleComponent ( rectangle, 1 );
    interval z = getRectangleComponent ( rectangle, 2 );
    // Evaluate map
    interval x1 = x / ( x + (1.0-x)*exp(a*( (y+z) - b )) );
    interval y1 = y / ( y + (1.0-y)*exp(a*( (x+z) - b )) );
    interval z1 = z / ( z + (1.0-z)*exp(a*( (x+y) - b )) );
    
    // Return result
    return makeRectangle ( x1, y1, z1 );
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
  RectGeo makeRectangle ( interval const& x1, interval const& y1, interval const& z1) const {
    RectGeo return_value ( 3 );
    return_value . lower_bounds [ 0 ] = x1 . lower ();
    return_value . upper_bounds [ 0 ] = x1 . upper ();
    return_value . lower_bounds [ 1 ] = y1 . lower ();
    return_value . upper_bounds [ 1 ] = y1 . upper ();
    return_value . lower_bounds [ 2 ] = z1 . lower ();
    return_value . upper_bounds [ 2 ] = z1 . upper ();
    return return_value;
  }
};

#endif

/* 1D Replicator Map */

#ifndef CMDP_Rep1D_H
#define CMDP_Rep1D_H

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
  interval S,T;
  
  // Constructor: sets parameter variables
  void assign ( RectGeo const& rectangle ) {
    // Read parameter intervals from input rectangle
    S = getRectangleComponent ( rectangle, 0 );
    T = getRectangleComponent ( rectangle, 1 );  
  }

  // Map
  RectGeo operator () ( const RectGeo & rectangle ) const {    
    // Convert input to intervals
    interval x = getRectangleComponent ( rectangle, 0 );
    // Evaluate map
    //interval y = (S+T-1.0)*pow(x,3.0)+(1.0-2.0*S-T)*pow(x,2.0)+(1.0+S)*x;
    interval y = x + x*(1.0-x)*(S+(1.0-T-S)*x);
    // Return result
    return makeRectangle ( y );
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
  RectGeo makeRectangle ( interval const& y0 ) const {
    RectGeo return_value ( 1 );
    return_value . lower_bounds [ 0 ] = y0 . lower ();
    return_value . upper_bounds [ 0 ] = y0 . upper ();
    return return_value;
  }
};

#endif

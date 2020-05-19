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
    interval x1 = getRectangleComponent ( rectangle, 1 );
    interval y0 = getRectangleComponent ( rectangle, 2 );
    interval y1 = getRectangleComponent ( rectangle, 3 );

    interval X = x0+x1;
    interval Y = y0+y1;
    //Change of coordinate homeomorpshim
    //Change from Unit Square to Simplex
    //interval Mx = interval (std::max(x0.lower(),x1.lower()),std::max(x0.upper(),x1.upper()));
    //interval My = interval (std::max(y0.lower(),y1.lower()),std::max(y0.upper(),y1.upper()));

    //Homeomorphism by scaling
    double Mx = std::max(x0.upper(),x1.upper());
    double My = std::max(y0.upper(),y1.upper());
    double Sx = X.upper();
    double Sy = Y.upper();
    x0 = (Mx/Sx)*x0;
    x1 = (Mx/Sx)*x1;
    y0 = (My/Sy)*y0;
    y1 = (My/Sy)*y1;
    
    //Homeomorphism on Interior
    //x0 = x0*(1.0-x1);
    //y0 = y0*(1.0-y1);

    interval x2 = 1.0-X;
    interval y2 = 1.0-Y;
    
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
    
    //Change of coordinates back from simplex to square
    
    //Homeomorphism by scaling
    fx0 = (Sx/Mx)*x0;
    fx1 = (Sx/Mx)*x1;
    fy0 = (Sy/My)*y0;
    fy1 = (Sy/My)*y1;

    //Homeomorphism on interior
    //fx0 = fx0 / (1.0-fx1);
    //fy0 = fy0 / (1.0-fy1);

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

#ifndef CMDB_MODEL_H
#define CMDB_MODEL_H

#include "Boolean.h"
#include "database/program/Configuration.h"

class Model {
 public:
  typedef RectGeo Parameter;

  void initialize ( int argc, char * argv [] ); 
    // Load stuff from a file necessary for the methods
    // "map" and "phaseSpace" below to work properly.

  // boost::shared_ptr < AtlasMap > map ( const Parameter & p ) const;
  boost::shared_ptr < AtlasMap > map ( const Parameter & p );
  // boost::shared_ptr < AtlasMap > map ( const std::vector < double > & indexg, const std::vector < double > & indexs ) const;
  // return a shared ptr to a map function object corresponding to 
  // parameter p

  // boost::shared_ptr<Grid> phaseSpace ( const Parameter & p );
  boost::shared_ptr<Atlas> phaseSpace ( const Parameter & p );
  // boost::shared_ptr<Atlas> phaseSpace ( const std::vector < double > & indexg, const std::vector < double > & indexs );
  // return a shared ptr to the phase space Atlas corresponding to
  // parameter p

  void saveCharts ( int argc, char * argv [] );

private:
  BooleanConfig booleanconfig_;
  std::vector < Face > faces_;
  int dim_;
public:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & booleanconfig_;
    ar & faces_;
    ar & dim_;
  }
};

inline void Model::initialize ( int argc, char * argv [] ) { 
  //
  std::string directory ( argv[1] );
  // std::string filename ("/configboolean.xml");
  std::string filename ("/configspace.py");
  //
  // Hack, need the parameter values to construct the phase space
  Configuration config;
  config . loadFromFile ( argv[1] );
  RectGeo parameters = config . PARAM_BOUNDS;

  booleanconfig_ . load ( (directory+filename).c_str(), parameters );
  
  dim_ = booleanconfig_ . phasespace ( ) . dimension ( );

}

// Technically we don't need the parameter since the BooleanBox have been updated already
inline boost::shared_ptr < AtlasMap > Model::map ( const Parameter & param ) { 
  //
  //
  std::vector < Face > faces;
  std::vector < Face > * faces_ptr;
  faces_ptr = & faces;
  //
  phaseSpace ( param ); 
  //
  boost::shared_ptr < AtlasMap > atlasmap = constructMaps ( booleanconfig_ . phasespace ( ), 
                                                            booleanconfig_ . listboxes ( ), 
                                                            faces_ );
  //
  return atlasmap;
}

inline boost::shared_ptr<Atlas> Model::phaseSpace ( const Parameter & p ) {

  boost::shared_ptr < Atlas > myatlas ( new Atlas );

  std::vector < Face > faces;
  std::vector < Face > * faces_ptr;
  faces_ptr = & faces;

  constructFaces ( faces_ptr, booleanconfig_ . listboxes ( ) );
  faces_ = faces;

  // for ( unsigned int i=0; i<faces_.size(); ++i ) {
  //   std::cout << faces_[i];
  // }


  // exportFaces ( faces_ );
  // exportCharts ( faces_ );

  // for each face, add a chart to the atlas
  // This will keep the ordering between faces and charts consistent
  // Atlas atlas;
  //
  for ( unsigned int i=0; i<faces_.size(); ++i ) {

    std::vector < double > lb, ub;
    int dir;
    // pure convention. for a fixed point we assume spatial direction = 0
    if ( faces_ [ i ] . direction == -1 ) {
      dir = 0;
    } else { 
      dir = faces_ [ i ] . direction;
    }
    // the charts are codimension-1;
    for ( unsigned int j=0; j<dim_; ++j ) {
      if ( j != dir ) { 
        lb . push_back ( faces_ [ i ] . rect . lower_bounds [ j ] );
        ub . push_back ( faces_ [ i ] . rect . upper_bounds [ j ] );
      }
    }
    // check if it is a fixed point
    if ( faces_ [ i ]  . direction != - 1) {
      myatlas -> add_chart ( i, RectGeo ( dim_ - 1, lb, ub ) );
    } else { 
      myatlas -> add_chart ( i, 0, RectGeo ( dim_ - 1, lb, ub ) );
    }
  }
  //
  return myatlas;
}

inline void Model::saveCharts ( int argc, char * argv [] ) { 
  //
  std::string directory ( argv[1] );
  std::string filename ("/charts.dat");
  //
  exportCharts ( (directory+filename).c_str(), faces_ );

}

#endif

#include <vector>
#include <QString>
#include <list>
#include "Meteor.hpp"
#include "Constellation.hpp"
#include "StelObject.hpp"
#include "SolarSystem.hpp"
#include "Planet.hpp"

using namespace std;
$

CREATE STRUCT VIEW Meteor (
       alive BOOL FROM isAlive(),
       startHeight DOUBLE FROM startH,
       endHeight DOUBLE FROM endH,
       velocity DOUBLE FROM velocity,
       magnitude FLOAT FROM mag,
       observDistance DOUBLE FROM xydistance,
       scaleMagnitude DOUBLE FROM distMultiplier
)$

CREATE VIRTUAL TABLE Meteor 
       USING STRUCT VIEW Meteor
       WITH REGISTERED C NAME active
       WITH REGISTERED C TYPE vector<Meteor*>$

CREATE STRUCT VIEW Planet (
       name STRING FROM data()->getNameI18n().toStdString(),       
       hasAtmosphere BOOL FROM data()->hasAtmosphere(),
       radius DOUBLE FROM data()->getRadius(),
       period DOUBLE FROM data()->getSiderealDay(),
       rotObliquity DOUBLE FROM data()->getRotObliquity(),
       distance DOUBLE FROM data()->getDistance(),
       cloudDensity DOUBLE FROM data()->cloudDensity,
       cloudScale FLOAT FROM data()->cloudScale,
       cloudSharpness FLOAT FROM data()->cloudSharpness,
       albedo FLOAT FROM data()->albedo,
       axisRotation FLOAT FROM data()->axisRotation,
//       FOREIGN KEY(parentPlanet_id) FROM parent.data() REFERENCES Planet POINTER,
       FOREIGN KEY(satellites_id) FROM data()->satellites.toStdList() REFERENCES SatellitePlanet
)$

CREATE VIRTUAL TABLE Planet
       USING STRUCT VIEW Planet
       WITH REGISTERED C NAME allPlanets
       WITH REGISTERED C TYPE list<PlanetP>$

CREATE VIRTUAL TABLE SatellitePlanet
       USING STRUCT VIEW Planet
       WITH REGISTERED C TYPE list<QSharedPointer<Planet> >*$

CREATE STRUCT VIEW Constellation (
       constelName STRING FROM getNameI18n().toStdString(),
//       FOREIGN KEY(brightestStar_id) FROM getBrightestStarInConstellation().data() REFERENCES StelObject POINTER,
//       FOREIGN KEY(starList_id) FROM asterism->data() REFERENCES StelObject POINTER
)$

CREATE VIRTUAL TABLE Constellation 
       USING STRUCT VIEW Constellation
       WITH REGISTERED C NAME asterisms
       WITH REGISTERED C TYPE vector<Constellation*>$

CREATE STRUCT VIEW StelObject (
       starName STRING FROM getNameI18n().toStdString(),
       starSciName STRING FROM getEnglishName().toStdString(),
       starType STRING FROM getType().toStdString()
)$

CREATE VIRTUAL TABLE StelObject 
       USING STRUCT VIEW StelObject
       WITH REGISTERED C TYPE StelObject$

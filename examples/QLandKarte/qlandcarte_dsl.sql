#include <QString>
#include <map>
#include <vector>
#include "CTrackDB.h"
#include "CTrack.h"
#include "CMapDB.h"
#include "IMapSelection.h"
#include "CWptDB.h"
#include "CWpt.h"
#include "CRouteDB.h"
#include "CRoute.h"
#include "COverlayDB.h"
#include "IOverlay.h"
#include "CDiaryDB.h"
#include "CDiary.h"

using namespace std;
$

CREATE STRUCT VIEW IItem (
       name STRING FROM getName().toStdString(),
       description STRING FROM getDescription().toStdString(),
       comment STRING FROM getComment().toStdString(),
       timestamp INT FROM getTimestamp()
)$


CREATE STRUCT VIEW MapTrack (
       key STRING FROM first.toStdString(),
       INCLUDES STRUCT VIEW IItem FROM second POINTER,
       descend DOUBLE FROM second->getDescend(),
       ascend DOUBLE FROM second->getAscend(),
       distance DOUBLE FROM second->getTotalDistance(),
       totalTime INT FROM second->getTotalTime(),
       totalTimeMoving INT FROM second->getTotalTimeMoving(),
       FOREIGN KEY(trackWaypoints_id) FROM second->getStdTrackPoints() REFERENCES TrackWaypoint POINTER,
       FOREIGN KEY(stageWaypoints_id) FROM second->getStdStagePoints() REFERENCES StageWaypoint POINTER
)$

CREATE VIRTUAL TABLE MapTrack 
USING STRUCT VIEW MapTrack
WITH REGISTERED C NAME tracks 
WITH REGISTERED C TYPE map<QString, CTrack*>$

CREATE STRUCT VIEW Waypoint (
       INCLUDES STRUCT VIEW IItem,
       lon FLOAT FROM lon,
       lat FLOAT FROM lat,
       ele FLOAT FROM ele,
       prx FLOAT FROM prx,
       link STRING FROM link.toStdString(),
       urlName STRING FROM urlname.toStdString(),
       type STRING FROM type.toStdString(),
       FOREIGN KEY(geoData) FROM getGeocacheData() REFERENCES GeoData   
       // should not be separated according to design rules, however promotes modularity and encapsulation
)$

CREATE STRUCT VIEW MapWaypoint (
       key STRING FROM first.toStdString(),
       INCLUDES STRUCT VIEW Waypoint FROM second POINTER
)$

CREATE VIRTUAL TABLE MapWaypoint
USING STRUCT VIEW MapWaypoint
WITH REGISTERED C NAME wpts 
WITH REGISTERED C TYPE map<QString, CWpt*>$

CREATE STRUCT VIEW TrackWaypoint (
       lon FLOAT FROM lon,
       lat FLOAT FROM lat,
       ele FLOAT FROM ele,
       timestamp INT FROM timestamp,
       speed FLOAT FROM speed,
       avgspeed FLOAT FROM avgspeed,
       delta FLOAT FROM delta,
       azimuth DOUBLE FROM azimuth,
       heartrateBPM INT FROM heartReateBpm,
       cadenceRPM INT FROM cadenceRpm,
       slope FLOAT FROM slope,
       altitude FLOAT FROM altitude,
       height FLOAT FROM height,
       velocity FLOAT FROM velocity,
       heading FLOAT FROM heading,
       magnetic FLOAT FROM magnetic,
       vdop FLOAT FROM vdop,
       hdop FLOAT FROM hdop,
       pdop FLOAT FROM pdop,
       x FLOAT FROM x,
       y FLOAT FROM y,
       z FLOAT FROM z,
       vx FLOAT FROM vx,
       vy FLOAT FROM vy,
       vz FLOAT FROM vz,
       dem FLOAT FROM dem
)$

CREATE VIRTUAL TABLE TrackWaypoint
USING STRUCT VIEW TrackWaypoint
WITH REGISTERED C TYPE list<CTrack::pt_t>$

CREATE STRUCT VIEW StageWaypoint (
       INCLUDES STRUCT VIEW Waypoint FROM wpt POINTER,
       x DOUBLE FROM x,
       y DOUBLE FROM y,
       d DOUBLE FROM d,
       FOREIGN KEY(pt_t_id) FROM trkpt REFERENCES TrackWaypoint
)$

CREATE VIRTUAL TABLE StageWaypoint 
USING STRUCT VIEW StageWaypoint
WITH REGISTERED C TYPE list<CTrack::wpt_t>$

CREATE STRUCT VIEW GeoData (
       difficulty FLOAT FROM difficulty,
       terrain FLOAT FROM terrain,
       status STRING FROM status.toStdString(),
       name STRING FROM name.toStdString(),
       owner STRING FROM owner.toStdString(),
       type STRING FROM type.toStdString(),
       container STRING FROM container.toStdString(),
       shortDesc STRING FROM shortDesc.toStdString(),
       longDesc STRING FROM longDesc.toStdString(),
       hint STRING FROM hint.toStdString(),
       country STRING FROM country.toStdString(),
       state STRING FROM state.toStdString(),
       locale STRING FROM locale.toStdString()
)$

CREATE VIRTUAL TABLE GeoData 
USING STRUCT VIEW GeoData
WITH REGISTERED C TYPE CWpt::geocache_t$

CREATE STRUCT VIEW Route (
       key STRING FROM first.toStdString(),
       INCLUDES STRUCT VIEW IItem FROM second POINTER,
       distance DOUBLE FROM second->getDistance(),
       time INT FROM second->getTime()
)$

CREATE VIRTUAL TABLE Route
USING STRUCT VIEW Route
WITH REGISTERED C NAME routes 
WITH REGISTERED C TYPE map<QString, CRoute*>$


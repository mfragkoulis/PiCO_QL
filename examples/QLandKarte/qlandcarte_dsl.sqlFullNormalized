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
;

CREATE STRUCT VIEW IItem (
       name STRING FROM getName().toStdString(),
       description STRING FROM getDescription().toStdString(),
       comment STRING FROM getComment().toStdString(),
       timestamp INT FROM getTimestamp()
);


CREATE STRUCT VIEW MapTracks (
       key STRING FROM first.toStdString(),
       FOREIGN KEY(track_id) FROM second REFERENCES Track POINTER
);

CREATE VIRTUAL TABLE LandKarte.MapTracks 
USING STRUCT VIEW MapTracks
WITH REGISTERED C NAME tracks 
WITH REGISTERED C TYPE map<QString, CTrack*>;

CREATE STRUCT VIEW Track (
       INHERITS STRUCT VIEW IItem,
       descend DOUBLE FROM getDescend(),
       ascend DOUBLE FROM getAscend(),
       distance DOUBLE FROM getTotalDistance(),
       totalTime INT FROM getTotalTime(),
       totalTimeMoving INT FROM getTotalTimeMoving(),
       FOREIGN KEY(trackWaypoints_id) FROM getStdTrackPoints() REFERENCES TrackWaypoints POINTER,
       FOREIGN KEY(stageWaypoints_id) FROM getStdStagePoints() REFERENCES StageWaypoints POINTER
);

CREATE VIRTUAL TABLE LandKarte.Track 
USING STRUCT VIEW Track
WITH REGISTERED C TYPE CTrack;

CREATE STRUCT VIEW TrackWaypoints (
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
);

CREATE VIRTUAL TABLE LandKarte.TrackWaypoints 
USING STRUCT VIEW TrackWaypoints
WITH REGISTERED C TYPE list<CTrack::pt_t>;

CREATE STRUCT VIEW StageWaypoints (
       FOREIGN KEY(wpt_id) FROM wpt REFERENCES Waypoint POINTER,
       x DOUBLE FROM x,
       y DOUBLE FROM y,
       d DOUBLE FROM d,
       FOREIGN KEY(pt_t_id) FROM trkpt REFERENCES TrackWaypoint
);

CREATE VIRTUAL TABLE LandKarte.StageWaypoints 
USING STRUCT VIEW StageWaypoints
WITH REGISTERED C TYPE list<CTrack::wpt_t>;

CREATE STRUCT VIEW Waypoints (
       key STRING FROM first.toStdString(),
       FOREIGN KEY(wpt_id) FROM second REFERENCES Waypoint POINTER
);

CREATE VIRTUAL TABLE LandKarte.Waypoints 
USING STRUCT VIEW Waypoints
WITH REGISTERED C NAME wpts 
WITH REGISTERED C TYPE map<QString, CWpt*>;

CREATE STRUCT VIEW Waypoint (
       INHERITS STRUCT VIEW IItem,
       lon FLOAT FROM lon,
       lat FLOAT FROM lat,
       ele FLOAT FROM ele,
       prx FLOAT FROM prx,
       link STRING FROM link.toStdString(),
       urlName STRING FROM urlname.toStdString(),
       type STRING FROM type.toStdString(),
       FOREIGN KEY(geoData) FROM getGeocacheData() REFERENCES GeoData   // should not be separated according to design rules, however promotes modularity and encapsulation
);

CREATE VIRTUAL TABLE LandKarte.Waypoint 
USING STRUCT VIEW Waypoint
WITH REGISTERED C TYPE CWpt;

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
);

CREATE VIRTUAL TABLE LandKarte.GeoData 
USING STRUCT VIEW GeoData
WITH REGISTERED C TYPE CWpt::geocache_t;

CREATE STRUCT VIEW Routes (
       key STRING FROM first.toStdString(),
       FOREIGN KEY(route_id) FROM second REFERENCES Route POINTER
);

CREATE VIRTUAL TABLE LandKarte.Routes 
USING STRUCT VIEW Routes
WITH REGISTERED C NAME routes 
WITH REGISTERED C TYPE map<QString, CRoute*>;

CREATE STRUCT VIEW Route (
       INHERITS STRUCT VIEW IItem,
       distance DOUBLE FROM getDistance(),
       time INT FROM getTime()
);

CREATE VIRTUAL TABLE LandKarte.Route 
USING STRUCT VIEW Route
WITH REGISTERED C TYPE CRoute;

CREATE STRUCT VIEW Diaries (
       key STRING FROM first.toStdString(),
       FOREIGN KEY(diary_id) FROM second REFERENCES Diary POINTER
);

//CREATE VIRTUAL TABLE LandKarte.Diaries 
//USING STRUCT VIEW Diaries
//WITH REGISTERED C NAME diaries 
//WITH REGISTERED C TYPE map<QString, CDiary*>;

//CREATE STRUCT VIEW Diary (
//       INHERITS STRUCT VIEW IItem,
//       modified BOOL FROM isModified()
//);

//CREATE VIRTUAL TABLE LandKarte.Diary 
//USING STRUCT VIEW Diary
//WITH REGISTERED C TYPE CDiary;

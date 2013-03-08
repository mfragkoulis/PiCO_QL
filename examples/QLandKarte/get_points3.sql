% PiCO QL query

SELECT name, lon, lat, ele 
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id 
WHERE ele > 20
UNION 
SELECT name, lon, lat, ele 
FROM MapWayPoint 
WHERE ele > 20;

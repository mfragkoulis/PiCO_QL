PiCO QL evaluation query 1:

SELECT name, descend, ascend, distance, totalTime, 
       totalTimeMoving, azimuth, max(speed)
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id 
GROUP BY azimuth 
ORDER BY max(speed);

PiCO QL evaluation query 2:

SELECT name, descend, ascend, distance, totalTime, 
       totalTimeMoving, HeartRateBpm, ele, max(avgSpeed)
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id
GROUP BY heartRateBpm, ele 
HAVING max(avgSpeed) > 0 
ORDER BY max(avgSpeed) DESC;

PiCO QL evaluation query 3:

SELECT name, lon, lat, ele 
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id 
WHERE ele > 20
UNION 
SELECT name, lon, lat, ele 
FROM MapWayPoint 
WHERE ele > 20;

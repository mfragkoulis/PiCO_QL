% PiCO QL query

SELECT name, descend, ascend, distance, totalTime, 
       totalTimeMoving, HeartRateBpm, ele, max(avgSpeed)
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id
GROUP BY heartRateBpm, ele 
HAVING max(avgSpeed) > 0 
ORDER BY max(avgSpeed) DESC;

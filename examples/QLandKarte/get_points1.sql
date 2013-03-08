% PiCO QL query

SELECT name, descend, ascend, distance, totalTime, 
       totalTimeMoving, azimuth, max(speed)
FROM MapTrack 
JOIN TrackWayPoint 
ON TrackWayPoint.base = MapTrack.trackwaypoints_id 
GROUP BY azimuth 
ORDER BY max(speed);

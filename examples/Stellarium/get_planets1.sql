% PiCO QL query

SELECT P.name, P.axisRotation, min(SP.axisRotation) 
FROM Planet AS P 
JOIN SatellitePlanet AS SP 
ON SP.base = P. satellites_id
WHERE P.axisRotation > SP.axisRotation 
AND SP.base NOT LIKE '(empty)' 
GROUP BY P.name 
ORDER BY P.axisRotation DESC;

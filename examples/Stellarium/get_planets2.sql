% PiCO QL query

SELECT P.name, P.radius, P.period, P.albedo, count(*) 
FROM Planet as P 
JOIN SatellitePlanet as SP 
ON SP.base = P.satellites_id
WHERE P.onScreen
AND SP.hasAtmosphere 
GROUP BY P.name;

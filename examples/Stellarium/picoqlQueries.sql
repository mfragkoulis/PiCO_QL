PiCO QL evaluation query 1:

SELECT observDistance, velocity, magnitude, scaleMagnitude 
FROM Meteor
WHERE alive 
AND observDistance <
      (SELECT min(distance) 
       FROM Planet 
       WHERE name 
       NOT LIKE 'Earth');

PiCO QL evaluation query 2:

SELECT P.name, P.axisRotation, min(SP.axisRotation) 
FROM Planet AS P 
JOIN SatellitePlanet AS SP 
ON SP.base = P. satellites_id
WHERE P.axisRotation > SP.axisRotation 
AND SP.base NOT LIKE '(empty)' 
GROUP BY P.name 
ORDER BY P.axisRotation DESC;

PiCO QL evaluation query 3:

SELECT P.name, P.radius, P.period, P.albedo, count(*) 
FROM Planet as P 
JOIN SatellitePlanet as SP 
ON SP.base = P.satellites_id
WHERE SP.hasAtmosphere 
GROUP BY P.name;

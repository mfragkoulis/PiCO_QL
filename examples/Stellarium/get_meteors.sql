% PiCO QL query

SELECT observDistance, velocity, magnitude, scaleMagnitude 
FROM Meteor
WHERE alive 
AND observDistance <
      (SELECT min(distance) 
       FROM Planet 
       WHERE name 
       NOT LIKE 'Earth');

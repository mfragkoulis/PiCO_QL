% PiCO QL query

SELECT Callname 
FROM Functionmap
WHERE NOT CallFilereadOnly 
AND CallFANIN=0
ORDER BY Callname;

PiCO QL evaluation query 1:

SELECT Identifierid 
FROM IdentifierProperties 
WHERE EclassisLscope 
AND NOT EclassisCscope 
AND EclassisOrdinary 
AND NOT Identifierxfile 
AND NOT EclassisReadonly 
AND NOT EclassisFunction 
AND NOT Eclassunused 
ORDER BY Identifierid;

PiCO QL evaluation query 2:

SELECT DISTINCT Filepath
FROM IdentifierProperties
LEFT JOIN Tokid
ON Tokid.BASE = IdentifierProperties.Eclassmembers
WHERE EclassisLscope
AND Eclassunused
AND NOT EclassisReadonly
ORDER BY Filepath;

PiCO QL evaluation query 3:

SELECT Callname 
FROM Functionmap
WHERE NOT CallFilereadOnly 
AND CallFANIN=0
ORDER BY Callname;

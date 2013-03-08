% PiCO QL query

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

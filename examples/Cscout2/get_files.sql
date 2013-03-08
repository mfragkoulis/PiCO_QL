% PiCO QL query

SELECT DISTINCT Filepath
FROM IdentifierProperties
LEFT JOIN Tokid
ON Tokid.BASE = IdentifierProperties.Eclassmembers
WHERE EclassisLscope
AND Eclassunused
AND NOT EclassisReadonly
ORDER BY Filepath;

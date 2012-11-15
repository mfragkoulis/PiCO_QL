//.cpp #include <vector>
#include "Monetary_System.h"
#include "Money.h"
//.cpp using namespace std;
;

CREATE UNION VIEW price (
       CASE price_mode
       WHEN 0 THEN main DOUBLE FROM main
       WHEN 1 THEN name TEXT FROM name
       WHEN 2 THEN sub INT FROM sub
);

CREATE STRUCT VIEW Price (
       union_mode INT FROM price_mode,
       price union FROM p
);

CREATE VIRTUAL TABLE myCApp.Price
USING STRUCT VIEW Price
WITH REGISTERED C NAME price
WITH REGISTERED C TYPE price;

CREATE STRUCT VIEW GoldEquivalent (
       pound DOUBLE FROM pound,
       ounce DOUBLE FROM ounce,
       grain DOUBLE FROM grain,
       exchange_rate DOUBLE FROM exchange_rate
);

CREATE VIRTUAL TABLE myCApp.GoldEquivalent
USING STRUCT VIEW GoldEquivalent
WITH REGISTERED C TYPE goldEquivalent;

CREATE UNION VIEW weight (
       CASE weight_mode
       WHEN 0 THEN name TEXT FROM name
       WHEN 1 THEN kg DOUBLE FROM kg
       WHEN 2 THEN gr INT FROM gr
       WHEN 3 THEN FOREIGN KEY(goldEquivalent_id) 
       	    FROM g_weight REFERENCES GoldEquivalent
);

CREATE STRUCT VIEW Money (
       name TEXT FROM name,
       price_mode INT FROM price_mode,
       price UNION FROM prc,
       weight_mode INT FROM weight_mode,
       weight UNION FROM wgt
);

CREATE VIRTUAL TABLE myCApp.Money
USING STRUCT VIEW Money
//WITH REGISTERED C NAME money
WITH REGISTERED C TYPE clist<struct Money*>
USING ITERATOR NAME next;
//.cpp C TYPE vector<Money>;

CREATE VIRTUAL TABLE myCApp.MoneyList
USING STRUCT VIEW Money
WITH REGISTERED C NAME money
WITH REGISTERED C TYPE clist<struct Money*>
USING ITERATOR NAME next;
//.cpp C TYPE vector<Money>;

CREATE STRUCT VIEW MonetarySystem (
       nCurrency INT FROM nCurrency,
       FOREIGN KEY(currency) FROM root REFERENCES Money POINTER
);

CREATE VIRTUAL TABLE myCApp.MonetarySystem
USING STRUCT VIEW MonetarySystem
WITH REGISTERED C NAME monetary_system
WITH REGISTERED C TYPE Monetary_System;
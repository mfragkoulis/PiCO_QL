#include "SuperAccount.h"
#include "Account.h"
#include "SpecialAccount.h"
#include <map>
#include <list>
#include <vector>
#include <string>

using namespace std;
$
// SuperAccount description
CREATE STRUCT VIEW SuperAccount (
       balance DOUBLE FROM get_balance(),
       account_no TEXT FROM get_account_no(),
       rate DOUBLE FROM get_rate(),
       isbn BIGINT FROM get_isbn(),
       // Column delimeter is",\n". Comments should occupy a separate line
       isbn_root DOUBLE FROM get_math_root(this.get_isbn())
)$

CREATE STRUCT VIEW SuperAccounts (
       id STRING FROM first,
       INCLUDES STRUCT VIEW SuperAccount FROM second)$

CREATE VIRTUAL TABLE SuperAccounts 
USING STRUCT VIEW SuperAccounts
WITH REGISTERED C NAME superaccounts 
WITH REGISTERED C TYPE map<string,SuperAccount>$

CREATE STRUCT VIEW Account (
       INCLUDES STRUCT VIEW SuperAccount,
       type TEXT FROM type)$

CREATE VIRTUAL TABLE Accounts 
USING STRUCT VIEW Account
WITH REGISTERED C NAME accounts 
WITH REGISTERED C TYPE vector<Account>$

CREATE STRUCT VIEW SpecialAccount (
       INCLUDES STRUCT VIEW SuperAccount,
       bonus DOUBLE FROM bonus)$

CREATE VIRTUAL TABLE Specialaccounts 
USING STRUCT VIEW SpecialAccount
WITH REGISTERED C NAME specialaccounts 
WITH REGISTERED C TYPE list<SpecialAccount>$

CREATE VIEW Myaccounts AS 
SELECT balance,type
FROM Accounts 
WHERE balance > 100$

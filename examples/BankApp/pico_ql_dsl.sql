#include "SuperAccount.h"
#include "Account.h"
#include "SpecialAccount.h"
#include <map>
#include <list>
#include <vector>
;

CREATE ELEMENT TABLE SuperAccount (
       balance DOUBLE FROM get_balance(),
       account_no TEXT FROM get_account_no(),
       rate DOUBLE FROM get_rate(),
       isbn INT FROM get_isbn());

CREATE ELEMENT TABLE SuperAccounts (
       id STRING FROM first,
       $SuperAccount FROM second.);

CREATE TABLE Bank.SuperAccounts WITH BASE = superaccounts AS SELECT * FROM map<string,SuperAccount>;

CREATE ELEMENT TABLE Account (
       $SuperAccount,
       type TEXT FROM type);

CREATE TABLE Bank.Accounts WITH BASE=accounts AS SELECT * FROM vector<Account>;

CREATE ELEMENT TABLE SpecialAccount (
       $SuperAccount,
       bonus DOUBLE FROM bonus);

CREATE TABLE Bank.Specialaccounts WITH BASE=specialaccounts AS SELECT * FROM list<SpecialAccount>;


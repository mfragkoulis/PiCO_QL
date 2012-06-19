/*
 *   Implement the main method for the Bank Application.
 *
 *   Copyright 2012 Marios Fragkoulis
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS.
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *  permissions and limitations under the License.
 */

#include <cstdio>
#include <string>
#include <map>
#include <list>
#include <vector>
#include "SpecialAccount.h"
#include "Account.h"
#include "SuperAccount.h"
/* PiCO_QL header */
#include "pico_ql_search.h"
/*------------*/

using namespace std;

int main() {

    map<string,SuperAccount> superaccounts;
    list<SpecialAccount> specialaccounts;
    vector<Account> accounts;
    
    Account acc1("10068", 500.0, 478923743, "credit");
    Account acc2("10234", 394.28, 692346729, 
		 "debit-consuming");
    Account acc3("22974", -347389, 580383483, 
		 "debit-first house");
    Account acc4("34369", 48923, 347304830, "credit");
    Account acc5("47023", 445567, 830343479, 
		 "debit-business");
    Account acc6("34807", 23489, 394847344, 
		 "credit-fundtrust");
    accounts.push_back(acc1);
    accounts.push_back(acc2);
    accounts.push_back(acc3);
    accounts.push_back(acc4);
    accounts.push_back(acc5);
    accounts.push_back(acc6);

    SpecialAccount spa1("23999", 1000.00, 639269243);
    spa1.calculate_bonus();
    SpecialAccount spa2("69263", 131.00, 457349341);
    spa2.calculate_bonus();
    SpecialAccount spa3("45483", 4784.58, 580540484);
    spa3.calculate_bonus();
    SpecialAccount spa4("28902", 2784.59, 197396445);
    spa4.calculate_bonus();
    SpecialAccount spa5("04023", 284.58, 347893488);
    spa5.calculate_bonus();
    SpecialAccount spa6("28904", -589045.32, 808023242);
    spa6.calculate_bonus();
    specialaccounts.push_back(spa1);
    specialaccounts.push_back(spa2);
    specialaccounts.push_back(spa3);
    specialaccounts.push_back(spa4);
    specialaccounts.push_back(spa5);
    specialaccounts.push_back(spa6);

    SuperAccount sa1("46738", 46423, 362626823);
    SuperAccount sa2("34734", 26834, 236326823);
    SuperAccount sa3("2532", 346.23, 234628621);
    SuperAccount sa4("26738", -262, 234674389);
    SuperAccount sa5("23683", 2363, 729367263);
    SuperAccount sa6("3462", 47235, 676328962);
    map<string,SuperAccount>::iterator it;
    superaccounts.insert(make_pair("0", acc1));
    superaccounts.insert(make_pair("1", acc2));
    superaccounts.insert(make_pair("12", acc3));
    superaccounts.insert(make_pair("18", acc4));
    superaccounts.insert(make_pair("15", acc5));
    superaccounts.insert(make_pair("10", acc6));
    superaccounts.insert(make_pair("23", spa1));
    superaccounts.insert(make_pair("31", spa2));
    superaccounts.insert(make_pair("34", spa3));
    superaccounts.insert(make_pair("36", spa4));
    superaccounts.insert(make_pair("39", spa5));
    superaccounts.insert(make_pair("33", spa6));
    superaccounts.insert(make_pair("41", sa1));
    superaccounts.insert(make_pair("59", sa2));
    superaccounts.insert(make_pair("22", sa3));
    superaccounts.insert(make_pair("5", sa4));
    superaccounts.insert(make_pair("9", sa5));
    superaccounts.insert(make_pair("21", sa6));

    pico_ql_register(&accounts, "accounts");
    pico_ql_register(&superaccounts, "superaccounts");
    pico_ql_register(&specialaccounts, "specialaccounts");
    while (pico_ql_serve()) {};

    for (it = superaccounts.begin(); it != superaccounts.end(); it++) {
	printf("%i\n", it->second.get_isbn());
	
    }

}

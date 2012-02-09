#include <stdio.h>
#include <string>
#include <map>
#include "SpecialAccount.h"
#include "Account.h"
#include "SuperAccount.h"
#include "stl_search.h"

using namespace std;

map<string,SuperAccount> accounts;

int main(){
    
    Account acc1("10068", 500.0, 478923743, -13);
    Account acc2("10234", 394.28, 692346729, -25);
    SpecialAccount spa1("23999", 1000.00, 639269243, 34);
    SpecialAccount spa2("69263", 131.00, 197396445, 72);
    SuperAccount sa1(39);
    SuperAccount sa2(91);
//    map<string,SuperAccount> accounts;
    map<string,SuperAccount>::iterator it;
    accounts.insert(make_pair("0", acc1));
    accounts.insert(make_pair("1", acc2));
    accounts.insert(make_pair("2", spa1));
    accounts.insert(make_pair("3", spa2));
    accounts.insert(make_pair("4", sa1));
    accounts.insert(make_pair("5", sa2));
    int re_sqtl = call_sqtl();
    printf("Thread sqlite returned %i\n", re_sqtl);

    for (it=accounts.begin(); it != accounts.end();it++) {
	printf("%i\n", it->second.get_iba());
	
    }

}

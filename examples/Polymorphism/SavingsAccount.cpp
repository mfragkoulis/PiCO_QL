#include <string>
#include "SavingsAccount.h"
#include "Account.h"

using namespace std;

SavingsAccount::SavingsAccount(string d, double b,
			       double aa, double ba, 
			       string dd) : Account(d, b, 
						    aa, ba)
{

  due_date = dd;

}

double SavingsAccount::get_balance() {
  return balance*1.1;
}

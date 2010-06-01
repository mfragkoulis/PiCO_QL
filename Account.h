#ifndef ACCOUNT_H
#define ACCOUNT_H


class Account {

 private:
  char *account_no;
  float balance;

 public:
  Account(char *acc_no, float b);
  float get_balance();
  char * get_account_no();
};


#endif

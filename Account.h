#ifndef ACCOUNT_H
#define ACCOUNT_H


class Account{

 private:
  const char *account_no;
  float balance;

 public:
  Account(const char *acc_no, float b);
  float get_balance() const;
  const char * get_account_no() const;
};


#endif

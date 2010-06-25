#ifndef ACCOUNT_H
#define ACCOUNT_H


class Account{

 private:
  const char *account_no;
  float balance;
  int isbn;

 public:
  Account(const char *acc_no, float b, int i);
  Account(const char *acc_no, float b);
  float get_balance() const;
  const char * get_account_no() const;
  int get_isbn() const;
};


#endif

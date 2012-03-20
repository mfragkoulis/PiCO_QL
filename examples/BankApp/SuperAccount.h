#ifndef SUPERACCOUNT_H
#define SUPERACCOUNT_H

class SuperAccount {

 protected:
  const char *account_no;
  float balance;
  int isbn;

 public:
  SuperAccount(const char *acc_no, float b, int i);
  SuperAccount(const char *acc_no, float b);
  float get_balance() const;
  const char * get_account_no() const;
  int get_isbn() const;
  virtual double get_rate();

};

#endif

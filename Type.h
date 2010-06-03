#ifndef TYPE_H
#define TYPE_H

class Type {

 private:
  char *text;
  float doub;
  int integ;
  void *blob;
  int type_of_value;

 public:
  Type();
  void set_int(int i);
  void set_doub(float f);
  void set_text(const unsigned char * t);
  void set_blob(const void * b);
  void set_type_of_value(int tov);
  int get_type_of_value();
  int get_int();
  float get_doub();
  char * get_text();

};


#endif

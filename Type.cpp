#include "Type.h"

Type::Type() {
  text='\0';
  doub=0.0;
  integ=0;
  blob=0;
  type_of_value=-1;
}

void Type::set_int(int i) {
  integ=i;
}

void Type::set_doub(float f) {
  doub=f;
}

void Type::set_text(const unsigned char * t) {
  text=(char *)t;
}

void Type::set_blob(const void * b) {
  blob=(void *)b;
}

float Type::get_doub() {
  return doub;
}

void Type::set_type_of_value(int tov) {
  type_of_value=tov;
}

int Type::get_type_of_value() {
  return type_of_value;
}

char * Type::get_text() {
  return text;
}

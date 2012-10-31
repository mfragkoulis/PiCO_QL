#ifndef MONEY_H
#define MONEY_H


typedef union {

  double main;
  char name[30];
  int sub;

} uprice;

typedef struct {

  double pound;
  double ounce;
  double grain;
  double exchange_rate;

} goldEquivalent;

typedef union {

  char name[30];
  double kg;
  int gr;
  goldEquivalent g_weight;

} weight;

typedef struct {

  char name[30];
  int price_mode;
  uprice prc;
  int weight_mode;
  weight wgt;
  
} Money;

typedef struct {

  int price_mode;
  uprice p;

} price;

#endif

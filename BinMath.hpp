// Borland C++ Builder
// Copyright (c) 1995, 2005 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Binmath.pas' rev: 10.00

#ifndef BinmathHPP
#define BinmathHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Binmath
{
//-- type declarations -------------------------------------------------------
typedef double __fastcall (*TFunkcjaObliczen0arg)(void);

typedef double __fastcall (*TFunkcjaObliczen1arg)(double A);

typedef double __fastcall (*TFunkcjaObliczen2arg)(double A, double B);

typedef double __fastcall (*TFunkcjaObliczen3arg)(double A, double B, double C);

typedef double __fastcall (*TFunkcjaObliczen4arg)(double A, double B, double C, double D);

class DELPHICLASS TStosDane;
class PASCALIMPLEMENTATION TStosDane : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	AnsiString Nazwa;
	int Priorytet;
	
public:
	__fastcall TStosDane(AnsiString S, int I);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TStosDane(void) { }
	#pragma option pop
	
};


class DELPHICLASS TStos;
class PASCALIMPLEMENTATION TStos : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	Classes::TList* FLista;
	
public:
	__fastcall TStos(void);
	__fastcall virtual ~TStos(void);
	void __fastcall Dodaj(AnsiString S, int Priorytet = 0x0);
	AnsiString __fastcall Zdejmij();
	bool __fastcall Istnieje(void);
	int __fastcall AktualnyPriorytet(void);
	void __fastcall Wyczysc(void);
};


class DELPHICLASS TZmienneDane;
class PASCALIMPLEMENTATION TZmienneDane : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	AnsiString Nazwa;
	double Wartosc;
	
public:
	__fastcall TZmienneDane(AnsiString N, double W);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TZmienneDane(void) { }
	#pragma option pop
	
};


class DELPHICLASS TFunkcjeDane;
class PASCALIMPLEMENTATION TFunkcjeDane : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	AnsiString Nazwa;
	int Priorytet;
	void *Adres;
	int Argumentow;
	
public:
	__fastcall TFunkcjeDane(AnsiString N, int P, void * D, int A);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TFunkcjeDane(void) { }
	#pragma option pop
	
};


class DELPHICLASS TObliczenia;
class PASCALIMPLEMENTATION TObliczenia : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	Classes::TList* FListaFunkcji;
	Classes::TList* FZmienne;
	TStos* FStosONP;
	TStos* FStosWynik;
	AnsiString FWyrazenie;
	AnsiString FONP;
	int FKodError;
	void __fastcall GenerujONP(void);
	
public:
	void __fastcall Wyrazenie(AnsiString S);
	double __fastcall Wynik(void);
	AnsiString __fastcall ONP();
	int __fastcall Blad(void);
	AnsiString __fastcall BladOpis();
	void __fastcall FunkcjeRejestruj(AnsiString Nazwa, int Priorytet, int Arg, void * Adres);
	int __fastcall FunkcjeID(AnsiString Nazwa);
	TFunkcjeDane* __fastcall FunkcjeInfo(int Id);
	void __fastcall FunkcjeWyczysc(void);
	void __fastcall ZmienneDodaj(AnsiString N, double W);
	void __fastcall ZmienneWyczysc(void);
	__fastcall TObliczenia(void);
	__fastcall virtual ~TObliczenia(void);
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE AnsiString __fastcall Eval(AnsiString Expression);
extern PACKAGE AnsiString __fastcall Interpreter(AnsiString Input);

}	/* namespace Binmath */
using namespace Binmath;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Binmath

//$$---- Form CPP ----
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <Math.hpp>

#include "FncProperity.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFFncProperity *FFncProperity;
//---------------------------------------------------------------------------
__fastcall TFFncProperity::TFFncProperity(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFFncProperity::AddNext(String aCaption, String aData)
{
	TListItem *Item = ListView->Items->Add();
	aCaption = aCaption.TrimLeft();
	if (LastHeader != aCaption) Item->Caption = aCaption;
	else Item->Caption = "";
	Item->SubItems->Add(aData);
	LastHeader = aCaption;
}
//---------------------------------------------------------------------------
void TFFncProperity::RefreshList()
{
	ListView->Items->BeginUpdate();
	ListView->Items->Clear();
	LastHeader = "";

	if(Wzr1->Checked) AddNext("Wzór", OwnResult->Expression);
	if(Zmienne1->Checked) AddNext("Zmienne", Varibles);
	if(Wartomaksimin1->Checked){
		int AV = OwnResult->Varibles->Count;
		int Count = OwnResult->ResultCount - 2;
		double V1 = OwnResult->Result[0][AV];
		double V2 = OwnResult->Result[Count][AV];  
		if(MaxValue == V1 || MaxValue == V2) {
			AddNext("Wartoœæ maks.", "nie istnieje");
		} else AddNext("Wartoœæ maks.", MaxValue);
		if(MinValue == V1 || MinValue == V2) {
			AddNext("Wartoœæ min.", "nie istnieje");
		} else AddNext("Wartoœæ min.", MinValue);
	}
	if(Miejscazerowe1->Checked){
		for (int i = 0; i < ZeroPlaces->Count; i++) {
			AddNext("Miejsca zerowe", ZeroPlaces->operator [](i));
		}
	}
	ListView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void TFFncProperity::ShowProperity(TCalcResult *Result)
{
	OwnResult = Result;

	Varibles = "";
	int aVC = OwnResult->Varibles->Count;
	for (int i = 0; i < aVC; i++) {
		Varibles = Varibles + " " + Result->Varibles->operator [](i);
	}
	Varibles = Varibles.TrimLeft();

	ZeroPlaces->Clear();
	double Last = Result->Result[0][aVC];
	String Temp;
	MaxValue = 0;
	MinValue = 0;

	for (int i = 0; i < Result->ResultCount - 1; i++) {
		double AResult = Result->Result[i][aVC];
		MaxValue = Max(AResult, MaxValue);
		MinValue = Min(AResult, MinValue);
		if(AResult == 0 || (Last != 0 && Sign(Last) != Sign(Result->Result[i][aVC]))) {
			Temp = "";
			for (int j = 0; j < aVC; j++) {
				Temp = Temp + " " + Result->Varibles->operator [](j) + "=" + FloatToStr(Result->Result[i][j]);
			}
			Temp = Temp.TrimLeft();
			ZeroPlaces->Add(Temp);
		}

		Last = Result->Result[i][aVC];
	}
	Miejscazerowe1->Checked = OwnResult->Varibles->Count == 1;
	Miejscazerowe1->Caption = "Miejsca zerowe(" + IntToStr(ZeroPlaces->Count) + ")";
	RefreshList();
}
//---------------------------------------------------------------------------
void __fastcall TFFncProperity::FormCreate(TObject *Sender)
{
	 ZeroPlaces = new TStringList();
}
//---------------------------------------------------------------------------
void __fastcall TFFncProperity::FormDestroy(TObject *Sender)
{
	delete ZeroPlaces;
}
//---------------------------------------------------------------------------
void __fastcall TFFncProperity::Miejscazerowe1Click(TObject *Sender)
{
	RefreshList();
}
//---------------------------------------------------------------------------


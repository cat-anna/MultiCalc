//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FncForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFFncEdt *FFncEdt;
//---------------------------------------------------------------------------
__fastcall TFFncEdt::TFFncEdt(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TFFncEdt::LoadList(TFunctionsList* NewList)
{
	List = NewList;
	RefreshList();
}
//---------------------------------------------------------------------------
void TFFncEdt::RefreshList()
{
	List->Sort();
	lbFncs->Items->BeginUpdate();
	lbFncs->Items->Clear();
	for (int i = 0; i < List->Count; i++) {
		TFunction *Fnc = List->GetFunction(i);
		lbFncs->Items->Add(Fnc->Name);
	}
	lbFncs->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::lbFncsDrawItem(TWinControl *Control, int Index,
	  TRect &Rect, TOwnerDrawState State)
{
	lbFncs->Canvas->FillRect(Rect);
	switch (List->GetFunction(Index)->Type) {
		case flitInternal:
			lbFncs->Canvas->Font->Color = clSilver;
		break;
		case flitString:
			lbFncs->Canvas->Font->Color = clBlack;
		break;
	}
	lbFncs->Canvas->TextOut(Rect.Left + 2, Rect.Top + 2, List->GetFunction(Index)->Name);
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::lbFncsClick(TObject *Sender)
{
	if (lbFncs->ItemIndex == -1)return;

	TFunction* Fnc = List->GetFunction(lbFncs->ItemIndex);
	leName->Text = Fnc->Name;
	lParCount->Caption = "Iloœæ parametrów: " + IntToStr(Fnc->ParamsCount);
	bUsun->Enabled = Fnc->Type != flitInternal;
	bModify->Enabled = Fnc->Type != flitInternal;
	switch (Fnc->Type) {
		case flitInternal:
			rbwew->Checked = true;
		break;
		case flitString:
			rbwers->Checked = true;
			leData->Text = Fnc->Data;
		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::rbwersClick(TObject *Sender)
{
	GroupBox3->Enabled = true;
	bModify->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::rbwewClick(TObject *Sender)
{
	GroupBox3->Enabled = false;
	bModify->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::bModifyClick(TObject *Sender)
{
	if (!Check()|| rbwew->Checked) return;
	if (List->FunctionExists(leName->Text)) {
		List->ModifyFunction(leData->Text, leName->Text, AParCount);
	} else {
		List->AddFunction(leData->Text, leName->Text, AParCount);
	}
	RefreshList();
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::bUsunClick(TObject *Sender)
{
	List->DeleteFunction(leName->Text.LowerCase());
}
//---------------------------------------------------------------------------
bool TFFncEdt::Check()
{
	String Temp = leData->Text.LowerCase();
	if (Temp.AnsiPos("[") == 0 || Temp.AnsiPos("]") == 0) {
		return false;
	}

	AParCount = 0;
	String Numbers = "0123456789";
	int i = 1;
	int Last;
	bool Opened = false;
	while (i <= Temp.Length()){
		if (Temp[i] == ']') {
			if (Opened){
				AParCount = Max(StrToInt(Temp.SubString(Last + 1, (i - 1) - Last)) + 1, AParCount);
			}else return false;
			Opened = false;
		}
		if (Opened && Numbers.AnsiPos(Temp[i]) == 0) return false;
		if (Temp[i] == '[') {
			if (Opened) return false;
			Opened = true;
			Last = i;
		}
		i++;
	}
	
	TWord *Word = new TWord(List->ControllClass);
	bool Result = Word->CheckProperity(Temp);
	delete Word;
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TFFncEdt::leDataChange(TObject *Sender)
{
	if (Check()) {
		lParCount->Caption = "Iloœæ parametrów: " + IntToStr(AParCount);
	} else lParCount->Caption = "W wyra¿eniu jest b³¹d!";
}
//---------------------------------------------------------------------------


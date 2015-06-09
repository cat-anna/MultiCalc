
#pragma hdrstop

#include "Calculator.h"
#include <Math.hpp>
#include <StrUtils.hpp>
/* ((6,6-3-3/14)*(5+5/6))/((21-0,25)/2,5) */

const String OpQueue[3][2] = {"^", "^", "*", "/", "-", "+"};
const int CompFncData[2][2] = {0, 1, -1, 0};
//------------------------------------------------------------------------------

TCharType CharType(char c)
{
	switch (c) {
		case '-': return ctMinus;
		case '(': case '[': return ctOpenBracket;
		case ')': case ']': return ctCloseBracket;
		case '=': return ctEqual;
		case ';': return ctSemi;
		case ',': return ctComma;
	}
	String N = "0123456789";
	if (N.Pos(c) > 0) return ctNumber;
	N = "*+/^!";
	if (N.Pos(c) > 0) return ctOperator;
	N = "zxcvbnmasdfghjklpoiuytrewq";
	if (N.Pos(c) > 0) return ctChar;
	return ctUnknown;
}

int FindOperator(int OperatorPair, TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		TWord *W = ((TWord*)List->Items[i]);
		if (W->SelfType == wtOperator) {
			String Last = W->SelfExpression;
			if (Last.Pos(OpQueue[OperatorPair][0]) > 0 ||
				Last.Pos(OpQueue[OperatorPair][1])) {
				return i;
			}
		}
	}
	return -1;
}

inline String ChangeComma(String S)
{
	for(int i = 1; i <= S.Length(); i++){
		if(S[i] == '.') S[i] = ',';
	}
	return S;
}

bool IsInt(String S)
{
	try {
		S.ToInt();
	}
	catch (...) {
		return false;
	}
	return true;
}

bool IsNumber(String S)
{
	try{
		StrToFloat(S);
	}
	catch(...){
		return false;
	}
	return true;
}

RType STF(String S)
{
	try
	{
		for(int i = 1; i <= S.Length(); i++){
			if(S[i] == '.') S[i] = ',';
		}
		return S.ToDouble();
	}
	catch(...)
	{
        return 0;
    }
}

String FTS(RType D)
{
	String S = FloatToStr(D);
	for(int i = 1; i <= S.Length(); i++) {
		if(S[i] == ',') S[i] = '.';
	}
	return S;
}

//---------TStackData-----------------------------------------------------------

TStackData::TStackData(String Data, int Priority)
{
	FPriority = Priority;
	FData = Data;
}

void TStackData::Set(String Data, int Priority)
{
	FPriority = Priority;
	FData = Data;
}

//---------TStack---------------------------------------------------------------

TONPStack::TONPStack()
{
	FStack = new TList();
	FBackStack = new TList();
}

TONPStack::~TONPStack()
{
	Clear();
	delete FStack;
	for (int i = 0; i < FStack->Count ; i++) {
		delete (TStackData*)FBackStack->Items[i];
	}
	FBackStack->Clear();
	delete FBackStack;
}

void TONPStack::Put(String Data, int Priority)
{
	if (FBackStack->Count > 0) {
		TStackData *D = (TStackData*)FBackStack->Items[0];
		FBackStack->Delete(0);
		D->Set(Data, Priority);
		FStack->Add(D);
	} else FStack->Add(new TStackData(Data, Priority));
}

String TONPStack::PickUp()
{
	if(FStack->Count < 1)return "";
	String Result = ((TStackData*)FStack->Last())->FData;
	FBackStack->Add(FStack->Last());
	FStack->Delete(FStack->Count - 1);
	return Result;
}

bool TONPStack::Exists()
{
	return FStack->Count > 0;
}

int TONPStack::ActualPriority()
{
	if(FStack->Count > 0) return ((TStackData*)FStack->Last())->FPriority;
	return 0;
}

void TONPStack::Clear()
{
	while (FStack->Count > 0){
		FBackStack->Add(FStack->Items[0]);
		FStack->Delete(0);
	}
}

//---------TONPCalculator-------------------------------------------------------

RType fAdd(FncParams){return Param[0] + Param[1];}
RType fSub(FncParams){return Param[0] - Param[1];}
RType fMult(FncParams){return Param[0] * Param[1];}
RType fDiv(FncParams){return Param[0] / Param[1];}

TONPCalculator::TONPCalculator(TControllClass *Controll)
{
	ControllClass = Controll;
	Functions = new TFunctionsList(ControllClass);
	FONPStack = new TONPStack();
	FCalcStack = new TONPStack();
	LastErrorCode = 0;
	FONPList = new TStringList();
	FONPList->Delimiter = ' ';
	Functions->DeleteAllFunctions();
	Functions->AddFunction(&fAdd, "+", 2, 5);
	Functions->AddFunction(&fSub, "-", 2, 5);
	Functions->AddFunction(&fMult, "*", 2, 10);
	Functions->AddFunction(&fDiv, "/", 2, 10);
}

TONPCalculator::~TONPCalculator()
{
	delete FONPStack;
	delete FCalcStack;
	delete Functions;
	delete FONPList;
}

void TONPCalculator::GenerateONP()
{
	String Last = "";
	FONPData = "";
	FONPStack->Clear();

	int Start = 1;
	int Length = FExpression.Length();

	while (Start <= Length){
		//pobierzmy pojedynczy token (ci¹g znaków, liczbê, operator itp)
		int i;
		for(i = Start; i <= Length; i++) {
			TCharType T = CharType(FExpression[i]);
			if(T == ctNumber || T == ctChar || T == ctComma ||
			   FExpression[i] != '_') break;
		}
		if(i == Start)i++;
		String Token = FExpression.SubString(Start, i - Start);
		Start = i;
		//u¿ytkownik mo¿e u¿ywaæ dowolnych nawiasów,
		//dla nas one wszystkie to jedno: ( i )
		if (Token == "[" || Token == "{") Token = "(";
		if (Token == "]" || Token == "}") Token = ")";
		//mamy token - operacje na tokenie...
		//jeœli symbolem jest "-" - mo¿e to oznaczaæ b¹dŸ to ODJ¥Æ, b¹dŸ te¿
		//ZMIENZNAK. Kiedy wiêc poprzedni Token by³ pusty (pocz¹tek wyra¿enia)
		//lub poprzedni Token to "(" - czyli pocz¹tek wyra¿enia w nawiasie,
		//jest to ZMIENZNAK!
		if ((Token == "-" && Last == "") || Last == "(") Token = "zmienznak";
		//jeœli nasz Token to LICZBA, doklejamy j¹ do wyniku ONP
		if (IsNumber(Token)) FONPData = FONPData + Token + " ";
		else if(Token == "("){//otwieramy nawias
			FONPStack->Put("(", 0);  //dodajemy na stos z priorytetem 0 (zarezerwowany)
		} else if(Token == ")")	{
			//koniec nawiasu
			//zdejmujemy najpierw ze stosu wszystko do czasu napotkania elementu
			//o priorytecie 0, czyli pocz¹tku nawiasu
			while(FONPStack->Exists() && FONPStack->ActualPriority() != 0)
				FONPData = FONPData + FONPStack->PickUp() + " ";
			//sam nawias tak¿e zdejmijmy
			FONPStack->PickUp();
		} else if(!Functions->FunctionExists(Token)){
			//to nie ¿adna funkcja (pewnie zmienna), doklejmy do ONP
			//nie doklejamy jednak znaku przecinka, który oddziela argumenty
			if(Token != ",") FONPData = FONPData + Token + " ";
//			UnknownVaribles->Add(Token);
		} else {
			//to funkcja - pobierzmy jej priorytet
			int Priorytet = Functions->GetFunction(Token)->Priority;
			//FunkcjeInfo(Id).Priorytet;
			//najpierw zdejmujemy ze stosu wszystko o WY¯SZYCH PRIORYTETACH
			while(FONPStack->Exists() && FONPStack->ActualPriority() >= Priorytet)
				FONPData = FONPData + FONPStack->PickUp() + " ";
			//i teraz na stos nasz¹ funkcjê
			FONPStack->Put(Token, Priorytet);
		}
		//zapamiêtujemy token jako "Poprzedni"
		Last = Token;
	}
	//wszystko co zosta³o na stosie zdejmujemy
	while(FONPStack->Exists())
		FONPData = FONPData + FONPStack->PickUp() + " ";
}

void TONPCalculator::SetSelf(String Expression)
{
	FExpression = Expression.LowerCase();
	FExpression = FExpression.Trim();
	LastErrorCode = 0;
	GenerateONP();
	FONPList->DelimitedText = FONPData;
}

RType TONPCalculator::Calculate()
{
	//czyœcimy stos wyniku i dodajemy jako wynik 0
	//priorytet nie jest istotny - wiêc go nie podajemy
	FCalcStack->Clear();
//	FCalcStack->Put("5");

	int Start = 0;
	while(Start < FONPList->Count){
		//pobierzmy pojedynczy token (ci¹g znaków, liczbê, operator itp)
		String Token = FONPList->operator [](Start);
		Start++;
		//jeœli to liczba - to na stos
		if(IsNumber(Token)) FCalcStack->Put(Token);
		else if(!Functions->FunctionExists(Token)){
			//jeœli to nie funkcja, to pewnie zmienna...
			if(ControllClass->VariblesList->VaribleExists(Token))
				FCalcStack->Put(FTS(ControllClass->VariblesList->GetValue(Token)));
			else {
				LastErrorCode = 8;
//            	FCalcStack->Put("0");
			}
		} else {
			//to funkcja, wiêc pobierzmy jej numer i adres
			try
			{
				TFunction *F = Functions->GetFunction(Token);
				RType* Params = new RType[F->ParamsCount];
				for (int j = F->ParamsCount - 1; j >= 0 ; j--) {
					if(FCalcStack->Exists())Params[j] = STF(FCalcStack->PickUp());
					else Params[j] = 0;
				}
				FCalcStack->Put(F->Run(Params, F->ParamsCount));
				delete[] Params;
			}
			catch(...)
			{
				LastErrorCode = 5;
			}
		}
	}
	return 0;
	//STF(FCalcStack->PickUp());
}

//---------TWord----------------------------------------------------------------

bool TWord::CheckProperity(String Data)
{
	if (Data == "")return false;
	String Expression = Data.LowerCase();
	int Count = Expression.Length() + 1;
	int* TypeTable = new int[Count];
	for (int i = 1; i < Count; i++) TypeTable[i] = CharType(Expression[i]);

	int AHlp = 0; //zmienna pomocnicza
	//sprawdzamy ró¿ne mo¿liwoœci
	for (int i = 1; i < Count; i++) {
		if (TypeTable[i] == ctOpenBracket) AHlp++;
		if (TypeTable[i] == ctCloseBracket) AHlp--;
		if (TypeTable[i] == ctComma) {
			if ((i > 1 && TypeTable[i - 1] != ctNumber) ||
				(i < Count - 1 && TypeTable[i + 1] != ctNumber)) return false;
		}
	}
	if (AHlp != 0) return false;

	//sprawdzamy czy nie ma zdublowanych operatorów
	for (int i = 1; i < Count - 1; i++)
		if (Expression[i] != '!' && TypeTable[i] == ctOperator && TypeTable[i + 1] == ctOperator)
			return false;
	if (TypeTable[Count - 1] == ctOperator && Expression[Count - 1] != '!') return false;

	delete[] TypeTable;
	return true;
}

String TWord::StringCalculate()
{
	return Calculate();
}

RType TWord::Calculate()
{
	RType R;
	try
	{
		switch (SelfType){
			case wtNumber:
				R = NumberData;
			break;
			case wtExpression:
				if (Children->Count == 1) {
					R = ((TWord*)Children->Items[0])->Calculate();
				} else {//niestety jest wiêcej
					int Position;
					for (int i = 0; i < 3; i++){ //dla ka¿dej pary operatorów
						Position = FindOperator(i, Children);
						while (Position > -1 && Children->Count > 2){ //dopóki jest dany operator
							TWord *LW = ((TWord*)Children->Items[Position - 1]);
							TWord *RW = ((TWord*)Children->Items[Position + 1]);
							TWord *Middle = (TWord*)Children->Items[Position];

							Middle->SetSelf(DoExpression(Middle->SelfExpression,
								 LW->Calculate(), RW->Calculate()), wtNumber, true);
							ErrorCode += LW->LastError + RW->LastError;
							ControllClass->PutWord(LW);
							ControllClass->PutWord(RW);
							Children->Delete(Position + 1);
							Children->Delete(Position - 1);
							Position = FindOperator(i, Children);
						}
					}
					R = ((TWord*)Children->Items[0])->Calculate();
				}
			break;
			case wtFunction:
				R = ((TFunction*)DataPointer)->Run(CalculateParams(), Params->Count - 1);
			break;
			case wtVarible:
				R = ((TVarible*)DataPointer)->Value;
			break;
		default:
			throw;
		}
	}
	catch(...)
	{
		ErrorCode += 1;
		R = 0;
    }
	if (!Positive) R = -R;
	return R;
}

bool TWord::SetList(String Data, TList *DestList)
{
	int* TypeTable = new int[Data.Length() + 1];

	for (int i = 1; i <= Data.Length(); i++) TypeTable[i] = CharType(Data[i]);

	int Length = Data.Length();
	int i = 1;
	while (i <= Length){
		switch (TypeTable[i]) {
			case ctOperator:
				if (Data[i] == '!') {
					TWord *W = ((TWord*)Children->Last());
					String S = "silnia(" + W->SelfExpression + ")";
					W->SetSelf(S);
				} else AddWord(Data[i], wtOperator, true, DestList);
				i++;
			break;
			case ctNumber:{
				String Temp = Data[i];
				bool APos = true;
				if(i > 1) if(TypeTable[i - 1] == ctMinus) APos = false;
				i++;
				while(i <= Length &&
					 (TypeTable[i] == ctNumber || TypeTable[i] == ctComma)){
					Temp = Temp + Data[i];
					i++;
				}
				AddWord(Temp, wtNumber, APos, DestList);
				if(i <= Length)
				if(TypeTable[i] == ctOpenBracket ||
				   TypeTable[i] == ctChar)AddWord('*', wtOperator, true, DestList);
			}
			break;
			case ctMinus:
				if(i > 1 && (TypeTable[i - 1]) != ctOperator)AddWord('+', wtOperator, true, DestList);
				i++;
			break;
			case ctOpenBracket:{
				String Temp = "";
				bool APos = true;
				if(i > 1) if(Data[i - 1] == '-') APos = false;
				int l = 1;
				i++;
				while(l > 0 && i <= Length){
				   if(TypeTable[i] == ctOpenBracket)l++;
				   if(TypeTable[i] == ctCloseBracket)l--;
				   if(l != 0) Temp = Temp + Data[i];
				   i++;
				}
				AddWord(Temp, wtExpression, APos, DestList);
				if(i <= Length)
					if(TypeTable[i] == ctOpenBracket ||
					   TypeTable[i] == ctChar ||
					   TypeTable[i] == ctNumber)
							AddWord('*', wtOperator, true, DestList);
			}
			break;
			case ctCloseBracket: i++; break;
			case ctChar:{
				String Temp = Data[i];
				TWordType NewType = wtUndefined;
				bool APos = true;
				if(i > 1) if(TypeTable[i - 1] == ctMinus) APos = false;
				i++;
				while(i <= Length && TypeTable[i] == ctChar){
					Temp = Temp + Data[i];
					i++;
				}

				if (ControllClass->VariblesList->VaribleExists(Temp)) {
					NewType = wtVarible;
				} else
				if (ControllClass->FunctionsList->FunctionExists(Temp)){
					NewType = wtFunction;
					if (i <= Length && TypeTable[i] == ctOpenBracket) {
						int l = 1;
						i++;
						Temp = Temp + "\n";
						while(i <= Length && l > 0){
						   if(TypeTable[i] == ctOpenBracket)l++;
						   if(TypeTable[i] == ctCloseBracket)l--;
						   if(l != 0){
								if(TypeTable[i] == ctSemi) Temp = Temp + "\n";
								else Temp = Temp + Data[i];
						   }
						   i++;
						}
					} else return false;
				} else{
					AddUnknownVarible(Temp);
					NewType = wtVarible;
				}

				AddWord(Temp, NewType, APos, DestList);
				if(i <= Length)
					if(TypeTable[i] == ctOpenBracket ||
					   TypeTable[i] == ctChar ||
					   TypeTable[i] == ctNumber)
							AddWord('*', wtOperator, true, DestList);
			}
			break;
		default:
		   i++;
		}
	}
	delete[] TypeTable;
	return true;
}

bool TWord::SetSelf(String Data, bool CPositive)
{
	Clear();
	Positive = CPositive;
	String Expression = Data.LowerCase();
	Params->Text = Expression;
	SelfType = wtExpression;

	return SetList(Expression, Children);
}

void TWord::AddWord(String Data, TWordType Type, bool NPositive, TList *Dest)
{
	TWord *Child = new TWord(ControllClass);
	if (Type == wtExpression) Child->SetSelf(Data, NPositive);
	else Child->SetSelf(Data, Type, NPositive);
	Dest->Add(Child);
}

String TWord::DoExpression(String Operator, RType L1, RType L2)
{
	try {
		switch (Operator[1]) {
			case '*': return L1 * L2;
			case '/': if (L2 == 0) {
						ErrorCode += 1;
						return 0;
					  }else	return L1 / L2;
			case '+': return L1 + L2;
			case '-': return L1 - L2;
			case '^': return Power(L1, L2);
			default:
				return 0;
		}
	} catch (...) {
		ErrorCode += 1;
		return 0;
	}
}

void TWord::AddLog(String Text)
{
	if (ControllClass->Log != NULL){ControllClass->Log->Add("TWord: " + Text);}
}

String TWord::GetListExpression(TList *List)
{
	String Result;
	for (int i = 0; i < List->Count; i++) {
		Result = Result + ((TWord*)List->Items[i])->GetExpression();
	}   	
	return Result;
}

String TWord::GetExpression()
{
	String Result = "";
	switch (SelfType) {
		case wtOperator: Result = SelfExpression; break;
		case wtFunction:
		case wtVarible:
		case wtNumber:
			Result = SelfExpression;
			if (!Positive) Result = "(-" + Result + ")";
		break;
		case wtExpression:
			Result = GetListExpression(Children);
			Result = "(" + Result + ")";
			if(!Positive)Result = "-" + Result;
		break;
	default:
		AddLog("B³¹d: Nieznany typ w GetExpression");
		Result = SelfExpression;
	}
	return Result;
}

void TWord::SetSelf(String Data, TWordType Type, bool CPositive)
{
	ClearList(Children);
	Positive = CPositive;
	SelfType = Type;
	
	switch (Type){
		case wtNumber:
			if (Data[1] == '-'){
				Positive = !Positive;
				Data.Delete(1, 1);
			}
			Params->Text = Data;
			NumberData = Data.ToDouble();
		break;
		case wtFunction:{
			Params->Text = Data;
			for (int i = 1; i < Params->Count; i++) {
				TWord *Child = new TWord(ControllClass);
				Child->SetSelf(Params->operator [](i));
				Children->Add(Child);
			}
			DataPointer = ControllClass->FunctionsList->GetFunction(Params->operator [](0));
		}
		break;
		case wtVarible:
			Params->Text = Data;
			DataPointer = ControllClass->VariblesList->GetVarible(Data);
		break;
	default:
		Params->Text = Data;
	}
}

TWord::TWord(TControllClass *Controll)
{
	Children = new TList();
	ControllClass = Controll;
	Params = new TStringList();
	UnknownVaribles = "";
}

TWord::TWord(TWord *Source)
{
	Children = new TList();
	Params = new TStringList();
	UnknownVaribles = "";
	Assign(Source);
}

TWord::~TWord()
{
	ClearList(Children);
	delete Params;
	delete Children;
}

String TWord::GetUnknownVaribles()
{
	String Temp = UnknownVaribles;

	for (int i = 0; i < Children->Count; i++) {
		TWord *Child = (TWord*)Children->Items[i];
		if (Child->SelfType == wtExpression ||
		    Child->SelfType == wtFunction) {
			Temp = Temp + "\n" + Child->GetUnknownVaribles();
		}
	}

	return Temp;
}

void TWord::AddUnknownVarible(String Name)
{
	if (UnknownVaribles.Length() > 0) {
		UnknownVaribles = UnknownVaribles + "\n";
	}
	UnknownVaribles = UnknownVaribles + Name;
}

void TWord::ClearList(TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		ControllClass->PutWord((TWord*)List->Items[i]);
	}
	List->Clear();
}

void TWord::Clear()
{
	Positive = true;
	ClearList(Children);
	Params->Clear();
	SelfType = wtUndefined;
	UnknownVaribles = "";
}

String TWord::GetSelfExp()
{
	String Result;
	if (Params->Count > 0) Result =  Params->operator [](0);
	if (Params->Count > 1) {
		Result = Result + "(" + Params->operator [](1);
		for (int i = 2; i < Params->Count; i++) {
			Result =  Result + ";" + Params->operator [](i);
		}
		Result = Result + ")";
		return Result;
	}
	return Result;
}

void TWord::SetParam(String Data)
{
	Params->Text = Data.LowerCase();
}

RType* TWord::CalculateParams()
{
	RType *Result = new RType[Params->Count - 1];
	for (int i = 0; i < Children->Count; i++) {
		Result[i] = ((TWord*)Children->Items[i])->Calculate();
	}
	return Result;
}

int TWord::GetLastError()
{
	int LR = ErrorCode;
	ErrorCode = 0;
	return LR;
}

void TWord::Assign(TWord *Source)
{
	ClearList(Children);
	ControllClass = Source->ControllClass;
	Params->Assign(Source->Params);
	Positive = Source->Positive;
	SelfType = Source->SelfType;
	NumberData = Source->NumberData;
	DataPointer = Source->DataPointer;

	for (int i = 0; i < Source->Children->Count; i++) {
		TWord *W = ControllClass->GetWord();
		W->Assign((TWord*)Source->Children->Items[i]);
		Children->Add(W);//new TWord((TWord*)Source->Children->Items[i]));
	}
}

//----------TVarible------------------------------------------------------------

TVarible::TVarible(TVarible *Source)
{
	Assign(Source);
	FName = Source->Name;
}

TVarible::TVarible(RType nIV, RType nD, RType nEV, String nName)
{
	Set(nIV, nD, nEV);
	if (nD == 0) FConstans = true; else FConstans = false;
	FName = nName;
	HaveRange = false;
}

void TVarible::Assign(TVarible *Source)
{
	Set(Source->InitialValue, Source->Delta, Source->EndValue);
	FConstans = Source->Constans;
	HaveRange = Source->HaveRange;
}

bool TVarible::Step()
{
	if (FValue <= FEndValue && !FConstans)FValue = RoundTo(FValue + FDelta, -3);
	return FValue > FEndValue;
}

void TVarible::Reset()
{
	FValue = FInitialValue;
}

void TVarible::Set(RType nIV, RType nD, RType nEV)
{
	FInitialValue = nIV;
	FDelta = nD;
	FEndValue = nEV;
	FValue = FInitialValue;
}

void TVarible::SetValue(RType NewValue)
{
	if (NewValue <= FEndValue && NewValue >= FInitialValue) {
		FValue = NewValue;
	}
}

//--------TVariblesList---------------------------------------------------------

void TVariblesList::AddLog(String Text)
{
	if (ControllClass->Log != NULL)ControllClass->Log->Add("TVariblesList: " + Text);
}

void TVariblesList::DeleteAllVaribles()
{
	for (int i = 0; i < Varibles->Count; i++) {
		delete ((TVarible*)Varibles->Items[i]);
	}
	Varibles->Clear();
}

TVariblesList::TVariblesList(TControllClass *Controll)
{
	Varibles = new TList();
	ControllClass = Controll;
}

TVariblesList::~TVariblesList()
{
	DeleteAllVaribles();
	delete Varibles;
}

int TVariblesList::GetVariblePos(String Name)
{
	for (int i = 0; i < Varibles->Count; i++) {
		if (((TVarible*)Varibles->Items[i])->Name == Name) {
			return i;
		}
	}
	return -1;
}

void TVariblesList::ResetAllVaribles()
{
	for (int i = 0; i < Varibles->Count; i++) {
		((TVarible*)Varibles->Items[i])->Reset();
	}
}

bool TVariblesList::UpdateVarible(String Name)
{
	return ((TVarible*)Varibles->Items[GetVariblePos(Name)])->Step();
}

void TVariblesList::ResetVarible(String Name)
{
	((TVarible*)Varibles->Items[GetVariblePos(Name)])->Reset();
}

RType TVariblesList::GetValue(String Name)
{
	if(VaribleExists(Name)){
		return ((TVarible*)Varibles->Items[GetVariblePos(Name)])->Value;
	}
	AddLog("B³¹d: Zmienna " + Name + " nie istnieje");
	return 0;
}

void TVariblesList::AddVarible(String Name, RType Value)
{
	AddVarible(Name, Value, 0, 0);
}

void TVariblesList::AddVarible(String Name, RType InitValue, RType Delta, RType EndValue)
{
	TVarible *V = new TVarible(InitValue, Delta, EndValue, Name);
	Varibles->Add(V);
}

void TVariblesList::AddVarible(TVarible *Source)
{
	TVarible *V = new TVarible(Source);
	Varibles->Add(V);
}

void TVariblesList::DeleteVarible(String Name)
{
	int i = GetVariblePos(Name);
	if (i != -1) {
		delete (TVarible*)Varibles->Items[i];
		Varibles->Delete(i);
	}
}

void TVariblesList::ModifyVarible(String Name, RType InitValue, RType Delta, RType EndValue)
{
	if(VaribleExists(Name)){
		((TVarible*)Varibles->Items[GetVariblePos(Name)])->Set(InitValue, Delta, EndValue);
	}
}

bool TVariblesList::VaribleExists(String Name)
{
	return GetVariblePos(Name) != -1;
}

bool TVariblesList::IsConstant(String Name)
{
	int i = GetVariblePos(Name);
	if (i > -1) {
		return ((TVarible*)Varibles->Items[i])->Constans;
	}
	return false;
}

TVarible* TVariblesList::GetVarible(int Index)
{
	return (TVarible*)Varibles->Items[Index];
}

TVarible* TVariblesList::GetVarible(String Name)
{
	int G = GetVariblePos(Name);
	if (G != -1) {
		return GetVarible(G);
	}
	return NULL;	
}

String TVariblesList::SaveVaribles()
{
	TStringList *OutData = new TStringList();
	#define AddDataf(X) VarData = VarData + " " + FloatToStr(X)

	for (int i = 0; i < Varibles->Count; i++) {
		TVarible *Var = GetVarible(i);
		String VarData = Var->Name;
		if (Var->Constans) {
			AddDataf(Var->Value);
		}
		else{
			AddDataf(Var->InitialValue);
			AddDataf(Var->Delta);
			AddDataf(Var->EndValue);
		}
		OutData->Add(VarData);
	}

	String Result = OutData->Text;
	delete OutData;
	return Result;
}

void TVariblesList::LoadVaribles(String Data)
{
	TStringList *InData = new TStringList();
	InData->Text = Data;

	TStringList *LineData = new TStringList();
	LineData->Delimiter = ' ';

	#define GetVar(X) LineData->operator [](X)
	for (int i = 0; i < InData->Count; i++) {
		LineData->DelimitedText = InData->operator [](i);
		if (LineData->Count == 2) {
			AddVarible(GetVar(0), StrToFloat(GetVar(1)));
		} else
		if (LineData->Count == 4) {
			AddVarible(GetVar(0), StrToFloat(GetVar(1)), StrToFloat(GetVar(2)), StrToFloat(GetVar(3)));
		}
	}

	delete LineData;
	delete InData;
}

void TVariblesList::Assign(TVariblesList *Source)
{
	DeleteAllVaribles();
	for (int i = 0; i < Source->VariblesCount; i++) {
		AddVarible(Source->GetVarible(i));
	}
}

int TVariblesList::GetVaribleCalcCount(int Index)
{
	 TVarible *Var = GetVarible(Index);
	 if(Var == NULL)return 0;

	 RType Delta = Var->Delta;

	 RType VarCalc = ((RType)Var->InitialValue - (RType)Var->EndValue) / Delta;
	 if(VarCalc == 0)VarCalc = 1;
     if(VarCalc < 0)VarCalc = -VarCalc;
	 VarCalc++;
	 return VarCalc;
}

//----------TFunctionsList------------------------------------------------------

TFunctionsList::TFunctionsList(TControllClass *Controll)
{
	ControllClass = Controll;
	Functions = new TList();
	AddStandard();
}

TFunctionsList::~TFunctionsList()
{
	DeleteAllFunctions();
	delete Functions;
}

RType TFunctionsList::RunFunction(String Name, RType *Params, int ParCount, int Precision)
{
	int Id = GetByName(Name);
	if (Id != -1) {
		TFunction *F = ((TFunction*)Functions->Items[Id]);
		if (F->ParamsCount != ParCount) {
			AddLog("B³¹d: Funkcja '" + Name + "' wymaga " + IntToStr(F->ParamsCount) + " parametrów");
		}
		return F->Run(Params, ParCount);
	}
	return 0;
}

void TFunctionsList::AddFunction(FncPointer(NewPointer), String Name, int ParamsCount, int Priority)
{
	TFunction *NewItem = new TFunction(NewPointer, Name.LowerCase(), ParamsCount, Priority);
	NewItem->Parrent = this;
	Functions->Add(NewItem);
}

void TFunctionsList::AddFunction(String FunctionData, String Name, int ParCount, int Priority)
{
	TFunction *NewItem = new TFunction(FunctionData.LowerCase(), Name.LowerCase(), ParCount, Priority);
	NewItem->Parrent = this;
	Functions->Add(NewItem);
}

void TFunctionsList::DeleteAllFunctions()
{
	for (int i = 0; i < Functions->Count; i++) {
		delete ((TFunction*)Functions->Items[i]);
	}
	Functions->Clear();
}

String TFunctionsList::SaveFunctions()
{
	TStringList *OutData = new TStringList();

	for (int i = 0; i < Functions->Count; i++) {
		TFunction *F = GetFunction(i);
		if (F->Type == flitString) {
			String FData = F->Name;
			FData = FData + " " + IntToStr(F->Type);
			FData = FData + " " + IntToStr(F->ParamsCount); 
			FData = FData + " " + F->FunctionData;
			OutData->Add(FData);
		}
	}

	String Result = OutData->Text;
	delete OutData;
	return Result;
}

void TFunctionsList::LoadFunctions(String Data)
{
	TStringList *InData = new TStringList();
	InData->Text = Data;

	TStringList *LineData = new TStringList();
	LineData->Delimiter = ' ';

	for (int i = 0; i < InData->Count; i++) {
		LineData->DelimitedText = InData->operator [](i);
		if (LineData->Count == 4 && GetVar(1) == flitString) {
			AddFunction(GetVar(3), GetVar(0), StrToInt(GetVar(2)));
		}
	}

	delete InData;
}

int TFunctionsList::GetByName(String Name)
{
	for (int i = 0; i < Functions->Count; i++) {
		if (((TFunction*)Functions->Items[i])->Name == Name) {
			return i;
		}
	}
	return -1;
}

bool TFunctionsList::FunctionExists(String Name)
{
	return GetByName(Name) != -1;
}

//#define FncParams RType *Param, int ParCount
RType fSin(FncParams){return Sin(Param[0]);}
RType fCos(FncParams){return Cos(Param[0]);}
RType fTg(FncParams){return Tan(Param[0]);}
RType fCtg(FncParams){if(Param[0] == 0)return 0; else return Cotan(Param[0]);}
RType fAbs(FncParams){return abs(Param[0]);}
RType fPi(FncParams){return 3.14159265358979323846;}
RType fSignum(FncParams){return Sign(Param[0]);}
RType fLoga(FncParams){return Log2(Param[0]);}
RType fLogb(FncParams){return Log10(Param[0]);}
RType fLogc(FncParams){if(Param[1] < 1 && Param[0] < 2) return 0; return LogN(Param[0], Param[1]);}
RType fPower(FncParams){if(Param[0] == 0 || Param[1] == 0) return 0; return Power(Param[0], Param[1]);}
RType fRandom(FncParams){Randomize; return Random(Param[0]);}
RType fExp(FncParams){return Exp(Param[0]);}
RType fSqrt(FncParams){if(Param[0] <= 0)return 0; return Sqrt(Param[0]);}
RType fSinH(FncParams){return Sinh(Param[0]);}
RType fCosH(FncParams){return Cosh(Param[0]);}
RType fTgH(FncParams){return Tanh(Param[0]);}
RType fCtgH(FncParams){if(Param[0] == 0)return 0; else return CotH(Param[0]);}
RType fArcSin(FncParams){return ArcSin(Param[0]);}
RType fArcCos(FncParams){return ArcCos(Param[0]);}
RType fArcTg(FncParams){return ArcTan(Param[0]);}
RType fArcCtg(FncParams){return ArcCot(Param[0]);}
RType fMax(FncParams){return MaxValue((double*)Param, ParCount);}
RType fMin(FncParams){return MinValue((double*)Param, ParCount);}
RType fSilnia(FncParams)
{
	RType Result = 1;
	for (int i = 1; i <= Param[0]; i++) {
		Result = Result * i;
	}
	return Result;
}
RType fNWD(FncParams)
{
	if (ParCount < 2) return 0;
	while(Param[0] != Param[1]) {
		if (Param[0] > Param[1]) Param[0] -= Param[1];
		else Param[1] -= Param[0];
	}
	return Param[0];
}
RType fNWW(FncParams)
{
	if (ParCount < 2) return 0;
	return (Param[0]*Param[1]) / fNWD(Param, ParCount);
}
//RType f(FncParams){return ;}

void TFunctionsList::AddStandard()
{
	AddFunction(&fSin, "sin", 1);
	AddFunction(&fCos, "cos", 1);
	AddFunction(&fTg,  "tg", 1);
	AddFunction(&fCtg, "ctg", 1);
	AddFunction(&fSinH, "sinh", 1);
	AddFunction(&fCosH, "cosh", 1);
	AddFunction(&fTgH, "tgh", 1);
	AddFunction(&fCtgH, "ctgh", 1);
	AddFunction(&fArcSin, "arcsin", 1);
	AddFunction(&fArcCos, "arccos", 1);
	AddFunction(&fArcTg, "arctg", 1);
	AddFunction(&fArcCtg, "arcctg", 1);

	AddFunction(&fAbs, "abs", 1);
	AddFunction(&fPi, "pi", 0);
	AddFunction(&fSignum, "sgn", 1);
	AddFunction(&fPower, "power", 2);
	AddFunction(&fExp, "exp", 1);
	AddFunction(&fSqrt, "sqrt", 1);

	AddFunction(&fLoga, "loga", 1);
	AddFunction(&fLogb, "logb", 1);
	AddFunction(&fLogc, "logc", 2);

	AddFunction(&fRandom, "random", 1);

	AddFunction(&fMax, "max", 2);
	AddFunction(&fMin, "min", 2);

	AddFunction(&fSilnia, "silnia", 1);
	AddFunction(&fNWD, "nwd", 2);
	AddFunction(&fNWW, "nww", 2);	
}

void TFunctionsList::AddLog(String Text)
{
	if (ControllClass->Log != NULL)ControllClass->Log->Add("TFunction: " + Text);
}

TFunction* TFunctionsList::GetFunction(int Index)
{
	return (TFunction*)Functions->Items[Index];
}

TFunction* TFunctionsList::GetFunction(String Name)
{
	int P = GetByName(Name);
	if (P == -1)return NULL;
	return (TFunction*)Functions->Items[P];
}

void TFunctionsList::DeleteFunction(String Name)
{
	if (!FunctionExists(Name)) return;
	int Index = GetByName(Name);
	delete (TFunction*)Functions->Items[Index];
	Functions->Delete(Index);
}

void TFunctionsList::ModifyFunction(String Name, String nData, int nParCount)
{
	if (!FunctionExists(Name)) return;
	TFunction* Fnc = GetFunction(GetByName(Name));
	if (Fnc->Type != flitString) return;
	Fnc->FunctionData = nData;
	Fnc->FParamsCount = nParCount;
}

int __fastcall FunctionsSort(void *I1, void *I2)
{
	int Result = CompFncData[((TFunction*)I1)->Type][((TFunction*)I2)->Type];
	if (Result == 0) {
		Result = CompareStr(((TFunction*)I1)->Name, ((TFunction*)I2)->Name);
	}
	return Result;
}
void TFunctionsList::Sort()
{
	Functions->Sort(&FunctionsSort);
}

//------TFunction---------------------------------------------------------------

TFunction::TFunction(FncPointer(NewPointer), String nName, int nParamsCount, int Priority)
{
	Pointer = NewPointer;
	FName = nName;
	FParamsCount = nParamsCount;
	FName = nName;
	FType = flitInternal;
	FPriority = Priority;
}

TFunction::TFunction(String fData, String nName, int nParamsCount, int Priority)
{
	FunctionData = fData;
	FName = nName;
	FType = flitString;
	FParamsCount = nParamsCount;
	FPriority = Priority;
}

RType TFunction::Run(RType *Params, int ParCount)
{
	RType Result = 0;
	switch (Type) {
		case flitInternal:
			Result = (*Pointer)(Params, ParCount);
		break;
		case flitString:{
			String MData = FunctionData;
			for (int i = 0; i < ParCount; i++) {
				MData = AnsiReplaceText(MData, "[" + IntToStr(i) + "]", Params[i]);
			}
			TWord* Word = Parrent->ControllClass->GetWord();
			Word->SetSelf(MData);
			Result = Word->Calculate();
			Parrent->ControllClass->PutWord(Word);
		}
		break;
	}
	delete[] Params;
	return Result;
}

//------TControllClass----------------------------------------------------------

TControllClass::TControllClass(TStrings *nLog)
{
	Log = nLog;
	VariblesList = new TVariblesList(this);
	FunctionsList = new TFunctionsList(this);
	WordStack = new TList;
}

TControllClass::~TControllClass()
{
	for (int i = 0; i < WordStack->Count; i++) {
		delete (TWord*)WordStack->Items[i];
	}

	delete VariblesList;
	delete FunctionsList;
}

TWord* TControllClass::GetWord()
{
	if (WordStack->Count > 0){
		TWord *W = (TWord*)WordStack->Items[0];
		WordStack->Delete(0);
		return W;
	}
	return new TWord(this);
}

void TControllClass::PutWord(TWord *W)
{
	W->ErrorCode = 0;
	WordStack->Add(W);
}

//---------TChart---------------------------------------------------------------

TChart::TChart(TControllClass *Controll)
{
	ControllClass = Controll;
	Varibles = new TStringList();
	VarArray = new TVariblesList(ControllClass);
	FAtWorking = false;
	FCancelActWork = false;
}

TChart::~TChart()
{
	Clear();
	delete Varibles;
	delete VarArray;
}

void TChart::AddLog(String Text)
{
	if (ControllClass->Log != NULL)ControllClass->Log->Add("TChart: " + Text);
}  

bool TChart::SetExpression(String Data)
{
	if (FExpression == Data)return true;
	TWord *Word = new TWord(ControllClass);
	//najpierw sprawdzamy poprawnoœæ wyra¿enia
	if (Word->CheckProperity(Data) && FExpression != Data) {
		Clear();
		FExpression = Data;
		Word->SetSelf(FExpression);
		//znajdŸ wszystkie zmienne w równaniu
		Varibles->Text = Word->GetUnknownVaribles();//i wpisz je na listê
		//znienne nie mog¹ siê powtarzaæ
		//wiêc niestety w pêtli kwadratowej nale¿ sprawdziæ i usun¹æ powtórzenia
		for (int i = 0; i < Varibles->Count; i++) {
			String ActVar = Varibles->operator [](i);
			for (int j = 0; j < Varibles->Count; j++) {
				if ((j != i && Varibles->operator [](j) == ActVar) || Varibles->operator [](j) == "") {
					Varibles->Delete(j);
				}
			}
		}
		//dodaj wszystkie nazwy zmienych do wew. listy
		for (int i = 0; i < Varibles->Count; i++){
			VarArray->AddVarible(Varibles->operator [](i), -5.0, 0.1, 5.0);
		}
	}
	delete Word;
	return FExpression == Data;
}

void TChart::Clear()
{
	FExpression = "";
	Varibles->Clear();
	VarArray->DeleteAllVaribles();
	FAtWorking = false;
	FCancelActWork = false;
	//i przyjaciele
}

void TChart::MoveVarible(String Name, bool DirUp)
{
	if (Varibles->IndexOf(Name) >= 0)//zmienna musi intnieæ!!!
	if (DirUp) {//przesówamy zmienn¹ w górê
		//trzeba sprawdziæ czy mo¿na przesun¹æ
		if (Varibles->IndexOf(Name) > 0) {
			Varibles->Move(Varibles->IndexOf(Name), Varibles->IndexOf(Name) - 1);
		}
	} else {//przesówamy zmienn¹ w dó³
		//trzeba sprawdziæ czy mo¿na przesun¹æ
		if (Varibles->IndexOf(Name) < Varibles->Count - 1) {
			Varibles->Move(Varibles->IndexOf(Name), Varibles->IndexOf(Name) + 1);
		}
    }
}

String TChart::GetVariblesQueue()
{
	return Varibles->Text;
}

void TChart::SetVariblesQueue(String Queue)
{
	Varibles->Text = Queue;
}

void TChart::QuickDrawRecurrence(int VaribleNumber, Graphics::TBitmap *Surface,
		int cX, int cY, TVarible *dX, TWord *Pattern, TChartInfo ChartInfo, TCalcResult *Result)
{
	if(VaribleNumber < Varibles->Count &&
	   ControllClass->VariblesList->VaribleExists(Varibles->operator [](VaribleNumber)) &&
	   Varibles->operator [](VaribleNumber) != "") {

		TVarible *Act = ControllClass->VariblesList->GetVarible(Varibles->operator [](VaribleNumber));
		Act->Reset();
		bool Continue = true;
		RType AResult, AError, LastError = 0;
		TWord *Word = ControllClass->GetWord();
		while (Continue){
			if (VaribleNumber + 1 < Varibles->Count)QuickDrawRecurrence(VaribleNumber + 1, Surface, cX, cY, dX, Pattern, ChartInfo, Result);
			else
			{
				Word->Assign(Pattern); //ustawiamy kalkulator
				//rysujemy kreskê i liczymy wartoœæ wyra¿enia
				AResult = Word->Calculate();
				AError = Word->LastError;
				if(!Result->TableEnd)
					Result->AddResult(AResult, ControllClass->VariblesList, AError);

				if (AError == 0 && LastError == 0)
					Surface->Canvas->LineTo(RoundTo(dX->Value * ChartInfo.Scale + cX, 0),
										RoundTo(-AResult * ChartInfo.Scale + cY, 0));
				else Surface->Canvas->MoveTo(RoundTo(dX->Value * ChartInfo.Scale + cX, 0),
										RoundTo(-AResult * ChartInfo.Scale + cY, 0));
				LastError = AError;
			}
			Continue = !Act->Step();
		}
		ControllClass->PutWord(Word);
	}
}

void TChart::QuickChartDraw(Graphics::TBitmap *Surface, TChartInfo *ChartInfo,
		TCalcResult *Result, bool CleanSurface)
{
	if (FAtWorking) return;
	FAtWorking = true;
	FCancelActWork = false;

	int cX = ChartInfo->Width / 2 - ChartInfo->Offset.x;
	int cY = ChartInfo->Height / 2 - ChartInfo->Offset.y;

	RType vStart = (RType)ChartInfo->Offset.x / (RType)ChartInfo->Scale  - ((RType)ChartInfo->Width / (RType)ChartInfo->Scale / 2);
	RType vEnd = (RType)ChartInfo->Offset.x / (RType)ChartInfo->Scale  + ((RType)ChartInfo->Width / (RType)ChartInfo->Scale / 2);

	RType Delta = 1.0 / (RType)ChartInfo->Scale;
	int CalcCount = Power((vStart - vEnd) / Delta + 1, Varibles->Count);
	while (!ChartInfo->HaveParam &&
		   CalcCount > 5000 * Power(2.0, Varibles->Count)){
		Delta += 0.1;
		CalcCount = Power((vStart - vEnd) / Delta + 1, Varibles->Count);
	}

	//dodajemy zmienne
	for (int i = 0; i < Varibles->Count; i++) {
		TVarible *V = VarArray->GetVarible(i);
		V->Delta = Delta;
		if (V->HaveRange)ControllClass->VariblesList->AddVarible(V);
		else ControllClass->VariblesList->AddVarible(Varibles->operator [](i), vStart, Delta, vEnd);
	}

	if (CleanSurface)DrawAxis(Surface, *ChartInfo);
	Surface->Canvas->Pen->Color = ChartInfo->LineColor;
	ControllClass->VariblesList->ResetAllVaribles();
	if(Result->TableWidth == 0){
		ChartInfo->XCount = GetListCalcCount(ControllClass->VariblesList);	
		Result->SetData(ControllClass->VariblesList, FExpression, Varibles, ChartInfo->XCount);
	}

	//a teraz trzeba policzyæ i narysowaæ za 1 zmachem
	if (Varibles->Count > 0) {
		TVarible *Var, *XVar;
		//pobieramy zmienn¹
		if (ChartInfo->HaveParam) {
			if (ControllClass->VariblesList->VaribleExists(ChartInfo->vP)){
				Var = ControllClass->VariblesList->GetVarible(ChartInfo->vP);
				Var->Value = vStart + (RType)ChartInfo->TimePos * Delta;
				Varibles->Delete(Varibles->IndexOf(ChartInfo->vP));
				Varibles->Insert(0, ChartInfo->vP);				
			}
			else
				Var = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));

		} else {
			if (ControllClass->VariblesList->VaribleExists(ChartInfo->vX))
				Var = ControllClass->VariblesList->GetVarible(ChartInfo->vX);
			else
				Var = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));
		}
		if (ControllClass->VariblesList->VaribleExists(ChartInfo->vX))
			XVar = ControllClass->VariblesList->GetVarible(ChartInfo->vX);
		else
			XVar = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));

		if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Var->Reset();//reset zmiennej pracujcej
		bool Continue = true;//warunek pêtli
		RType AResult, AError, LastError = 0;
		TWord *Word = ControllClass->GetWord();//tworzymy kalkulator
		TWord *Pattern = ControllClass->GetWord();//tworzymy wzrór wyra¿enia
		Pattern->SetSelf(FExpression);

		Word->Assign(Pattern);//ustawiamy kalkulator
		//na pocz¹tku przemieszczamy pióro do pierwszego punktu
		Surface->Canvas->MoveTo(RoundTo(XVar->Value * ChartInfo->Scale + cX, 0),
								RoundTo(-Word->Calculate() * ChartInfo->Scale + cY, 0));
		//aktualizujemy zmienn¹
		if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Var->Step();
		int i = 0;
		if (!Var->Constans)
		while (Continue){// pêtla g³ówna
			// jeœli jest wiêcej ni¿ 1 zmienna idziemy g³êbiej
			if (Varibles->Count > 1)QuickDrawRecurrence(1, Surface, cX, cY, XVar, Pattern, *ChartInfo, Result);
			else
			{
				Word->Assign(Pattern); //ustawiamy kalkulator
				//rysujemy kreskê i liczymy wartoœæ wyra¿enia
				AResult = Word->Calculate();
				AError = Word->LastError;
				if(!Result->TableEnd)
					Result->AddResult(AResult, ControllClass->VariblesList, AError);

				if (AError == 0 && LastError == 0)
					Surface->Canvas->LineTo(RoundTo(XVar->Value * ChartInfo->Scale + cX, 0),
										RoundTo(-AResult * ChartInfo->Scale + cY, 0));
				else Surface->Canvas->MoveTo(RoundTo(XVar->Value * ChartInfo->Scale + cX, 0),
										RoundTo(-AResult * ChartInfo->Scale + cY, 0));
				LastError = AError;
			}
			//sprawdzmy czy zmienna jest pe³na, jeœli tak to koniec pêtli
			if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Continue = !Var->Step();
			else Continue = false;
			i++;
			if (i % 20 == 0) Application->ProcessMessages();
		}
		ControllClass->PutWord(Word);
		ControllClass->PutWord(Pattern); 
	}

	ChartInfo->XCount = Result->ResultCount;
	if (ChartInfo->HaveParam) {
		 ChartInfo->PCount = ControllClass->VariblesList->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(ChartInfo->vP));
		 ChartInfo->XCount = ChartInfo->PCount;
	} else {
		ChartInfo->PCount = 0;
		ChartInfo->XCount = Result->TableWidth;
	}

	//usuñ zmienne dodane przed liczeniem
	for (int i = 0; i < Varibles->Count; i++) {
		ControllClass->VariblesList->DeleteVarible(Varibles->operator [](i));
	}
	FAtWorking = false;
}

int TChart::GetListCalcCount(TVariblesList *List)
{
  int Result = 1;
  for (int i = 0; i < List->VariblesCount; i++) {
	 TVarible *Var = List->GetVarible(i);
	 if (Var->Constans) continue;
	 RType Delta = Var->Delta;
	 if(Delta <= 0)Delta = 1;

	 RType VarCalc = ((RType)Var->InitialValue - (RType)Var->EndValue) / Delta;
	 if(VarCalc == 0)VarCalc = 1;
	 if(VarCalc < 0)VarCalc = -VarCalc;
	 VarCalc++;
	 Result *= VarCalc;
  }
  if (Result < 0) Result = -Result;
  return Result;
}

int TChart::GetCalcCount()
{
	return GetListCalcCount(VarArray);
}

void TChart::CalculateRecurrence(TCalcResult *Result, int VaribleNumber, TWord *Pattern, TWord *Word)
{
	if (Varibles->Count > VaribleNumber && ControllClass->VariblesList->VaribleExists(Varibles->operator [](VaribleNumber))) {
		String Name = Varibles->operator [](VaribleNumber);//pobieramy nazwê zmiennej
		ControllClass->VariblesList->ResetVarible(Name);
		bool Continue = true;//warunek pêtli
		while (Continue){// pêtla g³ówna
			// jeœli jest wiêcej ni¿ 1 zmienna idziemy g³êbiej
			if (Varibles->Count > VaribleNumber + 1)CalculateRecurrence(Result, VaribleNumber + 1, Pattern, Word);
			else{
				Word->Assign(Pattern);//ustawiamy kalkulator
				Result->AddResult(Word->Calculate(), ControllClass->VariblesList, Word->LastError);
			}

			Continue = !ControllClass->VariblesList->UpdateVarible(Name);
			if (FCancelActWork) Continue = false;
		}
	}
}

void TChart::Calculate(TCalcResult *Result)
{
	if (FAtWorking) return;
	FAtWorking = true;
	FCancelActWork = false;
		bool Cancelled = false;
	Result->SetData(VarArray, FExpression, Varibles, GetCalcCount());
	//dodaj zmienne z listy
	for (int i = 0; i < VarArray->VariblesCount; i++)
		ControllClass->VariblesList->AddVarible(VarArray->GetVarible(i));
	ControllClass->VariblesList->ResetAllVaribles();
	//Rozpoczynamy przeliczanie
	if (Varibles->Count > 0 && ControllClass->VariblesList->VaribleExists(Varibles->operator [](0))) {
		String Name = Varibles->operator [](0);//pobieramy nazwê zmiennej

		TVarible *Var = VarArray->GetVarible(0);
		RType Delta = Var->Delta;
		if(Delta <= 0)Delta = 1;   
		RType CalcCount = abs(Var->InitialValue - Var->EndValue) / Delta + 1;
		if(CalcCount == 0)CalcCount = 1;

//		int AProgress = 0;
//		int RefTime = RoundTo(100 / CalcCount, 0);
//		if(RefTime == 0)RefTime = 1;
		bool Continue = true;//warunek pêtli
		TWord *Word = new TWord(ControllClass);//tworzymy kalkulator
		TWord *Pattern = new TWord(ControllClass);//tworzymy wzrór wyra¿enia
		Pattern->SetSelf(FExpression);
		if (!ControllClass->VariblesList->IsConstant(Name))
		while (Continue && !Cancelled){// pêtla g³ówna
			// jeœli jest wiêcej ni¿ 1 zmienna idziemy g³êbiej
			if (Varibles->Count > 1)CalculateRecurrence(Result, 1, Pattern, Word);
			else{
				Word->Assign(Pattern);//ustawiamy kalkulator
				Result->AddResult(Word->Calculate(), ControllClass->VariblesList, Word->LastError); //i liczymy
			}
//			AProgress++;
//			if (ProgressForm != NULL && AProgress % RefTime == 0) {
//				ProgressForm->ProgressProc(RoundTo((RType(AProgress)) / CalcCount * 100, -1), &Cancelled);
//			}
			//sprawdzmy czy zmienna jest pe³na, jeœli tak to koniec pêtli
			Continue = !ControllClass->VariblesList->UpdateVarible(Name);
			if (FCancelActWork) Continue = false;
		}
		delete Word;
		delete Pattern;
//		if (ProgressForm != NULL)ProgressForm->ProgressProc(100, &Cancelled);
	}

	//usuñ zmienne dodane przed liczeniem
	for (int i = 0; i < VarArray->VariblesCount; i++){
		ControllClass->VariblesList->DeleteVarible(VarArray->GetVarible(i)->Name);
	}
//	if(!Cancelled) AddLog("Wyliczono " + IntToStr(Result->ResultCount) + " wartoœci.");
	FAtWorking = false;
}

void TChart::Render3DChart(TCalcResult* Result, TChartInfo ChartInfo)
{
	int ABSPos = ChartInfo.TimePos * ChartInfo.XCount;
	int APos;
	switch (ChartInfo.Type) {
		case RT_LINES:{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < ChartInfo.XCount - 1; i++) {
				if (i % ChartInfo.YCount == 0) {
					glEnd();
					glBegin(GL_LINE_STRIP);
				}
				APos = i + ABSPos;
				if (Result->TableResult[APos][ChartInfo.Z + 1] != 0) {
					glEnd();
					glBegin(GL_LINE_STRIP);
					continue;
				}
				glColor3f((Result->TableResult[APos][ChartInfo.Z] / Result->MaxValue),
						  (Result->TableResult[APos][ChartInfo.Z] / Result->MinValue), 1.0);

				glVertex3f(Result->TableResult[APos][ChartInfo.X],
						   Result->TableResult[APos][ChartInfo.Y],
						   Result->TableResult[APos][ChartInfo.Z]);
			}
			glEnd();
		}
		break;
		case RT_QUADS:{
			for (int W = 0; W < ChartInfo.YCount - 2; W++) {
				for (int j = 0; j < ChartInfo.XCount - 2; j++){
					glBegin(GL_QUADS);
					APos = ABSPos + (W + 1) * ChartInfo.XCount + j;
					glColor3f((Result->TableResult[APos][ChartInfo.Z] / Result->MaxValue),
							  (Result->TableResult[APos][ChartInfo.Z] / Result->MinValue), 1.0);
					glVertex3f(Result->TableResult[APos][ChartInfo.X],
							   Result->TableResult[APos][ChartInfo.Y],
							   Result->TableResult[APos][ChartInfo.Z]);
					APos = ABSPos + (W + 1) * ChartInfo.XCount + j + 1;
					glColor3f((Result->TableResult[APos][ChartInfo.Z] / Result->MaxValue),
							  (Result->TableResult[APos][ChartInfo.Z] / Result->MinValue), 1.0);
					glVertex3f(Result->TableResult[APos][ChartInfo.X],
							   Result->TableResult[APos][ChartInfo.Y],
							   Result->TableResult[APos][ChartInfo.Z]);
					APos = ABSPos + W * ChartInfo.XCount + j + 1;
					glColor3f((Result->TableResult[APos][ChartInfo.Z] / Result->MaxValue),
							  (Result->TableResult[APos][ChartInfo.Z] / Result->MinValue), 1.0);
					glVertex3f(Result->TableResult[APos][ChartInfo.X],
							   Result->TableResult[APos][ChartInfo.Y],
							   Result->TableResult[APos][ChartInfo.Z]);
					APos = ABSPos + W * ChartInfo.XCount + j;
					glColor3f((Result->TableResult[APos][ChartInfo.Z] / Result->MaxValue),
							  (Result->TableResult[APos][ChartInfo.Z] / Result->MinValue), 1.0);
					glVertex3f(Result->TableResult[APos][ChartInfo.X],
							   Result->TableResult[APos][ChartInfo.Y],
							   Result->TableResult[APos][ChartInfo.Z]);
					glEnd();
				}
			}
		}
		break;
	}
}

void TChart::Quick3DRecurrence(int VaribleNumber, TVarible *XVar, TVarible *YVar,
	TWord *Pattern, TChartInfo ChartInfo, TCalcResult *Result)
{
	if(VaribleNumber < Varibles->Count &&
	   ControllClass->VariblesList->VaribleExists(Varibles->operator [](VaribleNumber)) &&
	   Varibles->operator [](VaribleNumber) != "") {

		TVarible *Act = ControllClass->VariblesList->GetVarible(Varibles->operator [](VaribleNumber));
		Act->Reset();
		bool Continue = true;
		RType AResult, LastError;
		TWord *Word = ControllClass->GetWord();
		glEnd();
		glBegin(GL_LINE_STRIP);
		while (Continue){
			if (VaribleNumber + 1 < Varibles->Count)Quick3DRecurrence(VaribleNumber + 1, XVar, YVar, Pattern, ChartInfo, Result);
			else
			{
				Word->Assign(Pattern);
				AResult = Word->Calculate();
				LastError = Word->LastError;
				if(!Result->TableEnd)
					Result->AddResult(AResult, ControllClass->VariblesList, LastError);

				if (LastError == 0) {
					glColor3f((AResult / Result->MaxValue),
							  (AResult / Result->MinValue), 1.0);
					glVertex3f(XVar->Value, YVar->Value, AResult);
				} else {
					glEnd();
					glBegin(GL_LINE_STRIP);
				}
			}
			Continue = !Act->Step();
		}
		ControllClass->PutWord(Word);
	}

}

void TChart::QuickDraw(TCalcResult* Result, TChartInfo ChartInfo,
	Graphics::TBitmap *Surface, bool CleanSurface)
{

}

void TChart::Quick3DChart(TCalcResult* Result, TChartInfo *ChartInfo)
{
	if (FAtWorking) return;
	FAtWorking = true;
	FCancelActWork = false;

	//dodajemy zmienne
	for (int i = 0; i < Varibles->Count; i++) {
		TVarible *V = VarArray->GetVarible(i);
		V->Delta = 0.2;
		if (V->HaveRange)ControllClass->VariblesList->AddVarible(V);
		else ControllClass->VariblesList->AddVarible(Varibles->operator [](i), -8.0, 0.2, 8.0);
	}

	ControllClass->VariblesList->ResetAllVaribles();
	if(Result->TableWidth == 0){
		ChartInfo->XCount = GetListCalcCount(ControllClass->VariblesList);
		Result->SetData(ControllClass->VariblesList, FExpression, Varibles, ChartInfo->XCount);
	}

	//a teraz trzeba policzyæ i narysowaæ za 1 zmachem
	if (Varibles->Count > 0) {
		TVarible *Var, *XVar, *YVar;
		//pobieramy zmienn¹
		if (ChartInfo->HaveParam) {
			if (ControllClass->VariblesList->VaribleExists(ChartInfo->vP)){
				Var = ControllClass->VariblesList->GetVarible(ChartInfo->vP);
				Var->Value = Var->InitialValue + (RType)ChartInfo->TimePos * Var->Delta;
				Varibles->Delete(Varibles->IndexOf(ChartInfo->vP));
				Varibles->Insert(0, ChartInfo->vP);				
			}
			else
				Var = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));

		} else {
			if (ControllClass->VariblesList->VaribleExists(ChartInfo->vX))
				Var = ControllClass->VariblesList->GetVarible(ChartInfo->vX);
			else
				Var = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));
		}
		if (ControllClass->VariblesList->VaribleExists(ChartInfo->vX))
			XVar = ControllClass->VariblesList->GetVarible(ChartInfo->vX);
		else
			XVar = ControllClass->VariblesList->GetVarible(Varibles->operator [](0));

		if (ControllClass->VariblesList->VaribleExists(ChartInfo->vX))
			YVar = ControllClass->VariblesList->GetVarible(ChartInfo->vY);
		else
			YVar = ControllClass->VariblesList->GetVarible(Varibles->operator [](1));

		if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Var->Reset();//reset zmiennej pracujcej
		bool Continue = true;//warunek pêtli
		RType AResult, LastError;
		TWord *Word = ControllClass->GetWord();//tworzymy kalkulator
		TWord *Pattern = ControllClass->GetWord();//tworzymy wzrór wyra¿enia
		Pattern->SetSelf(FExpression);

		Word->Assign(Pattern);//ustawiamy kalkulator
		//aktualizujemy zmienn¹
		if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Var->Step();
		if (!Var->Constans)
		while (Continue){// pêtla g³ówna
			// jeœli jest wiêcej ni¿ 1 zmienna idziemy g³êbiej
			glBegin(GL_LINE_STRIP);
			if (Varibles->Count > 1)Quick3DRecurrence(1, XVar, YVar, Pattern, *ChartInfo, Result);
			else
			{
				Word->Assign(Pattern); //ustawiamy kalkulator
				//rysujemy kreskê i liczymy wartoœæ wyra¿enia
				AResult = Word->Calculate();
				LastError = Word->LastError;
				if(!Result->TableEnd)
					Result->AddResult(AResult, ControllClass->VariblesList, LastError);

				if (LastError == 0) {
					glColor3f((AResult / Result->MaxValue),
							  (AResult / Result->MinValue), 1.0);
					glVertex3f(XVar->Value, YVar->Value, AResult);
				} else {
					glEnd();
					glBegin(GL_LINE_STRIP);
				}
			}
			//sprawdzmy czy zmienna jest pe³na, jeœli tak to koniec pêtli
			if(Var->Name != ChartInfo->vP || !ChartInfo->HaveParam)Continue = !Var->Step();
			else Continue = false;
			glEnd();
		}
		ControllClass->PutWord(Word);
		ControllClass->PutWord(Pattern);
	}

	ChartInfo->XCount = Result->ResultCount;
	if (ChartInfo->HaveParam) {
		 ChartInfo->PCount = ControllClass->VariblesList->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(ChartInfo->vP));
		 ChartInfo->XCount = ChartInfo->PCount;
	} else {
		ChartInfo->PCount = 0;
		ChartInfo->XCount = Result->TableWidth;
	}

	//usuñ zmienne dodane przed liczeniem
	for (int i = 0; i < Varibles->Count; i++) {
		ControllClass->VariblesList->DeleteVarible(Varibles->operator [](i));
	}
	FAtWorking = false;
}

bool TChart::Draw2DChart(TCalcResult *Result, Graphics::TBitmap *Surface,
		TChartInfo ChartInfo, bool CleanSurface)
{
	if (FAtWorking) return false;
	FAtWorking = true;
	FCancelActWork = false;

	if (CleanSurface)DrawAxis(Surface, ChartInfo);
//	if (ProgressForm != NULL) ProgressForm->SetMode(StateDrawing);

	Surface->Canvas->Pen->Color = ChartInfo.LineColor;
	int cX = ChartInfo.Width / 2 - ChartInfo.Offset.x;
	int cY = ChartInfo.Height / 2 - ChartInfo.Offset.y;
	bool Cancelled = false;
	//int RefTime = ChartInfo.XCount / 100;
	int ABSPos = ChartInfo.TimePos * ChartInfo.PCount;

	Surface->Canvas->MoveTo(
		RoundTo( Result->TableResult[ABSPos][ChartInfo.X] * ChartInfo.Scale + cX, 0),
		RoundTo(-Result->TableResult[ABSPos][ChartInfo.Y] * ChartInfo.Scale + cY, 0));

	for (int i = 1; i < ChartInfo.XCount - 1 && !Cancelled; i++) {
		if(Result->TableResult[i + ABSPos][ChartInfo.Y + 1] == 0 &&
		   Result->TableResult[i + ABSPos - 1][ChartInfo.Y + 1] == 0)
			Surface->Canvas->LineTo(
				RoundTo( Result->TableResult[i + ABSPos][ChartInfo.X] * ChartInfo.Scale + cX, 0),
				RoundTo(-Result->TableResult[i + ABSPos][ChartInfo.Y] * ChartInfo.Scale + cY, 0));
		else
			Surface->Canvas->MoveTo(    
				RoundTo( Result->TableResult[i + ABSPos][ChartInfo.X] * ChartInfo.Scale + cX, 0),
				RoundTo(-Result->TableResult[i + ABSPos][ChartInfo.Y] * ChartInfo.Scale + cY, 0));


//		if (i % RefTime == 0 && ProgressForm != NULL)
//				ProgressForm->ProgressProc(RoundTo(((RType)(i) / (RType)(ChartInfo.XCount)) * 100, 0),
//						&Cancelled);
	}

//	if (ProgressForm != NULL)ProgressForm->ProgressProc(100, &Cancelled);
    FAtWorking = false;
	return Cancelled;
}

void TChart::PrepareChartInfo(TChartInfo *Info, TCalcResult *Result)
{
	Info->X = Result->VarQueue->IndexOf(Info->vX);                       
	switch (Info->Mode) {
		case cm2DChart:
			Info->Y = Result->VarQueue->Count;
			if (Info->HaveParam) {
				 Info->PCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vP));
				 Info->XCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vX));
			} else {
				Info->PCount = 0;
				Info->XCount = Result->TableWidth;
			}
		break;
		case cm3DChart:
			Info->Y = Result->VarQueue->IndexOf(Info->vY);
			Info->Z = Result->VarQueue->Count;
			Info->YCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vY));

			if (Info->HaveParam) {
				Info->PCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vP));

				if (Info->Type == RT_LINES) Info->XCount = Result->ResultCount / Info->PCount;
				else Info->XCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vX));
			} else {
				Info->PCount = 0;
				if (Info->Type == RT_LINES) Info->XCount = Result->ResultCount;
				else Info->XCount = Result->CalcVaribles->GetVaribleCalcCount(Result->CalcVaribles->GetVariblePos(Info->vX));
			}

		break;
	}
}

void TChart::DrawAxis(Graphics::TBitmap *Surface, TChartInfo ChartInfo)
{
	Surface->SetSize(ChartInfo.Width, ChartInfo.Height);

	Surface->Canvas->Brush->Color = clWhite;
	Surface->Canvas->Brush->Style = bsSolid;
	Surface->Canvas->FillRect(Rect(0, 0, ChartInfo.Width, ChartInfo.Height));
	Surface->Canvas->Pen->Width = 1;
	Surface->Canvas->Pen->Color = (TColor)14737632;

	int cX = ChartInfo.Width / 2 - ChartInfo.Offset.x;
	int cY = ChartInfo.Height / 2 - ChartInfo.Offset.y;

	for (int i = 0; i <= ChartInfo.Width; i++){//przejerzdza poziomo i rysuje kreski
		if (i % ChartInfo.Scale == 0) {//co ChartScaleX rysuje kreskê
			Surface->Canvas->MoveTo(cX + i, 0);//po prawej
			Surface->Canvas->LineTo(cX + i, ChartInfo.Height);
			Surface->Canvas->MoveTo(cX - i, 0);//po lewej
			Surface->Canvas->LineTo(cX - i, ChartInfo.Height);
		}
	}
	for (int i = 0; i <= ChartInfo.Height; i++){//przejerzdza poziomo i rysuje kreski
		if (i % ChartInfo.Scale == 0) {//co ChartScaleX rysuje kreskê
			Surface->Canvas->MoveTo(0, cY + i);//w dó³
			Surface->Canvas->LineTo(ChartInfo.Width, cY + i);
			Surface->Canvas->MoveTo(0, cY - i);//w górê
			Surface->Canvas->LineTo(ChartInfo.Width, cY - i);
		}
	}

	Surface->Canvas->Pen->Color = clBlack;
	//linie osi OX i OY
	Surface->Canvas->MoveTo(cX, 0);
	Surface->Canvas->LineTo(cX, ChartInfo.Height);
	Surface->Canvas->MoveTo(0, cY);
	Surface->Canvas->LineTo(ChartInfo.Width, cY);
	//jedna strza³ka
	Surface->Canvas->MoveTo(cX - 4, 8);
	Surface->Canvas->LineTo(cX - 1, -1);
	Surface->Canvas->MoveTo(cX + 4, 8);
	Surface->Canvas->LineTo(cX + 1, -1);
	//druga strza³ka
	Surface->Canvas->MoveTo(ChartInfo.Width - 8, cY - 4);
	Surface->Canvas->LineTo(ChartInfo.Width + 1, cY);
	Surface->Canvas->MoveTo(ChartInfo.Width - 8, cY + 4);
	Surface->Canvas->LineTo(ChartInfo.Width + 1, cY);
	//literki x i y
	Surface->Canvas->Brush->Style = bsClear;
	Surface->Canvas->TextOut(cX + 7, -2, "y");
	Surface->Canvas->TextOut(ChartInfo.Width - 8, cY + 2, "x");

	Surface->Canvas->Pen->Color = ChartInfo.LineColor;
}

void TChart::SetVarible(TVarible *Var)
{
	TVarible *OwnVar = GetVarible(Var->Name);
	if (OwnVar != NULL) {
		OwnVar->Assign(Var);
	}
}

TVarible* TChart::GetVarible(String Name)
{
	for (int i = 0; i < VarArray->VariblesCount; i++) {
		TVarible *Var = VarArray->GetVarible(i);
		if (Var->Name == Name) {
			return Var;
		}
	}
	return NULL;
}

void TChart::Cancel()
{
	if(FAtWorking)FCancelActWork = true;
}

void TChart::Assign(TChart *Source)
{
	if (Source == NULL){
		Clear();
		return;
	}
	FExpression = Source->FExpression;
	Varibles->Assign(Source->Varibles);
	ControllClass = Source->ControllClass;
	FAtWorking = false;
	FCancelActWork = false;
	VarArray->Assign(Source->VarArray);
}

void TChart::AssignResultData(TCalcResult* Result)
{
	Clear();
	SetExpression(Result->FExpression);
	VarArray->Assign(Result->CalcVaribles);
}

//------TCalcResult-------------------------------------------------------------

void TCalcResult::AddResult(RType Data, TVariblesList *Vars, RType ErrorCode)
{
	int APos = 0;
	for (int i = 0; i < Vars->VariblesCount; i++) {
		TVarible *Var = Vars->GetVarible(i);
		if (!Var->Constans) {
			TableResult[TablePos][APos] = Var->Value;
			APos++;
		}
	}
	MaxValue = Max(Data, MaxValue);
	MinValue = Min(Data, MinValue);

	if (MaxValue == 0) MaxValue = 1;
	if (MinValue == 0) MinValue = -1;

	TableResult[TablePos][APos] = Data;
	TableResult[TablePos][APos + 1] = ErrorCode;
	TablePos++;
}

void TCalcResult::Clear()
{
	FExpression = "";
	if(TableWidth > 0) DeleteTable();
	VarQueue->Clear();
	CalcVaribles->DeleteAllVaribles();
	MaxValue = 0;
	MinValue = 0;
	TablePos = 0;
}

void TCalcResult::CreateTable(int aWidth, int aVarCount)
{
	TableWidth = aWidth;
	VarCount = aVarCount;
	TableResult = new RType*[TableWidth];
	for (int i = 0; i < TableWidth; i++) {
		TableResult[i] = new RType[aVarCount + 2];
	}
}

void TCalcResult::DeleteTable()
{
	for (int i = 0; i < TableWidth; i++) {
		delete[] TableResult[i];
	}
	delete[] TableResult;
	TableWidth = 0;
	VarCount = 0;
	TablePos = 0;
}

bool TCalcResult::IsTableEnd()
{
    return TablePos >= TableWidth && TableWidth > 0; 
}

void TCalcResult::SetData(TVariblesList *VarList, String FExp, TStringList *VarQ, int CalcCount)
{
	Clear();
	FExpression = FExp;
	CalcVaribles->Assign(VarList);
	VarQueue->Assign(VarQ);
	CreateTable(CalcCount, VarQueue->Count);
}

int TCalcResult::GetVariblePos(String VarName)
{
	return VarQueue->IndexOf(VarName);
}

TCalcResult::TCalcResult(TControllClass *Controll)
{
	ControllClass = Controll;
	VarQueue = new TStringList();
	FExpression = "";
	CalcVaribles = new TVariblesList(ControllClass);
	TableWidth = 0;
	TablePos = -1;
}

TCalcResult::~TCalcResult()
{
	Clear();
	delete CalcVaribles;
	delete VarQueue;
}

bool TCalcResult::LoadFormFile(String FileName)
{
	Clear();
	TStringList *InData = new TStringList;
	InData->LoadFromFile(FileName);

	#define GetLoadedData(X) InData->operator [](X)

	if (GetLoadedData(0) != "MultiCalc Result File version 1.0") return false;

	FExpression = GetLoadedData(1);
	TStringList *Data = new TStringList();
	Data->Delimiter = ' ';
	Data->DelimitedText = GetLoadedData(2);
	if (Data->operator [](0) == "") return false;
	int aVC = StrToInt(Data->operator [](0));
	if (aVC <= 0) return false;
	int Pos = 3;
	for (int i = 0; i < aVC; i++) {
		CalcVaribles->LoadVaribles(GetLoadedData(Pos));
		Data->DelimitedText = GetLoadedData(Pos);
		VarQueue->Add(Data->operator [](0));
		Pos++;
	}

	Pos++;
	Data->DelimitedText = GetLoadedData(Pos);
	Pos++;
	if (Data->operator [](0) == "") return false;
	int Count = StrToInt(Data->operator [](0));
	if (Count <= 0) return false;
	CreateTable(Count, aVC);
	for (int i = 0; i < Count; i++) {

		Data->DelimitedText = GetLoadedData(Pos);
		for (int j = 0; j <= aVC; j++) {
			TableResult[i][j] = StrToFloat(Data->operator [](j));
		}
		MaxValue = Max(TableResult[i][aVC], MaxValue);
		MinValue = Min(TableResult[i][aVC], MinValue);
		Pos++;
	}

	delete Data;
	delete InData;
	return true;
}

void TCalcResult::SaveToFile(String FileName)
{
	TStringList *OutData = new TStringList;

	OutData->Add("MultiCalc Result File version 1.0");
	OutData->Add(FExpression);
	OutData->Add(IntToStr(CalcVaribles->VariblesCount) + " varibles");
	OutData->Add(CalcVaribles->SaveVaribles());
	OutData->Add(IntToStr(TableWidth) + " results");

	String LineData;
	for (int i = 0; i < TableWidth; i++) {
		LineData = RoundTo(TableResult[i][0], -3);
		for (int j = 1; j <= VarCount; j++) {
			LineData = LineData + " " + RoundTo(TableResult[i][j], -3);
		}
		OutData->Add(LineData);
	}

	OutData->SaveToFile(FileName);
	delete OutData;
}

void TCalcResult::Assign(TCalcResult *Source)
{
	if (Source == NULL){
		Clear();
		return;
	}
	FExpression = Source->FExpression;
	CalcVaribles->Assign(Source->CalcVaribles);
	VarQueue->Assign(Source->VarQueue);
	CreateTable(Source->TableWidth, VarQueue->Count);
	for (int i = 0; i < TableWidth; i++) {
		for (int j = 0; j <= VarCount; j++) {
         	TableResult[i][j] = Source->TableResult[i][j];
		}       
	}
	MaxValue = Source->MaxValue;
	MinValue = Source->MinValue;
}

void TCalcResult::GetVaribles(TStrings *List)
{
	List->Clear();
	for (int i = 0; i < CalcVaribles->VariblesCount; i++) {
		List->Add(CalcVaribles->GetVarible(i)->Name);
	}
}

//----------TTransform----------------------------------------------------------

TTransform::TTransform(TControllClass *Controll):TWord(Controll)
{
	TPos = 0;
}

TTransform::~TTransform()
{

}

void TTransform::AddLog(String Text)
{
	if (ControllClass->Log != NULL)ControllClass->Log->Add(IntToStr(TPos)+": " + Text);
}                                              //  TTran                          

bool TTransform::Simplify(String* InData)
{
	if (!CheckProperity(*InData)) return false;
	SetSelf(*InData);

	SimplifyList(Children);

	*InData = GetListExpression(Children);
	return true;
}

#define GetItem(A) ((TWord*)List->Items[A])
#define GetListItem(A, B) ((TWord*)A->Items[B])
#define DestroyWord(A) delete GetItem(A); List->Delete(A);
String Op = "*^";
void TTransform::SimplifyList(TList *List)
{
	if (List->Count == 0) return;
	TPos++;
	AddLog("Start: " + GetListExpression(List));
	bool Continue = true;
	while (Continue)
	//CutSimilar(List) || CalcSimilar(List) || CutNeedless(List) || TranslateFunction(List))
	{
		if (MoveInGroup(List)){
			AddLog("move: " + GetListExpression(List));
			continue;
		}
		if (CutNeedless(List)){
			AddLog("needless: " + GetListExpression(List));
			continue;
		}
		if (CutSimilar(List)){
			AddLog("cut: " + GetListExpression(List));
			continue;
		}
		if (CalcSimilar(List)){
			AddLog("calc: " + GetListExpression(List));
			continue;
		}
		/*if (GroupSimilar(List)){   MoveSimilar
			Continue = true;
			AddLog("Group: " + GetListExpression(List));
		}*/
		if (TranslateFunction(List)){
			AddLog("translate: " + GetListExpression(List));
			continue;   
		}
		if (TryToSimplify(List))continue;
		break;
	}
	if (List->Count == 0) AddWord("0", wtNumber, true, List);
	AddLog("end: " + GetListExpression(List));
	TPos--;
}

bool TTransform::CutSimilar(TList *List)
{
	for (int i = 0; i < List->Count; i++){
		for (int j = 0; j < List->Count; j++){
			TWord *JElem = GetItem(j);
			TWord *IElem = GetItem(i);
			//podstawowy warunek
			if((JElem->SelfType == wtVarible || JElem->SelfType == wtNumber) &&
			   (IElem->SelfType == wtVarible || IElem->SelfType == wtNumber) &&
			   IElem->SelfExpression == JElem->SelfExpression &&
			   IElem->Positive != JElem->Positive){
				bool Can = true;

				//warunki dodatkowe
				if(i > 0 && GetItem(i - 1)->SelfExpression[1] != '+') Can = false;
				if(i < List->Count - 1 && GetItem(i + 1)->SelfExpression[1] != '+')
					Can = false;
				if(j > 0 && GetItem(j - 1)->SelfExpression[1] != '+') Can = false;
				if(j < List->Count - 1 && GetItem(j + 1)->SelfExpression[1] != '+')
					Can = false;

				if (!Can) continue;

				List->Delete(j);
				delete JElem;
				if(j > 0){
					delete GetItem(j - 1);
					List->Delete(j - 1);
				}    
				List->Delete(i);
				delete IElem;
				if(i > 0){
					delete GetItem(i - 1);
					List->Delete(i - 1);
				}
				if (List->Count > 0 && GetItem(0)->SelfType == wtOperator) {
					delete GetItem(0);
					List->Delete(0);
				}

				return true;
			}
		}
	}
	return false;
}

bool TTransform::CalcSimilar(TList *List)
{
	if(List->Count < 3)return false;

	for (int T1 = 0; T1 < 3; T1++) {
		for (int T2 = 0; T2 < 2; T2++) {
			int i = 0;
			while(i < List->Count){
				if(i == 0 && GetItem(i)->SelfType == wtOperator){
					delete GetItem(0);
					List->Delete(0);
					continue;
				}

				if(GetItem(i)->SelfType == wtOperator &&
				   GetItem(i)->SelfExpression[1] == OpQueue[T1][T2][1] &&
				   GetItem(i - 1)->SelfType == wtNumber &&
				   GetItem(i + 1)->SelfType == wtNumber){
					bool Can = true;

					if (GetItem(i)->SelfExpression == "+") {
						if(i > 2)
							if(GetItem(i - 2)->SelfExpression != "+")
								Can = false;
						if(i < List->Count - 2)
							if(GetItem(i + 2)->SelfExpression != "+")
								Can = false;
					} else
					if (GetItem(i)->SelfExpression == "*"){
						if(i > 2)
							if(GetItem(i - 2)->SelfExpression != "*" &&
							   GetItem(i - 2)->SelfExpression != "/")
								Can = false;
						if(i < List->Count - 2)
							if(GetItem(i + 2)->SelfExpression != "*" &&
							   GetItem(i + 2)->SelfExpression != "/")
								Can = false;
					}

					if(Can){
						RType L1 = GetItem(i - 1)->Calculate();
						RType L2 = GetItem(i + 1)->Calculate();

						TWord *Middle = GetItem(i);
	  					Middle->SetSelf(DoExpression(Middle->SelfExpression, L1, L2), wtNumber, true);
						delete GetItem(i - 1);
						delete GetItem(i + 1);
						List->Delete(i + 1);
						List->Delete(i - 1);
						return true;
					}
				}
				i++;
			}//while
		}//for T2
	}//for T1   
	return false;
}

bool TTransform::CutNeedless(TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		TWord *W = GetItem(i);
		if (W->SelfType == wtNumber) {
			if (W->SelfExpression == "0") {
				if (i < List->Count - 1 && Op.Pos(GetItem(i + 1)->SelfExpression[1]) > 0) {
					//tniemy 0*
					DestroyWord(i);
					while (i < List->Count - 1 && Op.Pos(GetItem(i)->SelfExpression[1]) > 0){
						DestroyWord(i + 1)
						DestroyWord(i)
					}
					if(List->Count > 0){DestroyWord(i)}
					return true;
				} else
				if (i > 0 && Op.Pos(GetItem(i - 1)->SelfExpression[1]) > 0) {
					//tniemy *0
					DestroyWord(i);
					i--;
					while (i > 0 && Op.Pos(GetItem(i)->SelfExpression[1]) > 0){
						DestroyWord(i);
						DestroyWord(i - 1);
						i -= 2;
					}
					if(i < 0) i = 0;
					if(List->Count > 0){DestroyWord(i)};
					return true;
				}

				if (i < List->Count - 1 && GetItem(i + 1)->SelfExpression[1] == '+') {
					//tniemy 0+
					DestroyWord(i + 1);
					DestroyWord(i);
					return true;
				} else
				if (i > 0 && GetItem(i - 1)->SelfExpression[1] == '+') {
					//tniemy +0
					DestroyWord(i);
					DestroyWord(i - 1);
					return true;
				}
			} else
			if (W->SelfExpression == "1" && W->Positive){
				if (i < List->Count - 1 && GetItem(i + 1)->SelfExpression[1] == '*') {
					//tniemy 1*
					DestroyWord(i + 1);
					DestroyWord(i);
					return true;
				} else
				if (i > 0 && GetItem(i - 1)->SelfExpression[1] == '*') {
					//tniemy *1
					DestroyWord(i);
					DestroyWord(i - 1);
					return true;
				}
			}
		}
	}
	return false;
}

bool TTransform::TranslateFunction(TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		TWord *W = GetItem(i);
		if(W->SelfType == wtFunction){
		   TFunction *F = ControllClass->FunctionsList->GetFunction(W->Params->operator [](0));
			if(F->Type == flitString) {
				String FncData = F->Data;
				for (int j = 0; j < F->ParamsCount; j++) {
					FncData = AnsiReplaceText(FncData, "[" + IntToStr(j) + "]",
											  W->Params->operator [](j + 1));
				}
				W->SetSelf(FncData, W->Positive);
				AddLog(FncData);
				return true;
			}
		}
	}
	return false;
}

bool TTransform::TryToSimplify(TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		if(GetItem(i)->SelfType == wtExpression){
			int SthWas = 0;
			SimplifyList(GetItem(i)->Children);
			if(i > 1 && Op.Pos(GetItem(i - 1)->SelfExpression[1]) > 0){
				int Mode = Op.Pos(GetItem(i - 1)->SelfExpression[1]);
				if(Mode == 1 && i < List->Count - 2 &&
				   GetItem(i + 1)->SelfExpression == "^")
					Mode = 0;
				TWord *W1 = GetItem(i);
				TWord *W2 = GetItem(i - 2);
				if (Mode == 1 || (Mode == 2 && W2->SelfType == wtNumber && IsInt(W2->SelfExpression))) {
					SimplifyList(W2->Children);
					delete GetItem(i - 1);
					List->Delete(i - 1);
					List->Delete(i - 2);
					if(Mode == 1)MultiplyWords(W1, W2);
					else PowerWords(W1, W2);
					AddLog("l-mult: " + GetListExpression(List));
					return true;
				}
				SthWas++;
			} 
			if (i < List->Count - 2 && Op.Pos(GetItem(i + 1)->SelfExpression[1]) > 0) {
				int Mode = Op.Pos(GetItem(i + 1)->SelfExpression[1]);
				if(Mode == 1 && i > 1 && GetItem(i - 1)->SelfExpression == "^")
					Mode = 0;
				TWord *W1 = GetItem(i);
				TWord *W2 = GetItem(i + 2);
				if (Mode == 1 || (Mode == 2 && W2->SelfType == wtNumber && IsInt(W2->SelfExpression))) {
					SimplifyList(W2->Children);
					delete GetItem(i + 1);
					List->Delete(i + 2);
					List->Delete(i + 1);
					if(Mode == 1)MultiplyWords(W1, W2);
					else PowerWords(W1, W2);
					AddLog("p-mult: " + GetListExpression(List));
					return true;
				}
				SthWas++;
			}
			if(SthWas == 0) {
				String S = GetListExpression(List);
				bool Can = true;
				if (i > 1 && GetItem(i - 1)->SelfExpression != "+") Can = false;
				if (i < List->Count - 2 && GetItem(i + 1)->SelfExpression != "+")
					Can = false;
				if (Can){
					//mo¿na skróciæ nawias
					TWord *ToInsert = GetItem(i);
					List->Delete(i);
					InsertList(i, List, ToInsert->Children, ToInsert->Positive);
					delete ToInsert;
					AddLog("ins: " + GetListExpression(List));
					return true;
				}
			}
		}
	}
	return false;
}

bool TTransform::MoveInGroup(TList *List)
{
	for (int i = 0; i < List->Count; i++) {
		if(GetItem(i)->SelfExpression == "*" &&
		   GetItem(i - 1)->SelfType != wtNumber &&
		   GetItem(i + 1)->SelfType == wtNumber) {
			List->Exchange(i - 1, i + 1);
			return true;
		}
	}
	return false;
}

void TTransform::InsertList(int Pos, TList *DestList, TList *InsertedList, bool Positive)
{
	for (int i = 0; i < InsertedList->Count; i++) {
		if (!Positive) {
			if(i > 0 && GetListItem(InsertedList, i - 1)->SelfExpression[1] == '*')
				continue;
			GetListItem(InsertedList, i)->Positive =
					!GetListItem(InsertedList, i)->Positive;
		}
		DestList->Insert(Pos + i, new TWord(GetListItem(InsertedList, i)));
	}
}

void TTransform::MultiplyLists(TList *DestList, TList *F1, TList *F2)
{  
	if (F1->Count > F2->Count) {
		TList *L = F1;
		F1 = F2;
		F2 = L;
	}

	String S1 = GetListExpression(F1);
	String S2 = GetListExpression(F2);
	for (int i = 0; i < F1->Count; i++) {
		//F1[i] jest w zakresie [wtNumber, wtExpression, wtVarible]
		if(GetListItem(F1, i)->SelfType < 3 ||
		   (i > 0 && GetListItem(F1, i - 1)->SelfExpression[1] == '*'))
			continue;
		for (int j = 0; j < F2->Count; j++) {
			//F2[j] jest w zakresie [wtNumber, wtExpression, wtVarible]
			if(GetListItem(F2, j)->SelfType < 3)continue;
			// na koñcu dodajemy "+"
			if(DestList->Count > 0 && ((TWord*)DestList->Last())->SelfExpression != "+")
				AddWord("+", wtOperator, true, DestList);

			int k = i;
			while(k < F1->Count){
				DestList->Add(new TWord(GetListItem(F1, k)));
				if(k + 1 < F1->Count &&
				   GetListItem(F1, k + 1)->SelfExpression == "+")
					break;
				k++;
			}

			AddWord("*", wtOperator, true, DestList);
			while(j < F2->Count){
				DestList->Add(new TWord(GetListItem(F2, j)));
				if(j + 1 < F2->Count &&
				   GetListItem(F2, j + 1)->SelfExpression == "+")
					break;
				j++;
			}
		}
	}
}

void TTransform::GetList(TList *List, TWord *Source)
{
	if (List == NULL)return;

	switch (Source->SelfType) {
		//, wtFunction,
		case wtExpression:
			for (int i = 0; i < Source->Children->Count; i++) {
				List->Add(GetListItem(Source->Children, i));
			}
		break;
		case wtOperator:
		case wtNumber:
		case wtVarible:
			List->Add(Source);
		break;
		default:
			;
	}
}

void TTransform::MultiplyWords(TWord *W1, TWord *W2)
{
	//Listy W1List i W2List s¹ tylko "odbciami" obiektów W1 i W2
	TList *W1List = new TList();
	TList *W2List = new TList();
	TList *TempList = new TList();
	GetList(W1List, W1);
	GetList(W2List, W2);
	MultiplyLists(TempList, W1List, W2List);
	ClearList(W1->Children);
	W1->Positive = W1->Positive == W2->Positive;
	InsertList(0, W1->Children, TempList, W1->Positive);

	delete W2;
	W1List->Clear();
	W2List->Clear();
	ClearList(TempList);
	delete W1List;
	delete W2List;
	delete TempList;
}

void TTransform::PowerWords(TWord *W1, TWord *W2)
{
	TList *W1List = new TList();
	TList *TempList = new TList();
	TList *FactorList = new TList();
	GetList(W1List, W1);

	for (int j = 0; j < W1List->Count; j++) {
		FactorList->Add(new TWord(GetListItem(W1List, j)));
	}

	int Exponent = W2->SelfExpression.ToInt();
	for (int i = 1; i < Exponent; i++) {
		MultiplyLists(TempList, W1List, FactorList);
		ClearList(FactorList);
		for (int j = 0; j < TempList->Count; j++) {
			FactorList->Add(TempList->Items[j]);
		}
		TempList->Clear();
	}
	ClearList(W1->Children);
	String S = W2->SelfExpression;
	if (W2->Positive) {
		InsertList(0, W1->Children, FactorList, W1->Positive);
	} else {
		AddWord("1", wtNumber, true, W1->Children);
		AddWord("/", wtOperator, true, W1->Children);
		TWord *NW = new TWord(ControllClass);
		W1->Children->Add(NW);
		NW->Positive = W1->Positive;
		NW->SelfType = wtExpression;
		NW->SelfExpression = GetListExpression(FactorList);
		InsertList(0, NW->Children, FactorList, NW->Positive);
	}

	ClearList(TempList);
	ClearList(FactorList);
	W1List->Clear();
	delete W1List;	
	delete TempList;
	delete FactorList;
	delete W2;
}

//----------TEquation-----------------------------------------------------------

void TEquation::AddLog(String Text)
{
	if (ControllClass->Log != NULL)ControllClass->Log->Add("Eq: " + Text);
}

bool TEquation::Solve(String *InData)
{
	int EPos = InData->Pos("=");
	if (EPos == 0) {
		Simplify(InData);
		return true;
	}

	TList *LeftList = new TList();
	TList *RightList = new TList();
	TList *ResultList = new TList();

	SetList(InData->SubString(1, EPos - 1), LeftList);
	SetList(InData->SubString(EPos + 1, InData->Length() - EPos), RightList);

	SolveEquation(LeftList, RightList, ResultList, etEqual);

	ClearList(LeftList);
	ClearList(RightList);
	ClearList(ResultList);
	delete LeftList;
	delete RightList;
	delete ResultList;
	return true;
}

bool TEquation::SolveEquation(TList *LeftList, TList *RightList,
	TList *ResultList, TEquationType EqType)
{
	SimplifyList(LeftList);
	SimplifyList(RightList);



	return true;
}

bool TEquation::MoveFree(TList *Src, TList *Dest, TWordType What)
{
	return false;
/*function MoveFree(L1, L2: TList; What: TCharSet): bool;
  var i, Mark: integer;
      Tmp: string;
      label Koniec;
  begin
	result := false;
    for i := 0 to L1.Count - 1 do
      if(TTrWord(L1[i]).SelfTyp in What)then
      begin
        Mark := 0;
        if i > 0 then
        begin
        if TTrWord(L1[i - 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
        end else inc(mark);
        if i < L1.Count - 1 then
        begin
          if TTrWord(L1[i + 1]).SelfExpression[1] in ['+', '-'] then inc(mark) else dec(mark);
        end else inc(mark);

        if Mark = 2 then
        begin
          Tmp := TTrWord(L1[i]).GetExpression;
          if Tmp[1] = '-' then Tmp := '+' + copy(Tmp, 2, length(tmp)) else Tmp := '-' + Tmp;

          TTrWord(L1[i]).Free;
          L1.Delete(i);

          if i > 0 then
          begin
            TTrWord(L1[i - 1]).Free;
            L1.Delete(i - 1);
          end;

          InsertWords(Tmp, L2, L2.Count);
          Result := true;
          goto Koniec;
        end;
      end;
    Koniec:
  end;*/
}

#pragma package(smart_init)

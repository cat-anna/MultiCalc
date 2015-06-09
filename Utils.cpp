//---------------------------------------------------------------------------


#pragma hdrstop

#include "Utils.h"    /*9
//---------------------------------------------------------------------------
String IniReadString(String Section, String Key, String Default, String FileName)
{
	char *Read = new char[2047];
	GetPrivateProfileString(Section.c_str(), Key.c_str(), Default.c_str(), Read, 2047, FileName.c_str());
	String Result = Read;
	delete[] Read;
	return Result;
}
//---------------------------------------------------------------------------
void IniWriteString(String Section, String Key, String Text, String FileName)
{
	WritePrivateProfileString(Section.c_str(), Key.c_str(), Text.c_str(), FileName.c_str());
}    */
//---------------------------------------------------------------------------
String ReadIniSection(String Section, String FileName)
{
	if (!FileExists(FileName))return "";
	TStringList *Data = new TStringList();
	Data->LoadFromFile(FileName);
	int FPos = Data->IndexOf("[" + Section + "]");
	FPos++;

	String Result = "";
	String ALine = "";
	if (FPos > -1)
	while (FPos < Data->Count){
		ALine = Data->operator [](FPos);
		if (ALine != "")
			if (ALine[1] != '[') Result = Result + ALine + "\n";
				else break;
		FPos++;
	}
	delete Data;
	return Result;
}
//---------------------------------------------------------------------------
void WriteIniSection(String Section, String Data, String FileName)
{
	WritePrivateProfileString(Section.c_str(), 0, 0, FileName.c_str());
	TStringList *FileData = new TStringList();
	FileData->LoadFromFile(FileName);
	FileData->Text = FileData->Text + "[" + Section + "]";
	FileData->Text = FileData->Text + Data;
	FileData->SaveToFile(FileName);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

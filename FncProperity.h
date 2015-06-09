//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef FncProperityH
#define FncProperityH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "Calculator.h"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFFncProperity : public TForm
{
__published:	// IDE-managed Components
	TListView *ListView;
	TPopupMenu *PopupMenu1;
	TMenuItem *Miejscazerowe1;
	TMenuItem *Zmienne1;
	TMenuItem *Wartomaksimin1;
	TMenuItem *Wzr1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Miejscazerowe1Click(TObject *Sender);
private:	// User declarations
	String Varibles;
	TStringList *ZeroPlaces;
	double MaxValue, MinValue;

	String LastHeader;
	void AddNext(String aCaption, String aData);
	void RefreshList();
public:		// User declarations
	__fastcall TFFncProperity(TComponent* Owner);

	TCalcResult *OwnResult;

	void ShowProperity(TCalcResult *Result);
};
//---------------------------------------------------------------------------
extern PACKAGE TFFncProperity *FFncProperity;
//---------------------------------------------------------------------------
#endif

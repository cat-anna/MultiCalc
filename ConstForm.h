//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ConstFormH
#define ConstFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "FloatSpinEdit.hpp"
#include <ExtCtrls.hpp>
#include "Calculator.h"
//---------------------------------------------------------------------------
class TFIntConst : public TForm
{
__published:	// IDE-managed Components
	TListView *ListView1;
	TButton *Zamknij;
	TButton *Button2;
	TFloatSpinEdit *FloatSpinEdit1;
	TLabeledEdit *leConstName;
	TLabel *lValue;
	TButton *Button1;
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ListView1Click(TObject *Sender);
private:	// User declarations
	TVariblesList *VariblesList;

	void Reload();
public:		// User declarations
	__fastcall TFIntConst(TComponent* Owner);
	void LoadList(TVariblesList *List);
};
//---------------------------------------------------------------------------
extern PACKAGE TFIntConst *FIntConst;
//---------------------------------------------------------------------------
#endif

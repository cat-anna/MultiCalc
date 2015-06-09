//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef ExportH
#define ExportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FloatSpinEdit.hpp"
#include "Calculator.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFExport : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TListBox *lbVaribles;
	TGroupBox *GroupBox2;
	TLabel *Label3;
	TLabel *Label4;
	TFloatSpinEdit *lseVarFrom;
	TFloatSpinEdit *lseVarTo;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Button1;
	TButton *Button2;
	TLabel *Label5;
	TFloatSpinEdit *fseDelta;
	TSaveDialog *SaveDialog;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall lbVariblesClick(TObject *Sender);
	void __fastcall fseDeltaChange(TObject *Sender);
private:	// User declarations
	TVariblesList* ExportVar;
	String Expression;
public:		// User declarations
	__fastcall TFExport(TComponent* Owner);
	void Prepare(TCalcResult* Result);
};
//---------------------------------------------------------------------------
extern PACKAGE TFExport *FExport;
//---------------------------------------------------------------------------
#endif

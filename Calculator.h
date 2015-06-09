//------------------------------------------------------------------------------
#ifndef CalculatorH
#define CalculatorH

#include <Classes.hpp>
#include <Math.hpp>
#include <Graphics.hpp>
#include <ExtCtrls.hpp>
#include <gl.h>

#define FncPointer(id) RType(*id)(RType*, int)
#define FncParams RType *Param, int ParCount

#define StateCalc 1
#define StateDrawing 2

#define RT_LINES 1
#define RT_QUADS 2

#define RType float

class TControllClass;
class TFunctionsList;
//------------------------------------------------------------------------------
class TStackData
{
friend class TONPStack;
	int FPriority;
	String FData;
public:
	TStackData(String Data, int Priority);
	void Set(String Data, int Priority);
};

class TONPStack
{
	TList *FStack, *FBackStack;
public:
	TONPStack();
	~TONPStack();

	void Put(String Data, int Priority = 0);
	String PickUp();
	bool Exists();
	int ActualPriority();
	void Clear();
};

class TONPCalculator
{
	TFunctionsList *Functions;
	String FExpression, FONPData;
	TStringList *FONPList;
	int LastErrorCode;
	TONPStack *FONPStack, *FCalcStack;
	void GenerateONP();
public:
	TControllClass *ControllClass;

	TONPCalculator(TControllClass *Controll);
	~TONPCalculator();

	void SetSelf(String Expression);
	RType Calculate();

	__property int LastError = { read = LastErrorCode };
	__property String ONP = { read = FONPData };
//function ONP   : String;              //odczytanie wyra¿enia ONP
//function BladOpis :String;            //opis wystêpuj¹cego b³êdu
};
//------------------------------------------------------------------------------
class TVarible
{
	RType FInitialValue, FDelta, FEndValue, FValue;
	String FName;
	bool FConstans, FHaveRange;
	void SetValue(RType NewValue);
public:
	TVarible(RType nIV, RType nD, RType nEV, String nName);
	TVarible(TVarible *Source);
	void Assign(TVarible *Source);
	void Set(RType nIV, RType nD, RType nEV);
	bool Step();
	void Reset();
	__property bool Constans = { read = FConstans };
	__property RType Delta = { read = FDelta, write = FDelta };
	__property RType EndValue = { read = FEndValue };
	__property RType InitialValue = { read = FInitialValue };
	__property RType Value = { read = FValue, write = SetValue };
	__property String Name = { read = FName };
	__property bool HaveRange = { read = FHaveRange, write = FHaveRange };
};

class TVariblesList
{
	TList *Varibles;
	void AddLog(String Text);
	int VarCount(){return Varibles->Count;};
public:
	void AddVarible(String Name, RType Value);
	void AddVarible(String Name, RType InitValue, RType Delta, RType EndValue);
	void AddVarible(TVarible *Source);
	TVariblesList(TControllClass *Controll);
	~TVariblesList();
	void ResetAllVaribles();
	void DeleteAllVaribles();
	RType GetValue(String Name);
	bool UpdateVarible(String Name);
	void ResetVarible(String Name);
	void DeleteVarible(String Name);
	void ModifyVarible(String Name, RType InitValue, RType Delta, RType EndValue);
	String SaveVaribles();
	void LoadVaribles(String Data);
	bool VaribleExists(String Name);
	bool IsConstant(String Name);
	__property int VariblesCount = {read = VarCount};
	TVarible* GetVarible(int Index);
	TVarible* GetVarible(String Name);
	int GetVariblePos(String Name);
	void Assign(TVariblesList *Source);
	int GetVaribleCalcCount(int Index);
	TControllClass *ControllClass;	
};
//------------------------------------------------------------------------------
enum TFunctionType
{ flitInternal, flitString };

class TFunction
{
friend class TFunctionsList;
	FncPointer(Pointer);
	String FunctionData, FName;
	TFunctionType FType;
	int FParamsCount, FPriority;
    TFunctionsList *Parrent;
public:
	TFunction(FncPointer(NewPointer), String nName, int nParamsCount, int Priority);
	TFunction(String fData, String nName, int ParamsCount, int Priority);
	RType Run(RType *Params, int ParCount);
	__property String Name = { read = FName };
	__property int ParamsCount = { read = FParamsCount };
	__property TFunctionType Type = { read = FType };
	__property String Data = { read = FunctionData };
	__property int Priority = { read = FPriority };
};

class TFunctionsList
{
	TList *Functions;
	int GetByName(String Name);
	void AddStandard();
	void AddLog(String Text);
	int GetCount(){return Functions->Count;};
public:
	TFunctionsList(TControllClass *Controll);
	~TFunctionsList();
	void AddFunction(FncPointer(NewPointer), String nName, int nParamsCount, int Priority = 15);
	void AddFunction(String FunctionData, String Name, int ParCount, int Priority = 15);
	RType RunFunction(String Name, RType *Params, int ParCount, int Precision);
	String SaveFunctions();
	void LoadFunctions(String Data);
	void DeleteAllFunctions();
	void DeleteFunction(String Name);
	bool FunctionExists(String Name);
	TFunction* GetFunction(int Index);
	TFunction* GetFunction(String Name);
	void ModifyFunction(String Name, String nData, int nParCount);
	TControllClass *ControllClass;
	void Sort();
	__property int Count = { read = GetCount };
};
//------------------------------------------------------------------------------
enum TWordType
{ wtUndefined, wtOperator, wtFunction, wtNumber, wtExpression, wtVarible };

enum TCharType
{ ctUnknown, ctOperator, ctNumber, ctMinus, ctOpenBracket, ctCloseBracket,
  ctComma, ctEqual, ctChar, ctSemi };

class TWord
{
friend class TWord;
friend class TTransform;      
friend class TControllClass;
	RType NumberData;
	void *DataPointer;
	void inline SetParam(String Data);
	void inline AddLog(String Text);
	String GetSelfExp();
	void AddUnknownVarible(String Name);
	RType* CalculateParams();
	void SetSelf(String Data, TWordType Type, bool CPositive = true);
protected:
	TStringList *Params;
	TList *Children;
	String UnknownVaribles;
	void AddWord(String Data, TWordType Type, bool NPositive, TList *Dest);
	String DoExpression(String Operator, RType L1, RType L2);
	String GetExpression();
	String GetListExpression(TList *List);
	bool SetList(String Data, TList *DestList);
	void ClearList(TList *List);
	int ErrorCode;
	int GetLastError();
public:
	TControllClass *ControllClass;
	bool Positive;
	TWordType SelfType;
	TWord(TControllClass *Controll);
	TWord(TWord *Source);
	virtual ~TWord();
	void Clear();
	bool SetSelf(String Data, bool CPositive = true);
	RType Calculate();
	String StringCalculate();
	void Assign(TWord *Source);
	String GetUnknownVaribles();
	bool CheckProperity(String Data);
	__property String SelfExpression = { read = GetSelfExp, write = SetParam };
	__property int LastError = { read = GetLastError };
};
//------------------------------------------------------------------------------
class TControllClass
{
	TList *WordStack;
public:
	TVariblesList *VariblesList;
	TFunctionsList *FunctionsList;
	TStrings *Log;

	TControllClass(TStrings *nLog);
	~TControllClass();

	TWord* GetWord();
	void PutWord(TWord *W);
};
//------------------------------------------------------------------------------
enum TChartMode
{ cm2DChart, cm3DChart };

struct TChartInfo
{
	TChartMode Mode;
	int X, Y, Z, Type;
	int Width, Height, Scale, TimePos;
	int PCount, XCount, YCount;
	String vX, vY, vP;
	bool HaveParam;
	TColor LineColor;
	TPoint Offset;
};

class TCalcResult
{
friend class TChart;
	String FExpression;
	TVariblesList *CalcVaribles;
	TStringList *VarQueue;
	TControllClass *ControllClass;

	RType** TableResult;
	int TableWidth, VarCount, TablePos;

	RType MaxValue, MinValue;

	void AddResult(RType Data, TVariblesList *Vars, RType ErrorCode);
	void SetData(TVariblesList *VarList, String FExp, TStringList *VarQ, int CalcCount);
	int GetResultCount();

	void CreateTable(int aWidth, int aVarCount);
	void DeleteTable();
	bool IsTableEnd();

public:
	TCalcResult(TControllClass *Controll);
	~TCalcResult();

	bool LoadFormFile(String FileName);
	void SaveToFile(String FileName);
	void Assign(TCalcResult *Source);
	int GetVariblePos(String VarName);
	void Clear();
	__property int ResultCount = { read = TableWidth };
	__property String Expression = { read = FExpression };
	__property TStringList *Varibles = { read = VarQueue };
	__property RType** Result = { read = TableResult };
	__property bool TableEnd = { read = IsTableEnd };
	__property TVariblesList *VariblesList = { read = CalcVaribles };

	void GetVaribles(TStrings *List);
};
//------------------------------------------------------------------------------
class TChart
{
	void AddLog(String Text);
	void Clear();
	void DrawAxis(Graphics::TBitmap *Surface, TChartInfo ChartInfo);
	void Quick3DRecurrence(int VaribleNumber, TVarible *XVar, TVarible *YVar,
		TWord *Pattern, TChartInfo ChartInfo, TCalcResult *Result);
	void QuickDrawRecurrence(int VaribleNumber, Graphics::TBitmap *Surface,
		int cX, int cY, TVarible *dX, TWord *Pattern, TChartInfo ChartInfo, TCalcResult *Result);
	void CalculateRecurrence(TCalcResult *Result, int VaribleNumber,
		TWord *Pattern, TWord *Word);
	String FExpression;
	TStringList *Varibles;
	TControllClass *ControllClass;
	int FChartWidth, FChartHeight, FChartScaleX, FChartScaleY;
	TVariblesList* VarArray;
	bool FAtWorking, FCancelActWork;
	int GetListCalcCount(TVariblesList *List);
	int GetCalcCount();
public:
	TChart(TControllClass *Controll);
	~TChart();

	bool SetExpression(String Data);
	String GetVariblesQueue();
	void SetVariblesQueue(String Queue);
	void MoveVarible(String Name, bool DirUp);
	void SetVarible(TVarible *Var);
	TVarible* GetVarible(String Name);

	void PrepareChartInfo(TChartInfo *Info, TCalcResult *Result);

	void QuickChartDraw(Graphics::TBitmap *Surface, TChartInfo *ChartInfo,
		TCalcResult *Result, bool CleanSurface = true);

	bool Draw2DChart(TCalcResult *Result, Graphics::TBitmap *Surface,
		TChartInfo ChartInfo, bool CleanSurface = true);

	void Render3DChart(TCalcResult* Result, TChartInfo ChartInfo);
	void Quick3DChart(TCalcResult* Result, TChartInfo *ChartInfo);

	void QuickDraw(TCalcResult* Result, TChartInfo ChartInfo,
		Graphics::TBitmap *Surface = NULL, bool CleanSurface = true);

	void Calculate(TCalcResult *Result);

	void Cancel();
	void Assign(TChart *Source);
	void AssignResultData(TCalcResult* Result);

	__property bool Working = { read = FAtWorking };
	__property String Expression = { read = FExpression };
};
//------------------------------------------------------------------------------
class TTransform : protected TWord
{
	void inline AddLog(String Text);
protected:
	void SimplifyList(TList *List);
	bool CutSimilar(TList *List);
	bool CalcSimilar(TList *List);
	bool CutNeedless(TList *List);
	bool TranslateFunction(TList *List);
	bool TryToSimplify(TList *List);
	bool MoveInGroup(TList *List);
	void InsertList(int Pos, TList *DestList, TList *InsertedList, bool Positive);
	void MultiplyLists(TList *DestList, TList *F1, TList *F2);
	void MultiplyWords(TWord *W1, TWord *W2);
	void PowerWords(TWord *W1, TWord *W2);
int TPos;
	void GetList(TList *List, TWord *Source);
public:
	TTransform(TControllClass *Controll);
	virtual ~TTransform();

	bool Simplify(String* InData);
};
//------------------------------------------------------------------------------
enum TEquationType
{ etEqual, etSmaller, etBiggeer, etSmalerOrEqual, etBiggerOrEqual };

class TEquation : public TTransform
{
	void inline AddLog(String Text);
protected:
	bool SolveEquation(TList *LeftList, TList *RightList, TList *ResultList,
		TEquationType EqType);
	bool MoveFree(TList *Src, TList *Dest, TWordType What);
public:
	bool Solve(String *InData);
};
//------------------------------------------------------------------------------
#endif

//$$---- EXE CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Export.cpp", FExport);
USEFORM("MainForm.cpp", FMainForm);
USEFORM("ChartWindow.cpp", FChartWindow);
USEFORM("ChartPropertis.cpp", FChartPropertis);
USEFORM("ConstForm.cpp", FIntConst);
USEFORM("FncForm.cpp", FFncEdt);
USEFORM("FncProperity.cpp", FFncProperity);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TFMainForm), &FMainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

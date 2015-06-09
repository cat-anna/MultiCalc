object FMainForm: TFMainForm
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'MultiCalc'
  ClientHeight = 240
  ClientWidth = 455
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  Visible = True
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 455
    Height = 223
    ActivePage = TabSheet4
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Obliczenia'
      object mExpression: TMemo
        Left = 12
        Top = 24
        Width = 337
        Height = 105
        Lines.Strings = (
          '2+2*2')
        ScrollBars = ssVertical
        TabOrder = 0
        OnChange = mExpressionChange
      end
      object leCalcResult: TLabeledEdit
        Left = 12
        Top = 160
        Width = 337
        Height = 21
        AutoSelect = False
        EditLabel.Width = 29
        EditLabel.Height = 13
        EditLabel.Caption = 'Wynik'
        ReadOnly = True
        TabOrder = 1
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Wykresy'
      ImageIndex = 1
      object leChartExpression: TLabeledEdit
        Left = 5
        Top = 16
        Width = 167
        Height = 21
        EditLabel.Width = 25
        EditLabel.Height = 13
        EditLabel.Caption = 'Wz'#243'r'
        TabOrder = 0
        OnChange = leChartExpressionChange
      end
      object gbVaribles: TGroupBox
        Left = 3
        Top = 43
        Width = 330
        Height = 150
        Caption = 'Opcje'
        TabOrder = 1
        object Label6: TLabel
          Left = 176
          Top = 19
          Width = 91
          Height = 13
          Caption = 'O'#347' odci'#281'tych (OX):'
        end
        object LabelOY: TLabel
          Left = 176
          Top = 48
          Width = 89
          Height = 13
          Caption = 'O'#347' rz'#281'dnych (OY):'
        end
        object lParamName: TLabel
          Left = 200
          Top = 107
          Width = 48
          Height = 13
          Caption = 'Parametr:'
        end
        object lTimeStep: TLabel
          Left = 200
          Top = 126
          Width = 44
          Height = 13
          Caption = 'Szybko'#347#263
          WordWrap = True
        end
        object lParam: TLabel
          Left = 259
          Top = 107
          Width = 32
          Height = 13
          Caption = 'lParam'
        end
        object Label2: TLabel
          Left = 26
          Top = 19
          Width = 44
          Height = 13
          Caption = 'Zmienne:'
        end
        object lbVaribles: TListBox
          Left = 26
          Top = 41
          Width = 63
          Height = 100
          ItemHeight = 13
          TabOrder = 0
          OnClick = lbVariblesClick
        end
        object CSpinButton1: TCSpinButton
          Left = 3
          Top = 56
          Width = 20
          Height = 57
          DownGlyph.Data = {
            0E010000424D0E01000000000000360000002800000009000000060000000100
            200000000000D800000000000000000000000000000000000000008080000080
            8000008080000080800000808000008080000080800000808000008080000080
            8000008080000080800000808000000000000080800000808000008080000080
            8000008080000080800000808000000000000000000000000000008080000080
            8000008080000080800000808000000000000000000000000000000000000000
            0000008080000080800000808000000000000000000000000000000000000000
            0000000000000000000000808000008080000080800000808000008080000080
            800000808000008080000080800000808000}
          TabOrder = 1
          UpGlyph.Data = {
            0E010000424D0E01000000000000360000002800000009000000060000000100
            200000000000D800000000000000000000000000000000000000008080000080
            8000008080000080800000808000008080000080800000808000008080000080
            8000000000000000000000000000000000000000000000000000000000000080
            8000008080000080800000000000000000000000000000000000000000000080
            8000008080000080800000808000008080000000000000000000000000000080
            8000008080000080800000808000008080000080800000808000000000000080
            8000008080000080800000808000008080000080800000808000008080000080
            800000808000008080000080800000808000}
          OnDownClick = CSpinButton1DownClick
          OnUpClick = CSpinButton1UpClick
        end
        object cbHaveRange: TCheckBox
          Left = 95
          Top = 18
          Width = 68
          Height = 17
          Caption = 'Ma zakres'
          TabOrder = 2
          OnClick = cbHaveRangeClick
        end
        object cbOX: TComboBox
          Left = 273
          Top = 16
          Width = 48
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 3
          OnDropDown = cbOYDropDown
        end
        object cbOY: TComboBox
          Left = 273
          Top = 45
          Width = 48
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 4
          OnDropDown = cbOYDropDown
        end
        object GroupBox2: TGroupBox
          Left = 95
          Top = 38
          Width = 75
          Height = 103
          Caption = 'Zakres'
          TabOrder = 5
          object Label3: TLabel
            Left = 7
            Top = 18
            Width = 14
            Height = 13
            Caption = 'Od'
          end
          object Label4: TLabel
            Left = 7
            Top = 60
            Width = 13
            Height = 13
            Caption = 'Do'
          end
          object lseVarFrom: TFloatSpinEdit
            Tag = 1
            Left = 7
            Top = 40
            Width = 59
            Height = 22
            Increment = 0.100000001490116100
            MaxValue = 0
            MinValue = 0
            TabOrder = 0
            Precision = 3
            OnChange = lseVarFromChange
          end
          object lseVarTo: TFloatSpinEdit
            Tag = 2
            Left = 7
            Top = 78
            Width = 59
            Height = 22
            Increment = 0.100000001490116100
            MaxValue = 0
            MinValue = 0
            TabOrder = 1
            Precision = 3
            OnChange = lseVarFromChange
          end
        end
        object cbAnimateChart: TCheckBox
          Left = 176
          Top = 72
          Width = 109
          Height = 29
          Caption = 'Funkcja posiada parametr'
          TabOrder = 6
          WordWrap = True
          OnClick = cbAnimateChartClick
        end
        object fseTimeStep: TCSpinEdit
          Left = 260
          Top = 122
          Width = 49
          Height = 22
          TabOrder = 7
          Value = 10
          OnChange = fseTimeStepChange
        end
      end
      object cDraw: TButton
        Left = 339
        Top = 70
        Width = 75
        Height = 25
        Caption = 'Rysuj'
        TabOrder = 2
        OnClick = cDrawClick
      end
      object rgChartType: TRadioGroup
        Left = 178
        Top = 4
        Width = 119
        Height = 33
        Caption = 'Typ'
        Columns = 2
        ItemIndex = 0
        Items.Strings = (
          '2D'
          '3D')
        TabOrder = 3
        OnClick = rgChartTypeClick
      end
      object cbDrawChartAdd: TCheckBox
        Left = 339
        Top = 3
        Width = 95
        Height = 50
        Caption = 'Dorysuj wykres do okna:'
        TabOrder = 4
        WordWrap = True
        OnClick = cbDrawChartAddClick
      end
      object cbChartWindow: TComboBox
        Left = 362
        Top = 43
        Width = 73
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 5
        OnDropDown = cbChartWindowDropDown
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Opcje'
      ImageIndex = 2
      object GroupBox4: TGroupBox
        Left = 0
        Top = 3
        Width = 177
        Height = 105
        Caption = 'Edytuj:'
        TabOrder = 0
        object Button1: TButton
          Left = 14
          Top = 24
          Width = 105
          Height = 25
          Caption = 'Wewn'#281'trzne sta'#322'e'
          TabOrder = 0
          OnClick = Button1Click
        end
        object Button2: TButton
          Left = 14
          Top = 55
          Width = 105
          Height = 25
          Caption = 'Funkcje'
          TabOrder = 1
          OnClick = Button2Click
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'DB Test'
      ImageIndex = 3
      object GroupBox1: TGroupBox
        Left = 207
        Top = 3
        Width = 230
        Height = 182
        Caption = 'Log'
        TabOrder = 0
        object Memo1: TMemo
          Left = 2
          Top = 15
          Width = 226
          Height = 165
          Align = alClient
          TabOrder = 0
        end
      end
      object eTest: TEdit
        Left = 3
        Top = 18
        Width = 198
        Height = 21
        TabOrder = 1
        Text = 'eTest'
      end
      object bTest: TButton
        Left = 3
        Top = 45
        Width = 94
        Height = 25
        Caption = 'transform test'
        TabOrder = 2
        OnClick = bTestClick
      end
      object eOutTest: TEdit
        Left = 3
        Top = 76
        Width = 198
        Height = 21
        TabOrder = 3
        Text = 'eOutTest'
      end
      object Button5: TButton
        Left = 126
        Top = 160
        Width = 75
        Height = 24
        Caption = 'Button5'
        TabOrder = 4
        OnClick = Button5Click
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'DB Optymalizacja'
      ImageIndex = 4
      object Memo2: TMemo
        Left = 8
        Top = 8
        Width = 161
        Height = 193
        Lines.Strings = (
          'Memo2')
        TabOrder = 0
      end
      object RadioGroup1: TRadioGroup
        Left = 175
        Top = 8
        Width = 137
        Height = 113
        Caption = 'RadioGroup1'
        Items.Strings = (
          'word'
          'ONPCalc'
          'BinMath'
          'inpr'
          'eval')
        TabOrder = 1
      end
      object Button3: TButton
        Left = 175
        Top = 127
        Width = 67
        Height = 25
        Caption = 'test'
        TabOrder = 2
        OnClick = Button3Click
      end
      object Edit1: TEdit
        Left = 175
        Top = 158
        Width = 137
        Height = 21
        TabOrder = 3
        Text = 'sin(4)'
      end
      object Button4: TButton
        Left = 248
        Top = 127
        Width = 64
        Height = 25
        Caption = 'clear'
        TabOrder = 4
        OnClick = Button4Click
      end
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 223
    Width = 455
    Height = 17
    Panels = <>
  end
  object OpenDialog: TOpenDialog
    Left = 384
  end
  object SaveDialog: TSaveDialog
    Left = 416
  end
  object MainMenu1: TMainMenu
    Left = 349
    Top = 1
  end
end

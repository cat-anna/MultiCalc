object FIntConst: TFIntConst
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Wewn'#281'trzne sta'#322'e'
  ClientHeight = 193
  ClientWidth = 319
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object lValue: TLabel
    Left = 191
    Top = 51
    Width = 40
    Height = 13
    Caption = 'Warto'#347#263
  end
  object ListView1: TListView
    Left = 8
    Top = 9
    Width = 177
    Height = 176
    Columns = <
      item
        Caption = 'Nazwa'
        Width = 100
      end
      item
        Caption = 'Warto'#347#263
        Width = 52
      end>
    ColumnClick = False
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnClick = ListView1Click
  end
  object Zamknij: TButton
    Left = 237
    Top = 160
    Width = 75
    Height = 25
    Caption = 'Zamknij'
    ModalResult = 1
    TabOrder = 1
  end
  object Button2: TButton
    Left = 191
    Top = 98
    Width = 121
    Height = 25
    Caption = 'Modyfikuj/Dodaj'
    TabOrder = 2
    OnClick = Button2Click
  end
  object FloatSpinEdit1: TFloatSpinEdit
    Left = 191
    Top = 70
    Width = 121
    Height = 22
    Increment = 0.100000001490116100
    MaxValue = 0
    MinValue = 0
    TabOrder = 3
    Precision = 3
  end
  object leConstName: TLabeledEdit
    Left = 191
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 32
    EditLabel.Height = 13
    EditLabel.Caption = 'Nazwa'
    TabOrder = 4
  end
  object Button1: TButton
    Left = 191
    Top = 129
    Width = 121
    Height = 25
    Caption = 'Usu'#324
    TabOrder = 5
    OnClick = Button1Click
  end
end

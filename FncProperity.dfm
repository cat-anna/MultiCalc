object FFncProperity: TFFncProperity
  Left = 0
  Top = 0
  Caption = 'W'#322'asno'#347'ci funkcji'
  ClientHeight = 348
  ClientWidth = 253
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object ListView: TListView
    Left = 0
    Top = 0
    Width = 253
    Height = 348
    Align = alClient
    Columns = <
      item
        Caption = 'W'#322'asno'#347#263
        Width = 80
      end
      item
        Caption = 'Warto'#347#263
        Width = 150
      end>
    PopupMenu = PopupMenu1
    TabOrder = 0
    ViewStyle = vsReport
  end
  object PopupMenu1: TPopupMenu
    Left = 216
    Top = 24
    object Wzr1: TMenuItem
      AutoCheck = True
      Caption = 'Wz'#243'r'
      Checked = True
      OnClick = Miejscazerowe1Click
    end
    object Zmienne1: TMenuItem
      AutoCheck = True
      Caption = 'Zmienne'
      Checked = True
      OnClick = Miejscazerowe1Click
    end
    object Wartomaksimin1: TMenuItem
      AutoCheck = True
      Caption = 'Warto'#347#263' maks. i min.'
      Checked = True
      OnClick = Miejscazerowe1Click
    end
    object Miejscazerowe1: TMenuItem
      AutoCheck = True
      Caption = 'Miejsca zerowe'
      Checked = True
      OnClick = Miejscazerowe1Click
    end
  end
end

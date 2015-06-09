object FChartWindow: TFChartWindow
  Left = 44
  Top = 47
  Caption = 'Wykres'
  ClientHeight = 150
  ClientWidth = 166
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyUp = FormKeyUp
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object TabControl: TTabControl
    Left = 0
    Top = 10
    Width = 143
    Height = 115
    PopupMenu = PopupMenu1
    TabOrder = 0
    Tabs.Strings = (
      'a'
      'b')
    TabIndex = 0
    OnChange = TabControlChange
    object iDraw: TImage
      Left = 4
      Top = 24
      Width = 135
      Height = 87
      Align = alBottom
      Center = True
      PopupMenu = PopupMenu1
      OnMouseDown = i2DChartMouseDown
      OnMouseMove = i2DChartMouseMove
      OnMouseUp = i2DChartMouseUp
      ExplicitLeft = 35
      ExplicitTop = 32
      ExplicitWidth = 105
      ExplicitHeight = 105
    end
    object glWindow: TglWindow
      Left = 16
      Top = 32
      Width = 65
      Height = 65
      Visible = False
      OnMouseDown = glWindowMouseDown
      OnMouseMove = glWindowMouseMove
      OnMouseUp = glWindowMouseUp
      ColorDepth = c16bits
      DepthBits = c16bits
      DepthBufferEnabled = True
      StencBits = c16bits
      StencBufferEnabled = False
      AccumBits = c16bits
      AccumBufferEnabled = False
      WindowFlags = [wfDrawToWindow, wfSupportOpenGL, wfDoubleBuffer]
      OnInit = glWindowInit
    end
  end
  object Status: TStatusBar
    Left = 0
    Top = 131
    Width = 166
    Height = 19
    Panels = <
      item
        Width = 100
      end
      item
        Width = 100
      end
      item
        Alignment = taRightJustify
        Width = 50
      end>
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 40
  end
  object PopupMenu1: TPopupMenu
    OnPopup = PopupMenu1Popup
    Left = 72
    object Zamknij1: TMenuItem
      Caption = '&Zamknij'
      ShortCut = 90
      OnClick = Zamknij1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Zapisz1: TMenuItem
      Caption = 'Zapi&sz'
      ShortCut = 83
      OnClick = Zapisz1Click
    end
    object Zapiszdanewykresu1: TMenuItem
      Caption = 'Zapisz &dane wykresu'
      ShortCut = 16467
      Visible = False
      OnClick = Zapiszdanewykresu1Click
    end
    object EksportujdoplikuExcel1: TMenuItem
      Caption = 'Eksportuj do pliku Excel'
      ShortCut = 69
      Visible = False
      OnClick = EksportujdoplikuExcel1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Opjewykresu1: TMenuItem
      Caption = '&Opcje wykresu'
      ShortCut = 79
      OnClick = Opjewykresu1Click
    end
    object Reset1: TMenuItem
      Caption = '&Reset'
      ShortCut = 82
      OnClick = Reset1Click
    end
    object Wasnocifunkcji1: TMenuItem
      Caption = 'W'#322'asno'#347'ci funkcji'
      ShortCut = 87
      OnClick = Wasnocifunkcji1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Pauza1: TMenuItem
      AutoCheck = True
      Caption = '&Pauza'
      ShortCut = 80
      OnClick = Pauza1Click
    end
    object Zaptlczas1: TMenuItem
      Caption = 'Zap'#281'tl czas'
      ShortCut = 67
      OnClick = Zaptlczas1Click
    end
  end
  object SaveDialog: TSaveDialog
    Left = 8
  end
end

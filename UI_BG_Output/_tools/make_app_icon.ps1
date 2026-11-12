param(
    [string]$HeadPath = "C:\Workspace\Unreal\ZZakTungSoul\Content\Assets\Armor\ArmorImage\Icon_Coupang_Head.png",
    [string]$OutDir   = "C:\Workspace\Unreal\ZZakTungSoul\UI_BG_Output\Icon",
    [string]$Name     = "AppIcon",
    [double]$HelmetFill = 0.80
)
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force $OutDir | Out-Null

$S = 1024
$src = [System.Drawing.Bitmap]::FromFile($HeadPath)

# 1) 알파 기준으로 헬멧 영역만 잘라내기
$minX=$src.Width; $minY=$src.Height; $maxX=0; $maxY=0
for ($y=0; $y -lt $src.Height; $y+=2) { for ($x=0; $x -lt $src.Width; $x+=2) {
    if ($src.GetPixel($x,$y).A -gt 40) {
        if ($x -lt $minX){$minX=$x}; if ($x -gt $maxX){$maxX=$x}
        if ($y -lt $minY){$minY=$y}; if ($y -gt $maxY){$maxY=$y}
    } } }
$cw = $maxX-$minX+1; $ch = $maxY-$minY+1
$crop = New-Object System.Drawing.Rectangle $minX,$minY,$cw,$ch

$bmp = New-Object System.Drawing.Bitmap $S,$S,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = 'AntiAlias'; $g.InterpolationMode = 'HighQualityBicubic'; $g.PixelOffsetMode='HighQuality'
$g.Clear([System.Drawing.Color]::Transparent)

# 2) 둥근 사각형 배경(클립)
$r = 200
$path = New-Object System.Drawing.Drawing2D.GraphicsPath
$path.AddArc(0,0,$r,$r,180,90); $path.AddArc($S-$r,0,$r,$r,270,90)
$path.AddArc($S-$r,$S-$r,$r,$r,0,90); $path.AddArc(0,$S-$r,$r,$r,90,90); $path.CloseFigure()
$g.SetClip($path)

$bg = New-Object System.Drawing.Drawing2D.LinearGradientBrush ([System.Drawing.Point]::new(0,0)),([System.Drawing.Point]::new(0,$S)),([System.Drawing.Color]::FromArgb(255,52,36,28)),([System.Drawing.Color]::FromArgb(255,14,10,9))
$g.FillRectangle($bg,0,0,$S,$S)

# 따뜻한 화톳불 빛(방사형)
$gp = New-Object System.Drawing.Drawing2D.GraphicsPath
$gp.AddEllipse(-120,150,$S+240,$S+240)
$pg = New-Object System.Drawing.Drawing2D.PathGradientBrush $gp
$pg.CenterPoint = [System.Drawing.PointF]::new($S/2, $S*0.62)
$pg.CenterColor = [System.Drawing.Color]::FromArgb(210,235,120,35)
$pg.SurroundColors = @([System.Drawing.Color]::FromArgb(0,235,120,35))
$g.FillPath($pg,$gp)

# 3) 헬멧 배치 + 바닥 그림자
$dh = [int]($S*$HelmetFill); $dw = [int]($dh*$cw/$ch)
$dx = [int](($S-$dw)/2); $dy = [int](($S-$dh)/2 + $S*0.01)
for ($i=0; $i -lt 8; $i++) {
    $grow = $i*6
    $br = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(34,0,0,0))
    $g.FillEllipse($br, [single]($dx+$dw*0.10-$grow), [single]($dy+$dh*0.91-$grow*0.4), [single]($dw*0.80+2*$grow), [single]($dh*0.09+0.8*$grow))
    $br.Dispose()
}

$dest = New-Object System.Drawing.Rectangle $dx,$dy,$dw,$dh
$g.DrawImage($src,$dest,$crop.X,$crop.Y,$crop.Width,$crop.Height,[System.Drawing.GraphicsUnit]::Pixel)

# 4) 안쪽 테두리(은은한 외곽선)
$g.ResetClip()
$pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(110,255,190,120)),6
$g.DrawPath($pen,$path)
$g.Dispose()

$png1024 = Join-Path $OutDir "$Name`_1024.png"
$bmp.Save($png1024,[System.Drawing.Imaging.ImageFormat]::Png)

# 5) 멀티 사이즈 ICO (PNG 압축 엔트리)
$sizes = 256,128,64,48,32,16
$entries = @()
foreach ($s in $sizes) {
    $b = New-Object System.Drawing.Bitmap $s,$s,([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gg = [System.Drawing.Graphics]::FromImage($b)
    $gg.InterpolationMode='HighQualityBicubic'; $gg.SmoothingMode='AntiAlias'; $gg.PixelOffsetMode='HighQuality'
    $gg.DrawImage($bmp,0,0,$s,$s); $gg.Dispose()
    $ms = New-Object System.IO.MemoryStream
    $b.Save($ms,[System.Drawing.Imaging.ImageFormat]::Png)
    $entries += ,@($s,$ms.ToArray()); $b.Dispose()
}
$ico = New-Object System.IO.MemoryStream
$bw = New-Object System.IO.BinaryWriter $ico
$bw.Write([uint16]0); $bw.Write([uint16]1); $bw.Write([uint16]$entries.Count)
$offset = 6 + 16*$entries.Count
foreach ($e in $entries) {
    $s=$e[0]; $d=$e[1]
    $wb = if ($s -ge 256) {0} else {$s}
    $bw.Write([byte]$wb); $bw.Write([byte]$wb); $bw.Write([byte]0); $bw.Write([byte]0)
    $bw.Write([uint16]1); $bw.Write([uint16]32); $bw.Write([uint32]$d.Length); $bw.Write([uint32]$offset)
    $offset += $d.Length
}
foreach ($e in $entries) { $bw.Write($e[1]) }
$bw.Flush()
[System.IO.File]::WriteAllBytes((Join-Path $OutDir "$Name.ico"),$ico.ToArray())

# 작은 크기 미리보기(16/32/64를 한 줄로)
$pv = New-Object System.Drawing.Bitmap 420,150
$pg2 = [System.Drawing.Graphics]::FromImage($pv); $pg2.Clear([System.Drawing.Color]::FromArgb(255,240,240,240))
$pg2.InterpolationMode='HighQualityBicubic'
$x=10; foreach ($s in 128,64,32,16) { $pg2.DrawImage($bmp,$x,10,$s,$s); $x += $s+14 }
$pv.Save((Join-Path $OutDir "_Preview_$Name`_small.png"),[System.Drawing.Imaging.ImageFormat]::Png)
$pg2.Dispose(); $pv.Dispose(); $bmp.Dispose(); $src.Dispose()
"done: $OutDir"

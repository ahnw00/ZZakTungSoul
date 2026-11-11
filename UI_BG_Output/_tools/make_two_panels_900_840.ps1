param(
    [string]$SrcPath,
    [string]$OutDir
)

Add-Type -AssemblyName System.Drawing

$src = [System.Drawing.Image]::FromFile($SrcPath)
$SW = $src.Width
$SH = $src.Height

# Source 9-slice margins (source pixels). Measured from the torn-edge frame of the sheet.
$L = 75; $R = 95; $T = 125; $B = 110

$ia = New-Object System.Drawing.Imaging.ImageAttributes
$ia.SetWrapMode([System.Drawing.Drawing2D.WrapMode]::TileFlipXY)

function Blit($g, [int]$dx, [int]$dy, [int]$dw, [int]$dh, [double]$sx, [double]$sy, [double]$sw, [double]$sh) {
    $dest = New-Object System.Drawing.Rectangle($dx, $dy, $dw, $dh)
    $g.DrawImage($src, $dest, [single]$sx, [single]$sy, [single]$sw, [single]$sh, [System.Drawing.GraphicsUnit]::Pixel, $ia)
}

function New-Graphics($bmp) {
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
    return $g
}

function Save-Png($bmp, [string]$name) {
    $path = Join-Path $OutDir $name
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    "saved: $path ($($bmp.Width)x$($bmp.Height))"
}

# Plain resize (used for the full-screen version).
function New-Full([int]$W, [int]$H, [string]$name) {
    $bmp = New-Object System.Drawing.Bitmap($W, $H, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = New-Graphics $bmp
    Blit $g 0 0 $W $H 0 0 $SW $SH
    $g.Dispose()
    Save-Png $bmp $name
    $bmp.Dispose()
}

# 9-slice panel: corners at scale f, edges stretched along their length only,
# center cropped at scale f (no distortion when the source is large enough).
function New-Panel([int]$W, [int]$H, [string]$name) {
    $f = [math]::Min(1.0, [math]::Max(0.3, [math]::Min($W, $H) / 800.0))

    $bmp = New-Object System.Drawing.Bitmap($W, $H, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = New-Graphics $bmp

    $dl = [int][math]::Round($L * $f)
    $dr = [int][math]::Round($R * $f)
    $dt = [int][math]::Round($T * $f)
    $db = [int][math]::Round($B * $f)
    $cw = $W - $dl - $dr
    $ch = $H - $dt - $db

    $cSrcW = $SW - $L - $R
    $cSrcH = $SH - $T - $B

    # center
    $cropW = [math]::Min($cSrcW, $cw / $f)
    $cropH = [math]::Min($cSrcH, $ch / $f)
    $cropX = $L + ($cSrcW - $cropW) / 2
    $cropY = $T + ($cSrcH - $cropH) / 2
    Blit $g $dl $dt $cw $ch $cropX $cropY $cropW $cropH

    # edges
    Blit $g $dl 0 $cw $dt $L 0 $cSrcW $T
    Blit $g $dl ($H - $db) $cw $db $L ($SH - $B) $cSrcW $B
    Blit $g 0 $dt $dl $ch 0 $T $L $cSrcH
    Blit $g ($W - $dr) $dt $dr $ch ($SW - $R) $T $R $cSrcH

    # corners
    Blit $g 0 0 $dl $dt 0 0 $L $T
    Blit $g ($W - $dr) 0 $dr $dt ($SW - $R) 0 $R $T
    Blit $g 0 ($H - $db) $dl $db 0 ($SH - $B) $L $B
    Blit $g ($W - $dr) ($H - $db) $dr $db ($SW - $R) ($SH - $B) $R $B

    $g.Dispose()
    "scale f = $([math]::Round($f, 3))"
    Save-Png $bmp $name
    $bmp.Dispose()
}

if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }

New-Panel 900  900  "T_UI_BG_Panel_900x900.png"
New-Panel 840  745  "T_UI_BG_Panel_840x745.png"

$src.Dispose()


param(
    [string]$SrcPath,
    [string]$OutDir,
    [int]$W = 1040,
    [int]$H = 110,
    [double]$F = 0.2,
    [string]$Name = "T_UI_BG_BossHUDPanel_1040x110.png"
)

Add-Type -AssemblyName System.Drawing

$src = [System.Drawing.Image]::FromFile($SrcPath)
$SW = $src.Width
$SH = $src.Height

# Horizontally mirrored copy of the source: used for every other tile so tile seams are continuous.
$flip = New-Object System.Drawing.Bitmap($src)
$flip.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipX)

# Source 9-slice margins (source pixels), same as the other panels.
$L = 75; $R = 95; $T = 125; $B = 110

$ia = New-Object System.Drawing.Imaging.ImageAttributes
$ia.SetWrapMode([System.Drawing.Drawing2D.WrapMode]::TileFlipXY)

function Blit($g, $img, [double]$dx, [double]$dy, [double]$dw, [double]$dh, [double]$sx, [double]$sy, [double]$sw, [double]$sh) {
    $dest = New-Object System.Drawing.RectangleF([single]$dx, [single]$dy, [single]$dw, [single]$dh)
    $pts = [System.Drawing.PointF[]]@(
        (New-Object System.Drawing.PointF($dest.Left,  $dest.Top)),
        (New-Object System.Drawing.PointF($dest.Right, $dest.Top)),
        (New-Object System.Drawing.PointF($dest.Left,  $dest.Bottom)))
    $srcRect = New-Object System.Drawing.RectangleF([single]$sx, [single]$sy, [single]$sw, [single]$sh)
    $g.DrawImage($img, $pts, $srcRect, [System.Drawing.GraphicsUnit]::Pixel, $ia)
}

# Fill a horizontal band by repeating one source strip at scale $F, mirroring every other tile.
function Tile-Horizontal($g, [double]$dx, [double]$dy, [double]$dw, [double]$dh, [double]$sx, [double]$sy, [double]$sw, [double]$sh, [double]$phase = 0) {
    $tileW = $sw * $F
    $g.SetClip((New-Object System.Drawing.RectangleF([single]$dx, [single]$dy, [single]$dw, [single]$dh)))
    $i = 0
    $x = -$phase
    while ($x -lt $dw - 0.01) {
        if ($i % 2 -eq 0) {
            Blit $g $src ($dx + $x) $dy $tileW $dh $sx $sy $sw $sh
        } else {
            # flipped copy: source x is mirrored, take the same strip from the mirrored side
            $fx = $SW - ($sx + $sw)
            Blit $g $flip ($dx + $x) $dy $tileW $dh $fx $sy $sw $sh
        }
        $x += $tileW
        $i++
    }
    $g.ResetClip()
}

$dl = [int][math]::Round($L * $F)
$dr = [int][math]::Round($R * $F)
$dt = [int][math]::Round($T * $F)
$db = [int][math]::Round($B * $F)
$cw = $W - $dl - $dr
$ch = $H - $dt - $db

$cSrcW = $SW - $L - $R
$cSrcH = $SH - $T - $B

$bmp = New-Object System.Drawing.Bitmap($W, $H, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
$g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality

# center: flat cardboard, cropped from the middle of the source (height) and mirror-tiled along the width
$cropH = [math]::Min($cSrcH, $ch / $F)
$cropY = $T + ($cSrcH - $cropH) / 2
Tile-Horizontal $g $dl $dt $cw $ch $L $cropY $cSrcW $cropH

# top / bottom torn edges: repeated at scale F (not stretched)
Tile-Horizontal $g $dl 0 $cw $dt $L 0 $cSrcW $T 0
Tile-Horizontal $g $dl ($H - $db) $cw $db $L ($SH - $B) $cSrcW $B ($cSrcW * $F / 2)

# left / right edges: cropped from the middle of the source, uniform scale F
$edgeH = [math]::Min($cSrcH, $ch / $F)
$edgeY = $T + ($cSrcH - $edgeH) / 2
Blit $g $src 0 $dt $dl $ch 0 $edgeY $L $edgeH
Blit $g $src ($W - $dr) $dt $dr $ch ($SW - $R) $edgeY $R $edgeH

# corners
Blit $g $src 0 0 $dl $dt 0 0 $L $T
Blit $g $src ($W - $dr) 0 $dr $dt ($SW - $R) 0 $R $T
Blit $g $src 0 ($H - $db) $dl $db 0 ($SH - $B) $L $B
Blit $g $src ($W - $dr) ($H - $db) $dr $db ($SW - $R) ($SH - $B) $R $B

$g.Dispose()
if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }
$path = Join-Path $OutDir $Name
$bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
"edges L/R/T/B = $dl/$dr/$dt/$db px, interior = ${cw}x${ch}"
"saved: $path ($($bmp.Width)x$($bmp.Height))"
$bmp.Dispose(); $flip.Dispose(); $src.Dispose()

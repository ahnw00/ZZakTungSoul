param(
    [string]$InPath,
    [string]$OutPath,
    [int]$Thickness = 8,
    [string]$PreviewPath = ""
)

# Adds a solid black outline around a transparent-background logo PNG without changing the canvas size,
# so the logo keeps exactly the same position/scale in the material. Pixels outside the outline stay transparent
# with black RGB (avoids light halos when the texture is filtered).
$cs = @'
using System;
using System.Drawing;
using System.Drawing.Imaging;

public static class LogoOutline
{
    public static string Make(string inPath, string outPath, string previewPath, int T)
    {
        int W, H;
        int[] px;
        using (Bitmap raw = new Bitmap(inPath))
        {
            W = raw.Width; H = raw.Height;
            using (Bitmap src = raw.Clone(new Rectangle(0, 0, W, H), PixelFormat.Format32bppArgb))
            {
                BitmapData d = src.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                px = new int[W * H];
                System.Runtime.InteropServices.Marshal.Copy(d.Scan0, px, 0, W * H);
                src.UnlockBits(d);
            }
        }
        int N = W * H;
        float[] A = new float[N];
        int minX = W, minY = H, maxX = -1, maxY = -1;
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * W + x;
                A[i] = ((px[i] >> 24) & 255) / 255f;
                if (A[i] > 0.02f)
                {
                    if (x < minX) minX = x; if (x > maxX) maxX = x;
                    if (y < minY) minY = y; if (y > maxY) maxY = y;
                }
            }
        int mL = minX, mT = minY, mR = W - 1 - maxX, mB = H - 1 - maxY;

        // kernel: coverage by distance
        int K = T + 1;
        int KS = 2 * K + 1;
        float[] ker = new float[KS * KS];
        for (int dy = -K; dy <= K; dy++)
            for (int dx = -K; dx <= K; dx++)
            {
                float dist = (float)Math.Sqrt(dx * dx + dy * dy);
                float cov = T + 0.5f - dist;
                ker[(dy + K) * KS + (dx + K)] = cov < 0f ? 0f : (cov > 1f ? 1f : cov);
            }

        float[] D = new float[N];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                float a = A[y * W + x];
                if (a <= 0.004f) continue;
                for (int dy = -K; dy <= K; dy++)
                {
                    int yy = y + dy; if (yy < 0 || yy >= H) continue;
                    for (int dx = -K; dx <= K; dx++)
                    {
                        int xx = x + dx; if (xx < 0 || xx >= W) continue;
                        float v = a * ker[(dy + K) * KS + (dx + K)];
                        int j = yy * W + xx;
                        if (v > D[j]) D[j] = v;
                    }
                }
            }

        int[] outPx = new int[N];
        int[] prePx = new int[N];
        for (int i = 0; i < N; i++)
        {
            float a = A[i];
            float oa = a + D[i] * (1f - a);
            float r = 0f, g = 0f, b = 0f;
            if (oa > 0.0001f)
            {
                float lr = (px[i] >> 16) & 255, lg = (px[i] >> 8) & 255, lb = px[i] & 255;
                r = lr * a / oa; g = lg * a / oa; b = lb * a / oa;
            }
            int ia = (int)(oa * 255f + 0.5f); if (ia > 255) ia = 255;
            int ir = (int)(r + 0.5f), ig = (int)(g + 0.5f), ib = (int)(b + 0.5f);
            outPx[i] = (ia << 24) | (ir << 16) | (ig << 8) | ib;
            // preview over a red-orange box colour
            prePx[i] = (255 << 24)
                | ((int)(r * oa + 205f * (1f - oa) + 0.5f) << 16)
                | ((int)(g * oa + 72f * (1f - oa) + 0.5f) << 8)
                | (int)(b * oa + 28f * (1f - oa) + 0.5f);
        }
        Save(outPx, W, H, outPath);
        if (previewPath != null && previewPath.Length > 0) Save(prePx, W, H, previewPath);
        return "size=" + W + "x" + H + " logo margins L/T/R/B before outline = " + mL + "/" + mT + "/" + mR + "/" + mB + " px, outline = " + T + " px"
            + ((mL <= T || mT <= T || mR <= T || mB <= T) ? "  WARNING: outline may touch the canvas edge" : "  (fits inside the existing padding)");
    }

    static void Save(int[] px, int W, int H, string path)
    {
        using (Bitmap bm = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            BitmapData d = bm.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(px, 0, d.Scan0, W * H);
            bm.UnlockBits(d);
            bm.Save(path, ImageFormat.Png);
        }
    }
}
'@

Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition $cs -Language CSharp
$dir = Split-Path $OutPath -Parent
if ($dir -and -not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }
[LogoOutline]::Make($InPath, $OutPath, $PreviewPath, $Thickness)
"saved: $OutPath"

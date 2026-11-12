param(
    [Parameter(Mandatory=$true)][string]$SrcPath,
    [Parameter(Mandatory=$true)][string]$OutPath,
    [string]$CleanPath = "",
    [int]$Seed = 7
)

Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

public static class ExitIconMaker
{
    static int W, H;

    static float[] LoadRgb(string path)
    {
        using (Bitmap bmp = new Bitmap(path))
        {
            W = bmp.Width; H = bmp.Height;
            float[] a = new float[W * H * 3];
            BitmapData d = bmp.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            byte[] buf = new byte[d.Stride * H];
            System.Runtime.InteropServices.Marshal.Copy(d.Scan0, buf, 0, buf.Length);
            bmp.UnlockBits(d);
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++)
                {
                    int i = y * d.Stride + x * 4, o = (y * W + x) * 3;
                    a[o] = buf[i + 2]; a[o + 1] = buf[i + 1]; a[o + 2] = buf[i];
                }
            return a;
        }
    }

    static void SaveRgb(float[] a, string path)
    {
        using (Bitmap bmp = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            BitmapData d = bmp.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            byte[] buf = new byte[d.Stride * H];
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++)
                {
                    int i = y * d.Stride + x * 4, o = (y * W + x) * 3;
                    buf[i] = B(a[o + 2]); buf[i + 1] = B(a[o + 1]); buf[i + 2] = B(a[o]); buf[i + 3] = 255;
                }
            System.Runtime.InteropServices.Marshal.Copy(buf, 0, d.Scan0, buf.Length);
            bmp.UnlockBits(d);
            bmp.Save(path, ImageFormat.Png);
        }
    }

    static byte B(float v) { return (byte)(v < 0 ? 0 : (v > 255 ? 255 : v + 0.5f)); }
    static int Cl(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

    static float[] BoxBlur(float[] src, int w, int h, int r)
    {
        float[] tmp = new float[w * h], dst = new float[w * h];
        double inv = 1.0 / (2 * r + 1);
        for (int y = 0; y < h; y++)
        {
            int row = y * w; double sum = 0;
            for (int k = -r; k <= r; k++) sum += src[row + Cl(k, 0, w - 1)];
            for (int x = 0; x < w; x++)
            {
                tmp[row + x] = (float)(sum * inv);
                sum += src[row + Cl(x + r + 1, 0, w - 1)] - src[row + Cl(x - r, 0, w - 1)];
            }
        }
        for (int x = 0; x < w; x++)
        {
            double sum = 0;
            for (int k = -r; k <= r; k++) sum += tmp[Cl(k, 0, h - 1) * w + x];
            for (int y = 0; y < h; y++)
            {
                dst[y * w + x] = (float)(sum * inv);
                sum += tmp[Cl(y + r + 1, 0, h - 1) * w + x] - tmp[Cl(y - r, 0, h - 1) * w + x];
            }
        }
        return dst;
    }

    // ---------- hand-drawn geometry helpers ----------
    static List<PointF> Wobble(PointF[] poly, float amp, float step, Random r)
    {
        List<PointF> res = new List<PointF>();
        double p1 = r.NextDouble() * 6.28, p2 = r.NextDouble() * 6.28;
        int n = poly.Length;
        PointF[] v = new PointF[n];
        for (int i = 0; i < n; i++)
            v[i] = new PointF(poly[i].X + (float)((r.NextDouble() - 0.5) * amp), poly[i].Y + (float)((r.NextDouble() - 0.5) * amp));
        double s = 0;
        for (int i = 0; i < n; i++)
        {
            PointF a = v[i], b = v[(i + 1) % n];
            float dx = b.X - a.X, dy = b.Y - a.Y;
            float len = (float)Math.Sqrt(dx * dx + dy * dy);
            int k = Math.Max(1, (int)(len / step));
            float nx = -dy / len, ny = dx / len;
            for (int j = 0; j < k; j++)
            {
                float t = j / (float)k;
                double off = amp * (0.6 * Math.Sin(s * 0.018 + p1) + 0.4 * Math.Sin(s * 0.047 + p2));
                res.Add(new PointF(a.X + dx * t + nx * (float)off, a.Y + dy * t + ny * (float)off));
                s += len / k;
            }
        }
        return res;
    }

    static List<PointF> WobbleLine(PointF a, PointF b, float amp, float step, Random r)
    {
        List<PointF> res = new List<PointF>();
        double p1 = r.NextDouble() * 6.28, p2 = r.NextDouble() * 6.28;
        float dx = b.X - a.X, dy = b.Y - a.Y;
        float len = (float)Math.Sqrt(dx * dx + dy * dy);
        int k = Math.Max(2, (int)(len / step));
        float nx = -dy / len, ny = dx / len;
        for (int j = 0; j <= k; j++)
        {
            float t = j / (float)k;
            double off = amp * (0.6 * Math.Sin(t * len * 0.02 + p1) + 0.4 * Math.Sin(t * len * 0.05 + p2));
            res.Add(new PointF(a.X + dx * t + nx * (float)off, a.Y + dy * t + ny * (float)off));
        }
        return res;
    }

    static PointF[] Shift(PointF[] p, float dx, float dy)
    {
        PointF[] q = new PointF[p.Length];
        for (int i = 0; i < p.Length; i++) q[i] = new PointF(p[i].X + dx, p[i].Y + dy);
        return q;
    }

    static void DrawFill(Graphics g, PointF[] poly, Color green, Random r)
    {
        List<PointF> w = Wobble(Shift(poly, -8, 7), 6f, 24f, r);
        PointF[] arr = w.ToArray();
        using (SolidBrush br = new SolidBrush(green)) g.FillPolygon(br, arr);
        using (GraphicsPath clip = new GraphicsPath())
        {
            clip.AddPolygon(arr);
            g.SetClip(clip);
            RectangleF bb = clip.GetBounds();
            int n = (int)(bb.Width * bb.Height / 7000f);
            for (int i = 0; i < n; i++)
            {
                float x = bb.X + (float)r.NextDouble() * bb.Width, y = bb.Y + (float)r.NextDouble() * bb.Height;
                double ang = -(25 + r.NextDouble() * 35) * Math.PI / 180.0;
                float len = 50 + (float)r.NextDouble() * 150;
                PointF a = new PointF(x, y);
                PointF b = new PointF(x + (float)Math.Cos(ang) * len, y + (float)Math.Sin(ang) * len);
                List<PointF> line = WobbleLine(a, b, 3f, 40f, r);
                bool dark = r.NextDouble() < 0.35;
                Color c = dark ? Color.FromArgb(70 + r.Next(40), 85, 22, 20) : Color.FromArgb(30 + r.Next(45), 205, 125, 112);
                float wd = dark ? 9 + (float)r.NextDouble() * 8 : 3 + (float)r.NextDouble() * 4;
                using (Pen p = new Pen(c, wd)) { p.StartCap = LineCap.Round; p.EndCap = LineCap.Round; g.DrawLines(p, line.ToArray()); }
            }
            g.ResetClip();
        }
    }

    static void DrawOutline(Graphics g, PointF[] poly, Random r)
    {
        Color ink = Color.FromArgb(238, 20, 17, 14);
        List<PointF> w1 = Wobble(poly, 6f, 24f, r);
        using (Pen p = new Pen(ink, 17f)) { p.LineJoin = LineJoin.Round; g.DrawPolygon(p, w1.ToArray()); }
        for (int pass = 0; pass < 2; pass++)
            for (int i = 0; i < poly.Length; i++)
            {
                PointF a = poly[i], b = poly[(i + 1) % poly.Length];
                float dx = b.X - a.X, dy = b.Y - a.Y;
                float len = (float)Math.Sqrt(dx * dx + dy * dy);
                dx /= len; dy /= len;
                float nx = -dy, ny = dx;
                float o1 = (float)(r.NextDouble() * 24 - 6), o2 = (float)(r.NextDouble() * 24 - 6);
                float off = (float)((r.NextDouble() - 0.5) * 26);
                PointF s = new PointF(a.X - dx * o1 + nx * off, a.Y - dy * o1 + ny * off);
                PointF e = new PointF(b.X + dx * o2 + nx * off, b.Y + dy * o2 + ny * off);
                List<PointF> line = WobbleLine(s, e, 5f, 30f, r);
                using (Pen p = new Pen(Color.FromArgb(pass == 0 ? 215 : 165, 20, 17, 14), pass == 0 ? 10f : 6f))
                {
                    p.StartCap = LineCap.Round; p.EndCap = LineCap.Round;
                    g.DrawLines(p, line.ToArray());
                }
            }
    }

    static byte[] RenderLayer(Action<Graphics> draw)
    {
        using (Bitmap bmp = new Bitmap(W, H, PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(bmp))
            {
                g.Clear(Color.Transparent);
                g.SmoothingMode = SmoothingMode.AntiAlias;
                draw(g);
            }
            BitmapData d = bmp.LockBits(new Rectangle(0, 0, W, H), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            byte[] buf = new byte[d.Stride * H];
            System.Runtime.InteropServices.Marshal.Copy(d.Scan0, buf, 0, buf.Length);
            bmp.UnlockBits(d);
            return buf;
        }
    }

    // ---------- main ----------
    public static void Run(string srcPath, string outPath, string cleanPath, int seed)
    {
        Random rng = new Random(seed);
        float[] img = LoadRgb(srcPath);
        float cx = 1005f, cy = 1006f, R = 525f, Rf = 565f;

        // sample gear green
        double sr = 0, sg = 0, sb = 0; long cnt = 0;
        for (int y = (int)(cy - 520); y < (int)(cy + 520); y++)
            for (int x = (int)(cx - 520); x < (int)(cx + 520); x++)
            {
                float dx = x - cx, dy = y - cy;
                if (dx * dx + dy * dy > 520f * 520f) continue;
                int o = (y * W + x) * 3;
                float r = img[o], g = img[o + 1], b = img[o + 2];
                if (g > r + 12 && g > b + 6 && r > 30) { sr += r; sg += g; sb += b; cnt++; }
            }
        Color green = Color.FromArgb(255, 150, 42, 38);
        Console.WriteLine("green sample: " + green.R + "," + green.G + "," + green.B + " (" + cnt + " px)");

        // luminance high-pass of the cardboard
        float[] L = new float[W * H];
        for (int i = 0; i < W * H; i++) L[i] = 0.299f * img[i * 3] + 0.587f * img[i * 3 + 1] + 0.114f * img[i * 3 + 2];
        float[] Lb = BoxBlur(BoxBlur(L, W, H, 12), W, H, 12);
        float[] hp = new float[W * H];
        for (int i = 0; i < W * H; i++) hp[i] = L[i] - Lb[i];

        // low-frequency fill: harmonic interpolation on a coarse grid
        int cs = 8, gw = W / cs, gh = H / cs;
        float[] gr = new float[gw * gh * 3];
        bool[] unk = new bool[gw * gh];
        double[] ring = new double[3]; int rc = 0;
        for (int gy = 0; gy < gh; gy++)
            for (int gx = 0; gx < gw; gx++)
            {
                double s0 = 0, s1 = 0, s2 = 0;
                for (int yy = 0; yy < cs; yy++)
                    for (int xx = 0; xx < cs; xx++)
                    {
                        int o = ((gy * cs + yy) * W + gx * cs + xx) * 3;
                        s0 += img[o]; s1 += img[o + 1]; s2 += img[o + 2];
                    }
                int gi = (gy * gw + gx) * 3;
                gr[gi] = (float)(s0 / (cs * cs)); gr[gi + 1] = (float)(s1 / (cs * cs)); gr[gi + 2] = (float)(s2 / (cs * cs));
                float ccx = gx * cs + cs / 2f, ccy = gy * cs + cs / 2f;
                float d = (float)Math.Sqrt((ccx - cx) * (ccx - cx) + (ccy - cy) * (ccy - cy));
                unk[gy * gw + gx] = d <= Rf;
                if (d > Rf && d < Rf + 20) { ring[0] += gr[gi]; ring[1] += gr[gi + 1]; ring[2] += gr[gi + 2]; rc++; }
            }
        for (int i = 0; i < gw * gh; i++)
            if (unk[i]) for (int c = 0; c < 3; c++) gr[i * 3 + c] = (float)(ring[c] / rc);
        for (int iter = 0; iter < 800; iter++)
            for (int gy = 1; gy < gh - 1; gy++)
                for (int gx = 1; gx < gw - 1; gx++)
                {
                    int idx = gy * gw + gx;
                    if (!unk[idx]) continue;
                    for (int c = 0; c < 3; c++)
                    {
                        float nv = 0.25f * (gr[(idx - 1) * 3 + c] + gr[(idx + 1) * 3 + c] + gr[(idx - gw) * 3 + c] + gr[(idx + gw) * 3 + c]);
                        gr[idx * 3 + c] += 1.9f * (nv - gr[idx * 3 + c]);
                    }
                }

        // high-frequency fill: windowed patches from clean cardboard, variance-preserving blend
        int P = 128, S = 64;
        int[][] donors = new int[][] {
            new int[] { 170, 700, 500, 1350 },
            new int[] { 640, 170, 1360, 470 },
            new int[] { 1540, 600, 1700, 1350 }
        };
        float[] wh = new float[P];
        for (int i = 0; i < P; i++) { double s = Math.Sin(Math.PI * (i + 0.5) / P); wh[i] = (float)(s * s); }
        float[] q = new float[W * H], ws = new float[W * H];
        int qx0 = ((int)(cx - Rf - 64) / S) * S, qx1 = (int)(cx + Rf + 64);
        int qy0 = ((int)(cy - Rf - 64) / S) * S, qy1 = (int)(cy + Rf + 64);
        for (int py = qy0; py < qy1; py += S)
            for (int px = qx0; px < qx1; px += S)
            {
                int[] D = donors[rng.Next(donors.Length)];
                int dx0 = rng.Next(D[0], D[2] - P + 1), dy0 = rng.Next(D[1], D[3] - P + 1);
                int t = rng.Next(8);
                bool fx = (t & 1) != 0, fy = (t & 2) != 0, tr = (t & 4) != 0;
                for (int v = 0; v < P; v++)
                    for (int u = 0; u < P; u++)
                    {
                        int X = px + u, Y = py + v;
                        if (X < 0 || Y < 0 || X >= W || Y >= H) continue;
                        int su = fx ? P - 1 - u : u, sv = fy ? P - 1 - v : v;
                        if (tr) { int tmp = su; su = sv; sv = tmp; }
                        float w = wh[u] * wh[v];
                        q[Y * W + X] += w * hp[(dy0 + sv) * W + dx0 + su];
                        ws[Y * W + X] += w * w;
                    }
            }
        float[] hpq = new float[W * H];
        for (int i = 0; i < W * H; i++) hpq[i] = ws[i] > 1e-6f ? q[i] / (float)Math.Sqrt(ws[i]) : 0f;

        // erase gear
        float[] bg = (float[])img.Clone();
        for (int y = (int)(cy - Rf - 2); y <= (int)(cy + Rf + 2); y++)
            for (int x = (int)(cx - Rf - 2); x <= (int)(cx + Rf + 2); x++)
            {
                float dx = x - cx, dy = y - cy;
                float d = (float)Math.Sqrt(dx * dx + dy * dy);
                float a = d <= R ? 1f : (d >= Rf - 5 ? 0f : 1f - (d - R) / (Rf - 5 - R));
                if (a <= 0f) continue;
                float gxf = Math.Max(0f, Math.Min(gw - 1.001f, (x - (cs / 2f - 0.5f)) / cs));
                float gyf = Math.Max(0f, Math.Min(gh - 1.001f, (y - (cs / 2f - 0.5f)) / cs));
                int gx0 = (int)gxf, gy0 = (int)gyf; float fx2 = gxf - gx0, fy2 = gyf - gy0;
                for (int c = 0; c < 3; c++)
                {
                    float v00 = gr[(gy0 * gw + gx0) * 3 + c], v10 = gr[(gy0 * gw + gx0 + 1) * 3 + c];
                    float v01 = gr[((gy0 + 1) * gw + gx0) * 3 + c], v11 = gr[((gy0 + 1) * gw + gx0 + 1) * 3 + c];
                    float low = (v00 * (1 - fx2) + v10 * fx2) * (1 - fy2) + (v01 * (1 - fx2) + v11 * fx2) * fy2;
                    float val = low + hpq[y * W + x];
                    int o = (y * W + x) * 3 + c;
                    bg[o] = img[o] * (1 - a) + val * a;
                }
            }
        if (cleanPath != null && cleanPath.Length > 0) SaveRgb(bg, cleanPath);

        // ---------- draw the exit icon ----------
        float oy = 6f;
        PointF[] bracket = Shift(new PointF[] {
            new PointF(560, 540), new PointF(1020, 540), new PointF(1020, 690), new PointF(710, 690),
            new PointF(710, 1310), new PointF(1020, 1310), new PointF(1020, 1460), new PointF(560, 1460) }, 0, oy);
        PointF[] arrow = Shift(new PointF[] {
            new PointF(840, 925), new PointF(1150, 925), new PointF(1150, 780), new PointF(1450, 1000),
            new PointF(1150, 1220), new PointF(1150, 1075), new PointF(840, 1075) }, 0, oy);
        PointF[][] shapes = new PointF[][] { bracket, arrow };

        Random dr = new Random(seed * 31 + 5);
        byte[] fillBuf = RenderLayer(delegate(Graphics g) { foreach (PointF[] s in shapes) DrawFill(g, s, green, dr); });
        byte[] lineBuf = RenderLayer(delegate(Graphics g) { foreach (PointF[] s in shapes) DrawOutline(g, s, dr); });

        // dry-crayon grain for the strokes
        float[] n1 = new float[W * H], n2 = new float[W * H];
        for (int i = 0; i < W * H; i++) { n1[i] = (float)(rng.NextDouble() * 2 - 1); n2[i] = (float)(rng.NextDouble() * 2 - 1); }
        n1 = BoxBlur(n1, W, H, 1);
        n2 = BoxBlur(n2, W, H, 6);

        int stride = W * 4;
        float[] res = (float[])bg.Clone();
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
            {
                int i = y * stride + x * 4, o = (y * W + x) * 3;
                float fa = fillBuf[i + 3] / 255f;
                if (fa > 0f)
                {
                    float tex = hpq[y * W + x] * 0.7f;
                    res[o] = res[o] * (1 - fa) + (fillBuf[i + 2] + tex) * fa;
                    res[o + 1] = res[o + 1] * (1 - fa) + (fillBuf[i + 1] + tex) * fa;
                    res[o + 2] = res[o + 2] * (1 - fa) + (fillBuf[i] + tex) * fa;
                }
                float la = lineBuf[i + 3] / 255f;
                if (la > 0f)
                {
                    float m = 0.95f + 0.6f * n1[y * W + x] + 2.0f * n2[y * W + x];
                    m = m < 0.5f ? 0.5f : (m > 1f ? 1f : m);
                    la *= m;
                    res[o] = res[o] * (1 - la) + lineBuf[i + 2] * la;
                    res[o + 1] = res[o + 1] * (1 - la) + lineBuf[i + 1] * la;
                    res[o + 2] = res[o + 2] * (1 - la) + lineBuf[i] * la;
                }
            }
        SaveRgb(res, outPath);
        Console.WriteLine("saved " + outPath);
    }
}
'@

[ExitIconMaker]::Run($SrcPath, $OutPath, $CleanPath, $Seed)

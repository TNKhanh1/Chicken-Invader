import math
from PIL import Image, ImageDraw

img = Image.open('assets/coin.png').convert('RGBA')
w, h = img.size
cx, cy = w / 2, h / 2
R = 355

mask = Image.new('L', (w, h), 0)
draw = ImageDraw.Draw(mask)
draw.ellipse((cx - R, cy - R, cx + R, cy + R), fill=255)

img.putalpha(mask)

bbox = img.getbbox()
if bbox:
    img = img.crop(bbox)

img = img.resize((64, 64), Image.Resampling.LANCZOS)
img.save('assets/coin.png')
print("Processed successfully!")

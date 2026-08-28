from PIL import Image
import os

def remove_bg(filepath):
    print(f"Processing {filepath}...")
    img = Image.open(filepath).convert("RGBA")
    datas = img.getdata()

    # Get the color of the top-left pixel (assumed to be the background color)
    bg_color = datas[0]
    
    # Tolerances
    tol = 30
    
    newData = []
    for item in datas:
        # Check if the pixel color is similar to the background color
        if (abs(item[0] - bg_color[0]) < tol and 
            abs(item[1] - bg_color[1]) < tol and 
            abs(item[2] - bg_color[2]) < tol):
            newData.append((255, 255, 255, 0)) # transparent
        else:
            newData.append(item)

    img.putdata(newData)
    
    # Crop to bounding box of non-transparent pixels
    bbox = img.getbbox()
    if bbox:
        img = img.crop(bbox)
        
    img.save(filepath, "PNG")
    print(f"Saved {filepath}")

if __name__ == "__main__":
    files = ["assets/SwordItem.png", "assets/ShieldItem.png", "assets/HeartItem.png"]
    for f in files:
        if os.path.exists(f):
            remove_bg(f)
        else:
            print(f"File not found: {f}")

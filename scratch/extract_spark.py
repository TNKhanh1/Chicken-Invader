import numpy as np
from PIL import Image
import os
from scipy.ndimage import label, find_objects

src_path = "scratch/chicken10_sparks.png"
if not os.path.exists(src_path):
    print("Not found")
    exit(1)

img = Image.open(src_path).convert("RGBA")
arr = np.array(img)
alpha = arr[:, :, 3]

labeled_array, num_features = label(alpha > 0)
slices = find_objects(labeled_array)

best_spark = None
best_size = 0

for i, slc in enumerate(slices):
    if slc is None:
        continue
    comp_mask = (labeled_array[slc] == (i + 1))
    size = np.sum(comp_mask)
    # Want a decent sized spark, not the watermark
    if size > best_size and size < 1000: # Watermark might be large, sparks are small
        best_size = size
        
        # Crop out this spark
        spark_crop = arr[slc].copy()
        spark_crop[~comp_mask, 3] = 0
        best_spark = spark_crop

if best_spark is not None:
    Image.fromarray(best_spark).save("assets/enemy/spark.png")
    print("Saved assets/enemy/spark.png")
else:
    print("No spark found")

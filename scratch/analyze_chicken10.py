import numpy as np
from PIL import Image
import os

src_path = "assets/enemy/chicken10.png"
if not os.path.exists(src_path):
    print("Not found")
    exit(1)

img = Image.open(src_path).convert("RGBA")
arr = np.array(img)
alpha = arr[:, :, 3]

from scipy.ndimage import label
labeled_array, num_features = label(alpha > 0)
print(f"Found {num_features} connected components.")

sizes = np.bincount(labeled_array.ravel())
sizes[0] = 0 # background
main_component_label = sizes.argmax()

main_mask = labeled_array == main_component_label
spark_mask = (labeled_array > 0) & (labeled_array != main_component_label)

# Try to find watermark
spark_y, spark_x = np.where(spark_mask)
# if any component is near the bottom right, remove it
watermark_mask = np.zeros_like(spark_mask)
for i in range(1, num_features + 1):
    if i == main_component_label:
        continue
    comp_mask = labeled_array == i
    cy, cx = np.where(comp_mask)
    if cx.max() > arr.shape[1] - 50 and cy.max() > arr.shape[0] - 50:
        watermark_mask |= comp_mask
        spark_mask &= ~comp_mask

# Output main body
main_arr = arr.copy()
main_arr[~main_mask, 3] = 0
Image.fromarray(main_arr).save("scratch/chicken10_main.png")

# Output sparks
spark_arr = arr.copy()
spark_arr[~spark_mask, 3] = 0
Image.fromarray(spark_arr).save("scratch/chicken10_sparks.png")

print("Saved chicken10_main.png and chicken10_sparks.png")

# Leg detection on main_mask
row_sums = np.sum(main_mask, axis=1)
nz_rows = np.where(row_sums > 0)[0]
min_y, max_y = nz_rows[0], nz_rows[-1]
col_sums = np.sum(main_mask, axis=0)
nz_cols = np.where(col_sums > 0)[0]
min_x, max_x = nz_cols[0], nz_cols[-1]

print(f"Main body Y: {min_y} to {max_y}, X: {min_x} to {max_x}")

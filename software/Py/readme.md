# Python Section of the Application

Much easier to use OpenCV in Python, I'd also like to use some Python Exclusive libraries in the future, namely **Open3D** lib for easy Point Cloud Surface reconstruction algorithm built-ins, and potentially experiment with Mesh-Matching *(Lining up incomplete meshes)* using o3d's ICP algorithms!


# My Setup for this Project

First Time Setup:

```bash
conda create -n opencv_env python=3.8
conda activate opencv_env
conda install -c conda-forge opencv numpy
```

Quick Use: *(FOR JASON's CONDA TERMINAL)*
```bash
conda activate opencv_env
cd C:\Users\jason\Documents\GitHub\3D-IoT-Object-Scanner\software\Py
python process_images.py
```
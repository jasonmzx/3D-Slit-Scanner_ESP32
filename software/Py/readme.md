# Python Section of the Application

Much easier to use OpenCV in Python, I'd also like to use some Python Exclusive libraries in the future, namely: <br/>
**sk-learn** For simple regression models; RANSAC Regressor, 
**numpy** Linear Algebra Operations *( matmul, dot, T, shaped arrays)* 
**opencv** It's really nice in Python, very integrated with numpy, and easy to use in Python!
<br/>

Will potentially use **Open3D** Python lib for easy Point Cloud Surface reconstruction algorithm built-ins, and potentially experiment with Mesh-Matching *(Lining up incomplete meshes)* using o3d's ICP algorithms!


# My Setup for this Project

First Time Setup:

```bash
conda create -n opencv_env python=3.8
conda activate opencv_env
conda install -c conda-forge opencv numpy scikit-learn
```

Quick Use: *(FOR JASON's CONDA TERMINAL)*
```bash
conda activate opencv_env
cd C:\Users\jason\Documents\GitHub\3D-IoT-Object-Scanner\software\Py
python process_images.py
```
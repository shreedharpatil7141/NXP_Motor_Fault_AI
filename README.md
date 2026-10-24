# NXP Motor Fault AI

Python ML project for motor bearing fault classification from CWRU bearing vibration data.

## Setup on Windows

Install Python 3.10 or 3.11, then run:

```powershell
py -3.11 -m venv .venv
.\.venv\Scripts\python -m pip install --upgrade pip
.\.venv\Scripts\pip install -r requirements.txt
```

If Python 3.11 is unavailable, use `py -3.10` instead.

## Workflow

```powershell
git clone https://github.com/srigas/CWRU_Bearing_NumPy dataset/raw/CWRU_Bearing_NumPy
.\.venv\Scripts\python src\prepare_data.py
.\.venv\Scripts\python src\train_binary.py
.\.venv\Scripts\python src\train_4class.py
.\.venv\Scripts\python src\export_tflite.py
```

The held-out final test RPM is always `1772`. Float models, INT8 TFLite model, and normalization stats are written to `models/`; reports are written to `results/`.
